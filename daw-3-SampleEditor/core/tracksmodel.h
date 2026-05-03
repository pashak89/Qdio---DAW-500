#ifndef TRACKSMODEL_H
#define TRACKSMODEL_H

#include "audio/audioengine.h"
#include "audio/engine.h"
#include "audio/sampletrack.h"
#include "measure.h"
#include "sampleeditor.h"
#include "ui/automationitem.h"
#include "ui/backgrounditem.h"
#include "ui/beatbaritem.h"
#include "ui/clipshapeitem.h"
#include "ui/envelopeitem.h"
#include "ui/hotspotitem.h"
#include "ui/indicatoritem.h"
#include "ui/loopitem2.h"
#include "ui/markerareaitem.h"
#include "ui/plotitem.h"
#include "ui/scrubareaitem.h"
#include "ui/timeruleritem.h"
#include "ui/trackitem.h"
#include "ui/warpareaitem.h"
#include "warp.h"
#include <QAbstractListModel>
#include <QJsonArray>
#include <QQmlEngine>
#include <QQuickItem>
#include <QTimer>
#include <ui/timeruleritem.h>

class VisrEffect;
class MenuItem {
public:
    QString title;
    QVector<MenuItem> subItems;

    MenuItem(const QString& title, const QVector<MenuItem>& subItems = {})
        : title(title)
        , subItems(subItems)
    {
    }
};

class MenuModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum MenuRoles {
        TitleRole = Qt::UserRole + 1,
        SubItemsRole
    };

    explicit MenuModel(QObject* parent = nullptr);
    ~MenuModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;
    int count() const
    {
        return menuItems.size();
    }

    Q_INVOKABLE void addMenuItem(const QString& title, const QStringList& actions);

    Q_INVOKABLE void removeMenuItem(int index);
    Q_INVOKABLE void removeMenuItem(QString title);

    Q_INVOKABLE QVariant getTitle(int index) const;
    Q_INVOKABLE QStringList getActions(int index) const;

private:
    QVector<MenuItem> menuItems;
signals:
    void countChanged();
};

class TrackProperty : public QObject {
    Q_OBJECT
public:
    enum TrackRole {
        ROLE_Type = Qt::UserRole + 1,
        ROLE_TrackIndex,
        ROLE_PreText,
        ROLE_Collapsed,
        ROLE_ParentSet,
        ROLE_ChildrenSet,
        ROLE_TrackActivated,
        ROLE_SelectedSolo,
        ROLE_PartialySolo,
        ROLE_Solo,
        ROLE_SelectedTrackDependSolo,
        ROLE_CoverBound,
        ROLE_EditName,
        ROLE_InDraggingCounter,
        ROLE_SelectedTrack,
        ROLE_ChannelsCount,
        ROLE_RowsExpanded,
        ROLE_ImageURL,
        ROLE_TrackRect,
        ROLE_TotalTrackRect,

        ROLE_Clear,

        ROLE_TruePeak,

        ROLE_VolumeValue,
        ROLE_VolumeNormalized,
        ROLE_VolumeDefaultNormalized,
        ROLE_AutomatedVolumeNormalized,
        ROLE_AutomatedVolumeValue,
        ROLE_AutomatedVolumeActive,
        ROLE_AutomatedVolumeEnabled,

        ROLE_PanValue,
        ROLE_PanNormalized,
        ROLE_PanDefaultNormalized,
        ROLE_AutomatedPanNormalized,
        ROLE_AutomatedPanValue,
        ROLE_AutomatedPanActive,
        ROLE_AutomatedPanEnabled,

        ROLE_SpeakerOnValue,
        ROLE_SpeakerOnNormalized,
        ROLE_SpeakerOnDefaultNormalized,
        ROLE_AutomatedSpeakerOnNormalized,
        ROLE_AutomatedSpeakerOnValue,
        ROLE_AutomatedSpeakerOnActive,
        ROLE_AutomatedSpeakerOnEnabled,

        ROLE_AutomatedKeyFramesOnActive,

        ROLE_Mute,
        ROLE_PanSelected,
        ROLE_VolumeSelected,
        ROLE_SpeakerOnSelected,

        ROLE_Color,
        ROLE_RowsExpandedCount,
        ROLE_AutomationLaneEnabled,
        ROLE_AutomationLaneCount,
        ROLE_AutomationMenuTitle,
        ROLE_AutomationSubMenuTitle,

        ROLE_Value,
        ROLE_Normalized,
        ROLE_AutomatedNormalized,
        ROLE_AutomatedValue,
        ROLE_AutomatedActive,
        ROLE_AutomatedEnabled,

        ROLE_DefaultNormalized,

        ROLE_VISR,

        ROLE_ObjectEnabled,

        ROLE_KeyFrameType,
        ROLE_InputChannelsCount,
        ROLE_OutputChannelsCount,

    };
    Q_ENUM(TrackRole)

    enum TrackPropertyLevel {
        TrackPropertyLevel_0,
        TrackPropertyLevel_1,
        TrackPropertyLevel_2,

    };
    TrackProperty(TrackPropertyLevel level, QSharedPointer<TrackItem> trackItem, TrackProperty* parent = nullptr);

    ~TrackProperty();

    void appendChild(TrackProperty* item);
    void removeChild(int index);

    TrackProperty* child(int row);

    int childCount() const;

    int row(TrackProperty* item) const;

    QList<TrackProperty*> getChildren() const;

    TrackProperty* parent();

    QSharedPointer<TrackItem> trackItem() const;

    void automationChanged();

    TrackPropertyLevel level() const;

    QSharedPointer<MenuModel> menuModel() const;

private slots:

    void sltAutomationTrackPanChanged();
    void sltPanEnabledChanged();
    void panAutomatedLineChanged();

    void sltAutomationTrackVolumeChanged();
    void volumeAutomatedLineChanged();
    void sltVolumeEnabledChanged();

    void sltAutomationTrackSpeakerOnChanged();
    void speakerOnAutomatedLineChanged();
    void sltSpeakerOnEnabledChanged();

    void KeyFrameAutomatedLineChanged();

    void sltRowsExpandedChanged(int _index);

    void sltRectChanged();
    void sltAutomationLaneCountChanged(int _index);

    void sltEnableChanged();
    void sltLineChanged();
    void sltValueChanged();

private:
    TrackPropertyLevel _level;
    QSharedPointer<TrackItem> _trackItem = Q_NULLPTR;
    QList<TrackProperty*> childItems;
    TrackProperty* parentItem;
    QSharedPointer<MenuModel> _menuModel;

signals:
    void dataChanged(TrackProperty* item, QVector<int> roles);
    void sigUpdate();
};

class TracksModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit TracksModel(AreaInfo* info = Q_NULLPTR, QObject* parent = Q_NULLPTR);
    ~TracksModel();

    // This function is Enable or Disable for all trackitems from clipArea
    void setAutomationEnabed(bool enabled);

    // This function is Visible or Invisible for all trackitems from clipArea
    void setEnvelopesVisible(int i);

    // This function is for updating while playback is moving
    void automatedValueChanged(int i, double time);

    // For updating attenuide the mixers when played back is stopped #1218
    void updateTruePeak(int i);

    //    // Enable Vist for all Tracks
    //    void setVisrEnabled(int trackIndex, bool enabled);
    //    bool visrEnabled(int trackIndex);

    // Enable Vist for all Tracks
    void setLoadSpeakerConfig(QString path);

    Q_INVOKABLE void toggleTrackActivator(int i);
    Q_INVOKABLE void toggleSolo(int i);

    Q_INVOKABLE TruePeakModel* truePeakModel(int trackIndex);
    Q_INVOKABLE MenuModel* effectMenuModel(int trackIndex);
    Q_INVOKABLE AutomationLaneModel* automationLaneModel(int trackIndex);
    Q_INVOKABLE EffectChain* effectChain(int trackIndex);
    Q_INVOKABLE VisrEffect* visrEffect(int trackIndex);
    Q_INVOKABLE TrackItem* getTrackItem(int i);

    Q_INVOKABLE QMap<qint64, int> keyFrames(int trackIndex);

    Q_INVOKABLE QMap<qint64, double> objectPositionX(int trackIndex);
    Q_INVOKABLE QMap<qint64, double> objectPositionY(int trackIndex);
    Q_INVOKABLE QMap<qint64, double> objectPositionZ(int trackIndex);

    Q_INVOKABLE bool objectModelEnabled(int trackIndex);
    Q_INVOKABLE void setObjectModelEnabled(int trackIndex, bool enabled);
    Q_INVOKABLE void setObjectPosition(int trackIndex, double x, double y, double z);
    Q_INVOKABLE void saveObjectPosition(int trackIndex, double x, double y, double z);

    Q_INVOKABLE void saveKeyFrameEdited(int trackIndex, QString status);

    Q_INVOKABLE QJsonObject performObjectPosition(QJsonObject obj);

    Q_INVOKABLE void addKeyFrame(int trackIndex, qint64 time, int type);
    Q_INVOKABLE void removeKeyFrame(int trackIndex, qint64 time);

    Q_INVOKABLE void speakerOnAutomationSelected(int _index);
    Q_INVOKABLE void panAutomationSelected(int _index);
    Q_INVOKABLE void volumeAutomationSelected(int _index);
    Q_INVOKABLE void noneAutomationSelected(int _index);
    Q_INVOKABLE bool isAutomated(int _index, QString menu, QString subMenu);
    Q_INVOKABLE bool isSelected(int _index, QString menu, QString subMenu);
    Q_INVOKABLE bool isEnabled(int _index, QString menu, QString subMenu);

    Q_INVOKABLE void setAutomationLaneEnabled(int _index, bool enabled);
    Q_INVOKABLE void setAutomationEnabled(int _index, QString menu, QString subMenu, bool enabled);

    Q_INVOKABLE void setAutomationMenuTitle(int trackindex, QString menu, QString subMenu);

    Q_INVOKABLE void automationClear(int trackindex, QString menu, QString subMenu);

    // This function is for save normalized in Undo queue from UI
    Q_INVOKABLE void saveUndoNoramized(int trackindex, QString menu, QString subMenu);
    Q_INVOKABLE void performOperation(QJsonObject obj);
    Q_INVOKABLE void setCurrentObjectStatus(int trackIndex, QString currentChanges);
    // Q_INVOKABLE void setAutomationEnable(int trackindex, QString menu, QString subMenu);

    // Set for all Items
    void setCoverBound(bool state);

    QSharedPointer<TrackItem> trackItem(int i);
    Q_INVOKABLE int size();

    Q_INVOKABLE QRect getTrackTotalRect(int i);

private:
    AreaInfo* m_areaInfo;
    QMap<int, QString> m_currentObjectChanges;

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

    TrackProperty* _rootItem;

    QHash<int, QByteArray> m_roleNames;

private slots:

    void sltSelectedChanged(int _index);

public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex& index) const override;

    void updateTracksHeight();
    void updateTracksHeight(int i);
    void addTrackItem(QSharedPointer<TrackItem> trackitem);
    void removeTrackItem(int trackIndex);
    void sltDataChanged(TrackProperty* item, QVector<int> roles);
    void effectAdded(int trackIndex, int effectIndex);
    void effectRemoved(int trackIndex, int effectIndex);
    void effectUpdated(int trackIndex, int effectIndex);

    void visrAdded(int trackIndex);
    void visrRemoved(int trackIndex);
    Q_INVOKABLE bool isVisrLoaded(int trackIndex);

    virtual QVariant data(const QModelIndex& index, int role) const;

    // QAbstractItemModel interface
public:
    bool setData(const QModelIndex& index, const QVariant& value, int role);

    TrackProperty* rootItem() const;

signals:
    void sigUpdate();
    void sigClipChanged();
    void sigEffectAdded(int trackIndex, int effectIndex);
    void sigEffectUpdated(int trackIndex, int effectIndex);
    void sigEffectRemoved(int trackIndex, int effectIndex);

    void sigAutomationLaneEnabled(int trackIndex, bool enabled);

    void keyFramesChanged(int trackIndex);
};

#endif // TRACKSMODEL_H
