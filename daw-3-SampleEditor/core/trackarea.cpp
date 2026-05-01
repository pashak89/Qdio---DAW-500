#include "trackarea.h"

#include "audio/audioengine.h"
#include "audio/song.h"
#include "audio/track.h"
#include "cliparea.h"
#include <QDir>
#include <QDrag>
#include <QJsonArray>
#include <QMimeData>
#include <QThread>
#include <audio/engine.h>
#include <core/screeninterface.h>
#include <qdatetime.h>
QMutex _mutex_trackArea;
TrackArea::TrackArea(QQuickItem* parent)
    : INanoItem(parent)
{

    setZ(11e6);
}

void TrackArea::initialize()
{

    connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &TrackArea::update);
    connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &TrackArea::update);

    connect(_areaInfo, &AreaInfo::sigSelectedTrackItemRectChanged, this, &TrackArea::update);
    connect(_areaInfo, &AreaInfo::startContentYChanged, this, &TrackArea::update);

    connect(_areaInfo, &AreaInfo::sigUpdateUI, this, &TrackArea::update);

    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &TrackArea::update);

    auto _tracksModel = _clipArea->tracksModel();

    connect(_tracksModel, &TracksModel::sigUpdate, this, &TrackArea::update);
    if (_trackIndex < 0) {
        auto masterTrack = AudioManager::getSong()->addSampleTrack(_areaInfo, -1);
        _masterTrack = new TrackItem(-1, masterTrack, _areaInfo);
        _masterTrack->setMasterTrack(true);
        connect(_masterTrack, &TrackItem::rowsExpandedChanged, this, &TrackArea::sltMasterRowsExpandedChanged);
    } else {

        auto trackItem = _tracksModel->trackItem(_trackIndex);

        connect(trackItem.data(), &TrackItem::sigUpdate, this, &TrackArea::update);
    }

    update();
}

void TrackArea::setMasterTrackHeight(int posY)
{

    if (_trackIndex >= 0) {
        return;
    }

    if (posY > 0) {

        _masterTrack->setRowsExpandedCount(_masterTrack->rowsExpandedCount() + 1);

    } else if (posY < 0) {

        if (_masterTrack->rowsExpandedCount() > 1) {
            _masterTrack->setRowsExpandedCount(_masterTrack->rowsExpandedCount() - 1);
        }
    }

    _masterTrack->setLastRowExpanded(_masterTrack->rowsExpandedCount());

    if (_masterTrack->rowsExpandedCount() == 1) {

        if (_masterTrack->automationLaneModel()->count() > 0) {
            if (_masterTrack->automationLaneModel()->totalRowCount() == 1) {
                _masterTrack->setRowsExpanded(false);
            } else {
                _masterTrack->setRowsExpanded(false);
            }
        } else {
            _masterTrack->setRowsExpanded(false);
        }

    } else {
        _masterTrack->setRowsExpanded(true);
    }
}
bool TrackArea::masterTrackVisible() const
{
    return _masterTrackVisible;
}

void TrackArea::setMasterTrackVisible(bool masterTrackVisible)
{
    _masterTrackVisible = masterTrackVisible;
    Q_EMIT masterTrackVisibleChanged();
    update();
}

AreaInfo* TrackArea::areaInfo() const
{
    return _areaInfo;
}

void TrackArea::sltAddVisr(int trackIndex, bool enabled, bool reverbEnabled)
{
    if (_trackIndex < 0)
        return;

    if (trackIndex != _trackIndex)
        return;

    auto visr = QSharedPointer<VisrEffect>(new VisrEffect(_areaInfo));
    visr->initialize(_trackIndex, visr);
    auto sampleTrack = (SampleTrack*)_clipArea->tracksModel()->trackItem(_trackIndex)->track();
    sampleTrack->audioPort()->addVisr(visr->plugin());

    connect(visr.data(), &Effect::sigSetCursor, [](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(visr.data(), &Effect::sigCursorPosition, [this](QPoint pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(visr.data(), &Effect::sigUpdate, _areaInfo, &AreaInfo::sigUpdateUI, Qt::QueuedConnection);

    _clipArea->tracksModel()->visrAdded(_trackIndex);

    _clipArea->tracksModel()->visrEffect(_trackIndex)->setEnabled(enabled);
    _clipArea->tracksModel()->visrEffect(_trackIndex)->setReverbEnabled(reverbEnabled);
}

void TrackArea::sltMultiEqEffect(int trackIndex)
{
    if (_trackIndex < 0)
        return;

    if (trackIndex != _trackIndex)
        return;

    auto vst = QSharedPointer<MultiEqEffect>(new MultiEqEffect(trackIndex, _areaInfo));

    vst->initialize(trackIndex, vst);

    auto sampleTrack = (SampleTrack*)_clipArea->tracksModel()->trackItem(trackIndex)->track();
    int index = sampleTrack->audioPort()->effects()->appendEffect(vst);
    auto effect = sampleTrack->audioPort()->effects()->effect(index);

    effect->setEnabled(true);

    connect(effect.data(), &Effect::sigSetCursor, [](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(effect.data(), &Effect::sigCursorPosition, [this](QPoint pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(effect.data(), &Effect::sigUpdate, _areaInfo, &AreaInfo::sigUpdateUI, Qt::QueuedConnection);

    connect(effect.data(), &Effect::sigSaveInUndoItems, _clipArea, &ClipArea::saveInUndoItems, Qt::QueuedConnection);
    connect(effect.data(), &Effect::sigSaveInRedoItems, _clipArea, &ClipArea::saveInRedoItems, Qt::QueuedConnection);

    _clipArea->tracksModel()->effectAdded(trackIndex, index);

    Q_EMIT _clipArea->effectsChanged(trackIndex, index);
}

void TrackArea::sltAddEqEffect(int trackIndex, QString name, bool enabled)
{

    if (_trackIndex < 0)
        return;

    if (trackIndex != _trackIndex)
        return;

    auto vst = _clipArea->loadingEffects()[name];

    _clipArea->loadingEffects().remove(name);

    auto sampleTrack = (SampleTrack*)_clipArea->tracksModel()->trackItem(trackIndex)->track();

    int index = sampleTrack->audioPort()->effects()->appendEffect(vst);

    auto effect = sampleTrack->audioPort()->effects()->effect(index);

    connect(effect.data(), &Effect::sigSetCursor, [](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(effect.data(), &Effect::sigCursorPosition, [this](QPoint pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(effect.data(), &Effect::sigUpdate, _areaInfo, &AreaInfo::sigUpdateUI, Qt::QueuedConnection);

    connect(effect.data(), &Effect::sigControlVisibleChanged, _clipArea, &ClipArea::sltControlVisibleChanged);
    connect(effect.data(), &Effect::sigEnableChanged, _clipArea, &ClipArea::sltEffectEnableChanged);

    _clipArea->tracksModel()->effectAdded(trackIndex, index);
    vst->setEffectIndex(index);

    Q_EMIT _clipArea->effectsChanged(trackIndex, index);

    _clipArea->saveAddEffectInUndo(trackIndex, "vst", index);

    effect->setEnabled(enabled);
}

void TrackArea::sltMasterRowsExpandedChanged(int index)
{
}
void TrackArea::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;
    if (_trackIndex >= 0) {

        connect(_clipArea, &ClipArea::sigAddVisr, this, &TrackArea::sltAddVisr);

        connect(_clipArea, &ClipArea::sigAddEffect, this, &TrackArea::sltAddEqEffect);

        auto trackItem = _clipArea->tracksModel()->trackItem(_trackIndex);

        connect(trackItem.get(), &TrackItem::sigCursorPosition, [this](QPoint pos) {
            QCursor::setPos(mapToGlobal(pos).toPoint());
        });
    }
}

int TrackArea::trackIndex() const
{
    return _trackIndex;
}

void TrackArea::setTrackIndex(int newTrackIndex)
{
    _trackIndex = newTrackIndex;
}

TrackItem* TrackArea::masterTrack() const
{
    auto qobj = _masterTrack; // Pointer to member shared-ptr-managed object.
    QQmlEngine::setObjectOwnership(qobj, QQmlEngine::CppOwnership);
    return qobj;
}

void TrackArea::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;
    initialize();
}

QNanoQuickItemPainter* TrackArea::createItemPainter() const
{
    return new TrackAreaPainter(_trackIndex, this);
}

void TrackArea::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{

    //    if (_plotItem == Q_NULLPTR)
    //        return;

    //    QRect rect = newGeometry.toRect();
    //    _boundingRect = newGeometry.toRect();

    //    _boundingRect.setX(0);
    //    rect.setX(0);

    //    backgroundItem()->setRect(_boundingRect);

    //    rect = _boundingRect;
    //    rect.setY(0);
    //    rect.setHeight(_boundingRect.height());
    //    plotItem()->setRect(rect);

    //    rect.setY(0);
    //    rect.setHeight(_boundingRect.height());
    //    markerItem()->setRect(rect);

    //    if (_trackIndex < 0) {
    //        rect = _boundingRect;
    //        rect.setY(0);
    //        masterTrack()->setRect(rect);
    //    } else {

    //        clipArea()->tracksModel()->trackItem(_trackIndex)->setRect(_boundingRect);

    //        rect = _boundingRect;
    //        rect.setY(clipArea()->tracksModel()->trackItem(_trackIndex)->globalYPosition());
    //        rect.setHeight(_boundingRect.height());
    //        _areaInfo->selectionArea()->setTracksRect(_trackIndex, rect);
    //    }
}

ClipArea* TrackArea::clipArea() const
{
    return _clipArea;
}

TrackAreaPainter::TrackAreaPainter(int trackIndex, const TrackArea* trackArea)
    : INanoPainterItem()
    , _trackArea(trackArea)
    , _trackIndex(trackIndex)
{
}

void TrackAreaPainter::draw(IPainter* painter)
{

    if (_trackIndex < 0) {

        if (_trackArea->masterTrackVisible())
            _trackArea->masterTrack()->draw(painter);

    } else {

        _trackArea->clipArea()->tracksModel()->trackItem(_trackIndex)->draw(painter);
    }
}
