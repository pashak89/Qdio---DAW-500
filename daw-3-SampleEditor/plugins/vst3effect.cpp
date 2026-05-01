#include "vst3effect.h"
#include "audio/song.h"
#include <QQmlEngine>
#include <QtConcurrent>
Plugin::Descriptor vst3PluginDescriptor = {
    "VST3",
    "VST3",
    "VST3 description",
    Plugin::Effect,
    nullptr,

};

Vst3Effect::Vst3Effect(QString path, QString name, int index, AreaInfo* areaInfo, int trackIndex, int channelIn, int channelOut, QObject* parent)
    : Effect(&vst3PluginDescriptor, nullptr, parent)
    , m_pluginPath(path)
    , m_pluginIndex(index)
    , m_pluginName(name)
    , _areaInfo(areaInfo)
    , _trackIndex(trackIndex)
    , m_channelOut(channelOut)
    , m_channelIn(channelIn)
{
    qDebug() << "Vst3Effect" << path << index;
}

Vst3Effect::~Vst3Effect()
{

    for (int i = 0; i < m_plugin->parameters().size(); i++) {
        auto parameter = m_plugin->parameters();

        if (_automations.contains(parameter[i]._title)) {
            _automations[parameter[i]._title].reset();
            _automations.remove(parameter[i]._title);
        }

        if (_automationItems.contains(parameter[i]._title)) {
            _automationItems[parameter[i]._title].reset();
            _automationItems.remove(parameter[i]._title);
        }
    }
}

void Vst3Effect::addConfiguredParameter(int juceIndex)
{
    if (m_plugin) {
        m_plugin->addConfiguredParameter(juceIndex);
    }
}

bool Vst3Effect::initialize(int trackIndex, QSharedPointer<Effect> effect)
{
    m_plugin = QSharedPointer<Vst3Plugin>(new Vst3Plugin(effect));
    QtConcurrent::run([this, trackIndex, effect]() {
        m_plugin.data()->init(m_pluginPath.toStdString(), m_pluginName.toStdString(), m_pluginIndex,
            AudioManager::audioEngine()->outputSampleRate(), AudioManager::audioEngine()->framesPerPeriod(), m_channelIn, m_channelOut);
        Q_EMIT this->sigEffectLoaded(trackIndex);
    });
    return true;
}

bool Vst3Effect::processAudioBuffer(sampleFrame* _buf)
{

    if (m_plugin && dontRun() == false) {
        return m_plugin->process(_buf);
    }
    return true;
}

QString Vst3Effect::name() const
{
    if (m_plugin)
        return m_pluginName;
    return "";
}

QString Vst3Effect::type() const
{
    return "Vst3";
}

QSharedPointer<EffectControls> Vst3Effect::controls() const
{
    return _controls;
}

EffectControls* Vst3Effect::getControls() const
{
    EffectControls* qobj = _controls.data(); // Pointer to member shared-ptr-managed object.
    QQmlEngine::setObjectOwnership(qobj, QQmlEngine::CppOwnership);
    return qobj;
}

void Vst3Effect::showControl()
{
    if (m_plugin) {
        m_plugin->createView();
    }
}

void Vst3Effect::hideControl()
{
    if (m_plugin) {
        m_plugin->destroyView();
    }
}

bool Vst3Effect::controlVisible()
{
    return m_controlVisible;
}

bool Vst3Effect::initialPlugin(QSharedPointer<Effect> effect)
{

    m_plugin->prepareToplay();

    connect(m_plugin.data(), &Vst3Plugin::sigVisibleChanged, this, &Vst3Effect::sltControlVisibleChanged);

    if (m_plugin->failed()) {
        m_plugin.clear();
        return false;
    }

    connect(m_plugin.data(), &Vst3Plugin::sigNewParameterAdded, this, &Vst3Effect::sltNewParameterAdded);

    m_pluginName = QString::fromStdString(m_plugin->name());

    _controls = QSharedPointer<Vst3EffectControls>(new Vst3EffectControls(QString::fromStdString(m_plugin->name()), m_plugin->parameters(), effect));

    connect(m_plugin.data(), &Vst3Plugin::sigUpdateParams, this, &Vst3Effect::sltUpdateParams, Qt::QueuedConnection);
    connect(m_plugin.data(), &Vst3Plugin::sigPerformEdit, this, &Vst3Effect::sltDataChagedFromView);

    connect(_controls.data(), &Vst3EffectControls::sigDataChaged, this, &Vst3Effect::sltDataChagedFromModel);

    auto parameter = m_plugin->parameters();
    for (int i = 0; i < m_plugin->parameters().size(); i++) {

        if (parameter[i]._title == "DeviceOn") {
            _deviceOnAutomation = QSharedPointer<Automation>(new Automation(_areaInfo, _trackIndex, _controls->name(), "DeviceOn", 0.25, 0.75));
            _deviceOnAutomationItem = QSharedPointer<SpeakerOnAutomationItem>(new SpeakerOnAutomationItem(_trackIndex,
                _deviceOnAutomation, _areaInfo));

            _deviceOnAutomation->setNormalized(0.75);

            connect(_deviceOnAutomationItem.data(), &SpeakerOnAutomationItem::sigAutomatedNormalizedChanged,
                this, &Vst3Effect::deviceOnNormalizedChanged);

            connect(_deviceOnAutomationItem.data(), &SpeakerOnAutomationItem::sigCursorPosition, this, &Vst3Effect::sigCursorPosition);
            connect(_deviceOnAutomationItem.data(), &SpeakerOnAutomationItem::sigSetCursor, this, &Vst3Effect::sigSetCursor);

            connect(_deviceOnAutomation.data(), &Automation::sigUpdate, this, &Vst3Effect::sigUpdate);
            connect(_deviceOnAutomationItem.data(), &SpeakerOnAutomationItem::sigUpdate, this, &Vst3Effect::sigUpdate);

            connect(_deviceOnAutomation.data(), &Automation::sigLinesChanged, this, &Vst3Effect::sltDeviceOnLinesChanged);
            connect(_deviceOnAutomation.data(), &Automation::enabledChanged, this, &Vst3Effect::sltDeviceOnEnabledChanged);
            connect(_deviceOnAutomationItem.data(), &SpeakerOnAutomationItem::sigAutomatedNormalizedChanged,
                this, &Vst3Effect::sltDeviceOnAutomatedValueChanged);

            connect(_deviceOnAutomationItem.data(), &SpeakerOnAutomationItem::sigNormalizedChanged,
                this, &Vst3Effect::sltDeviceOnValueChanged);

        } else {
            _automations[parameter[i]._title] = QSharedPointer<Vst3Automation>(new Vst3Automation(_trackIndex, _controls->name(), parameter[i]._title, _areaInfo));
            _automationItems[parameter[i]._title] = QSharedPointer<Vst3AutomationItem>(new Vst3AutomationItem(_trackIndex, 1 - parameter[i]._defaultNormalizedValue,
                parameter[i]._title, m_plugin, _automations[parameter[i]._title], _areaInfo));

            connect(_automations[parameter[i]._title].data(), &Automation::sigUpdate, this, &Vst3Effect::sigUpdate);
            connect(_automationItems[parameter[i]._title].data(), &AutomationItem::sigUpdate, this, &Vst3Effect::sigUpdate);

            connect(_automationItems[parameter[i]._title].data(), &AutomationItem::sigCursorPosition, this, &Vst3Effect::sigCursorPosition);
            connect(_automationItems[parameter[i]._title].data(), &AutomationItem::sigSetCursor, this, &Vst3Effect::sigSetCursor);

            connect(_automationItems[parameter[i]._title].data(), &Vst3AutomationItem::sigVst3AutomatedNormalizedChanged,
                this, &Vst3Effect::sltAutomatedValueChanged);

            connect(_automationItems[parameter[i]._title].data(), &Vst3AutomationItem::sigVst3NormalizedChanged,
                this, &Vst3Effect::sltValueChanged);
        }
    }
    return true;
}

QVector<Parameter> Vst3Effect::parameters() const
{
    if (m_plugin) {
        return m_plugin->parameters();
    }
    return QVector<Parameter>();
}

QString Vst3Effect::pluginPath() const
{
    return m_pluginPath;
}

void Vst3Effect::deviceOnNormalizedChanged()
{
    auto value = _deviceOnAutomationItem->value();

    if (_deviceOnAutomationItem->automation()->isAutomated()) {
        if (value) {
            setDontRun(false);
        } else {
            setDontRun(true);
        }
    } else {
        setDontRun(false);
    }
}

void Vst3Effect::sltDeviceOnAutomatedEnabled()
{
    _deviceOnAutomationItem->setEnabled(true);
}

void Vst3Effect::sltDeviceOnEnabled()
{

    if (!_deviceOnAutomationItem->automation()->isAutomated()) {
        qDebug() << "_deviceOnAutomation->normalized()" << _deviceOnAutomation->normalized();
        _deviceOnAutomationItem->setNormalized(1 - _deviceOnAutomation->normalized());
    } else {
        if (_deviceOnAutomation->enabled()) {
            _deviceOnAutomationItem->setEnabled(false);
            _deviceOnAutomationItem->setNormalized(1 - _deviceOnAutomationItem->automation()->automatedNormalized());
        } else {
            _deviceOnAutomationItem->setNormalized(1 - _deviceOnAutomation->normalized());
        }
    }

    auto value = _deviceOnAutomationItem->value();

    qDebug() << "_deviceOnAutomation->normalized()" << (value == 1) << value;
    if (_deviceOnAutomationItem->automation()->isAutomated()) {
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

QString Vst3Effect::pluginName() const
{
    return m_pluginName;
}

int Vst3Effect::pluginIndex() const
{
    return m_pluginIndex;
}

void Vst3Effect::sltUpdateParams(int index, QString value, double normalized)
{
    _controls->updateValue(index, value, normalized);
}

void Vst3Effect::sltDataChagedFromModel(uint32 rowIndex, double normalized)
{
    if (m_plugin) {

        QString _shortName;
        auto list = m_plugin->parameters();
        _shortName = list[rowIndex]._title;

        m_plugin->updateParameter(list[rowIndex]._id, normalized);

        _automationItems[_shortName]->automation()->setEnabled1(false);
        _automationItems[_shortName]->setNormalized(1 - normalized);
        Q_EMIT sigUpdate();
    }
}
void Vst3Effect::sltDataChagedFromView(uint32 juceIndex, double normalized)
{

    if (m_plugin) {

        QString _title = "";

        Parameter parameter;

        bool result = m_plugin->parameter(juceIndex, parameter);

        if (result) {

            _title = parameter._title;

            int rowIndex = _controls->getRowIndex(_title);

            if (rowIndex < 0) {
                qDebug() << "Bug8:sltDataChagedFromView" << _title << juceIndex;
                return;
            }

            auto value = m_plugin->getParamValue(juceIndex, normalized);

            _automationItems[_title]->setCurrentValue(value);

            _controls->updateValue(rowIndex, value, normalized);

            _automationItems[_title]->automation()->setEnabled1(false);
            _automationItems[_title]->setNormalized(1 - normalized);
            Q_EMIT sigUpdate();
        } else {
            qDebug() << "Bug7:sltDataChagedFromView";
        }
    }
}

void Vst3Effect::sltNewParameterAdded(int juceIndex)
{
    Parameter parameter;

    bool result = m_plugin->parameter(juceIndex, parameter);
    if (!result) {
        qDebug() << "BUG41" << juceIndex;
        return;
    }

    _controls->addItem(parameter);

    _automations[parameter._title] = QSharedPointer<Vst3Automation>(new Vst3Automation(_trackIndex, _controls->name(), parameter._title, _areaInfo));
    _automationItems[parameter._title] = QSharedPointer<Vst3AutomationItem>(new Vst3AutomationItem(_trackIndex, 1 - parameter._defaultNormalizedValue,
        parameter._title, m_plugin, _automations[parameter._title], _areaInfo));

    connect(_automations[parameter._title].data(), &Automation::sigUpdate, this, &Vst3Effect::sigUpdate);
    connect(_automationItems[parameter._title].data(), &AutomationItem::sigUpdate, this, &Vst3Effect::sigUpdate);

    connect(_automationItems[parameter._title].data(), &AutomationItem::sigCursorPosition, this, &Vst3Effect::sigCursorPosition);
    connect(_automationItems[parameter._title].data(), &AutomationItem::sigSetCursor, this, &Vst3Effect::sigSetCursor);

    connect(_automationItems[parameter._title].data(), &Vst3AutomationItem::sigVst3AutomatedNormalizedChanged,
        this, &Vst3Effect::sltAutomatedValueChanged);

    connect(_automationItems[parameter._title].data(), &Vst3AutomationItem::sigVst3NormalizedChanged,
        this, &Vst3Effect::sltValueChanged);

    if (effectIndex() >= 0) {
        Q_EMIT sigEffectUpdated(_trackIndex, effectIndex());
    } else {
        qDebug() << "ERROR" << _trackIndex;
    }
}

void Vst3Effect::sltDeviceOnLinesChanged()
{
    if (!_deviceOnAutomation->isAutomated()) {
        setEnabled(false);
    } else {
        setEnabled(true);
    }
}

void Vst3Effect::sltDeviceOnEnabledChanged()
{
    // m_plugin->deviceOnAutomated(_deviceOnAutomation->enabled());

    auto value = _deviceOnAutomationItem->value();
    Effect::setEnabled(value == 1);
    Q_EMIT sigEnableChanged(_trackIndex, name());

    if (_deviceOnAutomationItem->automation()->isAutomated()) {
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

void Vst3Effect::sltDeviceOnValueChanged()
{
    auto value = _deviceOnAutomationItem->value();

    if (_deviceOnAutomationItem->automation()->isAutomated()) {
        if (value) {
            setDontRun(false);
        } else {
            setDontRun(true);
        }
    } else {
        setDontRun(false);
    }

    Effect::setEnabled(value == 1);
    Q_EMIT sigEnableChanged(_trackIndex, name());
}

void Vst3Effect::sltDeviceOnAutomatedValueChanged()
{
    auto value = _deviceOnAutomationItem->value();

    Effect::setEnabled(value == 1);
    Q_EMIT sigEnableChanged(_trackIndex, name());
    if (_deviceOnAutomationItem->automation()->isAutomated()) {
        if (value) {
            setDontRun(false);
        } else {
            setDontRun(true);
        }
    } else {
        setDontRun(false);
    }
}

void Vst3Effect::setEnabled(bool newEnabled)
{

    Effect::setEnabled(newEnabled);
    sltDeviceOnEnabled();
    Q_EMIT sigEnableChanged(_trackIndex, name());
}

void Vst3Effect::sltControlVisibleChanged(bool visible)
{

    m_controlVisible = visible;
    Q_EMIT sigControlVisibleChanged(_trackIndex, name());
}

void Vst3Effect::sltAutomatedValueChanged(QString shorName)
{

    auto normalized = _automations[shorName]->automatedNormalized();

    int juceIndex = _controls->getJuceIndex(shorName);
    int rowIndex = _controls->getRowIndex(shorName);

    if (juceIndex < 0 || rowIndex < 0) {
        qDebug() << "Bug5" << shorName;
    }

    // blew line is just for applying norm to get correct value
    m_plugin->updateParameter(juceIndex, 1 - normalized);
    auto value = m_plugin->getParamValue(juceIndex, 1 - normalized);

    _automationItems[shorName]->setCurrentValue(value);

    _controls->updateValueFromAutomation(rowIndex, value, 1 - normalized);
}

void Vst3Effect::sltValueChanged(QString shorName)
{

    auto normalized = _automations[shorName]->normalized();

    int juceIndex = _controls->getJuceIndex(shorName);
    int rowIndex = _controls->getRowIndex(shorName);

    if (juceIndex < 0 || rowIndex < 0) {
        qDebug() << "Bug6" << shorName;
    }

    m_plugin->updateParameter(juceIndex, 1 - normalized);
    auto value = m_plugin->getParamValue(juceIndex, 1 - normalized);

    _automationItems[shorName]->setCurrentValue(value);

    _controls->updateValueFromAutomation(rowIndex, value, 1 - normalized);
}

Vst3EffectControls::Vst3EffectControls(QString name, QVector<Parameter> list, QSharedPointer<Effect> _eff, QObject* parent)
    : EffectControls(_eff, parent)

    , _name(name)
{

    m_roleNames[ROLE_Name] = "name";
    m_roleNames[ROLE_Index] = "index";
    m_roleNames[ROLE_Normalized] = "normalized";
    m_roleNames[ROLE_Value] = "value";
    m_roleNames[ROLE_StepSize] = "stepSize";
    m_roleNames[ROLE_DefaultNormalized] = "defaultNormalized";
    m_roleNames[ROLE_Active] = "active";

    for (int i = 0; i < list.size(); i++) {
        _list.append(list[i]);
    }
}

Vst3EffectControls::~Vst3EffectControls()
{
}

void Vst3EffectControls::addItem(Parameter parameter)
{
    Q_EMIT beginResetModel();
    _list.append(parameter);

    Q_EMIT endResetModel();
}

int Vst3EffectControls::getJuceIndex(QString title)
{
    for (int i = 0; i < _list.size(); i++) {
        if (_list[i]._title == title) {
            return _list[i]._id;
        }
    }
    return -1;
}

int Vst3EffectControls::getRowIndex(QString title)
{
    for (int i = 0; i < _list.size(); i++) {
        if (_list[i]._title == title) {
            return i;
        }
    }
    return -1;
}

void Vst3EffectControls::updateValue(int i, QString value, double normalized)
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

void Vst3EffectControls::updateValueFromAutomation(int i, QString value, double normalized)
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

QString Vst3EffectControls::name()
{
    return _name;
}

bool Vst3EffectControls::setData(const QModelIndex& index, const QVariant& value, int role)
{

    if (_list.size() == 0 || index.row() < 0 || index.row() > _list.size())
        return false;

    if (index.isValid() && role == ROLE_Normalized) {

        _list[index.row()]._normalized = value.toDouble();

        _list[index.row()]._active = false;
        Q_EMIT sigDataChaged(index.row(), _list[index.row()]._normalized);
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

QVector<Parameter> Vst3EffectControls::list() const
{
    return _list;
}

QVariant Vst3EffectControls::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (_list.size() == 0 || index.row() < 0)
        return QVariant();

    Parameter value = _list[index.row()];

    switch (role) {
    case ROLE_Index:
        return index.row();
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
