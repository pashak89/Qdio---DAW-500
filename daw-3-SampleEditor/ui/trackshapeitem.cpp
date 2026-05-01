#include "trackshapeitem.h"
#include "core/cliparea.h"
#include "core/global.h"
#include "ui/clipshapeitem.h"

#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QMimeData>
#include <sndfile.h>
TrackShapeItem::TrackShapeItem(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemAcceptsDrops, true); // required for QQuickItem
    setAcceptedMouseButtons(Qt::AllButtons); // optional; not strictly required
    setFlag(ItemHasContents, true);
    setFlag(ItemAcceptsDrops, true);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    setClip(true);

    _timerWidth.setInterval(300);
    connect(&_timerWidth, &QTimer::timeout, this, &TrackShapeItem::sltTimerWidthTimeout);
}

TrackItem* TrackShapeItem::masterTrack() const
{
    auto qobj = _masterTrack; // Pointer to member shared-ptr-managed object.
    QQmlEngine::setObjectOwnership(qobj.data(), QQmlEngine::CppOwnership);
    return qobj.data();
}

bool TrackShapeItem::masterTrackVisible() const
{
    return _masterTrackVisible;
}

void TrackShapeItem::setMasterTrackVisible(bool masterTrackVisible)
{
    _masterTrackVisible = masterTrackVisible;
    Q_EMIT masterTrackVisibleChanged();
    update();
}

void TrackShapeItem::setMasterTrackHeight(int posY)
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
    _clipArea->drawGrids();
}

void TrackShapeItem::sltTimerWidthTimeout()
{
    int _width = width();
    if (_areaInfo->lastWidth() != _width && _width > 0) {
        int min = ScreenInterface::instance()->freeResolution(20);
        int max = ScreenInterface::instance()->freeResolution(40);

        double barDuration = 2.0;
        double final_size = (max + min) / 2.0;

        _areaInfo->init(_width, final_size, barDuration);

        update();
    }
}

ClipArea* TrackShapeItem::clipArea() const
{
    return _clipArea;
}

void TrackShapeItem::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;

    auto _tracksModel = _clipArea->tracksModel();

    if (_trackIndex >= 0) {

        connect(_clipArea, &ClipArea::sigAddVisr, this, &TrackShapeItem::sltAddVisr);

        connect(_clipArea, &ClipArea::sigAddEffect, this, &TrackShapeItem::sltAddEqEffect);

        auto trackItem = _clipArea->tracksModel()->trackItem(_trackIndex);

        connect(trackItem.get(), &TrackItem::sigCursorPosition, [this](QPoint pos) {
            QCursor::setPos(mapToGlobal(pos).toPoint());
        });

        _timerWidth.start();
    } else {
        _masterTrack = _clipArea->masterTrack();
    }

    connect(_clipArea, &ClipArea::sigUpdateUI, this, &TrackShapeItem::sltClipPropertiesChanged);
}

void TrackShapeItem::sltAddVisr(int trackIndex, bool enabled, bool reverbEnabled)
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

void TrackShapeItem::sltAddEqEffect(int trackIndex, QString name, bool enabled)
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

int TrackShapeItem::trackIndex() const
{
    return _trackIndex;
}

void TrackShapeItem::setTrackIndex(int index)
{
    _trackIndex = index;
}

int TrackShapeItem::outputSampleRate()
{
    return AudioManager::audioEngine()->outputSampleRate();
}

void TrackShapeItem::addClip(ClipShapeItem* clip)
{

    //    auto _tracksModel = _clipArea->tracksModel();
    //    auto trackItem = _tracksModel->trackItem(_trackIndex);
    //    trackItem->addSampleClip(clip);
    //    emit clipsChanged();
}

void TrackShapeItem::clearClips()
{

    emit clipsChanged();
}

AreaInfo* TrackShapeItem::areaInfo() const
{
    return _areaInfo;
}

void TrackShapeItem::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;

    connect(_areaInfo, &AreaInfo::sigSelectedTrackItemRectChanged, this, &TrackShapeItem::sltClipPropertiesChanged);
    connect(_areaInfo, &AreaInfo::sigSelectedTrackItemChanged, this, &TrackShapeItem::sltClipPropertiesChanged);

    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &TrackShapeItem::sltClipPropertiesChanged);

    connect(_areaInfo, &AreaInfo::timeSelectionChanged, this, &TrackShapeItem::sltClipPropertiesChanged);
}

void TrackShapeItem::sltClipPropertiesChanged()
{

    m_linesA = _clipArea->trackSegLines(_trackIndex);

    if (_trackIndex == -1) {
        m_selected_rects = _clipArea->masterTrackSelectedRects();

    } else {
        m_selected_rects = _clipArea->trackSelectedRects(_trackIndex);
    }

    m_rectsA = _clipArea->trackSegRects(_trackIndex);

    update();
}

void TrackShapeItem::findSnap(QVector<QPair<double, double>> _snapPoints, double posX, double posY, double& final_xpos, double& final_ypos)
{
    double first = 0;
    double last = 0;

    if (_snapPoints.size() == 0) {
        final_xpos = posX;
        return;
    }

    for (int k = 0; k < _snapPoints.size(); k++) {

        if (_snapPoints[k].first <= posX && _snapPoints[k].second >= posX) {

            first = _snapPoints[k].first;
            last = _snapPoints[k].second;
            break;
        }
    }

    if (_snapPoints.size() > 0) {
        if (_snapPoints[_snapPoints.size() - 1].second < posX) {
            final_xpos = _snapPoints[_snapPoints.size() - 1].second;

            return;
        }

        if (_snapPoints[0].first > posX && _snapPoints[0].first >= 0 && posX > 0) {

            final_xpos = _snapPoints[0].first;

            return;
        }

        if (posX < 0) {

            final_xpos = _snapPoints[0].first;

            return;
        }

        if (_snapPoints[0].second > posX && _snapPoints[0].second >= 0) {

            final_xpos = _snapPoints[0].first;

            return;
        }
    }

    double mid = (last + first) / 2.0;

    if (posX < mid) {
        final_xpos = first;
    }
    if (posX >= mid) {
        final_xpos = last;
    }
}

bool TrackShapeItem::acceptMime(const QMimeData* m) const
{
    // Accept files, URLs, plain text, and common image data.
    if (!m)
        return false;
    if (m->hasUrls() || m->hasText() || m->hasImage())
        return true;

    // Example: accept custom type
    if (m->hasFormat("application/octet-stream"))
        return true;

    return false;
}

void TrackShapeItem::hoverLeaveEvent(QHoverEvent* event)
{

    if (_trackIndex < 0) {
        return;
    }

    auto trackItem = _clipArea->tracksModel()->trackItem(_trackIndex);
    auto status = trackItem->hoverMoveEvent(event);
}

void TrackShapeItem::hoverMoveEvent(QHoverEvent* event)
{

    if (_trackIndex < 0) {
        return;
    }
    auto trackItem = _clipArea->tracksModel()->trackItem(_trackIndex);
    auto status = trackItem->hoverMoveEvent(event);

    if (status == 0) {
        _clipArea->setOverrideCursor(Qt::ArrowCursor);
    } else if (status == 1) {
        _clipArea->setOverrideCursor(Qt::ArrowCursor);
    } else if (status == 2) {
        _clipArea->setOverrideCursor(Qt::ArrowCursor);
    } else if (status == 3) {
        _clipArea->setOverrideCursor(Qt::ArrowCursor);
    }
}

void TrackShapeItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{

    if (_clipArea == nullptr || _areaInfo == nullptr)
        return;

    if (_trackIndex < 0) {
        _masterTrack->setRect(boundingRect().toRect());
        return;
    }

    _clipArea->tracksModel()->trackItem(_trackIndex)->setRect(boundingRect().toRect());

    auto rect = boundingRect();
    rect.setY(clipArea()->tracksModel()->trackItem(_trackIndex)->globalYPosition());
    rect.setHeight(boundingRect().height());
    _areaInfo->selectionArea()->setTracksRect(_trackIndex, rect);
}

QSGNode* TrackShapeItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{

    QSGNode* rootNode = oldNode ? oldNode : new QSGNode;

    QRectF _rect = boundingRect();

    if (!m_bgNode) {
        m_bgNode = new QSGGeometryNode;
        QSGGeometry* geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 4);
        geom->setDrawingMode(GL_TRIANGLE_STRIP);
        m_bgNode->setGeometry(geom);
        m_bgNode->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial* mat = new QSGFlatColorMaterial();
        mat->setColor(Qt::black);
        m_bgNode->setMaterial(mat);
        m_bgNode->setFlag(QSGNode::OwnsMaterial);

        rootNode->appendChildNode(m_bgNode);
    }

    // qDebug() << "BOUND" << boundingRect() << _trackIndex;

    QSGGeometry::Point2D* v = m_bgNode->geometry()->vertexDataAsPoint2D();
    v[0].set(_rect.left(), _rect.top());
    v[1].set(_rect.right(), _rect.top());
    v[2].set(_rect.left(), _rect.bottom());
    v[3].set(_rect.right(), _rect.bottom());
    m_bgNode->geometry()->markVertexDataDirty();
    m_bgNode->markDirty(QSGNode::DirtyGeometry);

    if (_areaInfo == Q_NULLPTR)
        return rootNode;

    double final_size = 0;
    double stepDuration = 0;
    double offsetPix = 0;
    double offsetTime = 0;

    auto _currentDuration = _areaInfo->clipDuration();
    auto _lastWidth = _areaInfo->lastWidth();
    auto _startTime = _areaInfo->startTime();
    auto _lenghtOfBeat = _areaInfo->lenghtOfBeat();
    auto _beatPerBar = _areaInfo->beatPerBar();

    if (_lastWidth == 0)
        return rootNode;

    auto _tracksModel = _clipArea->tracksModel();

    QSharedPointer<TrackItem> trackItem;
    if (_trackIndex < 0) {
        trackItem = _masterTrack;
    } else {
        trackItem = _tracksModel->trackItem(_trackIndex);
    }

    auto clipItems = trackItem->getClipItems();

    if (!m_rects_node) {
        m_rects_node = new MultiColorRectsNode;
        rootNode->appendChildNode(m_rects_node);
    }

    if (!m_selected_rects_node) {
        m_selected_rects_node = new MultiColorRectsNode;
        rootNode->appendChildNode(m_selected_rects_node);
    }

    if (!m_lines_node) {
        m_lines_node = new MultiColorLinesNode;
        rootNode->appendChildNode(m_lines_node);
    }

    for (int i = 0; i < m_linesA.size(); i++) {
        m_linesA[i].p1.setY(_rect.top());
        m_linesA[i].p2.setY(_rect.bottom());
    }

    for (int i = 0; i < m_rectsA.size(); i++) {
        m_rectsA[i].rect.setTop(_rect.top());
        m_rectsA[i].rect.setBottom(_rect.bottom());
    }

    for (int i = 0; i < m_selected_rects.size(); i++) {
        m_selected_rects[i].rect.setTop(_rect.top());
        m_selected_rects[i].rect.setBottom(_rect.bottom());
    }

    m_rects_node->setLines(m_rectsA);

    m_selected_rects_node->setLines(m_selected_rects);

    m_lines_node->setLines(m_linesA);

    QMapIterator<QString, QSharedPointer<ClipItem>> i(clipItems);
    while (i.hasNext()) {
        i.next();
        const double pxPerMs = _lastWidth / _currentDuration;

        double pix1d = (i.value()->sampleClip()->startTime() - _startTime) * pxPerMs;
        double pix2d = (i.value()->sampleClip()->endTime() - _startTime) * pxPerMs;

        if (!m_clipNodes.contains(i.key())) {
            ClipPlot c;
            if (!c.rectNode) {

                c.rectNode = new QSGSimpleRectNode(QRectF(pix1d, _rect.y(), pix2d - pix1d, _rect.height()), i.value()->baseColor());
                c.rectNode->setFlag(QSGNode::OwnedByParent, false);

                c.rectNode->setRect(QRectF(pix1d, _rect.y(), pix2d - pix1d, _rect.height()));
                c.rectNode->setColor(i.value()->baseColor());

                m_clipNodes.insert(i.key(), c);

                rootNode->insertChildNodeAfter(c.rectNode, m_rects_node);
            }
        } else {

            auto& c = m_clipNodes[i.key()];
            c.rectNode->setRect(QRectF(pix1d, _rect.y(), pix2d - pix1d, _rect.height()));
            c.rectNode->setColor(i.value()->baseColor());
        }
    }

    QList<QString> keysToRemove;
    QMapIterator<QString, ClipPlot> it(m_clipNodes);
    while (it.hasNext()) {
        it.next();
        if (!clipItems.contains(it.key())) {
            rootNode->removeChildNode(it.value().rectNode);
            delete it.value().rectNode;
            keysToRemove << it.key();
        }
    }
    for (const QString& key : keysToRemove) {
        m_clipNodes.remove(key);
    }

    return rootNode;
}

void TrackShapeItem::dragEnterEvent(QDragEnterEvent* event)
{
    if (acceptMime(event->mimeData())) {
        if (event->mimeData()->hasUrls()) {
            if (event->mimeData()->urls().size() > 1) {

            } else {
                auto url = event->mimeData()->urls()[0].toLocalFile();
                QFileInfo info(url);
                SF_INFO sfinfo;
                SNDFILE* file = sf_open(url.toLatin1().data(), SFM_READ, &sfinfo);

                if (!file) {
                    return;
                }

                qDebug() << " // Check if the file format is WAV" << sfinfo.format;
                if ((sfinfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV || (sfinfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAVEX) {
                    event->acceptProposedAction();
                    sf_close(file);
                } else {
                    sf_close(file);
                }
            }
        }
    } else {
        event->ignore();
    }
}

void TrackShapeItem::dragMoveEvent(QDragMoveEvent* event)
{

    if (_trackIndex < 0)
        return;

    if (acceptMime(event->mimeData())) {
        if (event->mimeData()->hasUrls()) {

            event->acceptProposedAction();

            if (_draggingClipIndex == "") {
                auto url_ = event->mimeData()->urls()[0];
                const QUrl url(url_);

                QString fileName = QDir::toNativeSeparators(url.toLocalFile());
                double final_xpos = 0;
                double final_ypos = 0;

                final_xpos = _areaInfo->findSnap(event->pos().x());

                auto _tracksModel = _clipArea->tracksModel();
                auto trackItem = _tracksModel->trackItem(_trackIndex);
                auto track = _tracksModel->trackItem(_trackIndex)->track();

                // _sampleClip = static_cast<SampleClip*>(track->createClip(TimePos(0), fileName, true));

                _draggingClipIndex = QUuid::createUuid().toString();

                auto _startTime = _areaInfo->startTime();
                auto _clipDuration = _areaInfo->clipDuration();
                auto _lastWidth = _areaInfo->lastWidth();

                double timePerPixel = _lastWidth / _clipDuration;

                // Q_EMIT dragEntered(_trackIndex, _draggingClipIndex, final_xpos, pix2 - pix1, fileName, trackItem->getTitleColor().name());

                auto st = final_xpos * _areaInfo->clipDuration() / (double)_areaInfo->lastWidth() + _areaInfo->startTime();

                QRect rect = trackItem->rect();

                rect.setX(final_xpos);

                auto _draggingItem = trackItem->createDragSample(fileName, trackItem->getTitleColor(), _areaInfo, _trackIndex, _draggingClipIndex, trackItem->track(), rect, true);

                qint64 num_samples = _draggingItem->sampleClip()->sampleBuffer()->frames();
                double duration_in_seconds = (double)num_samples / _draggingItem->sampleClip()->sampleBuffer()->sampleRate();

                _draggingItem->sampleClip()->setBaseStartTime(st);
                _draggingItem->sampleClip()->setBaseEndTime(st + duration_in_seconds * 1000.0);

                _draggingItem->sampleClip()->setStartTime(st);
                _draggingItem->sampleClip()->setEndTime(st + duration_in_seconds * 1000.0);

                _draggingItem->sampleClip()->setCurrentFileDuration(duration_in_seconds * 1000.0);

                _draggingItem->sampleClip()->sampleBuffer()->setOffsetStartFrame(0);

                _draggingItem->sampleClip()->sampleBuffer()->setOffsetEndFrame(duration_in_seconds * AudioManager::audioEngine()->outputSampleRate());

                if (_draggingItem->sampleClip()->startTime() + duration_in_seconds * 1000.0 > _areaInfo->maxEndTime()) {
                    _areaInfo->setMaxEndTime(_draggingItem->sampleClip()->endTime());
                }

            } else {

                double final_xpos = 0;
                double final_ypos = 0;

                final_xpos = _areaInfo->findSnap(event->pos().x());

                auto _tracksModel = _clipArea->tracksModel();
                auto trackItem = _tracksModel->trackItem(_trackIndex);

                auto _draggingItem = trackItem->dragClipItem();

                qint64 num_samples = _draggingItem->sampleClip()->sampleBuffer()->frames();
                double duration_in_seconds = (double)num_samples / _draggingItem->sampleClip()->sampleBuffer()->sampleRate();

                auto st = final_xpos * _areaInfo->clipDuration() / (double)_areaInfo->lastWidth() + _areaInfo->startTime();
                auto et = st + duration_in_seconds * 1000.;

                _draggingItem->sampleClip()->setStartTime(st);
                _draggingItem->sampleClip()->setEndTime(st + duration_in_seconds * 1000.0);

                auto startFrame = _draggingItem->sampleClip()->startTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                auto endFrame = _draggingItem->sampleClip()->endTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                _draggingItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                _draggingItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

                _draggingItem->sampleClip()->sampleBuffer()->setOffsetStartFrame(0);
                _draggingItem->sampleClip()->sampleBuffer()->setOffsetEndFrame(duration_in_seconds * AudioManager::audioEngine()->outputSampleRate());

                if (_draggingItem->sampleClip()->startTime() + duration_in_seconds * 1000 > _areaInfo->maxEndTime()) {
                    _areaInfo->setMaxEndTime(_draggingItem->sampleClip()->endTime());
                }

                qDebug() << "editDragSample";
                trackItem->editDragSample();
            }

        } else {
            event->ignore();
        }
    }
}

void TrackShapeItem::dragLeaveEvent(QDragLeaveEvent* event)
{
    Q_UNUSED(event);

    if (_trackIndex < 0)
        return;

    _draggingClipIndex = "";
    emit dragLeft();

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);

    qDebug() << "dragLeaveEvent";
    trackItem->deleteDragSample();
}

void TrackShapeItem::dropEvent(QDropEvent* event)
{

    if (_trackIndex < 0)
        return;

    _draggingClipIndex = "";
    double final_xpos = 0;
    double final_ypos = 0;

    final_xpos = _areaInfo->findSnap(event->pos().x());

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);

    auto clipIndex = trackItem->addSampleClip(trackItem->dragClipItem()->clone(trackItem->dragClipItem()->sampleClip()->getTrack(), false));

    QJsonObject jObject;
    jObject["clip_index"] = clipIndex;
    jObject["operation"] = ActionStack::Operation_InsertClipItem;
    jObject["track_index"] = _trackIndex;

    QJsonArray jArray;
    jArray.append(jObject);
    clipArea()->saveInUndoItems(jArray);

    auto _draggingItem = trackItem->dragClipItem();

    auto list = trackItem->findOverlayItems(_draggingItem->sampleClip()->startTime(), _draggingItem->sampleClip()->endTime());
    for (int j = 0; j < list.size(); j++) {
        if (list[j] != clipIndex) {
            trackItem->cutClipItem(list[j], _draggingItem->sampleClip()->startTime(), _draggingItem->sampleClip()->endTime());
        }
    }

    auto item = trackItem->clipItems(clipIndex);
    if (item) {
        auto startFrame = item->sampleClip()->startTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        auto endFrame = item->sampleClip()->endTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        item->sampleClip()->movePosition(startFrame / 256.0);
        item->sampleClip()->changeLength((endFrame - startFrame) / 256.0);
        item->sampleClip()->sampleChange();
        item->sampleClip()->addWarpMarker(0, 0, width(), 0, 0, item->sampleClip()->currentFileDuration(), item->sampleClip()->currentFileDuration(), clipArea()->tempo());
    } else {
    }

    clipArea()->playbackSampleClipPositionChanged(_trackIndex, clipIndex);

    trackItem->deleteDragSample();

    emit dropped(final_xpos);

    event->acceptProposedAction();

    update();
}
