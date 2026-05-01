#include "sampleeditor.h"
#include "audio/audioengine.h"
#include "audio/song.h"
#include "audio/track.h"
#include <QDir>
#include <QThread>
#include <audio/engine.h>
#include <core/screeninterface.h>
#include <qdatetime.h>

SampleEditor::SampleEditor(QQuickItem* parent)
    : INanoItem(parent)
{

    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setMouseEventsEnabled(true);
    setMouseHoverEventsEnabled(true);
    setFlag(QQuickItem::ItemIsFocusScope, true);
    setFlag(QQuickItem::ItemAcceptsDrops, true);

    _areaInfo = new AreaInfo(Q_NULLPTR, AreaInfo::AreaType_SampleEditor);
    _timerWidth.setInterval(40);
    _timerWidth.start();
    connect(&_timerWidth, &QTimer::timeout, this, &SampleEditor::sltTimerWidthTimeout);

    _backgroundItem = QSharedPointer<BackgroundItem>(new BackgroundItem());
    _beatBarItem = QSharedPointer<BeatBarItem>(new BeatBarItem(nullptr, _areaInfo));
    _plotItem = QSharedPointer<PlotItem>(new PlotItem(_areaInfo));
    _waveItem = QSharedPointer<WaveItem>(new WaveItem(this, _areaInfo));
    _warpItem = QSharedPointer<WarpAreaItem>(new WarpAreaItem(this, _areaInfo));

    _scrubAreaItem = QSharedPointer<ScrubAreaItem>(new ScrubAreaItem(_areaInfo));
    _automation = QSharedPointer<Automation>(new Automation(_areaInfo, -1, "None", "None"));
    _automationItem = QSharedPointer<VolumeAutomationItem>(new VolumeAutomationItem(0, _automation, _areaInfo));
    _markerItem = QSharedPointer<MarkerAreaItem>(new MarkerAreaItem(_areaInfo));
    _markerItem->setShowIndicator(true);
    connect(_beatBarItem.data(), &BeatBarItem::sigZoomLevelChanged, [this](ZoomLevel level) {
        if (_zoomLevel != level) {
            _zoomLevel = level;
        }
    });

    connect(_beatBarItem.data(), &BeatBarItem::sigPlayMoveModeChanged, [this](int mode) {

    });

    //    connect(_beatBarItem.data(), &BeatBarItem::sigStartTimeChanged, [this](double time) {
    //        _areaInfo->setStartTime(time);
    //    });

    //    connect(_beatBarItem.data(), &BeatBarItem::sigEndTimeChanged, [this](double time) {
    //        _areaInfo->setEndTime(time);
    //    });

    //    connect(_beatBarItem.data(), &BeatBarItem::sigCurrentDurationChanged, [this](double time) {
    //        _areaInfo->setClipDuration(time);
    //    });

    connect(_beatBarItem.data(), &BeatBarItem::sigCursorPosition, [this](QPoint pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(_beatBarItem.data(), &BeatBarItem::sigSetCursor, [this](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(_warpItem.data(), &WarpAreaItem::sigCursorPosition, [this](QPointF pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(_warpItem.data(), &WarpAreaItem::sigSetCursor, [this](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(_scrubAreaItem.data(), &WarpAreaItem::sigSetCursor, [this](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(_scrubAreaItem.data(), &ScrubAreaItem::sigCursorPosition, [this](QPointF pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(_scrubAreaItem.data(), &ScrubAreaItem::sigSetCursor, [this](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(_automationItem.data(), &AutomationItem::sigCursorPosition, [this](QPointF pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(_automationItem.data(), &AutomationItem::sigSetCursor, [this](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(_scrubAreaItem.data(), &ScrubAreaItem::sigCurrentIndicatorChanged, this, &SampleEditor::sltScrubCurrentIndicatorChanged);

    _playbackUpdateTimer.setInterval(20);
    connect(&_playbackUpdateTimer, &QTimer::timeout, this, &SampleEditor::sltPlatBackUpdateTimeout);
    _playbackUpdateTimer.start();
}

QSharedPointer<MarkerAreaItem> SampleEditor::markerItem() const
{
    return _markerItem;
}

bool SampleEditor::automationEnabled() const
{
    return _automationEnabled;
}

void SampleEditor::setAutomationEnabled(bool automationEnabled)
{
    if (_automationEnabled != automationEnabled) {
        _automationEnabled = automationEnabled;
        _automationItem->setShowEnvelopes(automationEnabled);

        Q_EMIT automationEnabledChanged();
        update();
    }
}

QSharedPointer<AutomationItem> SampleEditor::automationItem() const
{
    return _automationItem;
}

QSharedPointer<ScrubAreaItem> SampleEditor::scrubAreaItem() const
{
    return _scrubAreaItem;
}

QSharedPointer<WarpAreaItem> SampleEditor::warpItem() const
{
    return _warpItem;
}

double SampleEditor::endMarker() const
{
    return _endMarker;
}

void SampleEditor::setEndMarker(double endMarker)
{
    _endMarker = endMarker;
}

double SampleEditor::startMarker() const
{
    return _startMarker;
}

void SampleEditor::setStartMarker(double startMarker)
{
    _startMarker = startMarker;
}

AreaInfo* SampleEditor::areaInfo() const
{
    return _areaInfo;
}

QSharedPointer<WaveItem> SampleEditor::waveItem() const
{
    return _waveItem;
}

SampleClip* SampleEditor::sampleClip() const
{
    return _sampleClip;
}

void SampleEditor::setSampleClip(SampleClip* sampleClip)
{
    _sampleClip = sampleClip;

    _areaInfo->setClipProperties(0, _sampleClip->currentFileDuration(), _sampleClip->currentFileDuration());

    update();

    _areaInfo->setMaxEndTime(_sampleClip->currentFileDuration());
    _areaInfo->setFileDuration(_sampleClip->currentFileDuration());

    _areaInfo->setEndBraceMarker(_sampleClip->currentFileDuration());
}

QSharedPointer<PlotItem> SampleEditor::plotItem() const
{
    return _plotItem;
}

QSharedPointer<BeatBarItem> SampleEditor::beatBarItem() const
{
    return _beatBarItem;
}

void SampleEditor::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{

    _boundingRect = newGeometry.toRect();
    QRect rect = _boundingRect;

    backgroundItem()->setRect(_boundingRect);

    rect.setY(20);
    rect.setHeight(20);
    beatBarItem()->setRect(rect);

    rect = _boundingRect;
    rect.setY(40);
    rect.setHeight(20);
    scrubAreaItem()->setRect(rect);

    rect = _boundingRect;
    rect.setY(60);
    rect.setHeight(20);
    warpItem()->setRect(rect);

    rect = _boundingRect;
    rect.setY(100);
    rect.setHeight(_boundingRect.height() - 100 - 10);
    automationItem()->setRect(rect);

    rect = _boundingRect;
    rect.setY(40);
    rect.setHeight(_boundingRect.height() - 40 - 10);
    plotItem()->setRect(rect);

    rect = _boundingRect;
    rect.setY(100);
    rect.setHeight(_boundingRect.height() - 100 - 10);
    waveItem()->setRect(rect);

    rect.setY(60);
    rect.setHeight(_boundingRect.height() - 60 - 10);
    markerItem()->setRect(rect);
}

void SampleEditor::mousePressEvent(QMouseEvent* event)
{
    _beatBarItem->mousePressEvent(event);
    _warpItem->mousePressEvent(event);

    _scrubAreaItem->mousePressEvent(event);
    _automationItem->mousePressEvent(event);
    update();
}

void SampleEditor::mouseMoveEvent(QMouseEvent* event)
{
    _beatBarItem->mouseMoveEvent(event);
    _warpItem->mouseMoveEvent(event);
    _scrubAreaItem->mouseMoveEvent(event);
    _automationItem->mouseMoveEvent(event);
    update();
}

void SampleEditor::mouseReleaseEvent(QMouseEvent* event)
{
    _beatBarItem->mouseReleaseEvent(event);
    _warpItem->mouseReleaseEvent(event);
    _scrubAreaItem->mouseReleaseEvent(event);
    _automationItem->mouseReleaseEvent(event);
}

void SampleEditor::hoverMoveEvent(QHoverEvent* event)
{

    auto warp_result = _warpItem->hoverMoveEvent(event);
    if (warp_result == 1) {
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
    }

    auto loop_result = _scrubAreaItem->hoverMoveEvent(event);

    if (loop_result == 1) {
        QGuiApplication::setOverrideCursor(Qt::SplitHCursor);
    } else if (loop_result == 2) {
        QGuiApplication::setOverrideCursor(Qt::IBeamCursor);
    }

    auto scrub_result = _scrubAreaItem->hoverMoveEvent(event);

    if (scrub_result == 1) {
        QGuiApplication::setOverrideCursor(Qt::PointingHandCursor);
    }

    _automationItem->hoverMoveEvent(event);

    if (warp_result != 0 && scrub_result != 0 && loop_result != 0)
        QGuiApplication::setOverrideCursor(Qt::ArrowCursor);

    update();
}

void SampleEditor::mouseDoubleClickEvent(QMouseEvent* event)
{
    _warpItem->mouseDoubleClickEvent(event);
    _automationItem->mouseDoubleClickEvent(event);
}

QSharedPointer<BackgroundItem> SampleEditor::backgroundItem() const
{
    return _backgroundItem;
}

QNanoQuickItemPainter* SampleEditor::createItemPainter() const
{
    return new SampleEditorPainter(this);
}

void SampleEditor::sltTimerWidthTimeout()
{
    if (_areaInfo->lastWidth() != width()) {

        int min = ScreenInterface::instance()->freeResolution(20);
        int max = ScreenInterface::instance()->freeResolution(40);

        double barDuration = 2.0;
        double final_size = (max + min) / 2.0;

        _areaInfo->init(width(), final_size, barDuration);

        update();
        Q_EMIT lastWidthChanged();
    }
}

void SampleEditor::sltScrubCurrentIndicatorChanged(double time)
{

    if (_sampleClip) {
        double samples = time / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        samples += _sampleClip->sampleBuffer()->baseStartFrame();
        AudioManager::getSong()->setToTime(samples / 256.0);
        AudioManager::getSong()->readyToPlay();
    }
}

void SampleEditor::sltPlatBackUpdateTimeout()
{
    if (_sampleClip) {
        if (AudioManager::getSong()->isPause() == false) {

            auto baseStartTime = _sampleClip->sampleBuffer()->baseStartFrame() * 1000.0 / AudioManager::audioEngine()->outputSampleRate();
            double duration_in_seconds = (AudioManager::getSong()->getPlayPos().getTicks() * 256.0) / AudioManager::audioEngine()->outputSampleRate();
            auto playbackMarker = duration_in_seconds * 1000.0 - baseStartTime;
            //_areaInfo->setPlaybackMarker(playbackMarker);
            update();
        }
    }
}

SampleEditorPainter::SampleEditorPainter(const SampleEditor* sampleEditor)
    : INanoPainterItem()
    , _sampleEditor(sampleEditor)
{
}

void SampleEditorPainter::draw(IPainter* painter)
{
    if (_sampleEditor == nullptr)
        return;

    _sampleEditor->backgroundItem()->draw(painter);
    _sampleEditor->beatBarItem()->draw(painter);
    _sampleEditor->plotItem()->draw(painter);
    _sampleEditor->waveItem()->draw(painter);
    _sampleEditor->warpItem()->draw(painter);

    _sampleEditor->scrubAreaItem()->draw(painter);
    _sampleEditor->automationItem()->draw(painter);
    _sampleEditor->markerItem()->draw(painter);
}
