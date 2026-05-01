#include "visreffect.h"

#include <audio/track.h>

#include <QQmlEngine>

Plugin::Descriptor visrPluginDescriptor = {
    "visr",
    "visr",
    "no description",
    Plugin::Effect,
    nullptr,

};
VisrEffect::VisrEffect(AreaInfo* areaInfo, QObject* parent)
    : Effect(&visrPluginDescriptor, nullptr, parent)
    , _areaInfo(areaInfo)
{
    _loadSpeakerConfig = QUrl::fromLocalFile("stereo.xml").toString();

    _reverbFilters = QSharedPointer<ReverbFilters>(new ReverbFilters());
    _reverbFilters->addFilter(3.87648e-01, 0.00000e+00, 0.00000e+00, -1.05734e+00, 5.69314e-01);
}

VisrEffect::~VisrEffect()
{
    qDebug() << "VisrEffect::~VisrEffect()";
    m_plugin.reset();
    Q_EMIT speakerCountChanged();
}

VisrHelper* VisrEffect::getPlugin()
{
    VisrHelper* qobj = m_plugin.data(); // Pointer to member shared-ptr-managed object.
    QQmlEngine::setObjectOwnership(qobj, QQmlEngine::CppOwnership);
    return qobj;
}

QSharedPointer<VisrHelper> VisrEffect::plugin()
{
    return m_plugin;
}

QString VisrEffect::loadSpeakerConfig()
{
    return m_plugin->speakerConfig();
}

void VisrEffect::setLoadSpeakerConfig(QString config)
{
    _loadSpeakerConfig = config;
    m_plugin->loadSpakerConfig(config, _frequencyDependentPanning);
    Q_EMIT loadSpeakerConfigChanged();
}
int VisrEffect::speakerCount() const
{
    return m_plugin->speakerCount();
}

bool VisrEffect::enabled() const
{
    return m_plugin->enabled();
}

void VisrEffect::setEnabled(bool enabled)
{
    m_plugin->setEnabled(enabled);
    Effect::setEnabled(enabled);
    Q_EMIT enabledChanged();
}

bool VisrEffect::frequencyDependentPanning() const
{
    return _frequencyDependentPanning;
}

void VisrEffect::setFrequencyDependentPanning(bool newEnabled)
{
    _frequencyDependentPanning = newEnabled;
    m_plugin->loadSpakerConfig(_loadSpeakerConfig, _frequencyDependentPanning);
}

QString VisrEffect::lateDiffusionFilters() const
{
    return _lateDiffusionFilters;
}

void VisrEffect::setLateDiffusionFilters(QString path)
{
    _lateDiffusionFilters = path;
}

int VisrEffect::numReverbObjects() const
{
    return _numReverbObjects;
}

int VisrEffect::numberOfBiquadsos() const
{
    return m_plugin->numberOfBiquadsos();
}

int VisrEffect::discreteReflectionsPerObject() const
{
    return m_plugin->discreteReflectionsPerObject();
}

double VisrEffect::lateFilterLengthSeconds() const
{
    return _lateFilterLengthSeconds;
}

void VisrEffect::setLateFilterLengthSeconds(double seconds)
{
    _lateFilterLengthSeconds = seconds;
}

double VisrEffect::maximumDiscreteReflectionDelay() const
{
    return _maximumDiscreteReflectionDelay;
}

void VisrEffect::setMaximumDiscreteReflectionDelay(double seconds)
{
    _maximumDiscreteReflectionDelay = seconds;
}

bool VisrEffect::reverbEnabled() const
{
    return _reverbEnabled;
}

void VisrEffect::setReverbEnabled(bool enabled)
{
    _reverbEnabled = enabled;
    Q_EMIT reverbEnabledChanged();
}

ReverbFilters* VisrEffect::reverbFilters()
{
    ReverbFilters* qobj = _reverbFilters.data(); // Pointer to member shared-ptr-managed object.
    QQmlEngine::setObjectOwnership(qobj, QQmlEngine::CppOwnership);
    return qobj;
}

QStringList VisrEffect::lateReverbLevels()
{
    return _lateReverbLevels;
}

QStringList VisrEffect::lateReverbAttackTime()
{
    return _lateReverbAttackTime;
}

QStringList VisrEffect::lateReverbDecayconst()
{

    return _lateReverbDecayconst;
}

double VisrEffect::lateReverbDelay() const
{
    return m_plugin->getOnsetDelay();
}

void VisrEffect::setLateReverbLevels(int index, QString value)
{
    _lateReverbLevels[index] = value;
    QVector<float> list;
    for (int i = 0; i < _lateReverbLevels.size(); i++) {
        list.append(_lateReverbLevels[i].toFloat());
    }

    m_plugin->setDecayCoeffs(list);
}

void VisrEffect::setLateReverbAttackTime(int index, QString value)
{
    _lateReverbAttackTime[index] = value;
    QVector<float> list;
    for (int i = 0; i < _lateReverbAttackTime.size(); i++) {
        list.append(_lateReverbAttackTime[i].toFloat());
    }

    m_plugin->setAttackTimes(list);
}

void VisrEffect::setLateReverbDecayconst(int index, QString value)
{
    _lateReverbDecayconst[index] = value;
    QVector<float> list;
    for (int i = 0; i < _lateReverbDecayconst.size(); i++) {
        list.append(_lateReverbDecayconst[i].toFloat());
    }

    m_plugin->setLateReverbLevels(list);
}

void VisrEffect::setLateReverbDelay(float value)
{
    m_plugin->setOnsetDelay(value);
}

void VisrEffect::setReverbFilterB0(int index, QString value)
{
    float b0, b1, b2, a1, a2;
    _reverbFilters->getFilter(index, b0, b1, b2, a1, a2);

    m_plugin->setDiscreteReflectionFilter(index, 0, value.toFloat(), b1, b2, a1, a2);
}

void VisrEffect::setReverbFilterB1(int index, QString value)
{
    float b0, b1, b2, a1, a2;
    _reverbFilters->getFilter(index, b0, b1, b2, a1, a2);
    m_plugin->setDiscreteReflectionFilter(index, 0, b0, value.toFloat(), b2, a1, a2);
}

void VisrEffect::setReverbFilterB2(int index, QString value)
{
    float b0, b1, b2, a1, a2;
    _reverbFilters->getFilter(index, b0, b1, b2, a1, a2);
    m_plugin->setDiscreteReflectionFilter(index, 0, b0, b1, value.toFloat(), a1, a2);
}

void VisrEffect::setReverbFilterA1(int index, QString value)
{
    float b0, b1, b2, a1, a2;
    _reverbFilters->getFilter(index, b0, b1, b2, a1, a2);

    m_plugin->setDiscreteReflectionFilter(index, 0, b0, b1, b2, value.toFloat(), a2);
}

void VisrEffect::setReverbFilterA2(int index, QString value)
{

    float b0, b1, b2, a1, a2;
    _reverbFilters->getFilter(index, b0, b1, b2, a1, a2);

    m_plugin->setDiscreteReflectionFilter(index, 0, b0, b1, b2, a1, value.toFloat());
}

void VisrEffect::applyReverb(int index)
{
    float b0, b1, b2, a1, a2;
    m_plugin->getDiscreteReflectionFilter(index, 0, b0, b1, b2, a1, a2);
    _reverbFilters->setFilter(index, b0, b1, b2, a1, a2);
}

void VisrEffect::setDiscreteReflectionPositionX(int index, QString value)
{
    float x, y, z;
    m_plugin->getDiscreteReflectionPosition(index, x, y, z);
    m_plugin->setDiscreteReflectionPosition(index, value.toFloat(), y, z);
}

void VisrEffect::setDiscreteReflectionPositionY(int index, QString value)
{
    float x, y, z;
    m_plugin->getDiscreteReflectionPosition(index, x, y, z);
    m_plugin->setDiscreteReflectionPosition(index, x, value.toFloat(), z);
}

void VisrEffect::setDiscreteReflectionPositionZ(int index, QString value)
{
    float x, y, z;
    m_plugin->getDiscreteReflectionPosition(index, x, y, z);
    m_plugin->setDiscreteReflectionPosition(index, x, y, value.toFloat());
}

float VisrEffect::getDiscreteReflectionPositionX(int index)
{
    float x, y, z;
    m_plugin->getDiscreteReflectionPosition(index, x, y, z);
    return x;
}

float VisrEffect::getDiscreteReflectionPositionY(int index)
{
    float x, y, z;
    m_plugin->getDiscreteReflectionPosition(index, x, y, z);
    return y;
}

float VisrEffect::getDiscreteReflectionPositionZ(int index)
{
    float x, y, z;
    m_plugin->getDiscreteReflectionPosition(index, x, y, z);
    return z;
}

float VisrEffect::getDiscreteReflectionDelay(int index)
{

    return m_plugin->getDiscreteReflectionDelay(index);
}

float VisrEffect::getDiscreteReflectionLevel(int index)
{
    return m_plugin->getDiscreteReflectionLevel(index);
}

void VisrEffect::setDiscreteReflectionDelay(int index, QString value)
{
    m_plugin->setDiscreteReflectionDelay(index, value.toFloat());
}

void VisrEffect::setDiscreteReflectionLevel(int index, QString value)
{
    m_plugin->setDiscreteReflectionLevel(index, value.toFloat());
}

QMap<QString, QSharedPointer<ObjectAutomation> > VisrEffect::automations() const
{
    return _automations;
}

SpeakerModel* VisrEffect::speakerModel() const
{
    auto qobj = m_plugin->speakerModel().data(); // Pointer to member shared-ptr-managed object.
    QQmlEngine::setObjectOwnership(qobj, QQmlEngine::CppOwnership);
    return qobj;
}

float VisrEffect::getValue(QString title)
{
    if (_controls->mapTitles().contains(title)) {
        int index = _controls->mapTitles()[title];
        return _controls->value(index).toFloat();
    }

    return 0;
}

void VisrEffect::setValue(QString shorName, float value)
{
    if (_controls->mapTitles().contains(shorName)) {
        int id = _controls->mapTitles()[shorName];

        double x, y, z;
        if (shorName == "PosX") {

            auto normalized = _automationItems[shorName]->value2Normalized(value);
            _controls->updateValue(id, QString::number(value), normalized);

            _automationItems[shorName]->automation()->setEnabled1(false);
            _automationItems[shorName]->automation()->setNormalized(normalized);

            x = value;
            int _index = _controls->mapTitles()["PosY"];
            y = _controls->value(_index).toDouble();
            _index = _controls->mapTitles()["PosZ"];
            z = _controls->value(_index).toDouble();
            m_plugin->setMovePosition(x, y, z);



            Q_EMIT positionChanged();
            Q_EMIT sigUpdate();
        }
        if (shorName == "PosY") {

            auto normalized = _automationItems[shorName]->value2Normalized(value);
            _controls->updateValue(id, QString::number(value), normalized);

            _automationItems[shorName]->automation()->setEnabled1(false);
            _automationItems[shorName]->automation()->setNormalized(normalized);


            y = value;
            int _index = _controls->mapTitles()["PosX"];
            x = _controls->value(_index).toDouble();
            _index = _controls->mapTitles()["PosZ"];
            z = _controls->value(_index).toDouble();
            m_plugin->setMovePosition(x, y, z);
            Q_EMIT positionChanged();
            Q_EMIT sigUpdate();
        }
        if (shorName == "PosZ") {

            auto normalized = _automationItems[shorName]->value2Normalized(value);
            _controls->updateValue(id, QString::number(value), normalized);

            _automationItems[shorName]->automation()->setEnabled1(false);
            _automationItems[shorName]->automation()->setNormalized(normalized);


            z = value;
            int _index = _controls->mapTitles()["PosY"];
            y = _controls->value(_index).toDouble();
            _index = _controls->mapTitles()["PosX"];
            x = _controls->value(_index).toDouble();
            m_plugin->setMovePosition(x, y, z);
            Q_EMIT positionChanged();
            Q_EMIT sigUpdate();
        }
    }
}

void VisrEffect::reload()
{
    if (_reverbEnabled) {

        m_plugin->loadSpakerConfigWithReverb(_loadSpeakerConfig, _numReverbObjects, 1,
            _lateFilterLengthSeconds, _lateDiffusionFilters,
            _maximumDiscreteReflectionDelay, _frequencyDependentPanning);
        m_plugin->setDiscreteReflectionFilter(0, 0, 3.87648e-01, 0.00000e+00, 0.00000e+00, -1.05734e+00, 5.69314e-01);

        m_plugin->setDiscreteReflectionLevel(0, 0);
        m_plugin->setDiscreteReflectionDelay(0, 0);
        m_plugin->setDiscreteReflectionPosition(0, 0, 0, 0);

        _lateReverbLevels.clear();
        auto levels = m_plugin->getLateReverbLevels();
        for (int i = 0; i < m_plugin->getLateReverbLevels().size(); i++) {
            _lateReverbLevels.append(QString::number(levels[i]));
        }

        _lateReverbAttackTime.clear();
        auto attackTimes = m_plugin->getAttackTimes();
        for (int i = 0; i < m_plugin->getAttackTimes().size(); i++) {
            _lateReverbAttackTime.append(QString::number(attackTimes[i]));
        }

        _lateReverbDecayconst.clear();
        auto DecayCoeffs = m_plugin->getDecayCoeffs();
        for (int i = 0; i < m_plugin->getDecayCoeffs().size(); i++) {
            _lateReverbDecayconst.append(QString::number(DecayCoeffs[i]));
        }

    } else {
        m_plugin->loadSpakerConfig(_loadSpeakerConfig, _frequencyDependentPanning);
    }
}
void VisrEffect::sltControlVisibleChanged(bool visible)
{
    qDebug() << "sltControlVisibleChanged" << visible;
}

void VisrEffect::sltAutomatedValueChanged(QString shorName)
{

    if (_controls->mapTitles().contains(shorName)) {
        int id = _controls->mapTitles()[shorName];

        double x, y, z;
        if (shorName == "PosX") {

            auto normalized = _automations[shorName]->automatedNormalized();
            auto value = _automationItems[shorName]->normalized2Value(normalized);
            _controls->updateValue(id, QString::number(value), normalized);

            x = value;
            int _index = _controls->mapTitles()["PosY"];
            y = _controls->value(_index).toDouble();
            _index = _controls->mapTitles()["PosZ"];
            z = _controls->value(_index).toDouble();
            m_plugin->setMovePosition(x, y, z);
            Q_EMIT positionChanged();
        }
        if (shorName == "PosY") {

            auto normalized = _automations[shorName]->automatedNormalized();
            auto value = _automationItems[shorName]->normalized2Value(normalized);
            _controls->updateValue(id, QString::number(value), normalized);

            y = value;
            int _index = _controls->mapTitles()["PosX"];
            x = _controls->value(_index).toDouble();
            _index = _controls->mapTitles()["PosZ"];
            z = _controls->value(_index).toDouble();
            m_plugin->setMovePosition(x, y, z);
            Q_EMIT positionChanged();
        }
        if (shorName == "PosZ") {

            auto normalized = _automations[shorName]->automatedNormalized();
            auto value = _automationItems[shorName]->normalized2Value(normalized);
            _controls->updateValue(id, QString::number(value), normalized);

            z = value;
            int _index = _controls->mapTitles()["PosY"];
            y = _controls->value(_index).toDouble();
            _index = _controls->mapTitles()["PosX"];
            x = _controls->value(_index).toDouble();
            m_plugin->setMovePosition(x, y, z);
            Q_EMIT positionChanged();
        }
    }
}

void VisrEffect::sltValueChanged(QString shorName)
{

    if (_controls->mapTitles().contains(shorName)) {
        int id = _controls->mapTitles()[shorName];

        double x, y, z;
        if (shorName == "PosX") {
            auto normalized = _automations[shorName]->normalized();
            auto value = _automationItems[shorName]->normalized2Value(normalized);
            _controls->updateValue(id, QString::number(value), normalized);

            x = value;
            int _index = _controls->mapTitles()["PosY"];
            y = _controls->value(_index).toDouble();
            _index = _controls->mapTitles()["PosZ"];
            z = _controls->value(_index).toDouble();
            m_plugin->setMovePosition(x, y, z);
            Q_EMIT positionChanged();
        }
        if (shorName == "PosY") {
            auto normalized = _automations[shorName]->normalized();
            auto value = _automationItems[shorName]->normalized2Value(normalized);
            _controls->updateValue(id, QString::number(value), normalized);

            y = value;
            int _index = _controls->mapTitles()["PosX"];
            x = _controls->value(_index).toDouble();
            _index = _controls->mapTitles()["PosZ"];
            z = _controls->value(_index).toDouble();
            m_plugin->setMovePosition(x, y, z);
            Q_EMIT positionChanged();
        }
        if (shorName == "PosZ") {
            auto normalized = _automations[shorName]->normalized();
            auto value = _automationItems[shorName]->normalized2Value(normalized);
            _controls->updateValue(id, QString::number(value), normalized);

            z = value;
            int _index = _controls->mapTitles()["PosY"];
            y = _controls->value(_index).toDouble();
            _index = _controls->mapTitles()["PosX"];
            x = _controls->value(_index).toDouble();
            m_plugin->setMovePosition(x, y, z);
            Q_EMIT positionChanged();
        }
    }
}

void VisrEffect::sltUpdateParams(int index, QString value, double normalized)
{
    _controls->updateValue(index, value, normalized);
}

void VisrEffect::sltDataChagedFromModel(int index, QString type, double value)
{

    if (m_plugin) {

        QString shorName = type + QString::number(index);

        double normalized = _automationItems[shorName]->value2Normalized(value);

        _controls->updateValue(index, QString::number(value), normalized);

        _automationItems[shorName]->automation()->setEnabled1(false);
        _automationItems[shorName]->automation()->setNormalized(normalized);

        Q_EMIT sigUpdate();
    }
}
void VisrEffect::sltDataChagedFromView(int index, QString type, double value)
{

    if (m_plugin) {

        QString _title = type + QString::number(index);

        double normalized = _automationItems[_title]->value2Normalized(value);

        _controls->updateValue(index, QString::number(value), normalized);

        _automationItems[_title]->automation()->setEnabled1(false);
        _automationItems[_title]->setNormalized(1 - normalized);

        Q_EMIT sigUpdate();
    }
}

QString VisrEffect::name() const
{
    return "Visr";
}
QString VisrEffect::type() const
{
    return "Visr";
}
QSharedPointer<EffectControls> VisrEffect::controls() const
{
    return _controls;
}

VisrControls* VisrEffect::getControls() const
{
    auto qobj = _controls.data(); // Pointer to member shared-ptr-managed object.
    QQmlEngine::setObjectOwnership(qobj, QQmlEngine::CppOwnership);
    return qobj;
}

bool VisrEffect::initialize(int trackIndex, QSharedPointer<Effect> effect)
{

    m_plugin = QSharedPointer<VisrHelper>(new VisrHelper(effect));

    _controls = QSharedPointer<VisrControls>(new VisrControls(name(), m_plugin->effectParameters(), effect));
    connect(m_plugin.data(), &VisrHelper::speakerCountChanged, this, &VisrEffect::speakerCountChanged);

    // connect(m_plugin.data(), &MultiEQ::sigUpdateParams, this, &VisrEffect::sltDataChagedFromView, Qt::QueuedConnection);

    connect(_controls.data(), &VisrControls::sigDataChaged, this, &VisrEffect::sltDataChagedFromModel);

    for (int i = 0; i < m_plugin->effectParameters().size(); i++) {
        auto parameter = m_plugin->effectParameters();

        _automations[parameter[i]._title] = QSharedPointer<ObjectAutomation>(new ObjectAutomation(trackIndex, _controls->name(), parameter[i]._title, _areaInfo));
        _automationItems[parameter[i]._title] = QSharedPointer<ObjectAutomationItem>(new ObjectAutomationItem(trackIndex, 1 - parameter[i]._defaultNormalizedValue,
            parameter[i]._title, m_plugin, _automations[parameter[i]._title], _areaInfo));

        connect(_automations[parameter[i]._title].data(), &Automation::sigUpdate, this, &VisrEffect::sigUpdate);
        connect(_automationItems[parameter[i]._title].data(), &ObjectAutomationItem::sigUpdate, this, &VisrEffect::sigUpdate);

        connect(_automationItems[parameter[i]._title].data(), &ObjectAutomationItem::sigCursorPosition, this, &VisrEffect::sigCursorPosition);
        connect(_automationItems[parameter[i]._title].data(), &ObjectAutomationItem::sigSetCursor, this, &VisrEffect::sigSetCursor);

        connect(_automationItems[parameter[i]._title].data(), &ObjectAutomationItem::sigVisrAutomatedNormalizedChanged,
            this, &VisrEffect::sltAutomatedValueChanged);

        connect(_automationItems[parameter[i]._title].data(), &ObjectAutomationItem::sigVisrNormalizedChanged,
            this, &VisrEffect::sltValueChanged);
    }

    return true;
}

bool VisrEffect::processAudioBuffer(sampleFrame* _buf)
{

    return true;
}

bool VisrEffect::controlVisible()
{
    return false;
}

QSharedPointer<Automation> VisrEffect::automation(QString name)
{
    if (_automations.contains(name))
        return _automations[name];
    return Q_NULLPTR;
}
QSharedPointer<AutomationItem> VisrEffect::automationItem(QString name)
{
    if (_automationItems.contains(name))
        return _automationItems[name];
    return Q_NULLPTR;
}

bool VisrEffect::contains(QString name)
{
    return _controls->shortTitleList().contains(name);
}
bool VisrEffect::isAutomated(QString name)
{
    return false;
}
bool VisrEffect::hasExternalControl()
{
    return true;
}

VisrControls::VisrControls(QString name, QVector<VisrHelper::EffectParameter> list, QSharedPointer<Effect> _eff, QObject* parent)
    : EffectControls(_eff, parent)
{
    m_roleNames[ROLE_Name] = "Name";
    m_roleNames[ROLE_Index] = "Index";
    m_roleNames[ROLE_Normalized] = "Normalized";
    m_roleNames[ROLE_Value] = "Value";
    m_roleNames[ROLE_StepSize] = "StepSize";
    m_roleNames[ROLE_DefaultNormalized] = "DefaultNormalized";

    for (int i = 0; i < list.size(); i++) {

        _list.append(list[i]);
        _mapTitles[list[i]._title] = list[i]._id;
        _mapIndexes[list[i]._id] = i;
    }
}

int VisrControls::controlCount()
{
    return _list.size();
}

int VisrControls::getIndex(int i)
{
    return _list[i]._index;
}

QString VisrControls::value(int i)
{
    if (i >= 0 && i < _list.size()) {
        return _list[i]._value;
    }
}
void VisrControls::updateValue(int i, QString value, double normalized)
{

    if (i >= 0 && i < _list.size()) {
        _list[i]._active = false;
        _list[i]._value = value;
        _list[i]._normalized = normalized;

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Value);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Normalized);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Active);
    }
}

void VisrControls::updateValueFromAutomation(int i, QString value, double normalized)
{

    if (i >= 0 && i < _list.size()) {
        _list[i]._normalized = normalized;
        _list[i]._active = true;
        _list[i]._value = value;

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Value);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Normalized);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Active);
    }
}

QMap<QString, uint> VisrControls::mapTitles() const
{
    return _mapTitles;
}
QMap<uint, int> VisrControls::mapIndexes() const
{
    return _mapIndexes;
}

QString VisrControls::name()
{
    return "Visr";
}

QStringList VisrControls::shortTitleList()
{
    QStringList list;
    for (int i = 0; i < _list.size(); i++) {
        list.append(_list[i]._title);
    }
    return list;
}

int VisrControls::rowCount(const QModelIndex& parent) const
{
    return _list.size();
}

bool VisrControls::setData(const QModelIndex& index, const QVariant& value, int role)
{

    if (_list.size() == 0 || index.row() < 0 || index.row() > _list.size())
        return false;

    if (index.isValid() && role == ROLE_Normalized) {

        _list[index.row()]._normalized = value.toDouble();

        _list[index.row()]._active = false;

        Q_EMIT sigDataChaged(_list[index.row()]._index, _list[index.row()]._name, _list[index.row()]._normalized);
        Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_Normalized);
        Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_Active);
        return true;
    }

    if (index.isValid() && role == ROLE_Active) {

        _list[index.row()]._active = value.toBool();

        Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_Active);
        return true;
    }

    return false;
}

QVariant VisrControls::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (_list.size() == 0 || index.row() < 0)
        return QVariant();

    VisrHelper::EffectParameter value = _list[index.row()];

    switch (role) {
    case ROLE_Index:
        return value._id;
    case ROLE_Name:
        return value._title;
    case ROLE_Normalized:
        return value._normalized;
    case ROLE_Value:
        return value._value;
    case ROLE_StepSize:
        return value._stepCount;
    case ROLE_Active:
        return value._active;
    case ROLE_DefaultNormalized:
        return value._defaultNormalizedValue;
    }
    return QVariant();
}
