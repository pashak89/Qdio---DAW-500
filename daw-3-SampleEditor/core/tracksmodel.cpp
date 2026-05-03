#include "tracksmodel.h"
#include "audio/mixer.h"
#include <audio/effectcontrols.h>
#include <eq/visreffect.h>
TrackProperty::TrackProperty(TrackPropertyLevel level, QSharedPointer<TrackItem> trackItem, TrackProperty* parent)
    : parentItem(parent)
    , _trackItem(trackItem)
    , _level(level)
{
    _menuModel = QSharedPointer<MenuModel>(new MenuModel(this));
}

TrackProperty::~TrackProperty()
{
    _menuModel.clear();
    childItems.clear();
}

void TrackProperty::appendChild(TrackProperty* _item)
{
    childItems.append(_item);

    connect(_item->trackItem().data(), &TrackItem::sigRectChanged, this, &TrackProperty::sltRectChanged);
    connect(_item->trackItem().data(), &TrackItem::sigAutomationLaneCountChanged, this, &TrackProperty::sltAutomationLaneCountChanged);

    connect(_item->trackItem().data(), &TrackItem::rowsExpandedChanged, this, &TrackProperty::sltRowsExpandedChanged);

    connect(_item->trackItem()->volumeAutomationItem().data(), &AutomationItem::sigNormalizedChanged, this, &TrackProperty::sltAutomationTrackVolumeChanged);
    connect(_item->trackItem()->volumeAutomationItem().data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &TrackProperty::sltAutomationTrackVolumeChanged);
    connect(_item->trackItem()->volumeAutomationItem()->automation().data(), &Automation::sigLinesChanged, this, &TrackProperty::volumeAutomatedLineChanged);
    connect(_item->trackItem()->volumeAutomationItem()->automation().data(), &Automation::enabledChanged, this, &TrackProperty::sltVolumeEnabledChanged);
    connect(_item->trackItem()->volumeAutomationItem()->automation().data(), &Automation::valueChanged, this, &TrackProperty::sltAutomationTrackVolumeChanged);

    connect(_item->trackItem()->panAutomationItem().data(), &AutomationItem::sigNormalizedChanged, this, &TrackProperty::sltAutomationTrackPanChanged);
    connect(_item->trackItem()->panAutomationItem().data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &TrackProperty::sltAutomationTrackPanChanged);
    connect(_item->trackItem()->panAutomationItem()->automation().data(), &Automation::sigLinesChanged, this, &TrackProperty::panAutomatedLineChanged);
    connect(_item->trackItem()->panAutomationItem()->automation().data(), &Automation::enabledChanged, this, &TrackProperty::sltPanEnabledChanged);
    connect(_item->trackItem()->panAutomationItem()->automation().data(), &Automation::valueChanged, this, &TrackProperty::sltAutomationTrackPanChanged);

    connect(_item->trackItem()->speakerOnAutomationItem().data(), &AutomationItem::sigNormalizedChanged, this, &TrackProperty::sltAutomationTrackSpeakerOnChanged);
    connect(_item->trackItem()->speakerOnAutomationItem().data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &TrackProperty::sltAutomationTrackSpeakerOnChanged);
    connect(_item->trackItem()->speakerOnAutomationItem()->automation().data(), &Automation::sigLinesChanged, this, &TrackProperty::speakerOnAutomatedLineChanged);
    connect(_item->trackItem()->speakerOnAutomationItem()->automation().data(), &Automation::enabledChanged, this, &TrackProperty::sltSpeakerOnEnabledChanged);
    connect(_item->trackItem()->speakerOnAutomationItem()->automation().data(), &Automation::valueChanged, this, &TrackProperty::sltAutomationTrackSpeakerOnChanged);

    connect(_item->trackItem()->keyFramesAutomationItem()->automation().data(), &Automation::sigLinesChanged, this, &TrackProperty::KeyFrameAutomatedLineChanged);
}

void TrackProperty::removeChild(int index)
{
    auto item = childItems.takeAt(index);
    while (item->childCount() > 0) {
        item->removeChild(0);
    }
    if (item->trackItem()) {
        item->trackItem().clear();
    }
    delete item;
    item = Q_NULLPTR;
}

TrackProperty* TrackProperty::child(int row)
{
    return childItems.value(row);
}

int TrackProperty::childCount() const
{
    return childItems.count();
}

int TrackProperty::row(TrackProperty* item) const
{
    if (parentItem) {
        return parentItem->childItems.indexOf(const_cast<TrackProperty*>(this));
    }
    return 0;
}

QList<TrackProperty*> TrackProperty::getChildren() const
{
    return childItems;
}

TrackProperty* TrackProperty::parent()
{
    return parentItem;
}

QSharedPointer<TrackItem> TrackProperty::trackItem() const
{
    return _trackItem;
}

void TrackProperty::automationChanged()
{

    auto effectItem = trackItem()->effectAutomationItem();

    if (effectItem) {

        disconnect(effectItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &TrackProperty::sltValueChanged);
        disconnect(effectItem.data(), &AutomationItem::sigNormalizedChanged, this, &TrackProperty::sltValueChanged);
        disconnect(effectItem->automation().data(), &Automation::sigLinesChanged, this, &TrackProperty::sltLineChanged);
        disconnect(effectItem->automation().data(), &Automation::enabledChanged, this, &TrackProperty::sltEnableChanged);
        disconnect(effectItem->automation().data(), &Automation::valueChanged, this, &TrackProperty::sltValueChanged);

        connect(effectItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &TrackProperty::sltValueChanged);
        connect(effectItem.data(), &AutomationItem::sigNormalizedChanged, this, &TrackProperty::sltValueChanged);
        connect(effectItem->automation().data(), &Automation::sigLinesChanged, this, &TrackProperty::sltLineChanged);
        connect(effectItem->automation().data(), &Automation::enabledChanged, this, &TrackProperty::sltEnableChanged);
        connect(effectItem->automation().data(), &Automation::valueChanged, this, &TrackProperty::sltValueChanged);
    }

    auto visrItem = trackItem()->visrAutomationItem();
    if (visrItem) {
        disconnect(visrItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &TrackProperty::sltValueChanged);
        disconnect(visrItem.data(), &AutomationItem::sigNormalizedChanged, this, &TrackProperty::sltValueChanged);
        disconnect(visrItem->automation().data(), &Automation::sigLinesChanged, this, &TrackProperty::sltLineChanged);
        disconnect(visrItem->automation().data(), &Automation::enabledChanged, this, &TrackProperty::sltEnableChanged);
        disconnect(visrItem->automation().data(), &Automation::valueChanged, this, &TrackProperty::sltValueChanged);

        connect(visrItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &TrackProperty::sltValueChanged);
        connect(visrItem.data(), &AutomationItem::sigNormalizedChanged, this, &TrackProperty::sltValueChanged);
        connect(visrItem->automation().data(), &Automation::sigLinesChanged, this, &TrackProperty::sltLineChanged);
        connect(visrItem->automation().data(), &Automation::enabledChanged, this, &TrackProperty::sltEnableChanged);
        connect(visrItem->automation().data(), &Automation::valueChanged, this, &TrackProperty::sltValueChanged);
    }
}
void TrackProperty::sltEnableChanged()
{

    QVector<int> roles;
    roles.append(ROLE_AutomatedVolumeEnabled);
    roles.append(ROLE_AutomatedPanEnabled);
    roles.append(ROLE_AutomatedSpeakerOnEnabled);
    roles.append(ROLE_AutomatedEnabled);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}
void TrackProperty::sltLineChanged()
{

    auto _sender = (Automation*)sender();

    _sender->setEnabled1(true);

    QVector<int> roles;
    roles.append(ROLE_AutomatedVolumeActive);
    roles.append(ROLE_AutomatedPanActive);
    roles.append(ROLE_AutomatedSpeakerOnActive);

    roles.append(ROLE_AutomatedActive);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);

    if (!_sender->isAutomated()) {
        _sender->setAutomatedNormalized(_sender->normalized());
    }
}
void TrackProperty::sltValueChanged()
{

    QVector<int> roles;
    roles.append(ROLE_Value);
    roles.append(ROLE_Normalized);
    roles.append(ROLE_AutomatedValue);
    roles.append(ROLE_AutomatedNormalized);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}

QSharedPointer<MenuModel> TrackProperty::menuModel() const
{
    return _menuModel;
}

TrackProperty::TrackPropertyLevel TrackProperty::level() const
{
    return _level;
}

void TrackProperty::sltAutomationLaneCountChanged(int _index)
{

    QVector<int> roles;
    roles.append(ROLE_TotalTrackRect);
    roles.append(ROLE_AutomationLaneCount);
    roles.append(ROLE_TrackRect);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}

void TrackProperty::sltRectChanged()
{

    QVector<int> roles;
    roles.append(ROLE_TrackRect);
    roles.append(ROLE_TotalTrackRect);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}
void TrackProperty::sltRowsExpandedChanged(int _index)
{

    QVector<int> roles;
    roles.append(ROLE_RowsExpanded);
    roles.append(ROLE_RowsExpandedCount);
    roles.append(ROLE_TotalTrackRect);
    roles.append(ROLE_TrackRect);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}
void TrackProperty::sltAutomationTrackVolumeChanged()
{

    QVector<int> roles;
    roles.append(ROLE_AutomatedVolumeValue);
    roles.append(ROLE_AutomatedVolumeNormalized);
    roles.append(ROLE_VolumeValue);
    roles.append(ROLE_VolumeNormalized);
    roles.append(ROLE_AutomatedValue);
    roles.append(ROLE_AutomatedNormalized);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}

void TrackProperty::sltAutomationTrackSpeakerOnChanged()
{

    QVector<int> roles;
    roles.append(ROLE_AutomatedSpeakerOnValue);
    roles.append(ROLE_AutomatedSpeakerOnNormalized);
    roles.append(ROLE_SpeakerOnValue);
    roles.append(ROLE_SpeakerOnNormalized);
    roles.append(ROLE_AutomatedValue);
    roles.append(ROLE_AutomatedNormalized);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}

void TrackProperty::sltAutomationTrackPanChanged()
{

    QVector<int> roles;
    roles.append(ROLE_AutomatedPanValue);
    roles.append(ROLE_AutomatedPanNormalized);
    roles.append(ROLE_PanValue);
    roles.append(ROLE_PanNormalized);
    roles.append(ROLE_AutomatedValue);
    roles.append(ROLE_AutomatedNormalized);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}
void TrackProperty::volumeAutomatedLineChanged()
{

    auto _sender = (Automation*)sender();

    _sender->setEnabled1(true);
    QVector<int> roles;
    roles.append(ROLE_AutomatedVolumeActive);
    roles.append(ROLE_AutomatedActive);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);

    if (!_sender->isAutomated()) {
        _sender->setAutomatedNormalized(_sender->normalized());
    }
}

void TrackProperty::speakerOnAutomatedLineChanged()
{

    auto _sender = (Automation*)sender();

    _sender->setEnabled1(true);
    QVector<int> roles;
    roles.append(ROLE_AutomatedSpeakerOnActive);
    roles.append(ROLE_AutomatedActive);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);

    if (!_sender->isAutomated()) {
        _sender->setAutomatedNormalized(_sender->normalized());
    }
}

void TrackProperty::panAutomatedLineChanged()
{

    auto _sender = (Automation*)sender();

    _sender->setEnabled1(true);
    QVector<int> roles;
    roles.append(ROLE_AutomatedPanActive);
    roles.append(ROLE_AutomatedActive);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);

    if (!_sender->isAutomated()) {

        _sender->setAutomatedNormalized(_sender->normalized());
    }
}

void TrackProperty::sltVolumeEnabledChanged()
{

    QVector<int> roles;
    roles.append(ROLE_AutomatedVolumeEnabled);
    roles.append(ROLE_AutomatedEnabled);

    roles.append(ROLE_VolumeNormalized);
    roles.append(ROLE_VolumeValue);

    roles.append(ROLE_AutomatedVolumeNormalized);
    roles.append(ROLE_AutomatedVolumeValue);

    roles.append(ROLE_AutomatedNormalized);
    roles.append(ROLE_Normalized);
    roles.append(ROLE_Value);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}

void TrackProperty::sltSpeakerOnEnabledChanged()
{

    QVector<int> roles;
    roles.append(ROLE_AutomatedSpeakerOnEnabled);
    roles.append(ROLE_AutomatedEnabled);

    roles.append(ROLE_SpeakerOnNormalized);
    roles.append(ROLE_SpeakerOnValue);

    roles.append(ROLE_AutomatedSpeakerOnNormalized);
    roles.append(ROLE_AutomatedSpeakerOnValue);

    roles.append(ROLE_AutomatedNormalized);
    roles.append(ROLE_Normalized);
    roles.append(ROLE_Value);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}

void TrackProperty::KeyFrameAutomatedLineChanged()
{

    auto _sender = (Automation*)sender();

    _sender->setEnabled1(true);
    QVector<int> roles;
    roles.append(ROLE_AutomatedKeyFramesOnActive);
    roles.append(ROLE_AutomatedActive);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);

    if (!_sender->isAutomated()) {
        _sender->setAutomatedNormalized(_sender->normalized());
    }
}

void TrackProperty::sltPanEnabledChanged()
{

    QVector<int> roles;
    roles.append(ROLE_AutomatedPanEnabled);
    roles.append(ROLE_AutomatedEnabled);

    roles.append(ROLE_PanNormalized);
    roles.append(ROLE_PanValue);

    roles.append(ROLE_AutomatedPanNormalized);
    roles.append(ROLE_AutomatedPanValue);

    roles.append(ROLE_AutomatedNormalized);
    roles.append(ROLE_Normalized);
    roles.append(ROLE_Value);

    Q_EMIT sigUpdate();
    Q_EMIT dataChanged(this, roles);
}

TracksModel::TracksModel(AreaInfo* info, QObject* parent)
    : m_areaInfo(info)
    , QAbstractListModel(parent)
{
    m_roleNames[TrackProperty::ROLE_Type] = "type";
    m_roleNames[TrackProperty::ROLE_TrackIndex] = "trackIndex";
    m_roleNames[TrackProperty::ROLE_PreText] = "pretext";
    m_roleNames[TrackProperty::ROLE_Collapsed] = "isCollapsed";
    m_roleNames[TrackProperty::ROLE_ParentSet] = "in_set_parent";
    m_roleNames[TrackProperty::ROLE_ChildrenSet] = "sub_set_children";

    m_roleNames[TrackProperty::ROLE_TrackActivated] = "trackActivated";
    m_roleNames[TrackProperty::ROLE_SelectedSolo] = "selectSolo";
    m_roleNames[TrackProperty::ROLE_PartialySolo] = "partialySolo";
    m_roleNames[TrackProperty::ROLE_Solo] = "solo";
    m_roleNames[TrackProperty::ROLE_SelectedTrackDependSolo] = "selectTrackDependSolo";
    m_roleNames[TrackProperty::ROLE_CoverBound] = "coverBound";
    m_roleNames[TrackProperty::ROLE_EditName] = "editName";
    m_roleNames[TrackProperty::ROLE_InDraggingCounter] = "inDraggingCounter";
    m_roleNames[TrackProperty::ROLE_SelectedTrack] = "selectedTrack";
    m_roleNames[TrackProperty::ROLE_ChannelsCount] = "channelCount";
    m_roleNames[TrackProperty::ROLE_RowsExpanded] = "rowsExpanded";
    m_roleNames[TrackProperty::ROLE_ImageURL] = "imgURL";
    m_roleNames[TrackProperty::ROLE_TrackRect] = "trackRect";
    m_roleNames[TrackProperty::ROLE_TotalTrackRect] = "totalTrackRect";
    m_roleNames[TrackProperty::ROLE_Clear] = "clear";

    m_roleNames[TrackProperty::ROLE_TruePeak] = "truePeak";

    m_roleNames[TrackProperty::ROLE_VolumeValue] = "volumeValue";
    m_roleNames[TrackProperty::ROLE_VolumeNormalized] = "volumeNormalized";
    m_roleNames[TrackProperty::ROLE_VolumeDefaultNormalized] = "volumeDefaultNormalized";
    m_roleNames[TrackProperty::ROLE_AutomatedVolumeNormalized] = "automatedVolumeNormalized";

    m_roleNames[TrackProperty::ROLE_PanValue] = "panValue";
    m_roleNames[TrackProperty::ROLE_PanNormalized] = "panNormalized";
    m_roleNames[TrackProperty::ROLE_PanDefaultNormalized] = "panDefaultNormalized";
    m_roleNames[TrackProperty::ROLE_AutomatedPanNormalized] = "automatedPanNormalized";

    m_roleNames[TrackProperty::ROLE_SpeakerOnValue] = "speakerOnValue";
    m_roleNames[TrackProperty::ROLE_SpeakerOnNormalized] = "speakerOnNormalized";
    m_roleNames[TrackProperty::ROLE_SpeakerOnDefaultNormalized] = "speakerOnDefaultNormalized";
    m_roleNames[TrackProperty::ROLE_AutomatedSpeakerOnNormalized] = "automatedSpeakerOnNormalized";

    m_roleNames[TrackProperty::ROLE_AutomatedVolumeValue] = "automatedVolumeValue";
    m_roleNames[TrackProperty::ROLE_AutomatedVolumeActive] = "automatedVolumeActive";
    m_roleNames[TrackProperty::ROLE_AutomatedVolumeEnabled] = "automatedVolumeEnabled";
    m_roleNames[TrackProperty::ROLE_AutomatedPanValue] = "automatedPanValue";
    m_roleNames[TrackProperty::ROLE_AutomatedPanActive] = "automatedPanActive";
    m_roleNames[TrackProperty::ROLE_AutomatedPanEnabled] = "automatedPanEnabled";
    m_roleNames[TrackProperty::ROLE_AutomatedSpeakerOnValue] = "automatedSpeakerOnValue";
    m_roleNames[TrackProperty::ROLE_AutomatedSpeakerOnActive] = "automatedSpeakerOnActive";
    m_roleNames[TrackProperty::ROLE_AutomatedSpeakerOnEnabled] = "automatedSpeakerOnEnabled";

    m_roleNames[TrackProperty::ROLE_Mute] = "mute";
    m_roleNames[TrackProperty::ROLE_PanSelected] = "panSelected";
    m_roleNames[TrackProperty::ROLE_VolumeSelected] = "volumeSelected";
    m_roleNames[TrackProperty::ROLE_SpeakerOnSelected] = "speakerOnSelected";

    m_roleNames[TrackProperty::ROLE_Color] = "color";
    m_roleNames[TrackProperty::ROLE_RowsExpandedCount] = "rowsExpandedCount";
    m_roleNames[TrackProperty::ROLE_AutomationLaneEnabled] = "automationLaneEnabled";
    m_roleNames[TrackProperty::ROLE_AutomationLaneCount] = "automationLaneCount";
    m_roleNames[TrackProperty::ROLE_AutomationMenuTitle] = "automationMenuTitle";
    m_roleNames[TrackProperty::ROLE_AutomationSubMenuTitle] = "automationSubMenuTitle";

    m_roleNames[TrackProperty::ROLE_Value] = "value";
    m_roleNames[TrackProperty::ROLE_Normalized] = "normalized";
    m_roleNames[TrackProperty::ROLE_AutomatedNormalized] = "automatedNormalized";
    m_roleNames[TrackProperty::ROLE_AutomatedValue] = "automatedValue";
    m_roleNames[TrackProperty::ROLE_AutomatedActive] = "automatedActive";
    m_roleNames[TrackProperty::ROLE_AutomatedEnabled] = "automatedEnabled";

    m_roleNames[TrackProperty::ROLE_DefaultNormalized] = "defaultNormalized";
    m_roleNames[TrackProperty::ROLE_VISR] = "visr";

    m_roleNames[TrackProperty::ROLE_ObjectEnabled] = "objectEnabled";

    m_roleNames[TrackProperty::ROLE_KeyFrameType] = "keyFrameType";

    m_roleNames[TrackProperty::ROLE_InputChannelsCount] = "inputChannelsCount";
    m_roleNames[TrackProperty::ROLE_OutputChannelsCount] = "outputChannelsCount";

    _rootItem = new TrackProperty(TrackProperty::TrackPropertyLevel_0, Q_NULLPTR);
}

TracksModel::~TracksModel()
{
    qDebug() << "~TracksModel()";
}

void TracksModel::setAutomationEnabed(bool enabled)
{
    for (int i = 0; i < _rootItem->childCount(); i++) {
        _rootItem->child(i)->trackItem()->panAutomationItem()->setShowEnvelopes(enabled);
        _rootItem->child(i)->trackItem()->volumeAutomationItem()->setShowEnvelopes(enabled);
        _rootItem->child(i)->trackItem()->speakerOnAutomationItem()->setShowEnvelopes(enabled);

        sltDataChanged(_rootItem->child(i), QVector<int>() << TrackProperty::ROLE_AutomationLaneEnabled);

        if (_rootItem->child(i)->trackItem()->automationLaneModel()->count() > 0) {
            _rootItem->child(i)->trackItem()->totalRowExpandedChanged();

            sltDataChanged(_rootItem->child(i), QVector<int>() << TrackProperty::ROLE_TotalTrackRect << TrackProperty::ROLE_TrackRect);
        }
    }
}

void TracksModel::setEnvelopesVisible(int i)
{
    //    for (int i = 0; i < _rootItem->childCount(); i++) {

    //        auto trackItem = _rootItem->child(i)->trackItem();
    //        if (trackItem->getAutomationMenuTitle() == "Pan") {
    //            trackItem->panAutomationItem()->setShowEnvelopes(true);
    //        }
    //        if (trackItem->getAutomationMenuTitle() == "Volume") {
    //            trackItem->volumeAutomationItem()->setShowEnvelopes(true);
    //        }
    //        if (trackItem->getAutomationMenuTitle() == "KeyFrames") {
    //            trackItem->keyFramesItem()->setKeyFramesEnabled(true);
    //        }

    //        // Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_AutomationLaneEnabled);
    //    }
    //    if (_trackItems[i]->automationLaneModel()->count() > 0) {
    //        _trackItems[i]->totalRowExpandedChanged();
    //        Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_TotalTrackRect);
    //        Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_TrackRect);
    //    }
}

void TracksModel::automatedValueChanged(int i, double time)
{

    if (i >= 0 && i < _rootItem->childCount()) {

        auto trackItem = _rootItem->child(i)->trackItem();
        auto volumeAutomation = trackItem->volumeAutomationItem()->automation();
        auto panAutomation = trackItem->panAutomationItem()->automation();
        auto speakerOnAutomation = trackItem->speakerOnAutomationItem()->automation();

        double value = 0;
        double Y = 0;

        if (panAutomation->isAutomated()) {
            Y = volumeAutomation->getY(time);
            volumeAutomation->setAutomatedNormalized(Y);

            sltDataChanged(_rootItem->child(i),
                QVector<int>() << TrackProperty::ROLE_AutomatedVolumeValue
                               << TrackProperty::ROLE_AutomatedVolumeNormalized
                               << TrackProperty::ROLE_AutomatedNormalized
                               << TrackProperty::ROLE_AutomatedValue);
        }
        if (panAutomation->isAutomated()) {
            Y = panAutomation->getY(time);

            panAutomation->setAutomatedNormalized(Y);

            sltDataChanged(_rootItem->child(i),
                QVector<int>() << TrackProperty::ROLE_AutomatedPanValue
                               << TrackProperty::ROLE_AutomatedPanNormalized
                               << TrackProperty::ROLE_AutomatedNormalized
                               << TrackProperty::ROLE_AutomatedValue);
        }

        if (speakerOnAutomation->isAutomated()) {
            Y = speakerOnAutomation->getY(time);

            speakerOnAutomation->setAutomatedNormalized(Y);

            if (qAbs(Y - 0.75) < 0.1) {

                trackItem->truePeakModel()->setMuted(true);
            } else {

                trackItem->truePeakModel()->setMuted(false);
            }

            sltDataChanged(_rootItem->child(i),
                QVector<int>() << TrackProperty::ROLE_AutomatedSpeakerOnValue
                               << TrackProperty::ROLE_AutomatedSpeakerOnNormalized
                               << TrackProperty::ROLE_AutomatedNormalized
                               << TrackProperty::ROLE_AutomatedValue);
        }

        auto visrItem = trackItem->visrAutomationItem(trackItem->getAutomationMenuTitle(), trackItem->automationSubMenuTitle());
        if (visrItem && visrItem->automation()->isAutomated()) {

            Y = visrItem->automation()->getY(time);

            visrItem->setAutomatedNormalized(Y);

            sltDataChanged(_rootItem->child(i),
                QVector<int>()
                    << TrackProperty::ROLE_AutomatedNormalized
                    << TrackProperty::ROLE_AutomatedValue);
        }

        auto sampleTrack = ((SampleTrack*)trackItem->track());
        auto effect_count = sampleTrack->audioPort()->effects()->size();

        for (int effectIndex = 0; effectIndex < effect_count; effectIndex++) {
            auto effect = sampleTrack->audioPort()->effects()->effect(effectIndex);

            if (effect) {

                auto list = effect->controls()->shortTitleList();

                for (int j = 0; j < list.size(); j++) {
                    if (effect->automationItem(list[j])) {
                        if (effect->automationItem(list[j])->automation()->isAutomated()) {
                            Y = effect->automationItem(list[j])->automation()->getY(time);

                            effect->automationItem(list[j])->setAutomatedNormalized(Y);

                            sltDataChanged(_rootItem->child(i),
                                QVector<int>() << TrackProperty::ROLE_AutomatedSpeakerOnValue
                                               << TrackProperty::ROLE_AutomatedSpeakerOnNormalized
                                               << TrackProperty::ROLE_AutomatedNormalized
                                               << TrackProperty::ROLE_AutomatedValue);
                        }
                    }
                }
            }
        }

        trackItem->updateTruePeak();
    }

    //    if (i >= 0 && i < _trackItems.size()) {
    //        QString index_ = _trackItems[i]->findClipItemTime(time);
    //        if (index_ != "") {

    //            if (_trackItems[i]->currentClipItem() != index_) {
    //                _trackItems[i]->setCurrentClipItem(index_);
    //                Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_ChannelsCount);
    //            }
    //        }
    //        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_TruePeakLevel);
    //    }
}

void TracksModel::updateTruePeak(int i)
{
    if (i >= 0 && i < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(i)->trackItem();
        trackItem->updateTruePeak();
    }
}

// void TracksModel::setVisrEnabled(int trackIndex, bool enabled)
//{
//     auto trackItem = _rootItem->child(trackIndex)->trackItem();
//     trackItem->track()->setVisrEnabled(enabled);
// }

// bool TracksModel::visrEnabled(int trackIndex)
//{
//     auto trackItem = _rootItem->child(trackIndex)->trackItem();
//     return trackItem->track()->visrEnabled();
// }

// void TracksModel::setLoadSpeakerConfig(QString path)
//{
//     for (int i = 0; i < _rootItem->childCount(); i++) {
//         auto trackItem = _rootItem->child(i)->trackItem();
//         trackItem->track()->setLoadSpeakerConfig(path);
//     }
// }

void TracksModel::toggleTrackActivator(int i)
{

    if (i >= 0 && i < _rootItem->childCount()) {

        _rootItem->child(i)->trackItem()->track()->setSelected(!_rootItem->child(i)->trackItem()->track()->isSelected());

        if (_rootItem->child(i)->trackItem()->track()->isSolo()) {
            _rootItem->child(i)->trackItem()->truePeakModel()->setMuted(false);
        } else {
            if (_rootItem->child(i)->trackItem()->track()->isSelected()) {
                _rootItem->child(i)->trackItem()->truePeakModel()->setMuted(false);
            } else {
                _rootItem->child(i)->trackItem()->truePeakModel()->setMuted(true);
            }
        }

        auto trackItem = _rootItem->child(i)->trackItem();

        // This leads to confilict between solo,selected, and muted
        //        if (!_rootItem->child(i)->trackItem()->isSolo()) {
        //            if (trackItem->automationItem()->automation()->isAutomated()) {
        //                if (trackItem->speakerOnAutomationItem()->automation()->enabled()) {
        //                    trackItem->speakerOnAutomationItem()->setEnabled(false);
        //                    trackItem->speakerOnAutomationItem()->setNormalized(1 - trackItem->speakerOnAutomationItem()->automation()->automatedNormalized());

        //                } else {
        //                    trackItem->speakerOnAutomationItem()->setNormalized(1 - trackItem->speakerOnAutomationItem()->automation()->normalized());
        //                }
        //            } else {
        //                trackItem->speakerOnAutomationItem()->setNormalized(1 - trackItem->speakerOnAutomationItem()->automation()->normalized());
        //            }
        //        }

        QVector<int> roles;
        roles.append(TrackProperty::ROLE_SpeakerOnValue);
        roles.append(TrackProperty::ROLE_SpeakerOnNormalized);
        roles.append(TrackProperty::ROLE_Normalized);
        roles.append(TrackProperty::ROLE_Value);
        roles.append(TrackProperty::ROLE_Solo);
        roles.append(TrackProperty::ROLE_TrackActivated);
        roles.append(TrackProperty::ROLE_Mute);

        sltDataChanged(_rootItem->child(i), roles);
    }
    Q_EMIT sigUpdate();
}

void TracksModel::toggleSolo(int i)
{

    auto ctrl = m_areaInfo->ctrlkeyPressed();
    QList<int> solo_list;
    if (ctrl) {
        for (int j = 0; j < _rootItem->childCount(); j++) {
            auto trackItem = _rootItem->child(j)->trackItem();
            if (trackItem->track()->isSolo()) {
                solo_list.append(j);
            }
        }
    }
    if (i >= 0 && i < _rootItem->childCount()) {

        auto trackItem = _rootItem->child(i)->trackItem();

        trackItem->track()->setSolo(!trackItem->track()->isSolo());
        if (trackItem->track()->isSolo() == true) {
            AudioManager::mixer()->addToSolo(trackItem->track()->trackIndex());
        } else {
            AudioManager::mixer()->removeFromSolo(trackItem->track()->trackIndex());
        }

        if (_rootItem->child(i)->trackItem()->track()->isSolo()) {
            _rootItem->child(i)->trackItem()->truePeakModel()->setMuted(false);
        } else {
            if (_rootItem->child(i)->trackItem()->track()->isSelected()) {
                _rootItem->child(i)->trackItem()->truePeakModel()->setMuted(false);
            } else {
                _rootItem->child(i)->trackItem()->truePeakModel()->setMuted(true);
            }
        }

        //  trackItem->track()->setSelected(false);

        sltDataChanged(_rootItem->child(i), QVector<int>() << TrackProperty::ROLE_Solo << TrackProperty::ROLE_Mute << TrackProperty::ROLE_TrackActivated);

        for (int j = 0; j < _rootItem->childCount(); j++) {
            if (j != i && !solo_list.contains(j)) {

                _rootItem->child(j)->trackItem()->track()->setSolo(false);
                AudioManager::mixer()->removeFromSolo(_rootItem->child(j)->trackItem()->track()->trackIndex());

                if (_rootItem->child(j)->trackItem()->track()->isSolo()) {
                    _rootItem->child(j)->trackItem()->truePeakModel()->setMuted(false);
                } else {
                    _rootItem->child(j)->trackItem()->truePeakModel()->setMuted(true);
                }

                sltDataChanged(_rootItem->child(j), QVector<int>() << TrackProperty::ROLE_Solo << TrackProperty::ROLE_Mute << TrackProperty::ROLE_TrackActivated);
            }
        }

        bool solo_disabled = true;
        for (int j = 0; j < _rootItem->childCount(); j++) {
            if (_rootItem->child(j)->trackItem()->track()->isSolo() == true) {
                solo_disabled = false;
            }
        }
        if (solo_disabled) {
            for (int j = 0; j < _rootItem->childCount(); j++) {
                _rootItem->child(j)->trackItem()->track()->setSelected(true);

                if (_rootItem->child(j)->trackItem()->track()->isSolo()) {
                    _rootItem->child(j)->trackItem()->truePeakModel()->setMuted(false);
                } else {
                    if (_rootItem->child(j)->trackItem()->track()->isSelected()) {
                        _rootItem->child(j)->trackItem()->truePeakModel()->setMuted(false);
                    } else {
                        _rootItem->child(j)->trackItem()->truePeakModel()->setMuted(true);
                    }
                }

                sltDataChanged(_rootItem->child(j), QVector<int>() << TrackProperty::ROLE_TrackActivated << TrackProperty::ROLE_Mute);
            }
        }
    }
}

TruePeakModel* TracksModel::truePeakModel(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {

        auto item = _rootItem->child(trackIndex)->trackItem()->truePeakModel().data();
        QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        // item->setParent(this);
        return item;
    }

    return Q_NULLPTR;
}

MenuModel* TracksModel::effectMenuModel(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto item = _rootItem->child(trackIndex)->menuModel().get();
        QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        // item->setParent(this);
        return item;
    }

    return Q_NULLPTR;
}

AutomationLaneModel* TracksModel::automationLaneModel(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {

        AutomationLaneModel* item = _rootItem->child(trackIndex)->trackItem()->automationLaneModel().data();
        QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        // item->setParent(this);
        return item;
    }

    return Q_NULLPTR;
}

EffectChain* TracksModel::effectChain(int trackIndex)
{

    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto sampleTrack = (SampleTrack*)_rootItem->child(trackIndex)->trackItem()->track();

        auto item = sampleTrack->audioPort()->effects().data();
        QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        // item->setParent(this);
        return item;
    }

    return Q_NULLPTR;
}

VisrEffect* TracksModel::visrEffect(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto sampleTrack = (SampleTrack*)_rootItem->child(trackIndex)->trackItem()->track();

        VisrEffect* item = (VisrEffect*)sampleTrack->audioPort()->visrEfect().data();

        QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        // item->setParent(item);
        return item;
    }

    return Q_NULLPTR;
}

TrackItem* TracksModel::getTrackItem(int i)
{
    if (i >= 0 && i < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(i)->trackItem().data();

        QQmlEngine::setObjectOwnership(trackItem, QQmlEngine::CppOwnership);

        return trackItem;
    }

    return Q_NULLPTR;
}

QMap<qint64, int> TracksModel::keyFrames(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(trackIndex)->trackItem();
        return trackItem->keyFramesAutomationItem()->keyFrames();
    }
    return QMap<qint64, int>();
}

QMap<qint64, double> TracksModel::objectPositionX(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(trackIndex)->trackItem();
        return trackItem->keyFramesAutomationItem()->objectPositionX();
    }
    return QMap<qint64, double>();
}

QMap<qint64, double> TracksModel::objectPositionY(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(trackIndex)->trackItem();
        return trackItem->keyFramesAutomationItem()->objectPositionY();
    }
    return QMap<qint64, double>();
}

QMap<qint64, double> TracksModel::objectPositionZ(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(trackIndex)->trackItem();
        return trackItem->keyFramesAutomationItem()->objectPositionZ();
    }
    return QMap<qint64, double>();
}

bool TracksModel::objectModelEnabled(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(trackIndex)->trackItem();
        return trackItem->objectEnabled();
    }
    return false;
}

void TracksModel::setObjectModelEnabled(int trackIndex, bool enabled)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto item = _rootItem->child(trackIndex);
        if (item->trackItem()->objectEnabled() != enabled) {
            item->trackItem()->setObjectEnabled(enabled);
            sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_ObjectEnabled);
            Q_EMIT sigUpdate();
        }
    }
}

void TracksModel::setObjectPosition(int trackIndex, double x, double y, double z)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(trackIndex)->trackItem();
        trackItem->keyFramesAutomationItem()->setObjectPosition(x, y, z);
    }
}
void TracksModel::setCurrentObjectStatus(int trackIndex, QString currentChanges)
{
    m_currentObjectChanges[trackIndex] = currentChanges;

    QJsonParseError parseError;
    QByteArray byteArray = currentChanges.toUtf8();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
    auto trackItem = _rootItem->child(trackIndex)->trackItem();
    trackItem->keyFramesAutomationItem()->clear();
    if (parseError.error == QJsonParseError::NoError && doc.isArray()) {

        QJsonArray objs = doc.array();

        for (int i = 0; i < objs.size(); i++) {

            QJsonObject obj = objs[i].toObject();

            trackItem->keyFramesAutomationItem()->addKeyFrame(quint64(obj["time"].toDouble()), obj["type"].toInt());
        }
    }
    _rootItem->child(trackIndex)->trackItem()->sigUpdate();
    _rootItem->child(trackIndex)->sigUpdate();
}
void TracksModel::saveObjectPosition(int trackIndex, double x, double y, double z)
{
    QJsonArray array;
    QJsonObject jObject2;

    QJsonObject jsonObj;

    jsonObj["posX"] = x;
    jsonObj["posY"] = y;
    jsonObj["posZ"] = z;
    jsonObj["track_index"] = trackIndex;

    jObject2["state"] = jsonObj;
    jObject2["operation"] = ActionStack::Operation_ObjectPosition;

    qDebug() << "saveObjectPosition" << x << y << z;
    array.append(jObject2);

    ActionStack::instance()->pushInUndo(array);
}

QJsonObject TracksModel::performObjectPosition(QJsonObject obj)
{

    QJsonObject jObject2;

    qDebug() << "performObjectPosition" << obj;
    auto operation = obj["operation"].toInt();

    if (operation == ActionStack::Operation_EditKeyFramesPoint) {
        auto state = obj["state"].toString();
        auto track_index = obj["track_index"].toInt();

        jObject2["state"] = m_currentObjectChanges[track_index];
        jObject2["track_index"] = track_index;
        jObject2["operation"] = ActionStack::Operation_EditKeyFramesPoint;

        Q_EMIT m_areaInfo->sigEditKeyFrameWithPosition(track_index, state);

        qDebug() << "Operation_EditKeyFramesPoint1" << jObject2["state"];
    }

    if (operation == ActionStack::Operation_ObjectPosition) {

        auto state = obj["state"].toObject();

        double x, y, z;
        x = state["posX"].toDouble();
        y = state["posY"].toDouble();
        z = state["posZ"].toDouble();
        int track_index = state["track_index"].toInt();

        m_areaInfo->setObjectPosition(track_index, x, y, z);

        // jObject2["state"] = m_currentObjectChanges;
        // jObject2["operation"] = ActionStack::Operation_ObjectPosition;
        // m_currentObjectChanges = state;
    }

    return jObject2;
}

void TracksModel::addKeyFrame(int trackIndex, qint64 time, int type)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(trackIndex)->trackItem();
        trackItem->keyFramesAutomationItem()->addKeyFrame(time, type);
    }
}
void TracksModel::saveKeyFrameEdited(int trackIndex, QString status)
{
    QJsonArray array;

    QJsonObject jObject2;

    jObject2["track_index"] = trackIndex;
    jObject2["state"] = status;
    jObject2["operation"] = ActionStack::Operation_EditKeyFramesPoint;
    array.append(jObject2);

    qDebug() << "saveKeyFrameEdited" << jObject2;

    ActionStack::instance()->pushInUndo(array);
}

void TracksModel::removeKeyFrame(int trackIndex, qint64 time)
{

    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto trackItem = _rootItem->child(trackIndex)->trackItem();
        trackItem->keyFramesAutomationItem()->removeKeyFrame(time);
    }
}

void TracksModel::panAutomationSelected(int _index)
{

    if (_index >= 0 && _index < _rootItem->childCount()) {

        if (!_rootItem->child(_index)->trackItem()->automationLaneModel()->isContained("Mixer", "Pan")) {

            _rootItem->child(_index)->trackItem()->setAutomationMenuTitle("Mixer");
            _rootItem->child(_index)->trackItem()->setAutomationSubMenuTitle("Pan");
            sltDataChanged(_rootItem->child(_index), QVector<int>() << TrackProperty::ROLE_AutomationMenuTitle << TrackProperty::ROLE_AutomationSubMenuTitle);
        }

        sltDataChanged(_rootItem->child(_index), QVector<int>() << TrackProperty::ROLE_PanSelected << TrackProperty::ROLE_VolumeSelected << TrackProperty::ROLE_SpeakerOnSelected);

        Q_EMIT sigUpdate();
    }
}

void TracksModel::speakerOnAutomationSelected(int _index)
{

    if (_index >= 0 && _index < _rootItem->childCount()) {

        if (!_rootItem->child(_index)->trackItem()->automationLaneModel()->isContained("Mixer", "SpeakerOn")) {

            _rootItem->child(_index)->trackItem()->setAutomationMenuTitle("Mixer");
            _rootItem->child(_index)->trackItem()->setAutomationSubMenuTitle("SpeakerOn");
            sltDataChanged(_rootItem->child(_index), QVector<int>() << TrackProperty::ROLE_AutomationMenuTitle << TrackProperty::ROLE_AutomationSubMenuTitle);
        }

        sltDataChanged(_rootItem->child(_index), QVector<int>() << TrackProperty::ROLE_SpeakerOnSelected << TrackProperty::ROLE_VolumeSelected << TrackProperty::ROLE_PanSelected);

        Q_EMIT sigUpdate();
    }
}

void TracksModel::volumeAutomationSelected(int _index)
{

    if (_index >= 0 && _index < _rootItem->childCount()) {

        if (!_rootItem->child(_index)->trackItem()->automationLaneModel()->isContained("Mixer", "Volume")) {

            _rootItem->child(_index)->trackItem()->setAutomationMenuTitle("Mixer");
            _rootItem->child(_index)->trackItem()->setAutomationSubMenuTitle("Volume");
            sltDataChanged(_rootItem->child(_index), QVector<int>() << TrackProperty::ROLE_AutomationMenuTitle << TrackProperty::ROLE_AutomationSubMenuTitle);
        }

        sltDataChanged(_rootItem->child(_index), QVector<int>() << TrackProperty::ROLE_PanSelected << TrackProperty::ROLE_VolumeSelected << TrackProperty::ROLE_SpeakerOnSelected);

        Q_EMIT sigUpdate();
    }
}

void TracksModel::noneAutomationSelected(int _index)
{

    if (_index >= 0 && _index < _rootItem->childCount()) {

        _rootItem->child(_index)->trackItem()->setAutomationMenuTitle("None");
        _rootItem->child(_index)->trackItem()->setAutomationSubMenuTitle("None");
        sltDataChanged(_rootItem->child(_index), QVector<int>() << TrackProperty::ROLE_AutomationMenuTitle << TrackProperty::ROLE_AutomationSubMenuTitle);

        Q_EMIT sigUpdate();
    }
}

bool TracksModel::isAutomated(int _index, QString menu, QString subMenu)
{
    if (_index >= 0 && _index < _rootItem->childCount()) {
        if (subMenu == "Volume") {
            return _rootItem->child(_index)->trackItem()->volumeAutomationItem()->automation()->isAutomated();
        } else if (subMenu == "Pan") {
            return _rootItem->child(_index)->trackItem()->panAutomationItem()->automation()->isAutomated();
        } else if (subMenu == "SpeakerOn") {
            return _rootItem->child(_index)->trackItem()->speakerOnAutomationItem()->automation()->isAutomated();
        } else {

            if (menu == "Visr") {
                auto sampleTrack = (SampleTrack*)(_rootItem->child(_index)->trackItem()->track());
                return sampleTrack->audioPort()->visrEfect()->isAutomated(subMenu);
            } else {
                auto sampleTrack = (SampleTrack*)(_rootItem->child(_index)->trackItem()->track());
                return sampleTrack->audioPort()->effects()->isAutomated(menu, subMenu);
            }
        }
    }
    return false;
}

bool TracksModel::isSelected(int _index, QString menu, QString subMenu)
{

    if (_index >= 0 && _index < _rootItem->childCount()) {

        return _rootItem->child(_index)->trackItem()->isSelected(menu, subMenu);
    }
    return false;
}

bool TracksModel::isEnabled(int _index, QString menu, QString subMenu)
{
    if (_index >= 0 && _index < _rootItem->childCount()) {
        if (subMenu == "Volume") {
            return _rootItem->child(_index)->trackItem()->volumeAutomationItem()->automation()->enabled();
        } else if (subMenu == "Pan") {
            return _rootItem->child(_index)->trackItem()->panAutomationItem()->automation()->enabled();
        } else if (subMenu == "SpeakerOn") {
            return _rootItem->child(_index)->trackItem()->speakerOnAutomationItem()->automation()->enabled();
        } else {
            if (menu == "Visr") {
                auto sampleTrack = (SampleTrack*)(_rootItem->child(_index)->trackItem()->track());
                return sampleTrack->audioPort()->visrEfect()->isEnabled();
            } else {
                auto sampleTrack = (SampleTrack*)(_rootItem->child(_index)->trackItem()->track());
                return sampleTrack->audioPort()->effects()->isAutomationEnabled(menu, subMenu);
            }
        }
    }
    return false;
}

void TracksModel::setAutomationLaneEnabled(int trackindex, bool enabled)
{

    if (trackindex >= 0 && trackindex < _rootItem->childCount()) {
        _rootItem->child(trackindex)->trackItem()->setAutomationLaneEnabled(enabled);

        if (enabled) {
            if (_rootItem->child(trackindex)->trackItem()->rowsExpandedCount() < 3) {
                _rootItem->child(trackindex)->trackItem()->setRowsExpandedCount(3);

                sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_TotalTrackRect << TrackProperty::ROLE_TrackRect);
            }

            if (_rootItem->child(trackindex)->trackItem()->automationLaneModel()->count() > 0) {
                _rootItem->child(trackindex)->trackItem()->totalRowExpandedChanged();

                sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_TotalTrackRect << TrackProperty::ROLE_TrackRect);
            }
        } else {

            if (_rootItem->child(trackindex)->trackItem()->automationLaneModel()->count() > 0) {
                _rootItem->child(trackindex)->trackItem()->totalRowExpandedChanged();
                sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_TotalTrackRect << TrackProperty::ROLE_TrackRect);
            }
        }
        sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_AutomationLaneEnabled);
        Q_EMIT sigUpdate();

        Q_EMIT sigAutomationLaneEnabled(trackindex, enabled);
    }
}

void TracksModel::setAutomationEnabled(int trackindex, QString menu, QString subMenu, bool enabled)
{

    if (trackindex >= 0 && trackindex < _rootItem->childCount()) {
        _rootItem->child(trackindex)->trackItem()->automationItem(menu, subMenu)->setEnabled(enabled);

        sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_AutomatedEnabled);

        Q_EMIT sigUpdate();
    }
}

void TracksModel::setAutomationMenuTitle(int trackindex, QString menu, QString subMenu)
{
    auto item = trackItem(trackindex);
    if (item) {

        item->setAutomationMenuTitle(menu);

        sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_AutomationMenuTitle << TrackProperty::ROLE_Value << TrackProperty::ROLE_Normalized << TrackProperty::ROLE_AutomatedEnabled);

        item->setAutomationSubMenuTitle(subMenu);

        sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_AutomationSubMenuTitle << TrackProperty::ROLE_Value << TrackProperty::ROLE_Normalized << TrackProperty::ROLE_AutomatedEnabled);

        _rootItem->child(trackindex)->automationChanged();

        Q_EMIT sigUpdate();
    }
}

void TracksModel::automationClear(int trackindex, QString menu, QString subMenu)
{
    auto trackItem = _rootItem->child(trackindex)->trackItem();
    if (trackItem) {
        if (trackItem->automationItem(menu, subMenu)) {
            auto automation = trackItem->automationItem(menu, subMenu)->automation();
            QJsonArray array;
            QJsonObject jObject2;
            jObject2["track_index"] = trackItem->trackIndex();
            jObject2["sub_track_index"] = automation->getSubTrackIndex();
            jObject2["menu"] = automation->type();
            jObject2["sub_menu"] = automation->subType();
            jObject2["state"] = automation->getLinesState();
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            array.append(jObject2);
            ActionStack::instance()->pushInUndo(array);

            trackItem->automationItem(menu, subMenu)->automation()->clear();
            trackItem->automationItem(menu, subMenu)->setEnabled(false);

            trackItem->automationItem(menu, subMenu)->setNormalized(automation->normalized());

            sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_Normalized << TrackProperty::ROLE_Value);

            Q_EMIT sigUpdate();
        }
    }
}

void TracksModel::saveUndoNoramized(int trackindex, QString menu, QString subMenu)
{

    auto trackItem = _rootItem->child(trackindex)->trackItem();
    if (trackItem) {
        if (trackItem->automationItem(menu, subMenu)) {
            auto automation = trackItem->automationItem(menu, subMenu)->automation();
            QJsonArray array;
            QJsonObject jObject2;
            jObject2["type"] = "normalized";
            jObject2["track_index"] = trackItem->trackIndex();
            jObject2["sub_track_index"] = automation->getSubTrackIndex();
            jObject2["menu"] = automation->type();
            jObject2["sub_menu"] = automation->subType();
            jObject2["value"] = automation->normalized();
            jObject2["automation"] = automation->enabled();
            jObject2["operation"] = ActionStack::Operation_NormalizedValueChanged;
            array.append(jObject2);
            ActionStack::instance()->pushInUndo(array);
        }
    }
}

void TracksModel::performOperation(QJsonObject obj)
{

    if (obj["operation"] == ActionStack::Operation_NormalizedValueChanged) {
        int track_index = obj["track_index"].toInt();
        QString menu = obj["menu"].toString();
        QString sub_menu = obj["sub_menu"].toString();

        if (obj.contains("automation")) {
            trackItem(track_index)->automationItem(menu, sub_menu)->automation()->setEnabled1(obj["automation"].toBool());
        }
        trackItem(track_index)->automationItem(menu, sub_menu)->setNormalized(obj["value"].toDouble());
        sltDataChanged(_rootItem->child(track_index), QVector<int>() << TrackProperty::ROLE_Normalized << TrackProperty::ROLE_Value);
    }
}

void TracksModel::setCoverBound(bool state)
{
    for (int j = 0; j < _rootItem->childCount(); j++) {
        _rootItem->child(j)->trackItem()->setCoverBound(state);

        sltDataChanged(_rootItem->child(j), QVector<int>() << TrackProperty::ROLE_CoverBound);
    }
}

QSharedPointer<TrackItem> TracksModel::trackItem(int i)
{
    return _rootItem->child(i)->trackItem();
}

int TracksModel::size()
{
    return _rootItem->childCount();
}

QRect TracksModel::getTrackTotalRect(int i)
{
    return _rootItem->child(i)->trackItem()->totalRect();
}

QHash<int, QByteArray> TracksModel::roleNames() const
{
    return m_roleNames;
}

TrackProperty* TracksModel::rootItem() const
{
    return _rootItem;
}

void TracksModel::sltSelectedChanged(int i)
{

    if (i >= 0 && i < _rootItem->childCount()) {
        m_areaInfo->setSelectedTrackItem(i);

        for (int j = 0; j < _rootItem->childCount(); j++) {

            _rootItem->child(j)->trackItem()->setSelected(false);

            sltDataChanged(_rootItem->child(j), QVector<int>() << TrackProperty::ROLE_SelectedTrack);
        }

        _rootItem->child(i)->trackItem()->setSelected(true);
        sltDataChanged(_rootItem->child(i), QVector<int>() << TrackProperty::ROLE_SelectedTrack);
    }
    Q_EMIT sigUpdate();
}

int TracksModel::rowCount(const QModelIndex& parent) const
{
    TrackProperty* parentItem = !parent.isValid() ? _rootItem : static_cast<TrackProperty*>(parent.internalPointer());
    return parentItem->childCount();
}

int TracksModel::columnCount(const QModelIndex& parent) const
{
    return 1; // Only one column for this example
}

QModelIndex TracksModel::index(int row, int column, const QModelIndex& parent) const
{

    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    TrackProperty* parentItem = !parent.isValid() ? _rootItem : static_cast<TrackProperty*>(parent.internalPointer());
    TrackProperty* childItem = parentItem->child(row);

    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex TracksModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    TrackProperty* childItem = static_cast<TrackProperty*>(index.internalPointer());
    TrackProperty* parentItem = childItem->parent();

    if (parentItem == _rootItem) {
        return QModelIndex();
    }
    if (parentItem == Q_NULLPTR) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(parentItem), 0, parentItem);
}

void TracksModel::updateTracksHeight()
{
    for (int j = 0; j < _rootItem->childCount(); j++) {

        sltDataChanged(_rootItem->child(j), QVector<int>() << TrackProperty::ROLE_TotalTrackRect << TrackProperty::ROLE_TrackRect);

        _rootItem->child(j)->trackItem()->automationLaneModel()->updateLanesHeight();
    }
}

void TracksModel::updateTracksHeight(int i)
{
    if (i >= 0 && i < _rootItem->childCount()) {

        sltDataChanged(_rootItem->child(i), QVector<int>() << TrackProperty::ROLE_TotalTrackRect << TrackProperty::ROLE_TrackRect);
        _rootItem->child(i)->trackItem()->automationLaneModel()->updateLanesHeight();
    }
}

void TracksModel::addTrackItem(QSharedPointer<TrackItem> trackitem)
{

    int row = _rootItem->childCount();

    beginInsertRows(QModelIndex(), row, row);

    auto item = new TrackProperty(TrackProperty::TrackPropertyLevel_1, trackitem, _rootItem);

    // This Second level is needed for SubTrack Items in UI
    auto secondLevel = new TrackProperty(TrackProperty::TrackPropertyLevel_2, trackitem, item);
    item->appendChild(secondLevel);

    _rootItem->appendChild(item);

    connect(trackitem.data(), &TrackItem::sigClipChanged, this, &TracksModel::sigClipChanged);
    connect(trackitem.data(), &TrackItem::sigSelectedChanged, this, &TracksModel::sltSelectedChanged);

    if (_rootItem->childCount() == 1) {
        auto firstItem = _rootItem->getChildren()[0];
        firstItem->trackItem()->setSelected(true);
        m_areaInfo->setSelectedTrackItem(0);
    }

    connect(item, &TrackProperty::dataChanged, this, &TracksModel::sltDataChanged);

    endInsertRows();
}

void TracksModel::removeTrackItem(int trackIndex)
{
    beginRemoveRows(QModelIndex(), trackIndex, trackIndex);
    _rootItem->removeChild(trackIndex);
    endRemoveRows();
}

void TracksModel::sltDataChanged(TrackProperty* item, QVector<int> roles)
{
    QModelIndex changedIndex = createIndex(item->row(item), 0, item);
    emit dataChanged(changedIndex, changedIndex, roles);
    Q_EMIT sigUpdate();
}

void TracksModel::effectAdded(int trackIndex, int effectIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {

        auto sampleTrack = (SampleTrack*)(_rootItem->child(trackIndex)->trackItem()->track());

        auto effect = sampleTrack->audioPort()->effects()->effect(effectIndex);

        auto name = effect->controls()->name();
        auto list = effect->controls()->shortTitleList();

        _rootItem->child(trackIndex)->menuModel()->addMenuItem(name, list);

        Q_EMIT sigEffectAdded(trackIndex, effectIndex);
    }
}

void TracksModel::effectRemoved(int trackIndex, int effectIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {
        auto sampleTrack = (SampleTrack*)(_rootItem->child(trackIndex)->trackItem()->track());
        auto effect = sampleTrack->audioPort()->effects()->effect(effectIndex);

        auto name = effect->controls()->name();

        _rootItem->child(trackIndex)->menuModel()->removeMenuItem(name);

        Q_EMIT sigEffectRemoved(trackIndex, effectIndex);
    }
}

void TracksModel::effectUpdated(int trackIndex, int effectIndex)
{
    if (trackIndex >= 0 && trackIndex < _rootItem->childCount()) {

        auto sampleTrack = (SampleTrack*)(_rootItem->child(trackIndex)->trackItem()->track());

        auto effect = sampleTrack->audioPort()->effects()->effect(effectIndex);

        auto name = effect->controls()->name();
        auto list = effect->controls()->shortTitleList();

        qDebug() << "UPDATED";
        _rootItem->child(trackIndex)->menuModel()->removeMenuItem(name);
        _rootItem->child(trackIndex)->menuModel()->addMenuItem(name, list);

        Q_EMIT sigEffectUpdated(trackIndex, effectIndex);
    }
}

void TracksModel::visrAdded(int trackindex)
{
    auto sampleTrack = (SampleTrack*)(_rootItem->child(trackindex)->trackItem()->track());
    auto effect = ((SampleTrack*)sampleTrack)->audioPort()->visrEfect();
    if (effect) {
        auto name = effect->controls()->name();
        auto list = effect->controls()->shortTitleList();

        sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_VISR);
        _rootItem->child(trackindex)->menuModel()->addMenuItem(name, list);
    }
}

void TracksModel::visrRemoved(int trackindex)
{

    if (trackindex >= 0 && trackindex < _rootItem->childCount()) {

        sltDataChanged(_rootItem->child(trackindex), QVector<int>() << TrackProperty::ROLE_VISR);
        _rootItem->child(trackindex)->menuModel()->removeMenuItem("Visr");
    }
}

bool TracksModel::isVisrLoaded(int trackIndex)
{
    auto sampleTrack = (SampleTrack*)(_rootItem->child(trackIndex)->trackItem()->track());
    if (((SampleTrack*)sampleTrack)->audioPort()->visrHelper()) {
        return true;
    }
    return false;
}

QVariant TracksModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid())
        return QVariant();

    TrackProperty* item = static_cast<TrackProperty*>(index.internalPointer());

    switch (role) {
    case TrackProperty::ROLE_TruePeak: {

        return item->trackItem()->truePeakModel()->size();
    }
    case TrackProperty::ROLE_Type:
        return "audio";
    case TrackProperty::ROLE_VISR: {
        auto sampleTrack = (SampleTrack*)item->trackItem()->track();
        if (sampleTrack->audioPort()->visrEfect()) {
            return true;
        } else {
            return false;
        }
    }
    case TrackProperty::ROLE_PreText:
        return "Audio";
    case TrackProperty::ROLE_Collapsed:
        return item->trackItem()->isCollapsed();
    case TrackProperty::ROLE_ParentSet:
        return QStringList();
    case TrackProperty::ROLE_ChildrenSet:
        return QStringList();
    case TrackProperty::ROLE_TrackActivated: {

        // this has three state ( on=1, sol=2, off=0) #QD-1535

        QList<int> solo_list;
        for (int j = 0; j < _rootItem->childCount(); j++) {
            auto trackItem = _rootItem->child(j)->trackItem();
            if (trackItem->track()->isSolo()) {
                solo_list.append(trackItem->trackIndex());
            }
        }

        if (solo_list.size() > 0) {
            if (item->trackItem()->track()->isSelected()) {
                return 2;
            } else {
                return 0;
            }
        } else {
            if (item->trackItem()->track()->isSelected()) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    case TrackProperty::ROLE_SelectedSolo:
        return item->trackItem()->selectSolo();
    case TrackProperty::ROLE_PartialySolo:
        return item->trackItem()->partialySolo();
    case TrackProperty::ROLE_Solo:
        return item->trackItem()->isSolo();
    case TrackProperty::ROLE_SelectedTrackDependSolo:
        return item->trackItem()->selectTrackDependSolo();
    case TrackProperty::ROLE_CoverBound:
        return item->trackItem()->coverBound();
    case TrackProperty::ROLE_EditName:
        return item->trackItem()->editName();
    case TrackProperty::ROLE_InDraggingCounter:
        return item->trackItem()->inDraggingCounter();
    case TrackProperty::ROLE_SelectedTrack:
        return item->trackItem()->isSelected();
    case TrackProperty::ROLE_ChannelsCount: {
        auto index_ = item->trackItem()->currentClipItem();
        auto clip = item->trackItem()->clipItems(index_);
        if (clip) {
            return clip->sampleClip()->sampleBuffer()->channels();
        }
        return AudioManager::audioEngine()->channels();
    }
    case TrackProperty::ROLE_AutomatedVolumeEnabled: {
        return item->trackItem()->volumeAutomationItem()->automation()->enabled();
    }
    case TrackProperty::ROLE_AutomatedPanEnabled:
        return item->trackItem()->panAutomationItem()->automation()->enabled();
    case TrackProperty::ROLE_PanValue: {
        return item->trackItem()->panAutomationItem()->value();
    }

    case TrackProperty::ROLE_AutomatedSpeakerOnEnabled:
        return item->trackItem()->speakerOnAutomationItem()->automation()->enabled();
    case TrackProperty::ROLE_SpeakerOnValue: {
        return item->trackItem()->speakerOnAutomationItem()->value();
    }

    case TrackProperty::ROLE_VolumeValue: {
        return item->trackItem()->volumeAutomationItem()->value();
    }
    case TrackProperty::ROLE_VolumeNormalized: {
        return item->trackItem()->volumeAutomationItem()->automation()->normalized();
    }
    case TrackProperty::ROLE_AutomatedVolumeValue:
        return item->trackItem()->volumeAutomationItem()->automatedValue();
    case TrackProperty::ROLE_AutomatedVolumeNormalized:
        return item->trackItem()->volumeAutomationItem()->automation()->automatedNormalized();
    case TrackProperty::ROLE_AutomatedVolumeActive: {
        return item->trackItem()->volumeAutomationItem()->automation()->isAutomated();
    }
    case TrackProperty::ROLE_VolumeDefaultNormalized:
        return item->trackItem()->volumeAutomationItem()->automation()->defaultNormalized();

    case TrackProperty::ROLE_AutomatedPanValue:
        return item->trackItem()->panAutomationItem()->automatedValue();
    case TrackProperty::ROLE_AutomatedPanNormalized:
        return item->trackItem()->panAutomationItem()->automation()->automatedNormalized();
    case TrackProperty::ROLE_PanNormalized:
        return item->trackItem()->panAutomationItem()->automation()->normalized();
    case TrackProperty::ROLE_PanDefaultNormalized:
        return item->trackItem()->panAutomationItem()->automation()->defaultNormalized();

    case TrackProperty::ROLE_AutomatedPanActive:
        return item->trackItem()->panAutomationItem()->automation()->isAutomated();
    case TrackProperty::ROLE_TrackRect: {
        return item->trackItem()->rect();
    }

    case TrackProperty::ROLE_AutomatedSpeakerOnValue:
        return item->trackItem()->speakerOnAutomationItem()->automatedValue();
    case TrackProperty::ROLE_AutomatedSpeakerOnNormalized:
        return item->trackItem()->speakerOnAutomationItem()->automation()->automatedNormalized();
    case TrackProperty::ROLE_SpeakerOnNormalized:
        return item->trackItem()->speakerOnAutomationItem()->automation()->normalized();
    case TrackProperty::ROLE_SpeakerOnDefaultNormalized:
        return item->trackItem()->speakerOnAutomationItem()->automation()->defaultNormalized();
    case TrackProperty::ROLE_AutomatedSpeakerOnActive:
        return item->trackItem()->speakerOnAutomationItem()->automation()->isAutomated();

    case TrackProperty::ROLE_TotalTrackRect: {
        return item->trackItem()->getTotalRect();
    }
    case TrackProperty::ROLE_RowsExpanded:
        return item->trackItem()->rowsExpanded();

    case TrackProperty::ROLE_Mute: {
        // return item->trackItem()->isMuted();
        qDebug() << "item->trackItem()->isMuted()" << item->trackItem()->isSelected() << item->trackItem()->isSolo() << item->trackItem()->isMuted();
        return (!item->trackItem()->isSelected() && item->trackItem()->isSolo()) || item->trackItem()->isMuted();
    }
    case TrackProperty::ROLE_ImageURL:
        return item->trackItem()->imageUrl();
    case TrackProperty::ROLE_PanSelected:
        return item->trackItem()->automationSubMenuTitle() == "Pan";
    case TrackProperty::ROLE_VolumeSelected:
        return item->trackItem()->automationSubMenuTitle() == "Volume";
    case TrackProperty::ROLE_SpeakerOnSelected:
        return item->trackItem()->automationSubMenuTitle() == "SpeakerOn";

    case TrackProperty::ROLE_Color:
        return item->trackItem()->getTitleColor();

    case TrackProperty::ROLE_RowsExpandedCount:
        return item->trackItem()->getRowsExpandedCount();
    case TrackProperty::ROLE_AutomationLaneEnabled: {
        return item->trackItem()->automationLaneEnabled();
    }
    case TrackProperty::ROLE_AutomationLaneCount:
        return item->trackItem()->automationLaneModel()->count();
    case TrackProperty::ROLE_AutomationMenuTitle:
        return item->trackItem()->getAutomationMenuTitle();
    case TrackProperty::ROLE_AutomationSubMenuTitle:
        return item->trackItem()->automationSubMenuTitle();

    case TrackProperty::ROLE_TrackIndex:
        return item->trackItem()->trackIndex();

    case TrackProperty::ROLE_ObjectEnabled:
        return item->trackItem()->objectEnabled();

    case TrackProperty::ROLE_KeyFrameType:
        return item->trackItem()->keyFramesAutomationItem()->automation()->pointShape();

    case TrackProperty::ROLE_InputChannelsCount:
        return item->trackItem()->inputChannelCount();

    case TrackProperty::ROLE_OutputChannelsCount:
        return item->trackItem()->outputChannelCount();

    case TrackProperty::ROLE_Value:

        if (item->trackItem()->automationSubMenuTitle() == "None") {
            return 0;
        } else if (item->trackItem()->automationSubMenuTitle() == "Volume") {

            return item->trackItem()->volumeAutomationItem()->value();
        } else if (item->trackItem()->automationSubMenuTitle() == "SpeakerOn") {

            return item->trackItem()->speakerOnAutomationItem()->value();
        } else if (item->trackItem()->automationSubMenuTitle() == "Pan") {
            return item->trackItem()->panAutomationItem()->value();
        } else {

            if (item->trackItem()->effectAutomationItem()) {
                //                qDebug() << item->trackItem()->automationSubMenuTitle()
                //                         << item->trackItem()->effectAutomationItem()->value();
                return item->trackItem()->effectAutomationItem()->value();
            }

            if (item->trackItem()->visrAutomationItem())
                return item->trackItem()->visrAutomationItem()->value();
            return 0;
        }

    case TrackProperty::ROLE_AutomatedValue:

        if (item->trackItem()->automationSubMenuTitle() == "None") {
            return 0;
        } else if (item->trackItem()->automationSubMenuTitle() == "Volume") {
            return item->trackItem()->volumeAutomationItem()->automatedValue();
        } else if (item->trackItem()->automationSubMenuTitle() == "Pan") {
            return item->trackItem()->panAutomationItem()->automatedValue();
        } else if (item->trackItem()->automationSubMenuTitle() == "SpeakerOn") {
            return item->trackItem()->speakerOnAutomationItem()->automatedValue();
        } else {
            if (item->trackItem()->effectAutomationItem())
                return item->trackItem()->effectAutomationItem()->automatedValue();

            if (item->trackItem()->visrAutomationItem())
                return item->trackItem()->visrAutomationItem()->automatedValue();

            return 0;
        }

    case TrackProperty::ROLE_DefaultNormalized:

        if (item->trackItem()->automationSubMenuTitle() == "None") {
            return 0;
        } else if (item->trackItem()->automationSubMenuTitle() == "Volume") {
            return item->trackItem()->volumeAutomationItem()->automation()->defaultNormalized();
        } else if (item->trackItem()->automationSubMenuTitle() == "Pan") {
            return item->trackItem()->panAutomationItem()->automation()->defaultNormalized();
        } else if (item->trackItem()->automationSubMenuTitle() == "SpeakerOn") {
            return item->trackItem()->speakerOnAutomationItem()->automation()->defaultNormalized();
        } else {
            if (item->trackItem()->effectAutomationItem())
                return item->trackItem()->effectAutomationItem()->automation()->defaultNormalized();

            if (item->trackItem()->visrAutomationItem())
                return item->trackItem()->visrAutomationItem()->automation()->defaultNormalized();

            return 0;
        }

    case TrackProperty::ROLE_AutomatedNormalized:

        if (item->trackItem()->automationSubMenuTitle() == "None") {

            return 0;
        } else if (item->trackItem()->automationSubMenuTitle() == "Volume") {

            return item->trackItem()->volumeAutomationItem()->automation()->automatedNormalized();
        } else if (item->trackItem()->automationSubMenuTitle() == "Pan") {

            return item->trackItem()->panAutomationItem()->automation()->automatedNormalized();
        } else if (item->trackItem()->automationSubMenuTitle() == "SpeakerOn") {

            return item->trackItem()->speakerOnAutomationItem()->automation()->automatedNormalized();
        } else {

            if (item->trackItem()->effectAutomationItem()) {
                return item->trackItem()->effectAutomationItem()->automation()->automatedNormalized();
            }
            if (item->trackItem()->visrAutomationItem())
                return item->trackItem()->visrAutomationItem()->automation()->automatedNormalized();
            return 0;
        }
    case TrackProperty::ROLE_Normalized:

        if (item->trackItem()->automationSubMenuTitle() == "None") {
            return 0;
        } else if (item->trackItem()->automationSubMenuTitle() == "Volume") {

            return item->trackItem()->volumeAutomationItem()->automation()->normalized();
        } else if (item->trackItem()->automationSubMenuTitle() == "Pan") {
            return item->trackItem()->panAutomationItem()->automation()->normalized();
        } else if (item->trackItem()->automationSubMenuTitle() == "SpeakerOn") {
            return item->trackItem()->speakerOnAutomationItem()->automation()->normalized();
        } else {
            if (item->trackItem()->effectAutomationItem()) {
                //                qDebug() << "item->trackItem()->automationSubMenuTitle()"
                //                         << item->trackItem()->effectAutomationItem()->automation()->normalized();
                return item->trackItem()->effectAutomationItem()->automation()->normalized();
            }

            if (item->trackItem()->visrAutomationItem())
                return item->trackItem()->visrAutomationItem()->automation()->normalized();

            return 0;
        }
    case TrackProperty::ROLE_AutomatedActive:

        if (item->trackItem()->automationSubMenuTitle() == "None") {
            return false;
        } else if (item->trackItem()->automationSubMenuTitle() == "Volume") {
            return item->trackItem()->volumeAutomationItem()->automation()->isAutomated();
        } else if (item->trackItem()->automationSubMenuTitle() == "Pan") {
            return item->trackItem()->panAutomationItem()->automation()->isAutomated();
        } else if (item->trackItem()->automationSubMenuTitle() == "SpeakerOn") {
            return item->trackItem()->speakerOnAutomationItem()->automation()->isAutomated();
        } else {
            if (item->trackItem()->effectAutomationItem())
                return item->trackItem()->effectAutomationItem()->automation()->isAutomated();

            if (item->trackItem()->visrAutomationItem())
                return item->trackItem()->visrAutomationItem()->automation()->isAutomated();

            return 0;
        }
    case TrackProperty::ROLE_AutomatedEnabled:

        if (item->trackItem()->automationSubMenuTitle() == "None") {
            return 0;
        } else if (item->trackItem()->automationSubMenuTitle() == "Volume") {
            return item->trackItem()->volumeAutomationItem()->automation()->enabled();
        } else if (item->trackItem()->automationSubMenuTitle() == "Pan") {
            return item->trackItem()->panAutomationItem()->automation()->enabled();
        } else if (item->trackItem()->automationSubMenuTitle() == "SpeakerOn") {
            return item->trackItem()->speakerOnAutomationItem()->automation()->enabled();
        } else {
            if (item->trackItem()->effectAutomationItem())
                return item->trackItem()->effectAutomationItem()->automation()->enabled();

            if (item->trackItem()->visrAutomationItem())
                return item->trackItem()->visrAutomationItem()->automation()->enabled();

            return 0;
        }
    }

    return QVariant();
}

bool TracksModel::setData(const QModelIndex& index, const QVariant& value, int role)
{

    if (!index.isValid()) {
        return false;
    }

    // Retrieve the EffectProperty instance from the QModelIndex
    TrackProperty* item = static_cast<TrackProperty*>(index.internalPointer());

    if (!item) {
        return false;
    }

    if (index.isValid() && role == TrackProperty::ROLE_SelectedTrack) {

        m_areaInfo->setSelectedTrackItem(index.row());

        for (int j = 0; j < rootItem()->childCount(); j++) {

            auto it = rootItem()->child(j);
            it->trackItem()->setSelected(false);
            sltDataChanged(it, QVector<int>() << TrackProperty::ROLE_SelectedTrack);
        }

        item->trackItem()->setSelected(true);
        sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_SelectedTrack);

        Q_EMIT sigUpdate();
    }

    if (index.isValid() && role == TrackProperty::ROLE_Clear) {
        if (value.toBool() == true) {

            auto automation = item->trackItem()->automationItem()->automation();
            QJsonArray array;
            QJsonObject jObject2;
            jObject2["track_index"] = item->trackItem()->trackIndex();
            jObject2["sub_track_index"] = automation->getSubTrackIndex();
            jObject2["menu"] = automation->type();
            jObject2["sub_menu"] = automation->subType();
            jObject2["state"] = automation->getLinesState();
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            array.append(jObject2);
            ActionStack::instance()->pushInUndo(array);

            item->trackItem()->automationItem()->automation()->clear();
            Q_EMIT sigUpdate();
            return true;
        }
    }
    if (index.isValid() && role == TrackProperty::ROLE_RowsExpanded) {

        if (value.toBool() == true) {

            if (item->trackItem()->getLastRowExpanded() == 1) {
                item->trackItem()->setRowsExpandedCount(m_areaInfo->minTitlebarCount());
            } else {
                item->trackItem()->setRowsExpandedCount(item->trackItem()->getLastRowExpanded());
            }

        } else {
            item->trackItem()->setLastRowExpanded(item->trackItem()->rowsExpandedCount());
            item->trackItem()->setRowsExpandedCount(1);
        }
        item->trackItem()->setRowsExpanded(value.toBool());
        Q_EMIT sigUpdate();

        sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_TrackRect);

        return true;
    }
    if (index.isValid() && role == TrackProperty::ROLE_ObjectEnabled) {
        item->trackItem()->setObjectEnabled(value.toBool());
        sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_ObjectEnabled);
        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_KeyFrameType) {
        item->trackItem()->keyFramesAutomationItem()->automation()->setPointShape((Automation::PointShape)value.toInt());
        sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_KeyFrameType);
        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_InputChannelsCount) {
        item->trackItem()->setInputChannelCount(value.toInt());
        sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_InputChannelsCount);

        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_OutputChannelsCount) {
        item->trackItem()->setOutputChannelCount(value.toInt());
        sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_OutputChannelsCount);

        qDebug() << "CHANGED" << value.toInt();
        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_ImageURL) {
        item->trackItem()->setImageUrl(value.toString());
        return true;
    }
    if (index.isValid() && role == TrackProperty::ROLE_AutomationLaneEnabled) {
        setAutomationLaneEnabled(item->trackItem()->trackIndex(), value.toBool());
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_VolumeNormalized) {
        item->trackItem()->volumeAutomationItem()->setNormalized(value.toFloat());
        sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_Normalized << TrackProperty::ROLE_Value);
        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_PanNormalized) {
        item->trackItem()->panAutomationItem()->setNormalized(value.toFloat());
        sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_Normalized << TrackProperty::ROLE_Value);

        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_SpeakerOnNormalized) {
        item->trackItem()->speakerOnAutomationItem()->setNormalized(value.toFloat());
        sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_Normalized << TrackProperty::ROLE_Value);

        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_AutomatedVolumeEnabled) {
        item->trackItem()->volumeAutomationItem()->setEnabled(value.toBool());

        sltDataChanged(item,
            QVector<int>()
                << TrackProperty::ROLE_AutomatedNormalized
                << TrackProperty::ROLE_AutomatedValue
                << TrackProperty::ROLE_AutomatedVolumeNormalized
                << TrackProperty::ROLE_VolumeValue
                << TrackProperty::ROLE_AutomatedVolumeEnabled
                << TrackProperty::ROLE_AutomatedEnabled);

        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_AutomatedSpeakerOnEnabled) {
        item->trackItem()->speakerOnAutomationItem()->setEnabled(value.toBool());

        sltDataChanged(item,
            QVector<int>()
                << TrackProperty::ROLE_AutomatedNormalized
                << TrackProperty::ROLE_AutomatedValue
                << TrackProperty::ROLE_AutomatedSpeakerOnNormalized
                << TrackProperty::ROLE_SpeakerOnValue
                << TrackProperty::ROLE_AutomatedSpeakerOnEnabled
                << TrackProperty::ROLE_AutomatedEnabled);

        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_AutomatedPanEnabled) {
        item->trackItem()->panAutomationItem()->setEnabled(value.toBool());

        sltDataChanged(item,
            QVector<int>()
                << TrackProperty::ROLE_AutomatedNormalized
                << TrackProperty::ROLE_AutomatedValue
                << TrackProperty::ROLE_AutomatedPanNormalized
                << TrackProperty::ROLE_PanValue
                << TrackProperty::ROLE_AutomatedSpeakerOnEnabled
                << TrackProperty::ROLE_AutomatedEnabled);

        Q_EMIT sigUpdate();
        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_AutomationMenuTitle) {

        QString current = item->trackItem()->getAutomationMenuTitle() + "," + item->trackItem()->automationSubMenuTitle();
        if (value.toString() != current) {

            QStringList list = value.toString().split(",");
            item->trackItem()->setAutomationMenuTitle(list[0]);
            item->trackItem()->setAutomationSubMenuTitle(list[1]);

            sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_AutomationMenuTitle << TrackProperty::ROLE_Value << TrackProperty::ROLE_Normalized << TrackProperty::ROLE_AutomatedEnabled << TrackProperty::ROLE_AutomatedActive << TrackProperty::ROLE_DefaultNormalized);
            sltDataChanged(item, QVector<int>() << TrackProperty::ROLE_AutomationSubMenuTitle << TrackProperty::ROLE_Value << TrackProperty::ROLE_Normalized << TrackProperty::ROLE_AutomatedEnabled << TrackProperty::ROLE_AutomatedActive);

            if (list[1] == "Volume" || list[1] == "Pan" || list[1] == "SpeakerOn" || list[1] == "None") {
                Q_EMIT sigUpdate();
                return true;
            }

            item->automationChanged();

            Q_EMIT sigUpdate();
        }

        return true;
    }

    if (index.isValid() && role == TrackProperty::ROLE_Normalized) {

        if (item->trackItem()->automationSubMenuTitle() == "Volume") {
            item->trackItem()->volumeAutomationItem()->setNormalized(value.toDouble());
        } else if (item->trackItem()->automationSubMenuTitle() == "Pan") {
            item->trackItem()->panAutomationItem()->setNormalized(value.toDouble());
        } else if (item->trackItem()->automationSubMenuTitle() == "SpeakerOn") {
            item->trackItem()->speakerOnAutomationItem()->setNormalized(value.toDouble());
        } else {
            if (item->trackItem()->effectAutomationItem())
                item->trackItem()->effectAutomationItem()->setNormalized(value.toDouble());

            if (item->trackItem()->visrAutomationItem())
                item->trackItem()->visrAutomationItem()->setNormalized(value.toDouble());
        }

        sltDataChanged(item,
            QVector<int>()
                << TrackProperty::ROLE_VolumeNormalized
                << TrackProperty::ROLE_VolumeValue
                << TrackProperty::ROLE_PanNormalized
                << TrackProperty::ROLE_PanValue
                << TrackProperty::ROLE_SpeakerOnNormalized
                << TrackProperty::ROLE_SpeakerOnValue
                << TrackProperty::ROLE_Normalized
                << TrackProperty::ROLE_Value);

        Q_EMIT sigUpdate();
        return true;
    }
    if (index.isValid() && role == TrackProperty::ROLE_AutomatedEnabled) {

        if (item->trackItem()->automationSubMenuTitle() == "Volume") {

            item->trackItem()->volumeAutomationItem()->setEnabled(value.toBool());
        } else if (item->trackItem()->automationSubMenuTitle() == "Pan") {
            item->trackItem()->panAutomationItem()->setEnabled(value.toBool());
        } else if (item->trackItem()->automationSubMenuTitle() == "SpeakerOn") {
            item->trackItem()->speakerOnAutomationItem()->setEnabled(value.toBool());
        } else {
            if (item->trackItem()->effectAutomationItem())
                item->trackItem()->effectAutomationItem()->setEnabled(value.toBool());

            if (item->trackItem()->visrAutomationItem())
                item->trackItem()->visrAutomationItem()->setEnabled(value.toBool());
        }

        sltDataChanged(item,
            QVector<int>()
                << TrackProperty::ROLE_AutomatedEnabled
                << TrackProperty::ROLE_AutomatedPanEnabled
                << TrackProperty::ROLE_AutomatedVolumeEnabled
                << TrackProperty::ROLE_AutomatedSpeakerOnEnabled);

        Q_EMIT sigUpdate();
        return true;
    }
}

MenuModel::MenuModel(QObject* parent)
    : QAbstractListModel(parent)
{
    // Initial menu items
    //    menuItems.append(MenuItem("File", { MenuItem("New"), MenuItem("Open") }));
    //    menuItems.append(MenuItem("Edit", { MenuItem("Cut"), MenuItem("Copy"), MenuItem("Paste") }));
    //    menuItems.append(MenuItem("Help"));
}

MenuModel::~MenuModel()
{
    qDebug() << "DELETED";
}

int MenuModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return menuItems.count();
}

QVariant MenuModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= menuItems.count()) {
        return QVariant();
    }

    const MenuItem& item = menuItems[index.row()];

    if (role == TitleRole) {
        return item.title;
    } else if (role == SubItemsRole) {
        QVariantList subItems;
        for (const auto& subItem : item.subItems) {
            subItems << QVariant::fromValue(subItem.title);
        }
        return subItems;
    }

    return QVariant();
}

QHash<int, QByteArray> MenuModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[SubItemsRole] = "subItems";
    return roles;
}

void MenuModel::addMenuItem(const QString& title, const QStringList& actions)
{

    beginInsertRows(QModelIndex(), menuItems.size(), menuItems.size());

    QVector<MenuItem> items;
    for (int i = 0; i < actions.size(); i++) {
        items.append(MenuItem(actions[i]));
    }

    menuItems.append(MenuItem(title, items));
    endInsertRows();

    Q_EMIT countChanged();
}

void MenuModel::removeMenuItem(int index)
{
    if (index >= 0 && index < menuItems.size()) {
        beginRemoveRows(QModelIndex(), index, index);
        menuItems.removeAt(index);
        endRemoveRows();
        Q_EMIT countChanged();
    }
}

void MenuModel::removeMenuItem(QString title)
{
    for (int i = 0; i < menuItems.size(); i++) {
        if (menuItems[i].title == title) {
            removeMenuItem(i);
        }
    }
}

QVariant MenuModel::getTitle(int index) const
{
    if (index >= 0 && index < menuItems.count()) {
        return menuItems.at(index).title;
    }
    return QVariant();
}

QStringList MenuModel::getActions(int index) const
{
    if (index >= 0 && index < menuItems.count()) {
        QStringList list;
        for (int i = 0; i < menuItems.at(index).subItems.size(); i++) {
            list.append(menuItems.at(index).subItems.at(i).title);
        }
        return list;
    }
    return QStringList();
}
