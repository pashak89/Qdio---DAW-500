#ifndef MULTIEQEFFECT_H
#define MULTIEQEFFECT_H

#include "multieq.h"
#include "ui/trackitem.h"
#include <QJsonObject>
#include <audio/effect.h>
#include <audio/effectcontrols.h>
class MultiEqControls : public EffectControls {
    Q_OBJECT

public:
    enum MultiEqRole {
        ROLE_Index = Qt::UserRole + 1,
        ROLE_Name,
        ROLE_Normalized,
        ROLE_Value,
        ROLE_StepSize,
        ROLE_DefaultNormalized,
        ROLE_Active,
    };
    Q_ENUM(MultiEqRole)

    MultiEqControls(QString name, QVector<MultiEQ::EffectParameter> list, QSharedPointer<Effect> _eff, QObject* parent = 0);
    int controlCount();

    void updateValue(int i, QString value, double normalized);
    void updateValueFromAutomation(int i, QString value, double normalized);

private:
    QVector<MultiEQ::EffectParameter> _list;
    QMap<QString, uint> _mapTitles;
    QMap<uint, int> _mapIndexes;

signals:
    void valueChanged();
    void sigDataChaged(uint32 index, QString type, double normalized);

    void sigActiveChanged(int index, bool automated);
    // EffectControls interface
public:
    QString name();

    // EffectControls interface
public:
    QStringList shortTitleList();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QMap<uint, int> mapIndexes() const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    QMap<QString, uint> mapTitles() const;
};

class EqAutomation : public Automation {
    Q_OBJECT
public:
    explicit EqAutomation(int trackIndex,
        QString name, QString shortName, AreaInfo* areaInfo)
        : Automation(areaInfo, trackIndex, name, shortName)
        , _shortName(shortName)
    {
        connect(this, &Automation::sigLinesChanged, this, &EqAutomation::sltLinesChanged);
        connect(this, &Automation::enabledChanged, this, &EqAutomation::sltEnabledChanged);
    }

public Q_SLOTS:
    void sltLinesChanged()
    {
        Q_EMIT sigEqLineChanged(_shortName);
    }

    void sltEnabledChanged()
    {
        Q_EMIT sigEqEnabledChanged(_shortName, this->enabled());
    }

private:
    QString _shortName;

signals:

    void sigEqLineChanged(QString shortname);
    void sigEqEnabledChanged(QString shortname, bool enabled);
};

class EqAutomationItem : public AutomationItem {
    Q_OBJECT
public:
    explicit EqAutomationItem(int trackIndex, double defaultNormalized,
        QString title, QSharedPointer<MultiEQ> plugin, QSharedPointer<Automation> automation, AreaInfo* areaInfo)
        : AutomationItem(trackIndex, automation, areaInfo)
        , _plugin(plugin)
        , _title(title)
    {

        automation->setDefaultNormalized(defaultNormalized);
        automation->setAutomatedNormalized(defaultNormalized);
        automation->setNormalized(defaultNormalized);

        connect(this, &AutomationItem::sigAutomatedNormalizedChanged, this, &EqAutomationItem::sltEqAutomatedNormalizedChanged);
        connect(this, &AutomationItem::sigNormalizedChanged, this, &EqAutomationItem::sltEqNormalizedChanged);
    }

public Q_SLOTS:
    void sltEqNormalizedChanged()
    {
        Q_EMIT sigEqNormalizedChanged(_title);
    }
    void sltEqAutomatedNormalizedChanged()
    {
        Q_EMIT sigEqAutomatedNormalizedChanged(_title);
    }

public:
    double normalized2Value(double y)
    {
        uint id;
        auto list = _plugin->effectParameters();
        for (int i = 0; i < list.size(); i++) {
            if (list[i]._title == _title) {
                id = list[i]._id;
                break;
            }
        }

        QString t1 = _plugin->getParamValue(id, y).split(" ")[0];

        return t1.trimmed().toDouble();
    }
    double value2Normalized(double y)
    {
        uint id;
        auto list = _plugin->effectParameters();
        for (int i = 0; i < list.size(); i++) {
            if (list[i]._title == _title) {
                id = list[i]._id;
                break;
            }
        }

        return _plugin->getParamNormalized(id, y);
    }

    QString normalized2Label(double y)
    {
        uint id;
        auto list = _plugin->effectParameters();
        for (int i = 0; i < list.size(); i++) {
            if (list[i]._title == _title) {
                id = list[i]._id;
                break;
            }
        }

        return _plugin->getParamValue(id, y);
    }

private:
    QSharedPointer<MultiEQ> _plugin;
    QString _title;

signals:
    void sigEqAutomatedNormalizedChanged(QString shortname);
    void sigEqNormalizedChanged(QString shortname);
};

class MultiEqEffect : public Effect {
    Q_OBJECT
    Q_PROPERTY(MultiEQ* plugin READ getPlugin NOTIFY pluginChanged)

public:
    MultiEqEffect(int trackIndex, AreaInfo* areaInfo, QObject* parent = 0);

private:
    QSharedPointer<MultiEqControls> _controls;
    QSharedPointer<MultiEQ> m_plugin;
    QMap<QString, QSharedPointer<EqAutomationItem>> _automationItems;
    QMap<QString, QSharedPointer<EqAutomation>> _automations;

    QSharedPointer<Automation> _deviceOnAutomation;
    QSharedPointer<SpeakerOnAutomationItem> _speakerOnAutomationItem;

    int _trackIndex;
    AreaInfo* _areaInfo;
    MultiEQ* getPlugin();

    QJsonObject _status;
    bool isDifferent(double a, double b)
    {
        return std::abs(a - b) > 0.01;
    }
private Q_SLOTS:
    void sltControlVisibleChanged(bool visible);
    void sltAutomatedValueChanged(QString shorName);
    void sltValueChanged(QString shorName);

    void sltDataChagedFromModel(int index, QString type, double value);
    void sltDataChagedFromView(int index, QString type, double value);

    void sltAutomatedEnabled(int index);

    void sltLinesChanged(QString shorName);
    void sltEnabledChanged(QString shorName, bool enabled);

    void sltDeviceOnLinesChanged();
    void sltDeviceOnEnabledChanged();
    void sltDeviceOnValueChanged();
    void sltDeviceOnAutomatedValueChanged();

    void deviceOnNormalizedChanged();
    void sltDeviceOnAutomatedEnabled();
    void sltDeviceOnEnabled(bool enabled);

public Q_SLOTS:
    void sltSaveInUndo(QJsonObject status, int op);
    void sltSaveInRedo(QJsonObject status);
    void sltUpdateState(QJsonObject status, int op);

signals:
    void pluginChanged();

public:
    QString name() const;
    QString type() const;
    QSharedPointer<EffectControls> controls() const;
    EffectControls* getControls() const;
    bool initialize(int trackIndex, QSharedPointer<Effect> effect);
    bool processAudioBuffer(sampleFrame* _buf);
    bool controlVisible();
    QSharedPointer<Automation> automation(QString name);
    QSharedPointer<AutomationItem> automationItem(QString name);
    bool contains(QString name);
    bool isAutomated(QString name);
    bool hasExternalControl();

    // Effect interface
public:
    QJsonObject performOperation(QJsonObject obj);
    QJsonObject getLastStatus();
    void setLastStatus(QJsonObject status);

    void saveUndoOperation();
    void saveRedoOperation();
};

#endif // MULTIEQEFFECT_H
