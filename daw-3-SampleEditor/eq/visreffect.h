#ifndef VISREFFECT_H
#define VISREFFECT_H

#include <audio/effectcontrols.h>
#include <audio/track.h>
#include <audio/visrhelper.h>

class ReverbFilters : public QAbstractListModel {
    Q_OBJECT

public:
    enum FilterRole {

        ROLE_B0 = Qt::UserRole + 1,
        ROLE_B1,
        ROLE_B2,
        ROLE_A1,
        ROLE_A2,
    };
    Q_ENUM(FilterRole)

    ReverbFilters(QObject* parent = 0)
        : QAbstractListModel(parent)
    {

        m_roleNames[ROLE_B0] = "B0";
        m_roleNames[ROLE_B1] = "B1";
        m_roleNames[ROLE_B2] = "B2";
        m_roleNames[ROLE_A1] = "A1";
        m_roleNames[ROLE_A2] = "A2";
    }
    int count()
    {
        return _filters.size();
    }
    void clear()
    {
        _filters.clear();
        beginResetModel();
        endResetModel();
    }
    void addFilter(float b0, float b1, float b2, float a1, float a2)
    {

        beginResetModel();
        _filters.append(Coeff { b0, b1, b2, a1, a2 });
        endResetModel();
    }
    void getFilter(int index, float& b0, float& b1, float& b2, float& a1, float& a2)
    {
        auto filter = _filters[index];
        b0 = filter.b0;
        b1 = filter.b1;
        b2 = filter.b2;
        a1 = filter.a1;
        a2 = filter.a2;
    }

    void setFilter(int index, float b0, float b1, float b2, float a1, float a2)
    {
        _filters[index].b0 = b0;
        _filters[index].b1 = b1;
        _filters[index].b2 = b2;
        _filters[index].a1 = a1;
        _filters[index].a2 = a2;

        beginResetModel();
        endResetModel();
    }

    struct Coeff {
        float b0;
        float b1;
        float b2;
        float a1;
        float a2;
    };

private:
    QVector<Coeff> _filters;

protected:
    QHash<int, QByteArray> m_roleNames;

protected:
    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }
    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex& parent) const
    {
        return _filters.size();
    }
    QVariant data(const QModelIndex& index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        if (_filters.size() == 0)
            return QVariant();

        Coeff value = _filters[index.row()];

        switch (role) {

        case ROLE_B0:
            return value.b0;
        case ROLE_B1:
            return value.b1;
        case ROLE_B2:
            return value.b2;
        case ROLE_A1:
            return value.a1;
        case ROLE_A2:
            return value.a2;
        }
        return QVariant();
    }

    // bool setData(const QModelIndex& index, const QVariant& value, int role);
};

class VisrControls : public EffectControls {
    Q_OBJECT

public:
    enum VisrRole {
        ROLE_Index = Qt::UserRole + 1,
        ROLE_Name,
        ROLE_Normalized,
        ROLE_Value,
        ROLE_StepSize,
        ROLE_DefaultNormalized,
        ROLE_Active,
    };
    Q_ENUM(VisrRole)

    VisrControls(QString name, QVector<VisrHelper::EffectParameter> list, QSharedPointer<Effect> _eff, QObject* parent = 0);
    int controlCount();

    int getIndex(int i);

    QString value(int i);
    void updateValue(int i, QString value, double normalized);
    void updateValueFromAutomation(int i, QString value, double normalized);

private:
    QMap<QString, uint> _mapTitles;
    QMap<uint, int> _mapIndexes;
    QVector<VisrHelper::EffectParameter> _list;
signals:
    void valueChanged();
    void sigDataChaged(uint32 index, QString type, double normalized);

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

class ObjectAutomation : public Automation {
    Q_OBJECT
public:
    explicit ObjectAutomation(int trackIndex,
        QString name, QString shortName, AreaInfo* areaInfo)
        : Automation(areaInfo, trackIndex, name, shortName)
        , _shortName(shortName)
    {
        setAutomatedNormalized(0.5);
    }

private:
    QString _shortName;
};

class ObjectAutomationItem : public AutomationItem {
    Q_OBJECT
public:
    explicit ObjectAutomationItem(int trackIndex, double defaultNormalized,
        QString title, QSharedPointer<VisrHelper> plugin, QSharedPointer<Automation> automation, AreaInfo* areaInfo)
        : AutomationItem(trackIndex, automation, areaInfo)
        , _plugin(plugin)
        , _title(title)
    {

        automation->setDefaultNormalized(defaultNormalized);
        automation->setAutomatedNormalized(defaultNormalized);
        automation->setNormalized(defaultNormalized);

        connect(this, &AutomationItem::sigAutomatedNormalizedChanged, this, &ObjectAutomationItem::sltVisrAutomatedNormalizedChanged);
        connect(this, &AutomationItem::sigNormalizedChanged, this, &ObjectAutomationItem::sltVisrNormalizedChanged);
    }

public Q_SLOTS:
    void sltVisrNormalizedChanged()
    {
        Q_EMIT sigVisrNormalizedChanged(_title);
    }
    void sltVisrAutomatedNormalizedChanged()
    {
        Q_EMIT sigVisrAutomatedNormalizedChanged(_title);
    }

public:
    double normalized2Value(double y)
    {

        return y * (100 - (-100)) + (-100);
    }
    double value2Normalized(double y)
    {

        return (y - (-100)) / (double)(100 - (-100));
    }

    QString normalized2Label(double y)
    {
        return QString::number(y * 100);
    }

private:
    QSharedPointer<VisrHelper> _plugin;
    QString _title;

signals:
    void sigVisrAutomatedNormalizedChanged(QString shortname);
    void sigVisrNormalizedChanged(QString shortname);
};

class CoeffAutomation : public Automation {
    Q_OBJECT
public:
    explicit CoeffAutomation(int trackIndex,
        QString name, QString shortName, AreaInfo* areaInfo)
        : Automation(areaInfo, trackIndex, name, shortName)
        , _shortName(shortName)
    {
    }

private:
    QString _shortName;
};

class CoeffAutomationItem : public AutomationItem {
    Q_OBJECT
public:
    explicit CoeffAutomationItem(int trackIndex, double defaultNormalized,
        QString title, QSharedPointer<VisrHelper> plugin, QSharedPointer<Automation> automation, AreaInfo* areaInfo)
        : AutomationItem(trackIndex, automation, areaInfo)
        , _plugin(plugin)
        , _title(title)
    {

        automation->setDefaultNormalized(defaultNormalized);
        automation->setAutomatedNormalized(defaultNormalized);
        automation->setNormalized(defaultNormalized);

        connect(this, &AutomationItem::sigAutomatedNormalizedChanged, this, &CoeffAutomationItem::sltVisrAutomatedNormalizedChanged);
        connect(this, &AutomationItem::sigNormalizedChanged, this, &CoeffAutomationItem::sltVisrNormalizedChanged);
    }

public Q_SLOTS:
    void sltVisrNormalizedChanged()
    {
        Q_EMIT sigVisrNormalizedChanged(_title);
    }
    void sltVisrAutomatedNormalizedChanged()
    {
        Q_EMIT sigVisrAutomatedNormalizedChanged(_title);
    }

public:
    double normalized2Value(double y)
    {

        return 20 * y - 10;
    }
    double value2Normalized(double y)
    {

        return (y + 10) / 20.0;
    }

    QString normalized2Label(double y)
    {
        return QString::number(20 * y - 10);
    }

private:
    QSharedPointer<VisrHelper> _plugin;
    QString _title;

signals:
    void sigVisrAutomatedNormalizedChanged(QString shortname);
    void sigVisrNormalizedChanged(QString shortname);
};

class VisrEffect : public Effect {
    Q_OBJECT
    Q_PROPERTY(VisrHelper* plugin READ getPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QString loadSpeakerConfig READ loadSpeakerConfig WRITE setLoadSpeakerConfig NOTIFY loadSpeakerConfigChanged)
    Q_PROPERTY(SpeakerModel* speakerModel READ speakerModel NOTIFY speakerCountChanged)
    Q_PROPERTY(ReverbFilters* reverbFilters READ reverbFilters NOTIFY reverbFiltersChanged)
    Q_PROPERTY(int speakerCount READ speakerCount NOTIFY speakerCountChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

    Q_PROPERTY(bool reverbEnabled READ reverbEnabled WRITE setReverbEnabled NOTIFY reverbEnabledChanged)
    Q_PROPERTY(bool frequencyDependentPanning READ frequencyDependentPanning WRITE setFrequencyDependentPanning)

    Q_PROPERTY(QString lateDiffusionFilters READ lateDiffusionFilters WRITE setLateDiffusionFilters)

    Q_PROPERTY(int numReverbObjects READ numReverbObjects)
    Q_PROPERTY(int discreteReflectionsPerObject READ discreteReflectionsPerObject)
    Q_PROPERTY(double lateFilterLengthSeconds READ lateFilterLengthSeconds WRITE setLateFilterLengthSeconds)
    Q_PROPERTY(double maximumDiscreteReflectionDelay READ maximumDiscreteReflectionDelay WRITE setMaximumDiscreteReflectionDelay)

public:
    VisrEffect(AreaInfo* areaInfo = Q_NULLPTR, QObject* parent = 0);
    ~VisrEffect();
    VisrHelper* getPlugin();
    QSharedPointer<VisrHelper> plugin();
    QString loadSpeakerConfig();
    void setLoadSpeakerConfig(QString config);

    int speakerCount() const;
    SpeakerModel* speakerModel() const;

    Q_INVOKABLE float getValue(QString title);
    Q_INVOKABLE void setValue(QString shorName, float value);

    Q_INVOKABLE void reload();

    bool enabled() const;
    void setEnabled(bool newEnabled);

    bool frequencyDependentPanning() const;
    void setFrequencyDependentPanning(bool newEnabled);

    QString lateDiffusionFilters() const;
    void setLateDiffusionFilters(QString path);

    int numReverbObjects() const;
    int numberOfBiquadsos() const;
    int discreteReflectionsPerObject() const;

    double lateFilterLengthSeconds() const;
    void setLateFilterLengthSeconds(double seconds);

    double maximumDiscreteReflectionDelay() const;
    void setMaximumDiscreteReflectionDelay(double seconds);

    bool reverbEnabled() const;
    void setReverbEnabled(bool enabled);

    ReverbFilters* reverbFilters();

    Q_INVOKABLE QStringList lateReverbLevels();
    Q_INVOKABLE QStringList lateReverbAttackTime();
    Q_INVOKABLE QStringList lateReverbDecayconst();
    Q_INVOKABLE double lateReverbDelay() const;

    Q_INVOKABLE void setLateReverbLevels(int index, QString value);
    Q_INVOKABLE void setLateReverbAttackTime(int index, QString value);
    Q_INVOKABLE void setLateReverbDecayconst(int index, QString value);
    Q_INVOKABLE void setLateReverbDelay(float value);

    Q_INVOKABLE void setReverbFilterB0(int index, QString value);
    Q_INVOKABLE void setReverbFilterB1(int index, QString value);
    Q_INVOKABLE void setReverbFilterB2(int index, QString value);
    Q_INVOKABLE void setReverbFilterA1(int index, QString value);
    Q_INVOKABLE void setReverbFilterA2(int index, QString value);

    Q_INVOKABLE void applyReverb(int index);

    Q_INVOKABLE void setDiscreteReflectionPositionX(int index, QString value);
    Q_INVOKABLE void setDiscreteReflectionPositionY(int index, QString value);
    Q_INVOKABLE void setDiscreteReflectionPositionZ(int index, QString value);

    Q_INVOKABLE float getDiscreteReflectionPositionX(int index);
    Q_INVOKABLE float getDiscreteReflectionPositionY(int index);
    Q_INVOKABLE float getDiscreteReflectionPositionZ(int index);

    Q_INVOKABLE float getDiscreteReflectionDelay(int index);
    Q_INVOKABLE float getDiscreteReflectionLevel(int index);
    Q_INVOKABLE void setDiscreteReflectionDelay(int index, QString value);
    Q_INVOKABLE void setDiscreteReflectionLevel(int index, QString value);

private:
    QStringList _lateReverbLevels;
    QStringList _lateReverbAttackTime;
    QStringList _lateReverbDecayconst;
    double _lateReverbDelay;

    double _maximumDiscreteReflectionDelay = 0.04;
    double _lateFilterLengthSeconds = 0.04;
    int _numReverbObjects = 1;
    bool _reverbEnabled = false;
    QString _lateDiffusionFilters;
    QString _loadSpeakerConfig;
    bool _frequencyDependentPanning = false;
    QSharedPointer<VisrControls> _controls;
    QSharedPointer<VisrHelper> m_plugin;
    QMap<QString, QSharedPointer<ObjectAutomationItem>> _automationItems;
    QMap<QString, QSharedPointer<ObjectAutomation>> _automations;

    QMap<QString, QSharedPointer<CoeffAutomationItem>> _coeffAutomationItems;
    QMap<QString, QSharedPointer<CoeffAutomation>> _coeffAutomations;

    AreaInfo* _areaInfo;
    bool _enabled;

    QSharedPointer<ReverbFilters> _reverbFilters;

private Q_SLOTS:
    void sltControlVisibleChanged(bool visible);
    void sltAutomatedValueChanged(QString shorName);
    void sltValueChanged(QString shorName);

    void sltUpdateParams(int index, QString value, double normalized);
    void sltDataChagedFromModel(int index, QString type, double value);
    void sltDataChagedFromView(int index, QString type, double value);

signals:
    void positionChanged();
    void speakerCountChanged();
    void pluginChanged();
    void loadSpeakerConfigChanged();
    void reverbFiltersChanged();
    void enabledChanged();
    void reverbEnabledChanged();

public:
    QString name() const;
    QString type() const;
    QSharedPointer<EffectControls> controls() const;
    VisrControls* getControls() const;
    bool initialize(int trackIndex, QSharedPointer<Effect> effect);
    bool processAudioBuffer(sampleFrame* _buf);
    bool controlVisible();
    QSharedPointer<Automation> automation(QString name);
    QSharedPointer<AutomationItem> automationItem(QString name);
    bool contains(QString name);
    bool isAutomated(QString name);
    bool hasExternalControl();
    QMap<QString, QSharedPointer<ObjectAutomation> > automations() const;
};

#endif // VISREFFECT_H
