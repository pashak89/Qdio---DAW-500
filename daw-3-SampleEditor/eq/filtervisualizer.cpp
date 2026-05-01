#include "filtervisualizer.h"
#include "core/screeninterface.h"
#include "multieqeffect.h"
FilterVisualizer::FilterVisualizer(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{

    m_filtersModel[0] = new FiltersModel();
    m_filtersModel[0]->addFilter(Filter { 0, true, "freq", "Hz", "Freq", true, 20, 22000, 30, 30, true, false, false });
    m_filtersModel[0]->addFilter(Filter { 0, true, "gain", "dB", "Gain", true, -20, 20, 0, 0, false, false, false });
    m_filtersModel[0]->addFilter(Filter { 0, true, "q", "", "Q", true, 0.1, 20, 0.7, 0.7, true, false, false });

    m_filtersModel[1] = new FiltersModel();
    m_filtersModel[1]->addFilter(Filter { 1, true, "freq", "Hz", "Freq", true, 20, 22000, 200, 200, true, false, false });
    m_filtersModel[1]->addFilter(Filter { 1, true, "gain", "dB", "Gain", true, -20, 20, 0, 0, false, false, false });
    m_filtersModel[1]->addFilter(Filter { 1, true, "q", "", "Q", true, 0.1, 20, 0.7, 0.7, true, false, false });

    m_filtersModel[2] = new FiltersModel();
    m_filtersModel[2]->addFilter(Filter { 2, true, "freq", "Hz", "Freq", true, 20, 22000, 1000, 1000, true, false, false });
    m_filtersModel[2]->addFilter(Filter { 2, true, "gain", "dB", "Gain", true, -20, 20, 0, 0, false, false, false });
    m_filtersModel[2]->addFilter(Filter { 2, true, "q", "", "Q", true, 0.1, 20, 0.7, 0.7, true, false, false });

    m_filtersModel[3] = new FiltersModel();
    m_filtersModel[3]->addFilter(Filter { 3, true, "freq", "Hz", "Freq", true, 20, 22000, 5000, 5000, true, false, false });
    m_filtersModel[3]->addFilter(Filter { 3, true, "gain", "dB", "Gain", true, -20, 20, 0, 0, false, false, false });
    m_filtersModel[3]->addFilter(Filter { 3, true, "q", "", "Q", true, 0.1, 20, 0.7, 0.7, true, false, false });

    m_bandModel = new BandModel();
    m_bandModel->addBand(BandModel::Band { MultiEQ::LowShelf, true, true, "1", 0, 0 });
    m_bandModel->addBand(BandModel::Band { MultiEQ::BellFilter, false, false, "2", 0, 0 });
    m_bandModel->addBand(BandModel::Band { MultiEQ::BellFilter, false, false, "3", 0, 0 });
    m_bandModel->addBand(BandModel::Band { MultiEQ::HighShelf, false, false, "4", 0, 0 });

    overallGainInDb = (0.0f);
    sampleRate = (48000.0);

    dyn = _settings.dbMax - _settings.dbMin;
    zero = 2.0f * _settings.dbMax / dyn;
    scale = 1.0f / (zero + std::tanh(_settings.dbMin / dyn * -2.0f));

    connect(this, &FilterVisualizer::sigUpdate, this, &FilterVisualizer::sltUpdate);

    mL = ScreenInterface::scaleSize2(7);
    mR = ScreenInterface::scaleSize2(7);
    mT = ScreenInterface::scaleSize2(0);
    mB = ScreenInterface::scaleSize2(5);
    OH = 0;

    //    Q_EMIT bandModelChanged();
}

void FilterVisualizer::setBandEnable(int index, bool active)
{

    if (index < 0)
        return;

    setActiveIndex(index);

    if (_multiEq) {
        _multiEq->parameters()->filterEnabled[index] = active;
        m_bandModel->setEnable(index, active);
        m_bandModel->setActive(index, active);
    }

    Q_EMIT bandEnabled(index, active);

    Q_EMIT _multiEq->sigUpdateUI();
    Q_EMIT sigUpdate();
}

bool FilterVisualizer::isBandEnabled(int index)
{
    if (index < 0)
        return false;

    if (index < 4 && _multiEq) {
        return _multiEq->parameters()->filterEnabled[index];
    }

    return false;
}

void FilterVisualizer::setActiveIndex(int index)
{

    if (index < 0)
        return;

    m_activeIndex = index;

    if (_multiEq) {
        _multiEq->updateGuiCoefficients();
        _multiEq->setActiveIndex(index);
    }

    for (int i = 0; i < 4; i++) {
        m_bandModel->setActive(i, false);
    }

    m_bandModel->setActive(index, true);

    Q_EMIT _multiEq->sigUpdateUI();
    Q_EMIT sigUpdate();

    Q_EMIT activeIndexChanged();

    Q_EMIT filtersModelChanged();

    Q_EMIT filterTypeChanged();
}

int FilterVisualizer::activeIndex() const
{
    return m_activeIndex;
}

void FilterVisualizer::setFilterEnabled(int index, bool enabled)
{
    m_filtersModel[m_activeIndex]->setEnabled(index, enabled);
    if (_multiEq)
        Q_EMIT _multiEq->sigUpdateUI();
}

void FilterVisualizer::setAutomatedEnabled(int index)
{
    if (_multiEq)
        _multiEq->automatedEnabled(m_activeIndex * 3 + index);
}

void FilterVisualizer::setSampleRate(const double newSampleRate)
{
    if (newSampleRate == 0)
        sampleRate = 48000.0;
    else
        sampleRate = newSampleRate;

    if (_multiEq)
        Q_EMIT _multiEq->sigUpdateUI();

    Q_EMIT sigUpdate();
}
void FilterVisualizer::bandUpdate()
{

    if (_multiEq == nullptr) {
        return;
    }

    const int size = 4;
    for (int i = 0; i < size; ++i) {
        QSharedPointer<MultiEQ::Parameters> handle = _multiEq->parameters();

        double m_circleY = 0;
        auto m_circleX = hzToX(handle->filterFrequency[i]);

        if ((handle->filterType[i] == MultiEQ::SecondOrderHighPass)
            || (handle->filterType[i] == MultiEQ::SecondOrderLowPass)
            || (handle->filterType[i] == MultiEQ::LinkwitzRileyHighPass)
            || (handle->filterType[i] == MultiEQ::PeakFilter)
            || (handle->filterType[i] == MultiEQ::LinkwitzRileyLowPass)) {
            double gain;
            if (handle->filterQ[i] < 1.16) {
                gain = 20.0 * std::log10(handle->filterQ[i]);
            } else {
                gain = 20.0 * (std::log10(handle->filterQ[i]) / std::log10(20.0));
            }
            m_circleY = dbToY(gain);

        } else {
            if (!_settings.gainHandleLin)
                m_circleY = dbToY(handle->filterGain[i]);
            else
                m_circleY = dbToY(gainToDecibels(handle->filterGain[i]));
        }
        m_bandModel->setPos(i, m_circleX, m_circleY);
    }
    Q_EMIT sigCircleMoved();
}

void FilterVisualizer::paint(QPainter* painter)
{

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);

    // Background
    painter->setPen(Qt::NoPen);
    QRectF rect(0, 0, width(), height());
    painter->setBrush(QColor("#040405"));
    painter->drawRect(rect);

    QPen pen(QColor("#040405"));
    painter->setPen(pen);

    QBrush brush(QColor("#040405"));
    painter->setBrush(brush);

    painter->strokePath(dbGridPath, QPen(QColor("#28292B"), 0.5f));
    painter->strokePath(hzGridPathBold, QPen(QColor("#28292B"), 1.0f));

    if (_multiEq == nullptr) {
        return;
    }

    if (magnitudes.size() == 0) {
        return;
    }
    // draw filter magnitude responses
    QPainterPath magnitude;
    allMagnitudesInDb.fill(overallGainInDb);

    const int xMin = hzToX(_settings.fMin);
    const int xMax = hzToX(_settings.fMax);
    const int yMin = qMax(dbToY(_settings.dbMax), 0);
    const int yMax = qMax(dbToY(_settings.dbMin), yMin);

    const float zeroDbY = dbToYFloat(0.0f);

    painter->save();

    QPen zeroDbPen(QColor("#4A4B4D")); // subtle gray

    if (_areaInfo) {
        if (_areaInfo->themeType() == 0) {
            zeroDbPen = QColor("#5E5E5E");
        }

        if (_areaInfo->themeType() == 1) {
            zeroDbPen = QColor("#B6B6B6");
        }
    }

    zeroDbPen.setWidthF(1.0f);

    painter->setPen(zeroDbPen);
    painter->drawLine(
        QPointF(xMin, zeroDbY),
        QPointF(xMax, zeroDbY));

    painter->restore();

    for (int b = 4; --b >= 0;) {

        magnitude = QPainterPath();

        QSharedPointer<MultiEQ::Parameters> handle = _multiEq->parameters(); // (&_multiEq->parameters[b]);
        const bool isEnabled = handle->filterEnabled[b];

        _multiEq->getMagnitudeForFrequencyArray(b, frequencies.data(), magnitudes.data(), numPixels,
            sampleRate);

        float additiveDB = 0.0f;

        float multGain = decibelsToGain(additiveDB);

        // overall magnitude update
        if (isEnabled) {
            for (int i = 0; i < numPixels; ++i) {
                const float dB = gainToDecibels(magnitudes[i] * multGain);
                allMagnitudesInDb[i] = (allMagnitudesInDb[i] + dB);
            }
        }
    }

    // all magnitudes combined
    magnitude = QPainterPath();

    magnitude.moveTo(xMin, dbToYFloat(allMagnitudesInDb[0]));

    for (int x = xMin + 1; x <= xMax; ++x) {
        magnitude.lineTo(x, dbToYFloat(allMagnitudesInDb[x - xMin]));
    }

    QPen pen_line = painter->pen();
    pen_line.setWidthF(1.5f);
    if (m_enabled) {
        pen_line.setColor("#FD9227");
    } else {
        pen_line.setColor("#3A3A3A");
    }

    painter->strokePath(magnitude, pen_line);
}
void FilterVisualizer::sltDeviceOnValueChanged(bool value)
{
    setDeviceOnValue(value);
}

void FilterVisualizer::sltDeviceOnActiveChanged(bool active)
{
    setDeviceOnActive(active);
}

void FilterVisualizer::sltDeviceOnAutomatedChanged(bool active)
{
    setDeviceOnAutomated(active);
}
void FilterVisualizer::sltParameterAutomated(int index, bool automated)
{

    int i = index / 3;
    int r = index % 3;

    m_filtersModel[i]->setAutomated(r, automated);
}

void FilterVisualizer::sltParameterActiveChanged(int index, bool active)
{
    int i = index / 3;
    int r = index % 3;
    m_filtersModel[i]->setActive(r, active);
}

void FilterVisualizer::sltParameterChanged()
{
    Q_EMIT freqChanged();
    Q_EMIT gainChanged();
    Q_EMIT qFactorChanged();

    //    m_filtersModel->setAutomated(0, true);
    //    m_filtersModel->setAutomated(1, true);
    //    m_filtersModel->setAutomated(2, true);

    //    m_filtersModel->setActive(0, true);
    //    m_filtersModel->setActive(1, true);
    //    m_filtersModel->setActive(2, true);

    if (_multiEq)
        Q_EMIT _multiEq->sigUpdateUI();
    Q_EMIT sigUpdate();
}

void FilterVisualizer::sltParameterUpdated(int index)
{
    int i = index / 3;
    int r = index % 3;

    if (!_multiEq)
        return;

    if (r == 0) {

        m_filtersModel[i]->setValue(r, _multiEq->parameters()->filterFrequency[i]);
        // Q_EMIT freqChanged();
    } else if (r == 1) {
        m_filtersModel[i]->setValue(r, _multiEq->parameters()->filterGain[i]);
        // Q_EMIT gainChanged();
    } else {
        m_filtersModel[i]->setValue(r, _multiEq->parameters()->filterQ[i]);
        // Q_EMIT qFactorChanged();
    }

    Q_EMIT _multiEq->sigUpdateUI();
    Q_EMIT sigUpdate();
}
void FilterVisualizer::setMultiEq(MultiEQ* multiEq)
{

    if (_multiEq)
        _multiEq->disconnect();

    _multiEq = multiEq;

    // connect(_multiEq, &MultiEQ::sigParameterChanged, this, &FilterVisualizer::sltParameterChanged);
    connect(_multiEq, &MultiEQ::sigParameterAutomated, this, &FilterVisualizer::sltParameterAutomated);
    connect(_multiEq, &MultiEQ::sigParameterActiveChanged, this, &FilterVisualizer::sltParameterActiveChanged);
    connect(_multiEq, &MultiEQ::sigParameterUpdated, this, &FilterVisualizer::sltParameterUpdated);

    connect(_multiEq, &MultiEQ::sigDeviceOnActiveChanged, this, &FilterVisualizer::sltDeviceOnActiveChanged);
    connect(_multiEq, &MultiEQ::sigDeviceOnAutomatedChanged, this, &FilterVisualizer::sltDeviceOnAutomatedChanged);
    connect(_multiEq, &MultiEQ::sigDeviceOnValueChanged, this, &FilterVisualizer::sltDeviceOnValueChanged);
    connect(_multiEq, &MultiEQ::sigUpdateStatus, this, &FilterVisualizer::sltUpdateStatus);
    connect(_multiEq, &MultiEQ::sigUpdateUI, this, &FilterVisualizer::sltUpdate);

    for (int var = 0; var < 4; ++var) {
        if (_multiEq) {

            if (_multiEq->parameters()->filterEnabled[var]) {
                setBandEnable(var, true);
                setActiveIndex(var);
            }
        }
    }

    sltUpdateStatus(_multiEq->status());

    Q_EMIT multieqChanged();
}

int FilterVisualizer::filterType()
{
    if (_multiEq) {
        return _multiEq->parameters()->filterType[m_activeIndex];
    }
    return -1;
}

void FilterVisualizer::setFilterType(int filterType)
{
    if (_multiEq) {

        // saveOperation();

        _multiEq->parameters()->filterType[m_activeIndex] = filterType;

        _multiEq->updateGuiCoefficients();
        Q_EMIT filterTypeChanged();

        Q_EMIT _multiEq->sigUpdateUI();
        Q_EMIT sigUpdate();
    }
}

float FilterVisualizer::gain()
{
    if (_multiEq) {
        return _multiEq->parameters()->filterGain[m_activeIndex];
    }
    return 0.0;
}

void FilterVisualizer::setGain(float gain)
{
    if (_multiEq) {

        _multiEq->parameters()->filterGain[m_activeIndex] = gain;
        _multiEq->updateGuiCoefficients();

        _multiEq->updateParameter(m_activeIndex, "Gain", gain);
        Q_EMIT gainChanged();

        Q_EMIT _multiEq->sigUpdateUI();
        Q_EMIT sigUpdate();
    }
}

int FilterVisualizer::freq()
{
    if (_multiEq) {

        return _multiEq->parameters()->filterFrequency[m_activeIndex];
    }
    return -1;
}

void FilterVisualizer::setFreq(float freq)
{
    if (_multiEq) {

        _multiEq->parameters()->filterFrequency[m_activeIndex] = freq;
        _multiEq->updateGuiCoefficients();

        Q_EMIT _multiEq->sigUpdateUI();
        Q_EMIT sigUpdate();

        _multiEq->updateParameter(m_activeIndex, "Freq", freq);
        Q_EMIT freqChanged();
    }
}

float FilterVisualizer::qFactor()
{
    if (_multiEq) {
        return _multiEq->parameters()->filterQ[m_activeIndex];
    }
    return -1;
}

void FilterVisualizer::setQfactor(float q)
{
    if (_multiEq) {

        _multiEq->parameters()->filterQ[m_activeIndex] = q;
        _multiEq->updateGuiCoefficients();

        _multiEq->updateParameter(m_activeIndex, "Q", q);
        Q_EMIT qFactorChanged();

        Q_EMIT _multiEq->sigUpdateUI();
        Q_EMIT sigUpdate();
    }
}

bool FilterVisualizer::enabled() const
{
    return m_enabled;
}

void FilterVisualizer::setEnabled(bool newEnabled)
{

    m_enabled = newEnabled;
    emit enabledChanged();
    update();
}

QSize FilterVisualizer::size() const
{
    return QSize(m_itemWidth, m_itemHeight);
}

void FilterVisualizer::setSize(QSize size)
{

    if (size.width() == 0 || size.height() == 0)
        return;

    initialized(size.width(), size.height());
    bandUpdate();
    update();

    Q_EMIT sizeChanged();
}

int FilterVisualizer::circleSize() const
{
    return m_circleSize;
}

void FilterVisualizer::setCircleSize(int size)
{
    m_circleSize = size;

    mL = size / 2;
    mR = size / 2;
    mT = size / 2;
    mB = size / 2;
    OH = 0;

    bandUpdate();

    //    Q_EMIT circleSizeChanged();
}

bool FilterVisualizer::deviceOnValue() const
{
    return _deviceOnValue;
}

void FilterVisualizer::setDeviceOnValue(int value)
{
    _deviceOnValue = value;
    Q_EMIT deviceOnValueChanged();
    m_enabled = value == 1;
    update();
}

bool FilterVisualizer::deviceOnActive() const
{
    return _deviceOnActive;
}

void FilterVisualizer::setDeviceOnActive(bool value)
{
    _deviceOnActive = value;
    Q_EMIT deviceOnActiveChanged();
}

bool FilterVisualizer::deviceOnAutomated() const
{
    return _deviceOnAutomated;
}

void FilterVisualizer::setDeviceOnAutomated(bool value)
{
    _deviceOnAutomated = value;

    Q_EMIT deviceOnAutomatedChanged();
}

void FilterVisualizer::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;
    Q_EMIT areaInfoChanged();
}

AreaInfo* FilterVisualizer::areaInfo()
{
    return _areaInfo;
}

void FilterVisualizer::sltUpdateStatus(QJsonObject status)
{
    if (!status.contains("status") || !status.contains("active_band"))
        return;

    QJsonArray sttaus_array = status["status"].toArray();
    m_activeIndex = status["active_band"].toInt();

    // qDebug() << status;
    QJsonObject jObject;

    QJsonArray status_array;

    jObject["active_band"] = m_activeIndex;
    for (int i = 0; i < 4; i++) {
        QJsonObject object;
        object["band_number"] = i;

        object["type"] = _multiEq->parameters()->filterType[i];
        object["enabled"] = _multiEq->parameters()->filterEnabled[i];
        object["freq"] = _multiEq->parameters()->filterFrequency[i];
        object["gain"] = _multiEq->parameters()->filterGain[i];
        object["q"] = _multiEq->parameters()->filterQ[i];

        status_array.append(object);
    }
    jObject["status"] = status_array;

    for (int i = 0; i < 4; i++) {
        QJsonObject object = sttaus_array[i].toObject();

        _multiEq->parameters()->filterType[i] = object["type"].toInt();
        _multiEq->parameters()->filterEnabled[i] = object["enabled"].toBool();
        _multiEq->parameters()->filterFrequency[i] = object["freq"].toDouble();
        _multiEq->parameters()->filterGain[i] = object["gain"].toDouble();
        _multiEq->parameters()->filterQ[i] = object["q"].toDouble();

        m_bandModel->setEnable(i, _multiEq->parameters()->filterEnabled[i]);

        m_filtersModel[i]->setValue(0, _multiEq->parameters()->filterFrequency[i]);
        m_filtersModel[i]->setValue(1, _multiEq->parameters()->filterGain[i]);
        m_filtersModel[i]->setValue(2, _multiEq->parameters()->filterQ[i]);
    }

    for (int i = 0; i < 4; i++) {
        m_bandModel->setActive(i, false);
    }
    m_bandModel->setActive(m_activeIndex, true);

    _multiEq->updateGuiCoefficients();

    Q_EMIT _multiEq->sigUpdateUI();
    Q_EMIT sigUpdate();

    Q_EMIT activeIndexChanged();

    Q_EMIT filtersModelChanged();

    Q_EMIT filterTypeChanged();

    bandUpdate();
    update();

    _multiEq->multiEqEffect()->sltUpdateState(jObject, 0);
}

void FilterVisualizer::saveUndoOperation(int op)
{

    if (_multiEq && _multiEq->parameters()) {
        QJsonObject jObject;

        QJsonArray status_array;

        jObject["active_band"] = m_activeIndex;
        for (int i = 0; i < 4; i++) {
            QJsonObject object;
            object["band_number"] = i;

            object["type"] = _multiEq->parameters()->filterType[i];
            object["enabled"] = _multiEq->parameters()->filterEnabled[i];
            object["freq"] = _multiEq->parameters()->filterFrequency[i];
            object["gain"] = _multiEq->parameters()->filterGain[i];
            object["q"] = _multiEq->parameters()->filterQ[i];

            status_array.append(object);
        }
        jObject["status"] = status_array;

        _multiEq->multiEqEffect()->sltSaveInUndo(jObject, op);
    }
}

void FilterVisualizer::saveRedoOperation()
{

    if (_multiEq && _multiEq->parameters()) {
        QJsonObject jObject;

        QJsonArray status_array;

        jObject["active_band"] = m_activeIndex;
        for (int i = 0; i < 4; i++) {
            QJsonObject object;
            object["band_number"] = i;

            object["type"] = _multiEq->parameters()->filterType[i];
            object["enabled"] = _multiEq->parameters()->filterEnabled[i];
            object["freq"] = _multiEq->parameters()->filterFrequency[i];
            object["gain"] = _multiEq->parameters()->filterGain[i];
            object["q"] = _multiEq->parameters()->filterQ[i];

            status_array.append(object);
        }
        jObject["status"] = status_array;

        _multiEq->multiEqEffect()->sltSaveInRedo(jObject);
    }
}

MultiEQ* FilterVisualizer::multieq() const
{
    return _multiEq;
}

FiltersModel* FilterVisualizer::filtersModel(int index) const
{
    return m_filtersModel[index];
}

BandModel* FilterVisualizer::bandModel() const
{
    return m_bandModel;
}

int FilterVisualizer::drawLevelMark(QPainter* g, int x, int width, const int level, const QString& label, int lastTextDrawPos)
{
    float yPos = dbToYFloat(level);
    x = x + 1.0f;
    width = width - 2.0f;

    if (yPos - 4 > lastTextDrawPos) {
        g->drawText(QRect(x + 2, yPos - 4, width - 4, 9), Qt::AlignCenter, label);
        return yPos + 5;
    }
    return lastTextDrawPos;
}

int FilterVisualizer::dbToY(const float dB)
{
    int ypos = dbToYFloat(dB);
    return ypos;
}

float FilterVisualizer::dbToYFloat(const float dB)
{

    const float height = static_cast<float>(m_itemHeight) - mB - mT;
    if (height <= 0.0f)
        return 0.0f;
    float temp;
    if (dB < 0.0f)
        temp = zero + std::tanh(dB / dyn * -2.0f);
    else
        temp = zero - 2.0f * dB / dyn;

    return mT + scale * height * temp;
}

float FilterVisualizer::yToDb(const float y)
{
    float height = static_cast<float>(m_itemHeight) - mB - mT;

    float temp = (y - mT) / height / scale - zero;

    float dB;
    if (temp > 0.0f)
        dB = std::atanh(temp) * dyn * -0.5f;
    else
        dB = -0.5f * temp * dyn;
    return std::isnan(dB) ? _settings.dbMin : dB;
}

float FilterVisualizer::yToQ(const float y)
{
    double q;
    if (y > 0) {
        q = (18.0f - 1.4f) / (double)(20.0f - 0.0f) * (y - 0.0f) + 1.4f;
    } else if (y < 0) {
        q = (1.4f - 0.1f) / (double)(0 - (-20.0f)) * (y - (-20.0f)) + 0.1f;
    } else {
        q = 1.4;
    }
    return q;
}

float FilterVisualizer::qToY(const float q)
{
    double y;
    if (q > 1.4) {
        y = (20.0f - 0.0f) / (double)(18.0f - 1.4f) * (q - 1.4f) + 0.0f;
    } else if (y < 1.4) {
        y = (0 - (-20.0f)) / (double)(1.4f - 0.1f) * (q - 0.1f) - 20.0f;
    } else {
        y = 0;
    }
    return y;
}

int FilterVisualizer::hzToX(float hz)
{
    double width = static_cast<float>(m_itemWidth) - mL - mR;
    int xpos = mL + width * (log(hz / _settings.fMin) / log(_settings.fMax / _settings.fMin));

    return xpos;
}

float FilterVisualizer::xToHz(int x)
{
    double width = static_cast<float>(m_itemWidth) - mL - mR;

    double hz = _settings.fMin * pow((_settings.fMax / _settings.fMin), ((x - mL) / width));

    return hz;
}

double FilterVisualizer::gainToDecibels(double gain, double minusInfinityDb)
{
    return gain > double() ? qMax(minusInfinityDb, static_cast<double>(std::log10(gain)) * double(20.0))
                           : minusInfinityDb;
}

double FilterVisualizer::jlimit(double lowerLimit, double upperLimit, double valueToConstrain)
{

    return valueToConstrain < lowerLimit ? lowerLimit
                                         : (upperLimit < valueToConstrain ? upperLimit
                                                                          : valueToConstrain);
}

double FilterVisualizer::decibelsToGain(double decibels, double minusInfinityDb)
{
    return decibels > minusInfinityDb ? std::pow(double(10.0), decibels * double(0.05))
                                      : double();
}

void FilterVisualizer::sltUpdate()
{
    if (m_initialized) {
        bandUpdate();
        update();
    }
}

void FilterVisualizer::sltGainChanged()
{
    Q_EMIT gainChanged();

    if (_multiEq)
        Q_EMIT _multiEq->sigUpdateUI();
    Q_EMIT sigUpdate();
}

void FilterVisualizer::sltQChanged()
{
    Q_EMIT qFactorChanged();

    if (_multiEq)
        Q_EMIT _multiEq->sigUpdateUI();
    Q_EMIT sigUpdate();
}

void FilterVisualizer::sltFreqChanged()
{

    Q_EMIT freqChanged();
    if (_multiEq)
        Q_EMIT _multiEq->sigUpdateUI();
    Q_EMIT sigUpdate();
}

void FilterVisualizer::initialized(int itemWidth, int itemHeight)
{

    m_itemWidth = itemWidth;
    m_itemHeight = itemHeight;
    int xMin = hzToX(_settings.fMin);
    int xMax = hzToX(_settings.fMax);
    numPixels = xMax - xMin + 1;

    frequencies.resize(numPixels);
    for (int i = 0; i < numPixels; ++i)
        frequencies[i] = xToHz(xMin + i);

    allMagnitudesInDb.resize(numPixels);
    magnitudes.resize(numPixels);
    magnitudes.fill(1.0f);
    phases.resize(numPixels);
    complexMagnitudes.resize(numPixels);

    const float width = itemWidth - mL - mR;
    dbGridPath = QPainterPath();

    float dyn = _settings.dbMax - _settings.dbMin;
    int numgridlines = dyn / _settings.gridDiv + 1;

    for (int i = 0; i < numgridlines; ++i) {
        float db_val = _settings.dbMax - i * _settings.gridDiv;

        int ypos = dbToY(db_val);

        dbGridPath.closeSubpath();
        dbGridPath.moveTo(mL - OH, ypos);
        dbGridPath.lineTo(mL + width + OH, ypos);
    }

    hzGridPath = QPainterPath();
    hzGridPathBold = QPainterPath();

    for (float f = _settings.fMin; f <= _settings.fMax; f += powf(10, floor(log10(f)))) {
        int xpos = hzToX(f);

        if ((f == 20) || (f == 50) || (f == 100) || (f == 500) || (f == 1000) || (f == 5000) || (f == 10000) || (f == 20000)) {
            hzGridPathBold.closeSubpath();
            hzGridPathBold.moveTo(xpos, dbToY(_settings.dbMax) - OH);
            hzGridPathBold.lineTo(xpos, dbToY(_settings.dbMin) + OH);

        } else {
            hzGridPathBold.closeSubpath();
            hzGridPath.moveTo(xpos, dbToY(_settings.dbMax) - OH);
            hzGridPath.lineTo(xpos, dbToY(_settings.dbMin) + OH);
        }
    }
    m_initialized = true;
}

void FilterVisualizer::mousePressEvent(int x, int y)
{

    QPoint pos = QPoint(x, y);

    for (int i = 4; --i >= 0;) {

        QSharedPointer<MultiEQ::Parameters> handle = _multiEq->parameters();

        if (handle->filterEnabled[i] == false) {
            continue;
        }

        float gain;
        if ((handle->filterType[i] == MultiEQ::SecondOrderHighPass)
            || (handle->filterType[i] == MultiEQ::SecondOrderLowPass)
            || (handle->filterType[i] == MultiEQ::LinkwitzRileyHighPass)
            || (handle->filterType[i] == MultiEQ::PeakFilter)
            || (handle->filterType[i] == MultiEQ::LinkwitzRileyLowPass)) {
            if (handle->filterQ[i] < 1.16) {
                gain = 20 * std::log10(handle->filterQ[i]);
            } else {
                gain = 20 * (std::log10(handle->filterQ[i]) / std::log10(20));
            }
        } else {
            if (!_settings.gainHandleLin)
                gain = handle->filterGain[i];
            else {
                gain = gainToDecibels(handle->filterGain[i]);
            }
        }

        QPoint filterPos(hzToX(handle->filterFrequency[i]), dbToY(gain));

        if (qSqrt(qPow(filterPos.x() - pos.x(), 2) + qPow(filterPos.y() - pos.y(), 2)) < 10) {
            m_activeIndex = i;
            break;
        }
    }
    if (m_activeIndex != -1)
        setActiveIndex(m_activeIndex);
}

void FilterVisualizer::deviceOnAutomatedEnable()
{
    if (_multiEq)
        _multiEq->deviceOnAutomatedEnabled();
}

void FilterVisualizer::deviceOnEnable(bool enabled)
{
    if (_multiEq)
        _multiEq->deviceOnEnabled(enabled);
}

void FilterVisualizer::mouseMoveEvent(int x, int y)
{

    QPoint pos = QPoint(x, y);

    float frequency = xToHz(pos.x());
    float gain;
    if (!_settings.gainHandleLin)
        gain = yToDb(pos.y());
    else
        gain = decibelsToGain(yToDb(pos.y()));

    if (frequency < 20) {

        frequency = 20;

    } else if (frequency > 22000) {
        frequency = 22000;
    }

    if (gain < -20) {
        gain = -20;

    } else if (gain > 20) {
        gain = 20;
    }

    if (m_activeIndex != -1) {

        QSharedPointer<MultiEQ::Parameters> handle = _multiEq->parameters();

        handle->filterFrequency[m_activeIndex] = (frequency);
        _multiEq->updateParameter(m_activeIndex, "Freq", frequency);

        if ((handle->filterType[m_activeIndex] == MultiEQ::SecondOrderHighPass)
            || (handle->filterType[m_activeIndex] == MultiEQ::SecondOrderLowPass)
            || (handle->filterType[m_activeIndex] == MultiEQ::LinkwitzRileyHighPass)
            || (handle->filterType[m_activeIndex] == MultiEQ::PeakFilter)
            || (handle->filterType[m_activeIndex] == MultiEQ::LinkwitzRileyLowPass)) {
            if (gain <= 0) {
                handle->filterQ[m_activeIndex] = qPow(10, gain / 20.0);
                _multiEq->updateParameter(m_activeIndex, "Q", qPow(10, gain / 20.0));
            }

            if (gain >= 0) {
                handle->filterQ[m_activeIndex] = qPow(20, gain / 20.0);
                _multiEq->updateParameter(m_activeIndex, "Q", qPow(20, gain / 20.0));
            }

        } else {
            handle->filterGain[m_activeIndex] = (gain);
            _multiEq->updateParameter(m_activeIndex, "Gain", gain);
        }

        _multiEq->updateGuiCoefficients();

        auto q = _multiEq->parameters()->filterQ[m_activeIndex];
        auto freq = _multiEq->parameters()->filterFrequency[m_activeIndex];
        auto g = _multiEq->parameters()->filterGain[m_activeIndex];

        m_filtersModel[m_activeIndex]->setValue(0, freq);
        m_filtersModel[m_activeIndex]->setValue(1, g);
        m_filtersModel[m_activeIndex]->setValue(2, q);

        Q_EMIT sigParamersChanged();
    }

    Q_EMIT _multiEq->sigUpdateUI();
    Q_EMIT sigUpdate();
}
