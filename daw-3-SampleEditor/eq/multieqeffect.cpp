#include "multieqeffect.h"

Plugin::Descriptor eqPluginDescriptor = {
    "eq",
    "eq",
    "no description",
    Plugin::Effect,
    nullptr,

};
MultiEqEffect::MultiEqEffect(int trackIndex, AreaInfo* areaInfo, QObject* parent)
    : Effect(&eqPluginDescriptor, nullptr, parent)
    , _areaInfo(areaInfo)
    , _trackIndex(trackIndex)
{
}

MultiEQ* MultiEqEffect::getPlugin()
{
    MultiEQ* qobj = m_plugin.data(); // Pointer to member shared-ptr-managed object.
    QQmlEngine::setObjectOwnership(qobj, QQmlEngine::CppOwnership);
    return qobj;
}

void MultiEqEffect::sltControlVisibleChanged(bool visible)
{
    qDebug() << "sltControlVisibleChanged" << visible;
}

void MultiEqEffect::sltLinesChanged(QString shorName)
{
    uint index = _controls->mapTitles()[shorName];

    if (!_automations[shorName]->isAutomated()) {
        m_plugin->parameterActive(index, false);
    } else {
        m_plugin->parameterActive(index, true);
    }
}

void MultiEqEffect::sltEnabledChanged(QString shorName, bool enabled)
{
    uint index = _controls->mapTitles()[shorName];
    m_plugin->parameterAutomated(index, enabled);
}

void MultiEqEffect::sltDeviceOnLinesChanged()
{
    if (!_deviceOnAutomation->isAutomated()) {
        m_plugin->deviceOnActive(false);
    } else {
        m_plugin->deviceOnActive(true);
    }
}

void MultiEqEffect::sltDeviceOnEnabledChanged()
{

    m_plugin->deviceOnAutomated(_deviceOnAutomation->enabled());

    auto value = _speakerOnAutomationItem->value();
    m_plugin->deviceOnValue(value == 1);

    if (_speakerOnAutomationItem->automation()->isAutomated()) {
        if (value) {
            setDontRun(false);
        } else {
            setDontRun(true);
        }
    } else {
        setDontRun(false);
    }

    Q_EMIT sigUpdate();
}

void MultiEqEffect::sltDeviceOnAutomatedValueChanged()
{

    auto value = _speakerOnAutomationItem->value();

    m_plugin->deviceOnValue(value == 1);
}

void MultiEqEffect::sltDeviceOnValueChanged()
{

    auto value = _speakerOnAutomationItem->value();

    if (_speakerOnAutomationItem->automation()->isAutomated()) {
        if (value) {
            setDontRun(false);
        } else {
            setDontRun(true);
        }
    } else {
        setDontRun(false);
    }

    m_plugin->deviceOnValue(value == 1);
}

void MultiEqEffect::deviceOnNormalizedChanged()
{
    auto value = _speakerOnAutomationItem->value();

    if (_speakerOnAutomationItem->automation()->isAutomated()) {
        if (value) {
            setDontRun(false);
        } else {
            setDontRun(true);
        }
    } else {
        setDontRun(false);
    }
}

void MultiEqEffect::sltDeviceOnEnabled(bool enabled)
{

    if (!_speakerOnAutomationItem->automation()->isAutomated()) {
        _speakerOnAutomationItem->setNormalized(1 - _deviceOnAutomation->normalized());
    } else {
        if (_deviceOnAutomation->enabled()) {
            _speakerOnAutomationItem->setEnabled(false);
            _speakerOnAutomationItem->setNormalized(1 - _speakerOnAutomationItem->automation()->automatedNormalized());
        } else {
            _speakerOnAutomationItem->setNormalized(1 - _deviceOnAutomation->normalized());
        }
    }

    auto value = _speakerOnAutomationItem->value();

    // m_plugin->deviceOnValue(value == 1);

    if (_speakerOnAutomationItem->automation()->isAutomated()) {
        if (value) {
            setDontRun(false);
        } else {
            setDontRun(true);
        }
    } else {
        setDontRun(false);
    }
    Q_EMIT sigUpdate();
}

QJsonObject MultiEqEffect::performOperation(QJsonObject obj)
{

    if (obj.contains("type") && obj.contains("effect")) {
        if (obj["type"] == "effect" && obj["effect"] == "eq") {
            if (obj["operation"] == ActionStack::Operation_EffectStatus) {

                if (obj["sub_action"] == 0) {
                    m_plugin->updateStatus(obj);
                } else {

                    _status["track_index"] = _trackIndex;
                    _status["operation"] = ActionStack::Operation_EffectStatus;
                    _status["sub_action"] = 1;
                    _status["effect"] = "eq";
                    _status["type"] = "effect";

                    _status["leftWetDryEnabled"] = leftWetDryEnabled();
                    _status["rightWetDryEnabled"] = rightWetDryEnabled();

                    _status["leftWetLevel"] = leftWetLevel();
                    _status["rightWetLevel"] = rightWetLevel();

                    setLeftWetDryEnabled(obj["leftWetDryEnabled"].toBool());
                    setRightWetDryEnabled(obj["rightWetDryEnabled"].toBool());
                    setRightWetLevel(obj["rightWetLevel"].toDouble());
                    setLeftWetLevel(obj["leftWetLevel"].toDouble());
                }

                return _status;
            }
        }
    }
    return QJsonObject();
}

QJsonObject MultiEqEffect::getLastStatus()
{
    QJsonObject status;
    status["plugin_type"] = "EQ";
    status["leftWetDryEnabled"] = leftWetDryEnabled();
    status["rightWetDryEnabled"] = rightWetDryEnabled();
    status["leftWetLevel"] = leftWetLevel();
    status["rightWetLevel"] = rightWetLevel();
    status["active_band"] = m_plugin->activeIndex();

    QJsonArray status_array;
    for (int i = 0; i < 4; i++) {
        QJsonObject object;
        object["band_number"] = i;

        object["type"] = m_plugin->parameters()->filterType[i];
        object["enabled"] = m_plugin->parameters()->filterEnabled[i];
        object["freq"] = m_plugin->parameters()->filterFrequency[i];
        object["gain"] = m_plugin->parameters()->filterGain[i];
        object["q"] = m_plugin->parameters()->filterQ[i];

        status_array.append(object);
    }
    status["status"] = status_array;

    return status;
}

void MultiEqEffect::setLastStatus(QJsonObject status)
{

    setLeftWetDryEnabled(status["leftWetDryEnabled"].toBool());
    setRightWetDryEnabled(status["rightWetDryEnabled"].toBool());
    setLeftWetLevel(status["leftWetLevel"].toDouble());
    setRightWetLevel(status["rightWetLevel"].toDouble());

    m_plugin->updateStatus(status);
}

void MultiEqEffect::saveRedoOperation()
{
    m_plugin->saveRedoOperation();
}
void MultiEqEffect::saveUndoOperation()
{
    m_plugin->saveUndoOperation();
}

void MultiEqEffect::sltSaveInRedo(QJsonObject status)
{

    //    status["track_index"] = _trackIndex;
    //    status["operation"] = ActionStack::Operation_EffectStatus;
    //    status["effect"] = "eq";
    //    status["type"] = "effect";

    //    status["leftWetDryEnabled"] = leftWetDryEnabled();
    //    status["rightWetDryEnabled"] = rightWetDryEnabled();

    //    status["leftWetLevel"] = leftWetLevel();
    //    status["rightWetLevel"] = rightWetLevel();

    //    QJsonArray jArray;
    //    jArray.append(status);

    //    Q_EMIT sigSaveInRedoItems(jArray);
}

void MultiEqEffect::sltUpdateState(QJsonObject status, int op)
{

    status["track_index"] = _trackIndex;
    status["operation"] = ActionStack::Operation_EffectStatus;
    status["sub_action"] = op;
    status["effect"] = "eq";
    status["type"] = "effect";

    _status = status;
}

void MultiEqEffect::sltSaveInUndo(QJsonObject status, int op)
{

    status["track_index"] = _trackIndex;
    status["operation"] = ActionStack::Operation_EffectStatus;

    status["sub_action"] = op;

    status["effect"] = "eq";
    status["type"] = "effect";

    status["leftWetDryEnabled"] = leftWetDryEnabled();
    status["rightWetDryEnabled"] = rightWetDryEnabled();

    status["leftWetLevel"] = leftWetLevel();
    status["rightWetLevel"] = rightWetLevel();

    QJsonArray jArray;
    jArray.append(status);

    Q_EMIT sigSaveInUndoItems(jArray);
}

void MultiEqEffect::sltDeviceOnAutomatedEnabled()
{
    _speakerOnAutomationItem->setEnabled(true);
}

void MultiEqEffect::sltAutomatedValueChanged(QString shorName)
{

    auto normalized = _automations[shorName]->automatedNormalized();
    auto value = (float)_automationItems[shorName]->normalized2Value(normalized);

    bool changed = false;
    if (shorName == "Freq0") {
        if (isDifferent(m_plugin->parameters()->filterFrequency[0], value)) {
            changed = true;
            m_plugin->parameters()->filterFrequency[0] = value;
        }
    }
    if (shorName == "Freq1") {
        if (isDifferent(m_plugin->parameters()->filterFrequency[1], value)) {
            changed = true;
            m_plugin->parameters()->filterFrequency[1] = value;
        }
    }
    if (shorName == "Freq2") {
        if (isDifferent(m_plugin->parameters()->filterFrequency[2], value)) {
            changed = true;
            m_plugin->parameters()->filterFrequency[2] = value;
        }
    }
    if (shorName == "Freq3") {
        if (isDifferent(m_plugin->parameters()->filterFrequency[3], value)) {
            changed = true;
            m_plugin->parameters()->filterFrequency[3] = value;
        }
    }
    if (shorName == "Gain0") {
        if (isDifferent(m_plugin->parameters()->filterGain[0], value)) {
            changed = true;
            m_plugin->parameters()->filterGain[0] = value;
        }
    }
    if (shorName == "Gain1") {
        if (isDifferent(m_plugin->parameters()->filterGain[1], value)) {
            changed = true;
            m_plugin->parameters()->filterGain[1] = value;
        }
    }
    if (shorName == "Gain2") {
        if (isDifferent(m_plugin->parameters()->filterGain[2], value)) {
            changed = true;
            m_plugin->parameters()->filterGain[2] = value;
        }
    }
    if (shorName == "Gain3") {
        if (isDifferent(m_plugin->parameters()->filterGain[3], value)) {
            changed = true;
            m_plugin->parameters()->filterGain[3] = value;
        }
    }
    if (shorName == "Q0") {
        if (isDifferent(m_plugin->parameters()->filterQ[0], value)) {
            changed = true;
            m_plugin->parameters()->filterQ[0] = value;
        }
    }
    if (shorName == "Q1") {
        if (isDifferent(m_plugin->parameters()->filterQ[1], value)) {
            changed = true;
            m_plugin->parameters()->filterQ[1] = value;
        }
    }
    if (shorName == "Q2") {
        if (isDifferent(m_plugin->parameters()->filterQ[2], value)) {
            changed = true;
            m_plugin->parameters()->filterQ[2] = value;
        }
    }
    if (shorName == "Q3") {
        if (isDifferent(m_plugin->parameters()->filterQ[3], value)) {
            changed = true;
            m_plugin->parameters()->filterQ[3] = value;
        }
    }

    if (changed) {
        m_plugin->updateGuiCoefficients();

        uint index = _controls->mapTitles()[shorName];

        //    //    qDebug() << "shrt" << index << value;
        m_plugin->parameterAutomated(index, true);
        m_plugin->parameterUpdated(index);
    }
}

void MultiEqEffect::sltValueChanged(QString shorName)
{

    auto normalized = _automations[shorName]->normalized();
    auto value = (double)_automationItems[shorName]->normalized2Value(normalized);

    bool changed = false;
    if (shorName == "Freq0") {
        if (isDifferent(m_plugin->parameters()->filterFrequency[0], value)) {
            changed = true;
            m_plugin->parameters()->filterFrequency[0] = value;
        }
    }
    if (shorName == "Freq1") {
        if (isDifferent(m_plugin->parameters()->filterFrequency[1], value)) {

            changed = true;
            m_plugin->parameters()->filterFrequency[1] = value;
        }
    }
    if (shorName == "Freq2") {
        if (isDifferent(m_plugin->parameters()->filterFrequency[2], value)) {

            changed = true;
            m_plugin->parameters()->filterFrequency[2] = value;
        }
    }
    if (shorName == "Freq3") {
        if (isDifferent(m_plugin->parameters()->filterFrequency[3], value)) {

            changed = true;
            m_plugin->parameters()->filterFrequency[3] = value;
        }
    }
    if (shorName == "Gain0") {
        if (isDifferent(m_plugin->parameters()->filterGain[0], value)) {

            changed = true;
            m_plugin->parameters()->filterGain[0] = value;
        }
    }
    if (shorName == "Gain1") {
        if (isDifferent(m_plugin->parameters()->filterGain[1], value)) {

            changed = true;
            m_plugin->parameters()->filterGain[1] = value;
        }
    }
    if (shorName == "Gain2") {
        if (isDifferent(m_plugin->parameters()->filterGain[2], value)) {

            changed = true;
            m_plugin->parameters()->filterGain[2] = value;
        }
    }
    if (shorName == "Gain3") {
        if (isDifferent(m_plugin->parameters()->filterGain[3], value)) {

            changed = true;
            m_plugin->parameters()->filterGain[3] = value;
        }
    }
    if (shorName == "Q0") {
        if (isDifferent(m_plugin->parameters()->filterQ[0], value)) {

            changed = true;
            m_plugin->parameters()->filterQ[0] = value;
        }
    }
    if (shorName == "Q1") {
        if (isDifferent(m_plugin->parameters()->filterQ[1], value)) {

            changed = true;
            m_plugin->parameters()->filterQ[1] = value;
        }
    }
    if (shorName == "Q2") {
        if (isDifferent(m_plugin->parameters()->filterQ[2], value)) {

            changed = true;
            m_plugin->parameters()->filterQ[2] = value;
        }
    }
    if (shorName == "Q3") {
        if (isDifferent(m_plugin->parameters()->filterQ[3], value)) {

            changed = true;
            m_plugin->parameters()->filterQ[3] = value;
        }
    }
    if (changed) {

        m_plugin->updateGuiCoefficients();
        m_plugin->parameterChanged();

        uint index = _controls->mapTitles()[shorName];

        m_plugin->parameterAutomated(index, false);
        m_plugin->parameterUpdated(index);
    }
}

void MultiEqEffect::sltDataChagedFromModel(int index, QString type, double value)
{

    if (m_plugin) {

        QString shorName = type + QString::number(index);

        int index = _controls->mapTitles()[shorName];
        double normalized = (double)_automationItems[shorName]->value2Normalized(value);

        _controls->updateValue(index, QString::number(value), normalized);

        bool changed = false;
        if (shorName == "Freq0") {
            if (isDifferent(m_plugin->parameters()->filterFrequency[0], value)) {
                changed = true;
                m_plugin->parameters()->filterFrequency[0] = value;
            }
        }
        if (shorName == "Freq1") {
            if (isDifferent(m_plugin->parameters()->filterFrequency[1], value)) {
                changed = true;
                m_plugin->parameters()->filterFrequency[1] = value;
            }
        }
        if (shorName == "Freq2") {
            if (isDifferent(m_plugin->parameters()->filterFrequency[2], value)) {
                changed = true;
                m_plugin->parameters()->filterFrequency[2] = value;
            }
        }
        if (shorName == "Freq3") {
            if (isDifferent(m_plugin->parameters()->filterFrequency[3], value)) {
                changed = true;
                m_plugin->parameters()->filterFrequency[3] = value;
            }
        }
        if (shorName == "Gain0") {
            if (isDifferent(m_plugin->parameters()->filterGain[0], value)) {
                changed = true;
                m_plugin->parameters()->filterGain[0] = value;
            }
        }
        if (shorName == "Gain1") {
            if (isDifferent(m_plugin->parameters()->filterGain[1], value)) {
                changed = true;
                m_plugin->parameters()->filterGain[1] = value;
            }
        }
        if (shorName == "Gain2") {
            if (isDifferent(m_plugin->parameters()->filterGain[2], value)) {
                changed = true;
                m_plugin->parameters()->filterGain[2] = value;
            }
        }
        if (shorName == "Gain3") {
            if (isDifferent(m_plugin->parameters()->filterGain[3], value)) {
                changed = true;
                m_plugin->parameters()->filterGain[3] = value;
            }
        }
        if (shorName == "Q0") {
            if (isDifferent(m_plugin->parameters()->filterQ[0], value)) {
                changed = true;
                m_plugin->parameters()->filterQ[0] = value;
            }
        }
        if (shorName == "Q1") {
            if (isDifferent(m_plugin->parameters()->filterQ[1], value)) {
                changed = true;
                m_plugin->parameters()->filterQ[1] = value;
            }
        }
        if (shorName == "Q2") {
            if (isDifferent(m_plugin->parameters()->filterQ[2], value)) {
                changed = true;
                m_plugin->parameters()->filterQ[2] = value;
            }
        }
        if (shorName == "Q3") {
            if (isDifferent(m_plugin->parameters()->filterQ[3], value)) {
                changed = true;
                m_plugin->parameters()->filterQ[3] = value;
            }
        }

        if (changed) {
            m_plugin->updateGuiCoefficients();

            _automationItems[shorName]->automation()->setEnabled1(false);
            _automationItems[shorName]->automation()->setNormalized(normalized);

            Q_EMIT sigUpdate();
        }
    }
}

void MultiEqEffect::sltAutomatedEnabled(int index)
{
    if (m_plugin) {
        QString type = "";
        int i = index / 3;
        int r = index % 3;

        switch (r) {
        case 0:
            type = "Freq";
            break;
        case 1:
            type = "Gain";
            break;
        case 2:
            type = "Q";
            break;
        }

        QString _title = type + QString::number(i);

        _automationItems[_title]->setEnabled(true);

        Q_EMIT sigUpdate();
    }
}
void MultiEqEffect::sltDataChagedFromView(int index, QString type, double value)
{

    if (m_plugin) {

        QString _title = type + QString::number(index);

        double normalized = _automationItems[_title]->value2Normalized(value);

        _controls->updateValue(index, QString::number(value), normalized);

        // qDebug() << "sltDataChagedFromView" << "_title" << _title << value;
        _automationItems[_title]->automation()->setEnabled1(false);
        _automationItems[_title]->setNormalized(normalized);

        Q_EMIT sigUpdate();
    }
}

QString MultiEqEffect::name() const
{
    return "EQ";
}
QString MultiEqEffect::type() const
{
    return "EQ";
}
QSharedPointer<EffectControls> MultiEqEffect::controls() const
{
    return _controls;
}

EffectControls* MultiEqEffect::getControls() const
{
    EffectControls* qobj = _controls.data(); // Pointer to member shared-ptr-managed object.
    QQmlEngine::setObjectOwnership(qobj, QQmlEngine::CppOwnership);
    return qobj;
}

bool MultiEqEffect::initialize(int trackIndex, QSharedPointer<Effect> effect)
{

    m_plugin = QSharedPointer<MultiEQ>(new MultiEQ(effect.dynamicCast<MultiEqEffect>()));
    _controls = QSharedPointer<MultiEqControls>(new MultiEqControls(name(), m_plugin->effectParameters(), effect));

    m_plugin->prepareToPlay(AudioManager::audioEngine()->outputSampleRate(), AudioManager::audioEngine()->framesPerPeriod());

    connect(m_plugin.data(), &MultiEQ::sigUpdateParams, this, &MultiEqEffect::sltDataChagedFromView, Qt::QueuedConnection);
    connect(m_plugin.data(), &MultiEQ::sigAutomatedEnabled, this, &MultiEqEffect::sltAutomatedEnabled, Qt::QueuedConnection);
    connect(m_plugin.data(), &MultiEQ::sigDeviceOnAutomatedEnabled, this, &MultiEqEffect::sltDeviceOnAutomatedEnabled, Qt::QueuedConnection);
    connect(m_plugin.data(), &MultiEQ::sigDeviceOnEnabledChanged, this, &MultiEqEffect::sltDeviceOnEnabled, Qt::QueuedConnection);
    // connect(m_plugin.data(), &MultiEQ::sigActiveFilterChanged, this, &MultiEqEffect::sltActiveFilterChanged, Qt::QueuedConnection);

    connect(_controls.data(), &MultiEqControls::sigDataChaged, this, &MultiEqEffect::sltDataChagedFromModel);
    auto parameter = m_plugin->effectParameters();
    for (int i = 0; i < m_plugin->effectParameters().size(); i++) {

        if (parameter[i]._title == "DeviceOn") {
            _deviceOnAutomation = QSharedPointer<Automation>(new Automation(_areaInfo, trackIndex, _controls->name(), "DeviceOn", 0.25, 0.75));
            _speakerOnAutomationItem = QSharedPointer<SpeakerOnAutomationItem>(new SpeakerOnAutomationItem(trackIndex,
                _deviceOnAutomation, _areaInfo));

            connect(_speakerOnAutomationItem.data(), &SpeakerOnAutomationItem::sigAutomatedNormalizedChanged,
                this, &MultiEqEffect::deviceOnNormalizedChanged);

            connect(_speakerOnAutomationItem.data(), &SpeakerOnAutomationItem::sigCursorPosition, this, &MultiEqEffect::sigCursorPosition);
            connect(_speakerOnAutomationItem.data(), &SpeakerOnAutomationItem::sigSetCursor, this, &MultiEqEffect::sigSetCursor);

            connect(_deviceOnAutomation.data(), &Automation::sigUpdate, this, &MultiEqEffect::sigUpdate);
            connect(_speakerOnAutomationItem.data(), &SpeakerOnAutomationItem::sigUpdate, this, &MultiEqEffect::sigUpdate);

            connect(_deviceOnAutomation.data(), &Automation::sigLinesChanged, this, &MultiEqEffect::sltDeviceOnLinesChanged);
            connect(_deviceOnAutomation.data(), &EqAutomation::enabledChanged, this, &MultiEqEffect::sltDeviceOnEnabledChanged);
            connect(_speakerOnAutomationItem.data(), &SpeakerOnAutomationItem::sigAutomatedNormalizedChanged,
                this, &MultiEqEffect::sltDeviceOnAutomatedValueChanged);

            connect(_speakerOnAutomationItem.data(), &SpeakerOnAutomationItem::sigNormalizedChanged,
                this, &MultiEqEffect::sltDeviceOnValueChanged);

        } else {
            _automations[parameter[i]._title] = QSharedPointer<EqAutomation>(new EqAutomation(trackIndex, _controls->name(), parameter[i]._title, _areaInfo));
            _automationItems[parameter[i]._title] = QSharedPointer<EqAutomationItem>(new EqAutomationItem(trackIndex, parameter[i]._defaultNormalizedValue,
                parameter[i]._title, m_plugin, _automations[parameter[i]._title], _areaInfo));

            connect(_automations[parameter[i]._title].data(), &Automation::sigUpdate, this, &MultiEqEffect::sigUpdate);
            connect(_automationItems[parameter[i]._title].data(), &EqAutomationItem::sigUpdate, this, &MultiEqEffect::sigUpdate);

            connect(_automationItems[parameter[i]._title].data(), &EqAutomationItem::sigCursorPosition, this, &MultiEqEffect::sigCursorPosition);
            connect(_automationItems[parameter[i]._title].data(), &EqAutomationItem::sigSetCursor, this, &MultiEqEffect::sigSetCursor);

            connect(_automations[parameter[i]._title].data(), &EqAutomation::sigEqLineChanged, this, &MultiEqEffect::sltLinesChanged);

            connect(_automations[parameter[i]._title].data(), &EqAutomation::sigEqEnabledChanged, this, &MultiEqEffect::sltEnabledChanged);

            connect(_automationItems[parameter[i]._title].data(), &EqAutomationItem::sigEqAutomatedNormalizedChanged,
                this, &MultiEqEffect::sltAutomatedValueChanged);

            connect(_automationItems[parameter[i]._title].data(), &EqAutomationItem::sigEqNormalizedChanged,
                this, &MultiEqEffect::sltValueChanged);
        }
    }

    return true;
}

bool MultiEqEffect::processAudioBuffer(sampleFrame* _buf)
{

    const fpp_t fpp = AudioManager::audioEngine()->framesPerPeriod();

    sampleFrame _tmpBuf(_buf->getNumChannels(), fpp);
    for (int i = 0; i < _buf->getNumChannels(); i++)
        _tmpBuf.copyFrom(i, 0, _buf->getReadPointer(i), fpp);

    if (leftWetDryEnabled() || rightWetDryEnabled()) {
        if (leftWetLevel() > 0)
            for (f_cnt_t f = 0; f < fpp; ++f) {
                if (leftWetDryEnabled()) {
                    _buf->setSample(1, f, _buf->getSample(1, f) * leftWetLevel());
                }
                if (rightWetDryEnabled()) {
                    _buf->setSample(0, f, _buf->getSample(0, f) * rightWetLevel());
                }
            }
    }

    if (m_plugin && dontRun() == false) {
        m_plugin->processBlock(*_buf);
    }

    if (leftWetDryEnabled() || rightWetDryEnabled()) {
        for (f_cnt_t f = 0; f < fpp; ++f) {
            if (leftWetDryEnabled()) {
                _buf->setSample(1, f, _tmpBuf.getSample(1, f) * leftDryLevel() + _buf->getSample(1, f));
            }
            if (rightWetDryEnabled()) {
                _buf->setSample(0, f, _tmpBuf.getSample(0, f) * rightDryLevel() + _buf->getSample(0, f));
            }
        }
    }
    _tmpBuf.clear();

    return true;
}

bool MultiEqEffect::controlVisible()
{
    return false;
}

QSharedPointer<Automation> MultiEqEffect::automation(QString name)
{
    if (name == "DeviceOn") {
        return _deviceOnAutomation;
    }

    if (_automations.contains(name))
        return _automations[name];
    return Q_NULLPTR;
}
QSharedPointer<AutomationItem> MultiEqEffect::automationItem(QString name)
{
    if (name == "DeviceOn") {
        return _speakerOnAutomationItem;
    }
    if (_automationItems.contains(name))
        return _automationItems[name];
    return Q_NULLPTR;
}

bool MultiEqEffect::contains(QString name)
{
    if (name == "DeviceOn") {
        return true;
    }
    return _controls->shortTitleList().contains(name);
}
bool MultiEqEffect::isAutomated(QString name)
{
    return false;
}
bool MultiEqEffect::hasExternalControl()
{
    return true;
}

MultiEqControls::MultiEqControls(QString name, QVector<MultiEQ::EffectParameter> list, QSharedPointer<Effect> _eff, QObject* parent)
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

int MultiEqControls::controlCount()
{
    return _list.size();
}

void MultiEqControls::updateValue(int i, QString value, double normalized)
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

void MultiEqControls::updateValueFromAutomation(int i, QString value, double normalized)
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

QMap<QString, uint> MultiEqControls::mapTitles() const
{
    return _mapTitles;
}

QMap<uint, int> MultiEqControls::mapIndexes() const
{
    return _mapIndexes;
}

QString MultiEqControls::name()
{
    return "EQ";
}

QStringList MultiEqControls::shortTitleList()
{
    QStringList list;
    for (int i = 0; i < _list.size(); i++) {
        list.append(_list[i]._title);
    }
    return list;
}

int MultiEqControls::rowCount(const QModelIndex& parent) const
{
    return _list.size();
}

bool MultiEqControls::setData(const QModelIndex& index, const QVariant& value, int role)
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

QVariant MultiEqControls::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (_list.size() == 0 || index.row() < 0)
        return QVariant();

    MultiEQ::EffectParameter value = _list[index.row()];

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
