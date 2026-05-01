#ifndef VST3EFFECT_H
#define VST3EFFECT_H

#include "vst3plugin.h"

#include <QDebug>
#include <QSharedPointer>
#include <audio/dummyeffect.h>
#include <audio/effect.h>
#include <audio/effectcontrols.h>
#include <audio/plugin.h>
#include <ui/automationitem.h>
#include <ui/trackitem.h>

class Vst3EffectControls : public EffectControls {
    Q_OBJECT
public:
    enum Vst3EffectRole {
        ROLE_Index = Qt::UserRole + 1,
        ROLE_Name,
        ROLE_Normalized,
        ROLE_Value,
        ROLE_StepSize,
        ROLE_DefaultNormalized,
        ROLE_Active,
    };
    Q_ENUM(Vst3EffectRole)
    Vst3EffectControls(QString name, QVector<Parameter> list, QSharedPointer<Effect> _eff, QObject* parent = 0);
    ~Vst3EffectControls();
    void addItem(Parameter parameter);

    int getJuceIndex(QString title);
    int getRowIndex(QString title);

    // EffectControls interface
public:
    void updateValue(int i, QString value, double normalized);
    void updateValueFromAutomation(int i, QString value, double normalized);
    int controlCount()
    {
        return _list.size();
    }

private:
    QString _name;

public:
    QString name();

private:
    QVector<Parameter> _list;

signals:
    void valueChanged();

    // QAbstractItemModel interface

public:
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent) const
    {
        return _list.size();
    }
    // EffectControls interface
public:
    QStringList shortTitleList()
    {

        QStringList list;
        for (int i = 0; i < _list.size(); i++) {
            list.append(_list[i]._title);
        }
        return list;
    }

    QVector<Parameter> list() const;

signals:
    void sigDataChaged(uint32 id, double normalized);
};

class Vst3Automation : public Automation {
    Q_OBJECT
public:
    explicit Vst3Automation(int trackIndex,
        QString name, QString shortName, AreaInfo* areaInfo)
        : Automation(areaInfo, trackIndex, name, shortName)
        , _shortName(shortName)
    {
    }

signals:
    void vst3ValueChanged(QString shortName);
    void vst3LinesChanged(QString shortName);

private:
    QString _shortName;
};

class Vst3AutomationItem : public AutomationItem {
    Q_OBJECT
public:
    explicit Vst3AutomationItem(int trackIndex, double defaultNormalized,
        QString title, QSharedPointer<Vst3Plugin> plugin, QSharedPointer<Automation> automation, AreaInfo* areaInfo)
        : AutomationItem(trackIndex, automation, areaInfo)
        , _plugin(plugin)
        , _title(title)
    {

        automation->setDefaultNormalized(defaultNormalized);
        automation->setAutomatedNormalized(defaultNormalized);
        automation->setNormalized(defaultNormalized);

        connect(this, &AutomationItem::sigAutomatedNormalizedChanged, this, &Vst3AutomationItem::sltVst3AutomatedNormalizedChanged);
        connect(this, &AutomationItem::sigNormalizedChanged, this, &Vst3AutomationItem::sltVst3NormalizedChanged);
    }

public Q_SLOTS:
    void sltVst3NormalizedChanged()
    {
        Q_EMIT sigVst3NormalizedChanged(_title);
    }
    void sltVst3AutomatedNormalizedChanged()
    {
        Q_EMIT sigVst3AutomatedNormalizedChanged(_title);
    }
    // AutomationItem interface
public:
    double normalized2Value(double y)
    {
        uint id;
        auto list = _plugin->parameters();
        for (int i = 0; i < list.size(); i++) {
            if (list[i]._title == _title) {
                id = list[i]._id;
                break;
            }
        }

        QString t1 = _plugin->getParamValue(id, 1 - y).split(" ")[0];

        return t1.trimmed().toDouble();
    }
    double value2Normalized(double y)
    {
        return 0;
    }

    QString normalized2Label(double y)
    {
        uint id;
        auto list = _plugin->parameters();
        for (int i = 0; i < list.size(); i++) {
            if (list[i]._title == _title) {
                id = list[i]._id;
                break;
            }
        }

        return _plugin->getParamValue(id, 1 - y);
    }

private:
    QSharedPointer<Vst3Plugin> _plugin;
    QString _title;

signals:
    void sigVst3AutomatedNormalizedChanged(QString shortname);
    void sigVst3NormalizedChanged(QString shortname);
};

class Vst3Effect : public Effect {
    Q_OBJECT

public:
    Vst3Effect(QString path = "", QString name = "", int pluginIndex = 0, AreaInfo* _areaInfo = Q_NULLPTR, int trackIndex = -1, int channelIn = 2, int channelOut = 2, QObject* parent = nullptr);
    ~Vst3Effect();

    void addConfiguredParameter(int juceIndex);

public:
    QString name() const;
    QString type() const;

    bool initialize(int trackIndex, QSharedPointer<Effect> effect);
    bool processAudioBuffer(sampleFrame* _buf);
    QSharedPointer<Automation> automation(QString name)
    {
        if (name == "DeviceOn") {
            return _deviceOnAutomation;
        }

        if (_automations.contains(name))
            return _automations[name];
        return Q_NULLPTR;
    }
    QSharedPointer<AutomationItem> automationItem(QString name)
    {
        if (name == "DeviceOn") {
            return _deviceOnAutomationItem;
        }

        if (_automationItems.contains(name))
            return _automationItems[name];
        return Q_NULLPTR;
    }
    bool contains(QString name)
    {
        if (name == "DeviceOn") {
            return true;
        }

        return _controls->shortTitleList().contains(name);
    }
    bool isAutomated(QString name)
    {
        return false;
    }
    bool hasExternalControl()
    {
        return true;
    }

    Q_INVOKABLE void showControl();
    Q_INVOKABLE void hideControl();

    bool controlVisible();

    bool initialPlugin(QSharedPointer<Effect> effect);

    QVector<Parameter> parameters() const;

private:
    int m_channelOut = 2;
    int m_channelIn = 2;
    QSharedPointer<Vst3Plugin> m_plugin;
    int m_pluginIndex = 0;
    QString m_pluginPath;
    QString m_pluginName;

    QSharedPointer<Vst3EffectControls> _controls;
    std::unique_ptr<juce::AudioPluginInstance>
    createPluginInstance(const juce::String& pluginPath, double initialSampleRate,
        int initialBlockSize);

private Q_SLOTS:
    void sltControlVisibleChanged(bool visible);
    void sltAutomatedValueChanged(QString shorName);
    void sltValueChanged(QString shorName);

public:
    QSharedPointer<EffectControls> controls() const;
    EffectControls* getControls() const;

    QMap<QString, QSharedPointer<Vst3AutomationItem>> _automationItems;
    QMap<QString, QSharedPointer<Vst3Automation>> _automations;
    AreaInfo* _areaInfo;
    int _trackIndex;

    int pluginIndex() const;

    QString pluginName() const;

    QString pluginPath() const;

    QSharedPointer<Automation> _deviceOnAutomation;
    QSharedPointer<SpeakerOnAutomationItem> _deviceOnAutomationItem;

private Q_SLOTS:
    void deviceOnNormalizedChanged();
    void sltDeviceOnAutomatedEnabled();
    void sltDeviceOnEnabled();

public Q_SLOTS:

    // this just for update from audio process
    void sltUpdateParams(int index, QString value, double normalized);
    void sltDataChagedFromModel(uint32 rowIndex, double normalized);
    void sltDataChagedFromView(uint32 juceIndex, double normalized);
    void sltNewParameterAdded(int juceIndex);

    void sltDeviceOnLinesChanged();
    void sltDeviceOnEnabledChanged();
    void sltDeviceOnValueChanged();
    void sltDeviceOnAutomatedValueChanged();

    // Effect interface
public:
    void setEnabled(bool newEnabled);
};

#endif // VST3EFFECT_H
