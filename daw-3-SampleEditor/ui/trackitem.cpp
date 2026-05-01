#include "trackitem.h"
#include "audio/mixer.h"
#include "core/sampleeditor.h"
#include "core/warp.h"
#include "global.h"
#include <QCursor>
#include <QDir>
#include <QJsonArray>
#include <QMimeData>
#include <QTime>
#include <QtMath>
#include <audio/audioengine.h>
#include <audio/engine.h>
#include <audio/samplebuffer.h>
#include <audio/sampleclip.h>
#include <audio/song.h>
#include <core/cliparea.h>
#include <memory>
#include <ui/clipitem.h>

TrackItem::TrackItem(int trackIndex, Track* track, AreaInfo* areaInfo)
    : IPainterItem()
    , _areaInfo(areaInfo)
    , _track(track)
    , _trackIndex(trackIndex)
{

    _isMasterTrack = false;

    _truePeakModel = QSharedPointer<TruePeakModel>(new TruePeakModel(2));

    // srand(1234);

    _titleColor = QColor(rand() % 255, rand() % 255, rand() % 255);

    _lastRowExpanded = _areaInfo->minTitlebarCount();
    _rowsExpandedCount = _areaInfo->minTitlebarCount();
    _rowsExpanded = true;

    _automationMenuTitle1 = "None";
    _automationSubMenuTitle = "None";

    _volumeAutomationItem = QSharedPointer<VolumeAutomationItem>(new VolumeAutomationItem(_trackIndex, _track->volumeAutomation(), _areaInfo));

    connect(_volumeAutomationItem.data(), &AutomationItem::sigSetCursor, this, &TrackItem::sigSetCursor);
    connect(_volumeAutomationItem.data(), &AutomationItem::sigCursorPosition, this, &TrackItem::sigCursorPosition);
    connect(_volumeAutomationItem.data(), &AutomationItem::sigUpdate, this, &TrackItem::sigUpdate);

    connect(_volumeAutomationItem->automation().data(), &Automation::valueChanged, this, &TrackItem::volumeAutomatedValueChanged);
    connect(_volumeAutomationItem->automation().data(), &Automation::sigLinesChanged, this, &TrackItem::volumeAutomatedLineChanged);
    connect(_volumeAutomationItem->automation().data(), &Automation::enabledChanged, this, &TrackItem::volumeEnabledChanged);

    _panAutomationItem = QSharedPointer<PanAutomationItem>(new PanAutomationItem(_trackIndex, _track->panAutomation(), _areaInfo));

    connect(_panAutomationItem.data(), &AutomationItem::sigSetCursor, this, &TrackItem::sigSetCursor);
    connect(_panAutomationItem.data(), &AutomationItem::sigCursorPosition, this, &TrackItem::sigCursorPosition);
    connect(_panAutomationItem.data(), &AutomationItem::sigUpdate, this, &TrackItem::sigUpdate);

    connect(_panAutomationItem->automation().data(), &Automation::valueChanged, this, &TrackItem::panAutomatedValueChanged);
    connect(_panAutomationItem->automation().data(), &Automation::sigLinesChanged, this, &TrackItem::panAutomatedLineChanged);
    connect(_panAutomationItem->automation().data(), &Automation::enabledChanged, this, &TrackItem::panEnabledChanged);

    _speakerOnAutomationItem = QSharedPointer<SpeakerOnAutomationItem>(new SpeakerOnAutomationItem(_trackIndex, _track->speakerAutomation(), _areaInfo));
    connect(_speakerOnAutomationItem.data(), &AutomationItem::sigSetCursor, this, &TrackItem::sigSetCursor);
    connect(_speakerOnAutomationItem.data(), &AutomationItem::sigCursorPosition, this, &TrackItem::sigCursorPosition);
    connect(_speakerOnAutomationItem.data(), &AutomationItem::sigUpdate, this, &TrackItem::sigUpdate);

    connect(_speakerOnAutomationItem->automation().data(), &Automation::valueChanged, this, &TrackItem::volumeAutomatedValueChanged);
    connect(_speakerOnAutomationItem->automation().data(), &Automation::sigLinesChanged, this, &TrackItem::volumeAutomatedLineChanged);
    connect(_speakerOnAutomationItem->automation().data(), &Automation::enabledChanged, this, &TrackItem::volumeEnabledChanged);

    _keyFrameAutomation = QSharedPointer<KeyFramesAutomation>(new KeyFramesAutomation(_areaInfo, trackIndex, "AudioObject", "KeyFrames", 0.5, 0.5));

    _keyFramesAutomationItem = QSharedPointer<KeyFramesAutomationItem>(new KeyFramesAutomationItem(_trackIndex, _keyFrameAutomation, _areaInfo));

    connect(_keyFramesAutomationItem.data(), &AutomationItem::sigSetCursor, this, &TrackItem::sigSetCursor);
    connect(_keyFramesAutomationItem.data(), &AutomationItem::sigCursorPosition, this, &TrackItem::sigCursorPosition);
    connect(_keyFramesAutomationItem.data(), &AutomationItem::sigUpdate, this, &TrackItem::sigUpdate);

    connect(_keyFramesAutomationItem->automation().data(), &Automation::valueChanged, this, &TrackItem::panAutomatedValueChanged);
    connect(_keyFramesAutomationItem->automation().data(), &Automation::sigLinesChanged, this, &TrackItem::panAutomatedLineChanged);
    connect(_keyFramesAutomationItem->automation().data(), &Automation::enabledChanged, this, &TrackItem::panEnabledChanged);

    _automationLaneModel = QSharedPointer<AutomationLaneModel>(new AutomationLaneModel(_areaInfo, this));
    connect(_automationLaneModel.data(), &AutomationLaneModel::sigValueChanged, this, &TrackItem::sigUpdate);

    connect(_automationLaneModel.data(), &AutomationLaneModel::sigRowAdded, this, &TrackItem::sltRowAdded);
    connect(_automationLaneModel.data(), &AutomationLaneModel::sigRowRemoved, this, &TrackItem::sltRowRemoved);
    connect(_automationLaneModel.data(), &AutomationLaneModel::sigRowChanged, this, &TrackItem::sltRowChanged);
}

TrackItem::~TrackItem()
{
    QMap<QString, QSharedPointer<ClipItem>>::iterator it;
    for (it = _clipItems.begin(); it != _clipItems.end(); ++it) {
        it.value().reset(); // Delete the ClipItem pointer
    }
    _clipItems.clear(); // Clear the map
    qDebug() << "~TrackItem()";
}

QRectF computeGlobalRect(
    const QVector<QRectF>& areas,
    int firstIndex,
    int lastIndex)
{
    if (firstIndex < 0)
        firstIndex = 0;

    if (lastIndex < 0)
        lastIndex = areas.size() - 1;

    if (lastIndex >= areas.size()) {
        lastIndex = areas.size() - 1;
    }

    QRectF result;
    bool firstFound = false;

    double left = std::numeric_limits<double>::max();
    double right = std::numeric_limits<double>::lowest();

    double top = 0;
    double bottom = 0;

    int lastNonNull = -1;

    for (int i = firstIndex; i <= lastIndex; ++i) {
        const QRectF& r = areas[i];
        if (r.isNull())
            continue;

        // first non-null rect → for TOP
        if (!firstFound) {
            firstFound = true;
            top = r.top();
        }

        // update left / right
        left = std::min(left, r.left());
        right = std::max(right, r.right());

        // record last non-null rect index
        lastNonNull = i;
    }

    if (!firstFound)
        return QRectF(); // all rects were null

    // bottom = bottom of last non-null rect
    bottom = areas[lastNonNull].bottom();

    return QRectF(QPointF(left, top), QPointF(right, bottom));
}

QRectF TrackItem::drawSelectedArea()
{

    auto offset = 0;

    if (_areaInfo == Q_NULLPTR)
        return QRect();

    auto automationLaneModel = this->automationLaneModel();
    auto rowsExpandedCount = this->rowsExpandedCount();
    auto automationLaneEnabled = this->automationLaneEnabled();

    auto _lastWidth = _areaInfo->lastWidth();
    auto _startTime = _areaInfo->startTime();
    auto _clipDuration = _areaInfo->clipDuration();

    auto _selectedArea = _areaInfo->selectionArea()->area();

    if (_selectedArea.left() >= 0 && _selectedArea.right() >= 0 && _selectedArea.right() != _selectedArea.left()) {

        double startX = ((_selectedArea.left() - _startTime) / (double)_clipDuration) * _lastWidth;
        double endX = ((_selectedArea.right() - _startTime) / (double)_clipDuration) * _lastWidth;
        if (_selectedArea.left() < _selectedArea.right()) {
            startX = ((_selectedArea.left() - _startTime) / (double)_clipDuration) * _lastWidth;
            endX = ((_selectedArea.right() - _startTime) / (double)_clipDuration) * _lastWidth;
        } else {
            endX = ((_selectedArea.left() - _startTime) / (double)_clipDuration) * _lastWidth;
            startX = ((_selectedArea.right() - _startTime) / (double)_clipDuration) * _lastWidth;
        }

        QRectF drawAreaTotal = QRectF(startX, totalRect().top() + offset, (endX - startX), totalRect().height());
        QRectF drawArea_ = QRectF(startX, totalRect().top() + offset, (endX - startX), rect().height());

        QVector<QRectF> drawAreas;

        for (int i = 0; i < automationLaneModel->count(); i++) {

            auto top = totalRect().top() + rowsExpandedCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight();
            auto H = automationLaneModel->totalRowCountBefore2(i) * _areaInfo->titleHeight() + i * _areaInfo->seperatorHeight();
            auto tempRect = totalRect();
            tempRect.setTop(top + H + offset);
            tempRect.setHeight(automationLaneModel->getItem(i)->automationLaneRowCount * _areaInfo->titleHeight());

            auto _drawArea = QRectF(startX, tempRect.top(), (endX - startX), tempRect.height());
            drawAreas.append(_drawArea);
        }

        if (!(_areaInfo->selectionArea()->isClipSelectedAtLast || _areaInfo->selectionArea()->isClipSelectedAtFirst)) {
            if (automationLaneEnabled) {

                drawArea_ = QRectF(startX, totalRect().top() + _areaInfo->titleHeight() + offset, (endX - startX), rect().height() - _areaInfo->titleHeight());
            }
        }
        if (automationLaneEnabled) {
            if (_areaInfo->selectionArea()->isClipSelectedAtLast == false) {

                drawArea_ = QRectF(startX, totalRect().top() + _areaInfo->titleHeight() + offset, (endX - startX), rect().height() - _areaInfo->titleHeight());
            }
        } else {

            if (!(_areaInfo->selectionArea()->isClipSelectedAtLast || _areaInfo->selectionArea()->isClipSelectedAtFirst)) {

                drawArea_ = QRectF();
            }
        }
        drawAreas.insert(0, drawArea_);

        if (_areaInfo->selectionArea()->firstTrackIndex() > _trackIndex || _trackIndex > _areaInfo->selectionArea()->lastTrackIndex()) {
            return QRectF();
        }

        if (_areaInfo->selectionArea()->isClipSelectedAtLast == true && _areaInfo->selectionArea()->isClipSelectedAtFirst == true) {

            return drawAreaTotal;
        } else {
            if (drawAreas.size() == 1) {
                if (!drawAreas[0].isNull())
                    return drawAreas[0];
            }
            if (drawAreas.size() > 1) {

                if (_areaInfo->selectionArea()->firstTrackIndex() == _trackIndex && _areaInfo->selectionArea()->lastTrackIndex() == _trackIndex) {
                    return computeGlobalRect(drawAreas, _areaInfo->selectionArea()->_firstSubTrackIndex + 1, _areaInfo->selectionArea()->_lastSubTrackIndex + 1);
                }
                if (_areaInfo->selectionArea()->firstTrackIndex() == _trackIndex && _areaInfo->selectionArea()->lastTrackIndex() != _trackIndex) {
                    return computeGlobalRect(drawAreas, _areaInfo->selectionArea()->_firstSubTrackIndex + 1, -1);
                }
                if (_areaInfo->selectionArea()->firstTrackIndex() != _trackIndex && _areaInfo->selectionArea()->lastTrackIndex() == _trackIndex) {
                    return computeGlobalRect(drawAreas, 0, _areaInfo->selectionArea()->_lastSubTrackIndex + 1);
                }
                if (_areaInfo->selectionArea()->firstTrackIndex() != _trackIndex && _areaInfo->selectionArea()->lastTrackIndex() != _trackIndex) {
                    return computeGlobalRect(drawAreas, 0, -1);
                }
            }
        }
    }
    return QRectF();
}

QSharedPointer<ClipItem> TrackItem::createNewSample(QString filePath, QColor titleColor, AreaInfo* areaInfo, int trackIndex, QString clipIndex, Track* track, QRect rect, bool isFake)
{

    auto _sampleClip = static_cast<SampleClip*>(track->createClip(TimePos(0), filePath, isFake));

    QSharedPointer<ClipItem> _clipItem = QSharedPointer<ClipItem>(new ClipItem(trackIndex, titleColor, clipIndex, areaInfo, _sampleClip));

    _clipItem->sampleClip()->setSampleFile(filePath);

    _clipItem->setRect(rect);

    return _clipItem;
}

QSharedPointer<ClipItem> TrackItem::createDragSample(QString filePath, QColor titleColor, AreaInfo* areaInfo, int trackIndex, QString clipIndex, Track* track, QRect rect, bool isFake)
{
    auto _sampleClip = static_cast<SampleClip*>(track->createClip(TimePos(0), filePath, isFake));

    _dragClipItem = QSharedPointer<ClipItem>(new ClipItem(trackIndex, titleColor, clipIndex, areaInfo, _sampleClip));

    _dragClipItem->sampleClip()->setSampleFile(filePath);

    _dragClipItem->setRect(rect);

    qDebug() << "TrackItem::createDragSample" << clipIndex;
    Q_EMIT sigDragClipAdded();
    return _dragClipItem;
}

void TrackItem::editDragSample()
{
    if (_dragClipItem) {
        Q_EMIT sigDragClipMoved(_dragClipItem->clipIndex());
    }
}

void TrackItem::deleteDragSample()
{
    if (_dragClipItem) {
        Q_EMIT sigDragClipRemoved(_dragClipItem->clipIndex());
        _dragClipItem.reset();
    }
}

QString TrackItem::addSampleClip(double time, QSharedPointer<ClipItem> item)
{

    auto final_xpos = (time - _areaInfo->startTime()) * _areaInfo->timePerPixel();

    QRect rect = _rect;
    rect.setX(final_xpos);

    item->setRect(rect);

    connect(item->sampleClip(), &SampleClip::positionChanged, [this](QString clipIndex) {
        Q_EMIT this->sigPositionChanged(_trackIndex, clipIndex);
    });

    connect(item->sampleClip(), &SampleClip::sigPlayingChanged, [this](bool state, QString clipIndex) {
        Q_EMIT this->sigPlayingChanged(state, _trackIndex, clipIndex);
    });

    connect(item->sampleClip()->sampleBuffer(), &SampleBuffer::sigFirstBufferFilled, this, &TrackItem::sigFirstBufferFilled);

    connect(item.data(), &ClipItem::sigSetCursor, this, &TrackItem::sigSetCursor);

    connect(item.data(), &ClipItem::sigCursorPosition, this, &TrackItem::sigCursorPosition);

    item->setTrackIndex(_trackIndex);

    item->sampleClip()->setBaseStartTime(_areaInfo->startTime());

    item->setAutomationLaneModel(_automationLaneModel);
    item->setRowsExpandedCount(_rowsExpandedCount);
    item->setAutomationLaneEnabled(_automationLaneEnabled);

    item->setBaseColor(_titleColor);
    _clipItems[item->clipIndex()] = item;

    Q_EMIT sigClipAdded(item->clipIndex());
    Q_EMIT sigClipChanged();
    return item->clipIndex();
}

QString TrackItem::addSampleClip(QSharedPointer<ClipItem> item)
{

    QRect rect = _rect;

    item->setRect(rect);

    connect(item->sampleClip(), &SampleClip::positionChanged, [this](QString clipIndex) {
        Q_EMIT this->sigPositionChanged(_trackIndex, clipIndex);
    });

    connect(item->sampleClip(), &SampleClip::sigPlayingChanged, [this](bool state, QString clipIndex) {
        Q_EMIT this->sigPlayingChanged(state, _trackIndex, clipIndex);
    });

    connect(item->sampleClip()->sampleBuffer(), &SampleBuffer::sigFirstBufferFilled, this, &TrackItem::sigFirstBufferFilled);

    connect(item.data(), &ClipItem::sigSetCursor, this, &TrackItem::sigSetCursor);

    connect(item.data(), &ClipItem::sigCursorPosition, this, &TrackItem::sigCursorPosition);

    item->setTrackIndex(_trackIndex);

    item->sampleClip()->setBaseStartTime(_areaInfo->startTime());
    item->setBaseColor(_titleColor);

    item->setAutomationLaneModel(_automationLaneModel);
    item->setRowsExpandedCount(_rowsExpandedCount);
    item->setAutomationLaneEnabled(_automationLaneEnabled);

    _clipItems[item->clipIndex()] = item;

    Q_EMIT sigClipAdded(item->clipIndex());
    Q_EMIT sigClipChanged();
    return item->clipIndex();
}

QString TrackItem::addSampleClip(double startTime, double endTime, double startOffset, double endOffset, QSharedPointer<ClipItem> item)
{

    auto final_xpos = (startTime - _areaInfo->startTime()) * _areaInfo->timePerPixel();

    QRect rect = _rect;
    rect.setX(final_xpos);

    item->setRect(rect);

    connect(item->sampleClip(), &SampleClip::positionChanged, [this](QString clipIndex) {
        Q_EMIT this->sigPositionChanged(_trackIndex, clipIndex);
    });

    connect(item->sampleClip(), &SampleClip::sigPlayingChanged, [this](bool state, QString clipIndex) {
        Q_EMIT this->sigPlayingChanged(state, _trackIndex, clipIndex);
    });

    connect(item->sampleClip()->sampleBuffer(), &SampleBuffer::sigFirstBufferFilled, this, &TrackItem::sigFirstBufferFilled);

    connect(item.data(), &ClipItem::sigSetCursor, this, &TrackItem::sigSetCursor);

    connect(item.data(), &ClipItem::sigCursorPosition, this, &TrackItem::sigCursorPosition);

    item->setTrackIndex(_trackIndex);

    item->sampleClip()->setBaseStartTime(_areaInfo->startTime());

    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

    item->sampleClip()->setStartTime(startTime);
    item->sampleClip()->setEndTime(endTime);

    item->sampleClip()->sampleBuffer()->setOffsetStartFrame(startOffset);
    item->sampleClip()->sampleBuffer()->setOffsetEndFrame(endOffset);

    item->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
    item->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

    item->sampleClip()->setStartTimeOffset((item->sampleClip()->sampleBuffer()->offsetStartFrame()) / 256.0);
    item->sampleClip()->movePosition(startFrame / 256.0);
    item->sampleClip()->changeLength((endFrame - startFrame) / 256.0);
    item->sampleClip()->sampleChange();

    item->setAutomationLaneModel(_automationLaneModel);
    item->setRowsExpandedCount(_rowsExpandedCount);
    item->setAutomationLaneEnabled(_automationLaneEnabled);

    item->setBaseColor(_titleColor);
    _clipItems[item->clipIndex()] = item;

    Q_EMIT sigClipAdded(item->clipIndex());
    Q_EMIT sigClipChanged();
    return item->clipIndex();
}

QJsonObject TrackItem::resizeSampleClip(QString clipIndex, QRect new_rect, double startTime, double endTime, double startOffset, double endOffset)
{

    if (!_clipItems.contains(clipIndex)) {
        return QJsonObject();
    }

    auto state = _clipItems[clipIndex]->sampleClip()->state();
    auto old_rect = _clipItems[clipIndex]->rect();
    QJsonObject jObject;
    jObject["clip_index"] = clipIndex;
    jObject["operation"] = ActionStack::Operation_ResizeClipItem;
    jObject["state"] = state;
    jObject["rect"] = QString("%1, %2, %3, %4").arg(old_rect.x()).arg(old_rect.y()).arg(old_rect.width()).arg(old_rect.height());
    jObject["track_index"] = _clipItems[clipIndex]->trackIndex();

    QRect rect = _rect;
    rect.setX(new_rect.x());

    _clipItems[clipIndex]->setRect(rect);

    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

    _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setOffsetEndFrame(endOffset);
    _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setOffsetStartFrame(startOffset);

    _clipItems[clipIndex]->sampleClip()->setEndTime(endTime);
    _clipItems[clipIndex]->sampleClip()->setStartTime(startTime);

    _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
    _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

    _clipItems[clipIndex]->sampleClip()->setStartTimeOffset((_clipItems[clipIndex]->sampleClip()->sampleBuffer()->offsetStartFrame()) / 256.0);
    _clipItems[clipIndex]->sampleClip()->movePosition(startFrame / 256.0);
    _clipItems[clipIndex]->sampleClip()->changeLength((endFrame - startFrame) / 256.0);
    _clipItems[clipIndex]->sampleClip()->sampleChange();

    Q_EMIT sigClipChanged();
    return jObject;
}

QJsonArray TrackItem::splitClipItem(QString clipIndex, double time)
{

    auto newClipItem1 = _clipItems[clipIndex]->clone(_track);
    auto newClipItem2 = _clipItems[clipIndex]->clone(_track);

    QJsonArray array;

    auto obj = removeSampleItem(clipIndex);
    array.append(obj);

    auto _shadowEndTime = time;
    auto _shadowStartTime = time;

    _mutex.lock();

    {

        auto startFrame = _shadowStartTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        auto endFrame = newClipItem1->sampleClip()->endTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

        auto frame_diff = (_shadowStartTime - newClipItem1->sampleClip()->startTime()) / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

        newClipItem1->sampleClip()->setStartTime(_shadowStartTime);

        newClipItem1->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
        newClipItem1->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
        newClipItem1->sampleClip()->sampleBuffer()->setOffsetStartFrame(newClipItem1->sampleClip()->sampleBuffer()->offsetStartFrame() + frame_diff);

        newClipItem1->sampleClip()->setStartTimeOffset(newClipItem1->sampleClip()->sampleBuffer()->offsetStartFrame() / 256.0);

        auto clipIndex = addSampleClip(_shadowStartTime, newClipItem1);

        QJsonObject jObject;
        jObject["clip_index"] = clipIndex;
        jObject["operation"] = ActionStack::Operation_InsertClipItem;
        jObject["track_index"] = newClipItem1->trackIndex();
        array.append(jObject);

        newClipItem1->sampleClip()->movePosition(startFrame / 256.0);
        newClipItem1->sampleClip()->changeLength((endFrame - startFrame) / 256.0);
        newClipItem1->sampleClip()->sampleChange();
    }
    {

        auto startFrame = newClipItem2->sampleClip()->startTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        auto endFrame = _shadowEndTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

        double frame_diff = (_shadowEndTime - newClipItem2->sampleClip()->endTime()) / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        newClipItem2->sampleClip()->sampleBuffer()->setOffsetEndFrame(newClipItem2->sampleClip()->sampleBuffer()->offsetEndFrame() + frame_diff);

        newClipItem2->sampleClip()->setEndTime(_shadowEndTime);

        newClipItem2->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
        newClipItem2->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

        auto clipIndex = addSampleClip(newClipItem2->sampleClip()->startTime(), newClipItem2);

        QJsonObject jObject;
        jObject["clip_index"] = clipIndex;
        jObject["operation"] = ActionStack::Operation_InsertClipItem;
        jObject["track_index"] = newClipItem2->trackIndex();
        array.append(jObject);

        newClipItem2->sampleClip()->movePosition(startFrame / 256.0);
        newClipItem2->sampleClip()->changeLength((endFrame - startFrame) / 256.0);

        newClipItem2->sampleClip()->sampleChange();
    }
    _mutex.unlock();

    Q_EMIT sigClipChanged();
    return array;
}

QJsonArray TrackItem::cutClipItem(QString clipIndex, double startTime, double endTime)
{
    QJsonArray array;

    if ((_clipItems[clipIndex]->sampleClip()->startTime() >= startTime && _clipItems[clipIndex]->sampleClip()->endTime() <= endTime)
        || (qAbs(_clipItems[clipIndex]->sampleClip()->startTime() - startTime) < 0.001 && qAbs(_clipItems[clipIndex]->sampleClip()->endTime() - endTime) < 0.001)) {

        auto obj = removeSampleItem(clipIndex);
        array.append(obj);
        Q_EMIT sigClipChanged();
        return array;
    }

    if ((startTime <= _clipItems[clipIndex]->sampleClip()->startTime()
            || qAbs(_clipItems[clipIndex]->sampleClip()->startTime() - startTime) < 0.001)
        && _clipItems[clipIndex]->sampleClip()->startTime() < endTime) {

        auto state = _clipItems[clipIndex]->sampleClip()->state();
        auto rect = _clipItems[clipIndex]->rect();
        QJsonObject jObject;
        jObject["clip_index"] = clipIndex;
        jObject["operation"] = ActionStack::Operation_ResizeClipItem;
        jObject["state"] = state;
        jObject["rect"] = QString("%1, %2, %3, %4)").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
        jObject["track_index"] = _clipItems[clipIndex]->trackIndex();
        array.append(jObject);

        _mutex.lock();

        auto _shadowStartTime = endTime;

        auto startFrame = _shadowStartTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        auto endFrame = _clipItems[clipIndex]->sampleClip()->endTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

        auto frame_diff = (_shadowStartTime - _clipItems[clipIndex]->sampleClip()->startTime()) / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

        _clipItems[clipIndex]->sampleClip()->setStartTime(_shadowStartTime);

        _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
        _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
        _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setOffsetStartFrame(_clipItems[clipIndex]->sampleClip()->sampleBuffer()->offsetStartFrame() + frame_diff);

        _clipItems[clipIndex]->sampleClip()->setStartTimeOffset(_clipItems[clipIndex]->sampleClip()->sampleBuffer()->offsetStartFrame() / 256.0);

        _clipItems[clipIndex]->sampleClip()->movePosition(startFrame / 256.0);
        _clipItems[clipIndex]->sampleClip()->changeLength((endFrame - startFrame) / 256.0);
        _clipItems[clipIndex]->sampleClip()->sampleChange();
        _mutex.unlock();

    } else if (startTime < _clipItems[clipIndex]->sampleClip()->endTime()
        && (_clipItems[clipIndex]->sampleClip()->endTime() <= endTime || qAbs(_clipItems[clipIndex]->sampleClip()->endTime() - endTime) < 0.001)) {

        auto state = _clipItems[clipIndex]->sampleClip()->state();
        auto rect = _clipItems[clipIndex]->rect();

        QJsonObject jObject;
        jObject["clip_index"] = clipIndex;
        jObject["operation"] = ActionStack::Operation_ResizeClipItem;
        jObject["state"] = state;
        jObject["rect"] = QString("%1, %2, %3, %4)").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
        jObject["track_index"] = _clipItems[clipIndex]->trackIndex();
        array.append(jObject);

        _mutex.lock();

        auto _shadowEndTime = startTime;

        auto startFrame = _clipItems[clipIndex]->sampleClip()->startTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        auto endFrame = _shadowEndTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

        double frame_diff = (_shadowEndTime - _clipItems[clipIndex]->sampleClip()->endTime()) / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setOffsetEndFrame(_clipItems[clipIndex]->sampleClip()->sampleBuffer()->offsetEndFrame() + frame_diff);

        _clipItems[clipIndex]->sampleClip()->setEndTime(_shadowEndTime);

        _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
        _clipItems[clipIndex]->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

        _clipItems[clipIndex]->sampleClip()->movePosition(startFrame / 256.0);
        _clipItems[clipIndex]->sampleClip()->changeLength((endFrame - startFrame) / 256.0);
        _clipItems[clipIndex]->sampleClip()->sampleChange();

        _mutex.unlock();
    } else {

        auto newClipItem1 = _clipItems[clipIndex]->clone(_track);
        auto newClipItem2 = _clipItems[clipIndex]->clone(_track);

        auto obj = removeSampleItem(clipIndex);
        array.append(obj);

        _mutex.lock();

        {
            auto _shadowStartTime = endTime;

            auto startFrame = _shadowStartTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
            auto endFrame = newClipItem1->sampleClip()->endTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

            auto frame_diff = (_shadowStartTime - newClipItem1->sampleClip()->startTime()) / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

            newClipItem1->sampleClip()->setStartTime(_shadowStartTime);

            newClipItem1->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
            newClipItem1->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
            newClipItem1->sampleClip()->sampleBuffer()->setOffsetStartFrame(newClipItem1->sampleClip()->sampleBuffer()->offsetStartFrame() + frame_diff);

            newClipItem1->sampleClip()->setStartTimeOffset(newClipItem1->sampleClip()->sampleBuffer()->offsetStartFrame() / 256.0);

            auto clipIndex = addSampleClip(_shadowStartTime, newClipItem1);

            QJsonObject jObject;
            jObject["clip_index"] = clipIndex;
            jObject["operation"] = ActionStack::Operation_InsertClipItem;
            jObject["track_index"] = newClipItem1->trackIndex();
            array.append(jObject);

            newClipItem1->sampleClip()->movePosition(startFrame / 256.0);
            newClipItem1->sampleClip()->changeLength((endFrame - startFrame) / 256.0);
            newClipItem1->sampleClip()->sampleChange();
        }
        {
            auto _shadowEndTime = startTime;

            auto startFrame = newClipItem2->sampleClip()->startTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
            auto endFrame = _shadowEndTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

            double frame_diff = (_shadowEndTime - newClipItem2->sampleClip()->endTime()) / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
            newClipItem2->sampleClip()->sampleBuffer()->setOffsetEndFrame(newClipItem2->sampleClip()->sampleBuffer()->offsetEndFrame() + frame_diff);

            newClipItem2->sampleClip()->setEndTime(_shadowEndTime);

            newClipItem2->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
            newClipItem2->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

            auto clipIndex = addSampleClip(newClipItem2->sampleClip()->startTime(), newClipItem2);

            QJsonObject jObject;
            jObject["clip_index"] = clipIndex;
            jObject["operation"] = ActionStack::Operation_InsertClipItem;
            jObject["track_index"] = newClipItem2->trackIndex();
            array.append(jObject);

            newClipItem2->sampleClip()->movePosition(startFrame / 256.0);
            newClipItem2->sampleClip()->changeLength((endFrame - startFrame) / 256.0);

            newClipItem2->sampleClip()->sampleChange();
        }
        _mutex.unlock();
    }

    Q_EMIT sigClipChanged();
    return array;
}

QJsonObject TrackItem::removeSampleItem(QString clipIndex)
{

    QJsonObject jObject;
    _mutex.lock();
    if (_clipItems.contains(clipIndex)) {

        _clipItems[clipIndex]->disconnect(this);
        _clipItems[clipIndex]->sampleClip()->disconnect(this);
        _clipItems[clipIndex]->sampleClip()->sampleBuffer()->disconnect(this);
        if (_clipItems[clipIndex]) {

            auto json = _clipItems[clipIndex]->sampleClip()->state();
            auto rect = _clipItems[clipIndex]->rect();
            jObject["clip_index"] = clipIndex;
            jObject["operation"] = ActionStack::Operation_DeleteClipItem;
            jObject["state"] = json;
            jObject["rect"] = QString("%1, %2, %3, %4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
            jObject["track_index"] = _clipItems[clipIndex]->trackIndex();

            _clipItems[clipIndex].reset();
            _clipItems.remove(clipIndex);
            Q_EMIT sigClipRemoved(clipIndex);
        }
    }

    if (_selectedClipIndex == clipIndex) {
        _selectedClipIndex = "";
    }

    _mutex.unlock();

    Q_EMIT sigClipChanged();
    return jObject;
}

QSharedPointer<ClipItem> TrackItem::clone(QString clipIndex, bool fake)
{
    return _clipItems[clipIndex]->clone(_track, fake);
}

QList<QString> TrackItem::setPositionChanged(double duration_in_seconds, bool toPlay)
{

    QList<QString> list;
    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();

        double samples = duration_in_seconds / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

        // I have changed it, now I have loaded all sampleclips before request time
        // if (i.value()->sampleClip()->startTime() <= duration_in_seconds && i.value()->sampleClip()->endTime() >= duration_in_seconds) {
        if (duration_in_seconds <= i.value()->sampleClip()->endTime()) {

            i.value()->sampleClip()->sampleBuffer()->sltPositionChanged(samples, i.key(), toPlay);
            list.append(i.key());
        }
    }

    return list;
}

QString TrackItem::setPositionChanged(QString clipIndex, double duration_in_seconds, bool toPlay)
{

    double samples = duration_in_seconds / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
    _clipItems[clipIndex]->sampleClip()->sampleBuffer()->sltPositionChanged(samples, clipIndex, toPlay);

    return clipIndex;
}

QList<QString> TrackItem::countPositionChanged(double duration_in_seconds)
{

    QList<QString> list;
    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();

        //        if (i.value()->sampleClip()->startTime() <= duration_in_seconds && i.value()->sampleClip()->endTime() >= duration_in_seconds) {
        //            list.append(i.key());
        //            break;
        //        }

        // I have changed it, now I have loaded all sampleclips before request time
        if (duration_in_seconds <= i.value()->sampleClip()->endTime()) {
            list.append(i.key());
            break;
        }
    }

    return list;
}

void TrackItem::setSelectedClipIndex(QString clipIndex, bool selected)
{
    if (_clipItems.contains(clipIndex)) {

        if (selected == true) {
            _selectedClipIndex = clipIndex;

        } else {
            _selectedClipIndex = "";
        }
    }
}

void TrackItem::deSelecteAll()
{
    _selectedClipIndex = "";
    _indicatorEnabled = IndicatorType_Disable;
}

bool TrackItem::contains_global(QPoint point)
{
    auto local_rect = getTotalRect();

    int top = _globalYPosition;
    int bottom = _globalYPosition + local_rect.height();

    if ((point.x() >= local_rect.x()) && (point.x() <= local_rect.x() + local_rect.width()) && (point.y() >= top) && (point.y() < bottom)) {
        return true;
    }

    return false;
}

// bool TrackItem::contains(QPoint point)
//{
//     return getTotalRect().contains(point);
// }

bool TrackItem::selectedAreaContains(SelectedArea selectedArea)
{

    if ((_trackIndex > selectedArea.firstTrackIndex() && _trackIndex < selectedArea.lastTrackIndex())
        || (_trackIndex == selectedArea.firstTrackIndex() && selectedArea.firstTrackIndex() != selectedArea.lastTrackIndex() && -1 >= selectedArea._firstSubTrackIndex)
        || (_trackIndex == selectedArea.lastTrackIndex() && selectedArea.firstTrackIndex() != selectedArea.lastTrackIndex() && -1 <= selectedArea._lastSubTrackIndex)
        || (selectedArea.firstTrackIndex() == _trackIndex && selectedArea.firstTrackIndex() == selectedArea.lastTrackIndex() && -1 >= selectedArea._firstSubTrackIndex
            && -1 <= selectedArea._lastSubTrackIndex)) {

        return true;
    } else {

        return false;
    }
}

QString TrackItem::findClipItemPixel(double pixel)
{
    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();

        if (i.value()->containsPixel(pixel)) {
            return i.key();
        }
    }
    return 0;
}

QString TrackItem::findClipItemTime(double time)
{
    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();

        if (i.value()->containsTime(time)) {
            return i.key();
        }
    }

    return "";
}

QList<QString> TrackItem::findOverlayItems(double startTime, double endTime)
{

    QList<QString> list;

    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();

        if (i.value()->sampleClip()->startTime() < endTime && i.value()->sampleClip()->endTime() >= endTime) {
            list.append(i.key());
        }

        else if (i.value()->sampleClip()->startTime() <= startTime && i.value()->sampleClip()->endTime() > startTime) {
            list.append(i.key());
        }

        else if (i.value()->sampleClip()->startTime() >= startTime && i.value()->sampleClip()->endTime() <= endTime) {
            list.append(i.key());
        }
    }
    return list;
}

QList<QString> TrackItem::findOverlayItems(QRectF selectedRect)
{
    QList<QString> list;
    if (selectedRect.left() >= 0 && selectedRect.right() >= 0 && selectedRect.left() != selectedRect.right() && (rect().top() >= selectedRect.top() && rect().bottom() <= selectedRect.bottom())) {

        QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
        while (i.hasNext()) {
            i.next();

            if (i.value()->sampleClip()->startTime() < selectedRect.right() && i.value()->sampleClip()->endTime() >= selectedRect.right()) {
                list.append(i.key());
            }

            else if (i.value()->sampleClip()->startTime() <= selectedRect.left() && i.value()->sampleClip()->endTime() > selectedRect.left()) {
                list.append(i.key());
            }

            else if (i.value()->sampleClip()->startTime() >= selectedRect.left() && i.value()->sampleClip()->endTime() <= selectedRect.right()) {
                list.append(i.key());
            }
        }
    }
    return list;
}

QSharedPointer<ClipItem> TrackItem::clipItems(QString index)
{
    if (_clipItems.contains(index)) {
        return _clipItems[index];
    } else {
        return Q_NULLPTR;
    }
}

QSharedPointer<ClipItem> TrackItem::getFirstClipItem()
{

    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    double minStartTime = _areaInfo->maxEndTime();
    QString index = "";

    while (i.hasNext()) {
        i.next();

        if (i.value()->sampleClip()->startTime() <= minStartTime) {
            minStartTime = i.value()->sampleClip()->startTime();

            index = i.key();
        }
    }
    return clipItems(index);
}

QSharedPointer<ClipItem> TrackItem::getLastClipItem()
{
    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    double minEndTime = 0;
    QString index = "";
    while (i.hasNext()) {
        i.next();
        if (i.value()->sampleClip()->endTime() >= minEndTime) {
            minEndTime = i.value()->sampleClip()->endTime();
            index = i.key();
        }
    }
    return clipItems(index);
}

bool TrackItem::containTitleGlobal(QPointF pos)
{
    QRectF _tmpRect = rect();
    _tmpRect.setTop(_globalYPosition);
    _tmpRect.setHeight(_areaInfo->titleHeight());

    if (_tmpRect.contains(pos)) {
        return true;
    }
    return false;
}

bool TrackItem::containTitle(QPointF pos)
{
    QRectF _tmpRect = rect();
    _tmpRect.setHeight(_areaInfo->titleHeight());

    if (_tmpRect.contains(pos)) {
        return true;
    }
    return false;
}

void TrackItem::setRect(const QRect& rect)
{

    IPainterItem::setRect(rect);

    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();

        i.value()->setRect(rect);
    }

    _volumeAutomationItem->automation()->update(rect);
    _panAutomationItem->automation()->update(rect);
    _speakerOnAutomationItem->automation()->update(rect);
    _keyFramesAutomationItem->automation()->update(rect);

    _volumeAutomationItem->sigUpdate();
    _panAutomationItem->sigUpdate();
    _speakerOnAutomationItem->sigUpdate();
    _keyFramesAutomationItem->sigUpdate();

    Q_EMIT sigUpdate();
}

QRect TrackItem::rect() const
{
    auto re = _rect;

    // qDebug() << "_rect" << _rect.top() << _rect.height() << _trackIndex << offset;
    re.setTop(_rect.top());
    re.setHeight(_rowsExpandedCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight());

    return re;
}

QRect TrackItem::totalRect()
{
    return getTotalRect();
}

QRect TrackItem::getTotalRect()
{
    auto _tmprect = TrackItem::rect();

    if (_rowsExpanded && automationLaneEnabled()) {
        _tmprect.setHeight(_rowsExpandedCount * _areaInfo->titleHeight() + automationLaneModel()->totalHeight() + _areaInfo->seperatorHeight());
    } else {

        _tmprect.setHeight(_rowsExpandedCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight());
    }

    return _tmprect;
}

QSharedPointer<AutomationItem> TrackItem::effectAutomationItem()
{
    auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
    auto effect = effectChain->getEffect(_automationMenuTitle1);
    if (effect) {
        if (effect->contains(_automationSubMenuTitle)) {
            return effect->automationItem(_automationSubMenuTitle);
        }
    }

    return Q_NULLPTR;
}

QSharedPointer<AutomationItem> TrackItem::effectAutomationItem(QString menuTitle, QString subMenuTitle)
{
    auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
    auto effect = effectChain->getEffect(menuTitle);

    if (effect) {
        if (effect->contains(subMenuTitle))
            return effect->automationItem(subMenuTitle);
    }

    return Q_NULLPTR;
}

QSharedPointer<AutomationItem> TrackItem::visrAutomationItem()
{
    auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
    if (effect) {
        if (effect->contains(_automationSubMenuTitle))
            return effect->automationItem(_automationSubMenuTitle);
    }

    return Q_NULLPTR;
}

QSharedPointer<AutomationItem> TrackItem::visrAutomationItem(QString menuTitle, QString subMenuTitle)
{

    if (menuTitle == "Visr") {
        auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
        if (effect) {
            if (effect->contains(subMenuTitle))
                return effect->automationItem(subMenuTitle);
        }
    }

    return Q_NULLPTR;
}

int TrackItem::isSelected(QString menu, QString subMenu)
{
    for (int i = 0; i < _automationLaneModel->count(); i++) {
        if (_automationLaneModel->getItem(i)->automationLaneMenuTitle == menu
            && _automationLaneModel->getItem(i)->automationLaneSubMenuTitle == subMenu) {
            return true;
        }
    }
    return false;
}

QJsonArray TrackItem::deleteSelectedArea(SelectedArea selectedRect)
{

    QJsonArray jArray;

    constexpr double EPS = 1e-9;
    // qDebug() << "selectedRect.area()" << selectedRect.area() << _globalYPosition;
    if (selectedRect.area().left() >= 0 && selectedRect.area().right() >= 0 && selectedRect.area().left() != selectedRect.area().right()) {

        auto list = findOverlayItems(selectedRect.area().left(), selectedRect.area().right());
        for (int i = 0; i < list.size(); i++) {

            auto selRect = selectedRect.area();
            auto item = _clipItems[list[i]];

            double clipStart = item->sampleClip()->startTime(); // time
            double clipEnd = item->sampleClip()->endTime(); // time

            bool noOverlapHoriz = (selRect.right() <= clipStart + EPS) || (selRect.left() >= clipEnd - EPS);
            bool noOverlapVert = (selectedRect.firstTrackIndex() > _trackIndex) || (selectedRect.lastTrackIndex() < _trackIndex);
            bool overlap = !(noOverlapHoriz || noOverlapVert);
            // Check if there's NO overlap (with epsilon tolerance)
            if (overlap) {
                auto array = cutClipItem(list[i], selRect.left(), selRect.right());
                jArray.append(array);
            }
        }
    }

    return jArray;
}

QVector<TrackItem::CopyItem> TrackItem::copySelectedArea(SelectedArea selectedRect)
{
    QVector<TrackItem::CopyItem> list;

    constexpr double EPS = 1e-9;
    if (selectedRect.area().left() >= 0 && selectedRect.area().right() >= 0 && selectedRect.area().left() != selectedRect.area().right() && (_globalYPosition >= selectedRect.area().top() && (_globalYPosition + rect().height()) <= selectedRect.area().bottom())) {

        QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
        while (i.hasNext()) {
            i.next();

            auto selRect = selectedRect.area();
            auto item = _clipItems[i.key()];

            double clipStart = item->sampleClip()->startTime(); // time
            double clipEnd = item->sampleClip()->endTime(); // time

            bool noOverlapHoriz = (selRect.right() <= clipStart + EPS) || (selRect.left() >= clipEnd - EPS);
            bool noOverlapVert = (selectedRect.firstTrackIndex() > _trackIndex) || (selectedRect.lastTrackIndex() < _trackIndex);
            bool overlap = !(noOverlapHoriz || noOverlapVert);

            if (overlap) {
                auto cloneditem = _clipItems[i.key()]->clone(_track, true);

                cloneditem->setAutomationLaneModel(_automationLaneModel);
                cloneditem->setRowsExpandedCount(_rowsExpandedCount);
                cloneditem->setAutomationLaneEnabled(_automationLaneEnabled);

                list.append(CopyItem {
                    selectedRect.area().left(),
                    selectedRect.area().right(),
                    _clipItems[i.key()]->sampleClip()->endTime() - _clipItems[i.key()]->sampleClip()->startTime(),
                    selectedRect.area().left() - _clipItems[i.key()]->sampleClip()->startTime(),
                    _clipItems[i.key()]->sampleClip()->endTime() - selectedRect.area().right(),
                    _trackIndex,
                    cloneditem, selectedRect.area().height() });
            }
        }

        if (list.size() == 0) {

            list.append(CopyItem { -1, -1, -1, -1, -1, _trackIndex, nullptr, selectedRect.area().height() });
        }
    }
    return list;
}

void TrackItem::setKeyFrameEnabled(bool enabled)
{
    // _keyFramesItem->setKeyFramesEnabled(enabled);
}

bool TrackItem::keyFramesEnabled()
{
    return true;
}

bool TrackItem::automationLaneEnabled() const
{
    return _automationLaneEnabled;
}

void TrackItem::setAutomationLaneEnabled(bool automationLaneEnabled)
{

    _automationLaneEnabled = automationLaneEnabled;

    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();
        i.value()->setAutomationLaneEnabled(_automationLaneEnabled);
    }
    Q_EMIT sigUpdate();
}

QSharedPointer<ClipItem> TrackItem::dragClipItem() const
{
    return _dragClipItem;
}

int TrackItem::outputChannelCount() const
{
    return _outputChannelCount;
}

void TrackItem::setOutputChannelCount(int newOutputChannelCount)
{
    _outputChannelCount = newOutputChannelCount;
}

int TrackItem::inputChannelCount() const
{
    return _inputChannelCount;
}

void TrackItem::setInputChannelCount(int newInputChannelCount)
{
    _inputChannelCount = newInputChannelCount;
}

int TrackItem::globalYPosition() const
{
    return _globalYPosition;
}

void TrackItem::setGlobalYPosition(int newGlobalYPosition)
{
    _globalYPosition = newGlobalYPosition;
}

QSharedPointer<KeyFramesAutomationItem> TrackItem::keyFramesAutomationItem() const
{
    return _keyFramesAutomationItem;
}

bool TrackItem::objectEnabled() const
{
    return _objectEnabled;
}

void TrackItem::setObjectEnabled(bool newObjectEnabled)
{
    _objectEnabled = newObjectEnabled;
}

QString TrackItem::automationSubMenuTitle() const
{
    return _automationSubMenuTitle;
}

void TrackItem::setAutomationSubMenuTitle(const QString& newAutomationSubMenuTitle)
{

    auto index = _automationLaneModel->getIndex(_automationMenuTitle1, newAutomationSubMenuTitle);

    if (index >= 0) {
        return;
    }

    _automationSubMenuTitle = newAutomationSubMenuTitle;
    setKeyFrameEnabled(false);

    // I disabled these -1, beacuse by enabling, if volumeitem is in second line, it will be shown in first one as well in UI
    if (_automationSubMenuTitle == "Volume") {
        //_volumeAutomationItem->automation()->setSubTrackIndex(-1);
        Q_EMIT _volumeAutomationItem->sigUpdate();
        if (_volumeAutomationItem->automation()->isAutomated())
            _volumeAutomationItem->setAutomatedNormalized(_volumeAutomationItem->automation()->getY(_currentIndicator));

    } else if (_automationSubMenuTitle == "Pan") {
        //_panAutomationItem->automation()->setSubTrackIndex(-1);
        Q_EMIT _panAutomationItem->sigUpdate();
        if (_panAutomationItem->automation()->isAutomated())
            _panAutomationItem->setAutomatedNormalized(_panAutomationItem->automation()->getY(_currentIndicator));

    } else if (_automationSubMenuTitle == "SpeakerOn") {
        // _speakerOnAutomationItem->automation()->setSubTrackIndex(-1);
        Q_EMIT _speakerOnAutomationItem->sigUpdate();

        if (_speakerOnAutomationItem->automation()->isAutomated())
            _speakerOnAutomationItem->setAutomatedNormalized(_speakerOnAutomationItem->automation()->getY(_currentIndicator));
    } else if (_automationSubMenuTitle == "KeyFrames") {
        // _keyFramesAutomationItem->automation()->setSubTrackIndex(-1);
        Q_EMIT _keyFramesAutomationItem->sigUpdate();

        if (_keyFramesAutomationItem->automation()->isAutomated())
            _keyFramesAutomationItem->setAutomatedNormalized(_keyFramesAutomationItem->automation()->getY(_currentIndicator));
    } else {

        if (automationItem(_automationMenuTitle1, _automationSubMenuTitle) && automationItem(_automationMenuTitle1, _automationSubMenuTitle)->automation()->isAutomated()) {
            // automationItem(_automationMenuTitle1, _automationSubMenuTitle)->automation()->setSubTrackIndex(-1);

            Q_EMIT automationItem(_automationMenuTitle1, _automationSubMenuTitle)->sigUpdate();

            automationItem(_automationMenuTitle1, _automationSubMenuTitle)->setAutomatedNormalized(automationItem(_automationMenuTitle1, _automationSubMenuTitle)->automation()->getY(_currentIndicator));
        }
    }
}

bool TrackItem::copyAllAutomationItemsInArea(SelectedArea area, QVector<AutomationList>& list, int trackIndex)
{
    bool _empty = true;
    if (volumeAutomationItem()->automation()->contains(area)) {

        if (volumeAutomationItem()->createCopyList(area)) {
            _empty = false;
            list.append(AutomationList { trackIndex, -1,
                "Mixer", "Volume",
                volumeAutomationItem()->automation()->copyList(), area });
        }
    }
    if (panAutomationItem()->automation()->contains(area)) {

        if (panAutomationItem()->createCopyList(area)) {
            _empty = false;
            list.append(AutomationList { trackIndex, -1,
                "Mixer", "Pan",
                panAutomationItem()->automation()->copyList(), area });
        }
    }

    if (speakerOnAutomationItem()->automation()->contains(area)) {

        if (panAutomationItem()->createCopyList(area)) {
            _empty = false;
            list.append(AutomationList { trackIndex, -1,
                "Mixer", "SpeakerOn",
                speakerOnAutomationItem()->automation()->copyList(), area });
        }
    }

    auto effect = ((SampleTrack*)track())->audioPort()->visrEfect();
    if (effect) {
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {
            if (effect->automation(properties[j])) {
                if (effect->automation(properties[j])->contains(area)) {

                    if (effect->automationItem(properties[j])->createCopyList(area)) {
                        _empty = false;
                        list.append(AutomationList { trackIndex, -1,
                            "Visr", properties[j],
                            effect->automation(properties[j])->copyList(), area });
                    }
                }
            }
        }
    }

    auto effectMap = ((SampleTrack*)track())->audioPort()->effects()->getEffectList();

    QMapIterator<QString, int> i(effectMap);
    while (i.hasNext()) {
        i.next();
        auto effect = ((SampleTrack*)track())->audioPort()->effects()->effect(i.value());
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {
            if (effect->automation(properties[j])) {
                if (effect->automation(properties[j])->contains(area)) {

                    if (effect->automationItem(properties[j])->createCopyList(area)) {
                        _empty = false;
                        list.append(AutomationList { trackIndex, -1,
                            i.key(), properties[j],
                            effect->automation(properties[j])->copyList(), area });
                    }
                }
            }
        }
    }

    return _empty;
}

bool TrackItem::copyCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, int trackIndex)
{
    bool _empty = true;

    if (automationItem()) {
        if (automationItem()->automation()->contains(area)) {
            if (automationItem()->createCopyList(area)) {
                _empty = false;
                list.append(AutomationList { trackIndex, -1,
                    getAutomationMenuTitle(), automationSubMenuTitle(),
                    automationItem()->automation()->copyList(), area });
            }
        }
    }

    return _empty;
}

bool TrackItem::copyCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, QString menu, QString submenu, int trackIndex, int laneIndex)
{

    bool _empty = true;
    if (submenu == "Volume") {
        if (volumeAutomationItem()->automation()->contains(area)) {

            if (volumeAutomationItem()->createCopyList(area)) {

                _empty = false;
                list.append(AutomationList { trackIndex, laneIndex,
                    menu, submenu,
                    volumeAutomationItem()->automation()->copyList(), area });
            }
        }
    } else if (submenu == "Pan") {
        if (panAutomationItem()->automation()->contains(area)) {

            if (panAutomationItem()->createCopyList(area)) {

                _empty = false;
                list.append(AutomationList { trackIndex, laneIndex,
                    menu, submenu,
                    panAutomationItem()->automation()->copyList(), area });
            }
        }
    } else if (submenu == "SpeakerOn") {
        if (speakerOnAutomationItem()->automation()->contains(area)) {

            if (speakerOnAutomationItem()->createCopyList(area)) {

                _empty = false;
                list.append(AutomationList { trackIndex, laneIndex,
                    menu, submenu,
                    speakerOnAutomationItem()->automation()->copyList(), area });
            }
        }
    } else {

        if (menu == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->automation(submenu)) {
                    if (effect->automation(submenu)->contains(area)) {

                        if (effect->automationItem(submenu)->createCopyList(area)) {

                            _empty = false;
                            list.append(AutomationList { trackIndex, laneIndex,
                                menu, submenu,
                                effect->automation(submenu)->copyList(), area });
                        }
                    }
                }
            }
        }

        auto effects = ((SampleTrack*)track())->audioPort()->effects();

        if (effects->getEffect(menu)) {
            auto effect = effects->getEffect(menu);
            if (effect->automation(submenu)) {
                if (effect->automation(submenu)->contains(area)) {

                    if (effect->automationItem(submenu)->createCopyList(area)) {

                        _empty = false;
                        list.append(AutomationList { trackIndex, laneIndex,
                            menu, submenu,
                            effect->automation(submenu)->copyList(), area });
                    }
                }
            }
        }
    }
    return _empty;
}

void TrackItem::copyEmptyAutomationItem(SelectedArea area, QVector<AutomationList>& list, int trackIndex)
{
    if (selectedAreaContains(area)) {

        list.append(AutomationList { trackIndex, -1,
            "None", "None",
            QJsonArray(), area });
    }
}

void TrackItem::copyEmptyAutomationItem(SelectedArea area, QVector<AutomationList>& list, QString menu, QString submenu, int trackIndex, int laneIndex)
{
    if (selectedAreaContains(area)) {

        list.append(AutomationList { trackIndex, laneIndex,
            menu, submenu,
            QJsonArray(), area });
    }
}

bool TrackItem::copyKeyFramesInArea(SelectedArea area, QList<KeyFramesItem::KeyFramesPoint>& list, int trackIndex)
{
    bool _empty = true;
    //    if (_keyFramesItem->contains(area)) {

    //        auto result = _keyFramesItem->createCopyList(area);
    //        if (result.size() > 0) {
    //            _empty = false;
    //            list = result;
    //        }
    //    }
    return _empty;
}

bool TrackItem::copyKeyFramesItemInArea(SelectedArea area, QList<KeyFramesItem::KeyFramesPoint>& list, int trackIndex)
{
    bool _empty = true;
    //    if (_automationSubMenuTitle == "KeyFrames") {
    //        if (_keyFramesItem->contains(area)) {
    //            auto result = _keyFramesItem->createCopyList(area);
    //            if (result.size() > 0) {
    //                _empty = false;
    //                list = result;
    //            }
    //        }
    //    }
    return _empty;
}

bool TrackItem::cutAllAutomationItemsInArea(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, int trackIndex)
{

    bool _empty = true;
    if (volumeAutomationItem()->automation()->contains(area)) {

        auto state = volumeAutomationItem()->automation()->getLinesState();
        if (volumeAutomationItem()->cutCopiedList(area)) {
            _empty = false;

            QJsonObject jObject2;
            jObject2["state"] = state;
            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = -1;

            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "Volume";

            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jArray.append(jObject2);

            list.append(AutomationList { trackIndex, -1,
                "Mixer", "Volume",
                volumeAutomationItem()->automation()->copyList(), area });
        }
    }
    if (panAutomationItem()->automation()->contains(area)) {

        auto state = panAutomationItem()->automation()->getLinesState();
        if (panAutomationItem()->cutCopiedList(area)) {
            _empty = false;

            QJsonObject jObject2;
            jObject2["state"] = state;
            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = -1;

            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "Pan";

            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jArray.append(jObject2);

            list.append(AutomationList { trackIndex, -1,
                "Mixer", "Pan",
                panAutomationItem()->automation()->copyList(), area });
        }
    }

    if (speakerOnAutomationItem()->automation()->contains(area)) {

        auto state = speakerOnAutomationItem()->automation()->getLinesState();
        if (speakerOnAutomationItem()->cutCopiedList(area)) {
            _empty = false;

            QJsonObject jObject2;
            jObject2["state"] = state;
            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = -1;

            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "SpeakerOn";

            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jArray.append(jObject2);

            list.append(AutomationList { trackIndex, -1,
                "Mixer", "SpeakerOn",
                speakerOnAutomationItem()->automation()->copyList(), area });
        }
    }

    auto effect = ((SampleTrack*)track())->audioPort()->visrEfect();
    if (effect) {
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {
            if (effect->automation(properties[j])) {
                if (effect->automation(properties[j])->contains(area)) {

                    QJsonObject jObject2;
                    jObject2["state"] = effect->automationItem(properties[j])->automation()->getLinesState();

                    if (effect->automationItem(properties[j])->cutCopiedList(area)) {
                        _empty = false;

                        jObject2["track_index"] = trackIndex;
                        jObject2["sub_track_index"] = -1;

                        jObject2["menu"] = "Visr";
                        jObject2["sub_menu"] = properties[j];
                        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                        jArray.append(jObject2);

                        list.append(AutomationList { trackIndex, -1,
                            "Visr", properties[j],
                            effect->automation(properties[j])->copyList(), area });
                    }
                }
            }
        }
    }

    auto effectMap = ((SampleTrack*)track())->audioPort()->effects()->getEffectList();

    QMapIterator<QString, int> i(effectMap);
    while (i.hasNext()) {
        i.next();
        auto effect = ((SampleTrack*)track())->audioPort()->effects()->effect(i.value());
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {
            if (effect->automation(properties[j])) {
                if (effect->automation(properties[j])->contains(area)) {

                    QJsonObject jObject2;
                    jObject2["state"] = effect->automationItem(properties[j])->automation()->getLinesState();

                    if (effect->automationItem(properties[j])->cutCopiedList(area)) {
                        _empty = false;

                        jObject2["track_index"] = trackIndex;
                        jObject2["sub_track_index"] = -1;

                        jObject2["menu"] = i.key();
                        jObject2["sub_menu"] = properties[j];
                        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                        jArray.append(jObject2);

                        list.append(AutomationList { trackIndex, -1,
                            i.key(), properties[j],
                            effect->automation(properties[j])->copyList(), area });
                    }
                }
            }
        }
    }

    return _empty;
}

bool TrackItem::cutCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, int trackIndex)
{
    bool _empty = true;
    if (automationItem()) {

        if (automationItem()->automation()->contains(area)) {

            QJsonObject jObject2;
            jObject2["state"] = automationItem()->automation()->getLinesState();
            if (automationItem()->cutCopiedList(area)) {
                _empty = false;

                jObject2["track_index"] = trackIndex;
                jObject2["sub_track_index"] = -1;

                jObject2["menu"] = getAutomationMenuTitle();
                jObject2["sub_menu"] = automationSubMenuTitle();

                jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                jArray.append(jObject2);

                list.append(AutomationList { trackIndex, -1,
                    getAutomationMenuTitle(),
                    automationSubMenuTitle(),
                    automationItem()->automation()->copyList(), area });
            }
        }
    }
    return _empty;
}

bool TrackItem::cutCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list,
    QJsonArray& jArray, QString menu, QString submenu, int trackIndex, int laneIndex)
{
    if (submenu == "Volume") {
        if (volumeAutomationItem()->automation()->contains(area)) {

            QJsonObject jObject2;
            jObject2["state"] = volumeAutomationItem()->automation()->getLinesState();
            if (volumeAutomationItem()->cutCopiedList(area)) {

                jObject2["track_index"] = trackIndex;
                jObject2["sub_track_index"] = laneIndex;
                jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;

                jObject2["menu"] = "Mixer";
                jObject2["sub_menu"] = "Volume";

                jArray.append(jObject2);

                list.append(AutomationList { trackIndex, laneIndex,
                    menu, submenu,
                    volumeAutomationItem()->automation()->copyList(), area });
            }
        }
    } else if (submenu == "Pan") {
        if (panAutomationItem()->automation()->contains(area)) {

            QJsonObject jObject2;
            jObject2["state"] = panAutomationItem()->automation()->getLinesState();
            if (panAutomationItem()->cutCopiedList(area)) {

                jObject2["track_index"] = trackIndex;
                jObject2["sub_track_index"] = laneIndex;
                jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                jObject2["menu"] = "Mixer";
                jObject2["sub_menu"] = "Pan";

                jArray.append(jObject2);

                list.append(AutomationList { trackIndex, laneIndex,
                    menu, submenu,
                    panAutomationItem()->automation()->copyList(), area });
            }
        }
    } else if (submenu == "SpeakerOn") {
        if (speakerOnAutomationItem()->automation()->contains(area)) {

            QJsonObject jObject2;
            jObject2["state"] = speakerOnAutomationItem()->automation()->getLinesState();
            if (speakerOnAutomationItem()->cutCopiedList(area)) {

                jObject2["track_index"] = trackIndex;
                jObject2["sub_track_index"] = laneIndex;
                jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                jObject2["menu"] = "Mixer";
                jObject2["sub_menu"] = "SpeakerOn";

                jArray.append(jObject2);

                list.append(AutomationList { trackIndex, laneIndex,
                    menu, submenu,
                    speakerOnAutomationItem()->automation()->copyList(), area });
            }
        }
    } else {

        if (menu == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->automation(submenu)) {
                    if (effect->automation(submenu)->contains(area)) {
                        QJsonObject jObject2;
                        jObject2["state"] = effect->automationItem(submenu)->automation()->getLinesState();
                        if (effect->automationItem(submenu)->cutCopiedList(area)) {

                            jObject2["track_index"] = trackIndex;
                            jObject2["sub_track_index"] = laneIndex;
                            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                            jObject2["menu"] = menu;
                            jObject2["sub_menu"] = submenu;

                            jArray.append(jObject2);
                            list.append(AutomationList { trackIndex, laneIndex,
                                menu, submenu,
                                effect->automation(submenu)->copyList(), area });
                        }
                    }
                }
            }
        }

        auto effects = ((SampleTrack*)track())->audioPort()->effects();

        if (effects->getEffect(menu)) {
            auto effect = effects->getEffect(menu);
            if (effect->automation(submenu)) {
                if (effect->automation(submenu)->contains(area)) {
                    QJsonObject jObject2;
                    jObject2["state"] = effect->automationItem(submenu)->automation()->getLinesState();
                    if (effect->automationItem(submenu)->cutCopiedList(area)) {

                        jObject2["track_index"] = trackIndex;
                        jObject2["sub_track_index"] = laneIndex;
                        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                        jObject2["menu"] = menu;
                        jObject2["sub_menu"] = submenu;

                        jArray.append(jObject2);
                        list.append(AutomationList { trackIndex, laneIndex,
                            menu, submenu,
                            effect->automation(submenu)->copyList(), area });
                    }
                }
            }
        }
    }

    return false;
}

void TrackItem::cutEmptyAutomationItem(SelectedArea area, QVector<AutomationList>& list, int trackIndex)
{
    if (selectedAreaContains(area)) {

        list.append(AutomationList { trackIndex, -1,
            "None", "None",
            QJsonArray(), area });
    }
}

void TrackItem::deleteAllAutomationItemsInArea(SelectedArea area, QJsonArray& jArray, int trackIndex)
{

    if (volumeAutomationItem()->automation()->contains(area)) {

        QJsonObject jObject2;
        jObject2["state"] = volumeAutomationItem()->automation()->getLinesState();
        if (volumeAutomationItem()->removedCopiedList(area)) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = -1;

            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "Volume";
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jArray.append(jObject2);
        }
    }
    if (panAutomationItem()->automation()->contains(area)) {

        QJsonObject jObject2;
        jObject2["state"] = panAutomationItem()->automation()->getLinesState();
        if (panAutomationItem()->removedCopiedList(area)) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = -1;

            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "Pan";
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jArray.append(jObject2);
        }
    }
    if (speakerOnAutomationItem()->automation()->contains(area)) {

        QJsonObject jObject2;
        jObject2["state"] = speakerOnAutomationItem()->automation()->getLinesState();
        if (speakerOnAutomationItem()->removedCopiedList(area)) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = -1;

            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "SpeakerOn";
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jArray.append(jObject2);
        }
    }
    auto effect = ((SampleTrack*)track())->audioPort()->visrEfect();
    if (effect) {
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {
            if (effect->automation(properties[j])) {
                if (effect->automation(properties[j])->contains(area)) {

                    QJsonObject jObject2;
                    jObject2["state"] = effect->automationItem(properties[j])->automation()->getLinesState();
                    if (effect->automationItem(properties[j])->removedCopiedList(area)) {

                        jObject2["track_index"] = trackIndex;
                        jObject2["sub_track_index"] = -1;

                        jObject2["menu"] = "Visr";
                        jObject2["sub_menu"] = properties[j];
                        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                        jArray.append(jObject2);
                    }
                }
            }
        }
    }

    auto effectMap = ((SampleTrack*)track())->audioPort()->effects()->getEffectList();

    QMapIterator<QString, int> i(effectMap);
    while (i.hasNext()) {
        i.next();
        auto effect = ((SampleTrack*)track())->audioPort()->effects()->effect(i.value());
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {
            if (effect->automation(properties[j])) {
                if (effect->automation(properties[j])->contains(area)) {

                    QJsonObject jObject2;
                    jObject2["state"] = effect->automationItem(properties[j])->automation()->getLinesState();
                    if (effect->automationItem(properties[j])->removedCopiedList(area)) {

                        jObject2["track_index"] = trackIndex;
                        jObject2["sub_track_index"] = -1;

                        jObject2["menu"] = i.key();
                        jObject2["sub_menu"] = properties[j];
                        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                        jArray.append(jObject2);
                    }
                }
            }
        }
    }
}

void TrackItem::deleteCurrentAutomationItemInArea(SelectedArea area, QJsonArray& jArray, int trackIndex)
{

    if (automationItem()) {

        if (automationItem()->automation()->contains(area)) {

            QJsonObject jObject2;
            jObject2["state"] = automationItem()->automation()->getLinesState();

            if (automationItem()->removedCopiedList(area)) {

                jObject2["track_index"] = trackIndex;
                jObject2["sub_track_index"] = -1;
                jObject2["menu"] = getAutomationMenuTitle();
                jObject2["sub_menu"] = automationSubMenuTitle();
                jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                jArray.append(jObject2);
            }
        }
    }
}

void TrackItem::deleteCurrentAutomationItemInArea(SelectedArea area, QJsonArray& jArray,
    QString menu, QString submenu, int trackIndex, int laneIndex)
{

    if (submenu == "Volume") {
        if (volumeAutomationItem()->automation()->contains(area)) {

            QJsonObject jObject2;
            jObject2["state"] = volumeAutomationItem()->automation()->getLinesState();
            if (volumeAutomationItem()->removedCopiedList(area)) {

                jObject2["track_index"] = trackIndex;
                jObject2["sub_track_index"] = laneIndex;
                jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                jObject2["menu"] = "Mixer";
                jObject2["sub_menu"] = "Volume";
                jArray.append(jObject2);
            }
        }
    } else if (submenu == "Pan") {
        if (panAutomationItem()->automation()->contains(area)) {

            QJsonObject jObject2;
            jObject2["state"] = panAutomationItem()->automation()->getLinesState();
            if (panAutomationItem()->removedCopiedList(area)) {

                jObject2["track_index"] = trackIndex;
                jObject2["sub_track_index"] = laneIndex;
                jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                jObject2["menu"] = "Mixer";
                jObject2["sub_menu"] = "Pan";
                jArray.append(jObject2);
            }
        }
    } else if (submenu == "Pan") {
        if (speakerOnAutomationItem()->automation()->contains(area)) {

            QJsonObject jObject2;
            jObject2["state"] = speakerOnAutomationItem()->automation()->getLinesState();
            if (speakerOnAutomationItem()->removedCopiedList(area)) {

                jObject2["track_index"] = trackIndex;
                jObject2["sub_track_index"] = laneIndex;
                jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                jObject2["menu"] = "Mixer";
                jObject2["sub_menu"] = "SpeakerOn";
                jArray.append(jObject2);
            }
        }
    } else {

        if (menu == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->automation(submenu)) {
                    if (effect->automation(submenu)->contains(area)) {
                        QJsonObject jObject2;
                        jObject2["state"] = effect->automationItem(submenu)->automation()->getLinesState();
                        if (effect->automationItem(submenu)->removedCopiedList(area)) {

                            jObject2["track_index"] = trackIndex;
                            jObject2["sub_track_index"] = laneIndex;
                            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                            jObject2["menu"] = menu;
                            jObject2["sub_menu"] = submenu;

                            jArray.append(jObject2);
                        }
                    }
                }
            }
        }

        auto effects = ((SampleTrack*)track())->audioPort()->effects();

        if (effects->getEffect(menu)) {
            auto effect = effects->getEffect(menu);
            if (effect->automation(submenu)) {
                if (effect->automation(submenu)->contains(area)) {
                    QJsonObject jObject2;
                    jObject2["state"] = effect->automationItem(submenu)->automation()->getLinesState();
                    if (effect->automationItem(submenu)->removedCopiedList(area)) {

                        jObject2["track_index"] = trackIndex;
                        jObject2["sub_track_index"] = laneIndex;
                        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                        jObject2["menu"] = menu;
                        jObject2["sub_menu"] = submenu;

                        jArray.append(jObject2);
                    }
                }
            }
        }
    }
}

double TrackItem::getStartTimeSelectAllAutomation()
{

    double startSelectedTime_automation = -1;
    if (volumeAutomationItem()->automation()->isAutomated() && volumeAutomationItem()->automation()->isAutomated()) {
        startSelectedTime_automation = volumeAutomationItem()->automation()->getFirstPoint();
    }
    if (panAutomationItem()->automation()->isAutomated()) {
        if (panAutomationItem()->automation()->getFirstPoint() < startSelectedTime_automation) {
            startSelectedTime_automation = panAutomationItem()->automation()->getFirstPoint();
        }
    }
    auto effectMap = ((SampleTrack*)track())->audioPort()->effects()->getEffectList();

    QMapIterator<QString, int> i(effectMap);
    while (i.hasNext()) {
        i.next();
        auto effect = ((SampleTrack*)track())->audioPort()->effects()->effect(i.value());
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {
            if (effect->automation(properties[j])->isAutomated()) {

                if (effect->automation(properties[j])->getFirstPoint() < startSelectedTime_automation) {
                    startSelectedTime_automation = effect->automation(properties[j])->getFirstPoint();
                }
            }
        }
    }
    return startSelectedTime_automation;
}

double TrackItem::getStartTimeSelectAllAutomation(QString menu, QString submenu)
{
    if (submenu == "Volume") {
        if (volumeAutomationItem()->automation()->isAutomated()) {

            return volumeAutomationItem()->automation()->getFirstPoint();
        }
    } else if (submenu == "Pan") {
        if (panAutomationItem()->automation()->isAutomated()) {

            return panAutomationItem()->automation()->getFirstPoint();
        }
    } else if (submenu == "SpeakerOn") {
        if (speakerOnAutomationItem()->automation()->isAutomated()) {

            return speakerOnAutomationItem()->automation()->getFirstPoint();
        }
    } else {

        if (menu == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->automation(submenu)) {
                    if (effect->automation(submenu)->isAutomated()) {
                        return effect->automation(submenu)->getFirstPoint();
                    }
                }
            }
        }

        auto effects = ((SampleTrack*)track())->audioPort()->effects();

        if (effects->getEffect(menu)) {
            auto effect = effects->getEffect(menu);
            if (effect->automation(submenu)) {
                if (effect->automation(submenu)->isAutomated()) {
                    return effect->automation(submenu)->getFirstPoint();
                }
            }
        }
    }
    return -1;
}

double TrackItem::getEndTimeSelectAllAutomation()
{

    double endSelectedTime_automation = -1;
    if (volumeAutomationItem()->automation()->isAutomated()) {
        endSelectedTime_automation = volumeAutomationItem()->automation()->getLastPoint();
    }
    if (panAutomationItem()->automation()->isAutomated()) {
        endSelectedTime_automation = panAutomationItem()->automation()->getLastPoint();
    }
    auto effectMap = ((SampleTrack*)track())->audioPort()->effects()->getEffectList();

    QMapIterator<QString, int> i(effectMap);
    while (i.hasNext()) {
        i.next();
        auto effect = ((SampleTrack*)track())->audioPort()->effects()->effect(i.value());
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {
            if (effect->automation(properties[j])->isAutomated()) {

                endSelectedTime_automation = effect->automation(properties[j])->getLastPoint();
            }
        }
    }
    return endSelectedTime_automation;
}

double TrackItem::getEndTimeSelectAllAutomation(QString menu, QString submenu)
{
    if (submenu == "Volume") {
        if (volumeAutomationItem()->automation()->isAutomated()) {

            return volumeAutomationItem()->automation()->getLastPoint();
        }
    } else if (submenu == "Pan") {
        if (panAutomationItem()->automation()->isAutomated()) {
            return panAutomationItem()->automation()->getLastPoint();
        }
    } else if (submenu == "SpeakerOn") {
        if (speakerOnAutomationItem()->automation()->isAutomated()) {

            return speakerOnAutomationItem()->automation()->getLastPoint();
        }
    } else {

        if (menu == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->automation(submenu)) {
                    if (effect->automation(submenu)->isAutomated()) {
                        return effect->automation(submenu)->getLastPoint();
                    }
                }
            }
        }

        auto effects = ((SampleTrack*)track())->audioPort()->effects();

        if (effects->getEffect(menu)) {
            auto effect = effects->getEffect(menu);
            if (effect->automation(submenu)) {
                if (effect->automation(submenu)->isAutomated()) {
                    return effect->automation(submenu)->getLastPoint();
                }
            }
        }
    }
    return -1;
}

void TrackItem::insertAllAutomationItems(QJsonArray& jArray, int trackIndex, bool dont_copy)
{

    QJsonObject jObject2;
    jObject2["state"] = volumeAutomationItem()->automation()->getLinesState();
    if (volumeAutomationItem()->insertCopiedList()) {

        jObject2["track_index"] = trackIndex;
        jObject2["sub_track_index"] = -1;

        jObject2["menu"] = "Mixer";
        jObject2["sub_menu"] = "Volume";
        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
        if (dont_copy == false)
            jArray.append(jObject2);
    }

    jObject2["state"] = panAutomationItem()->automation()->getLinesState();
    if (panAutomationItem()->insertCopiedList()) {

        jObject2["track_index"] = trackIndex;
        jObject2["sub_track_index"] = -1;

        jObject2["menu"] = "Mixer";
        jObject2["sub_menu"] = "Pan";
        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
        if (dont_copy == false)
            jArray.append(jObject2);
    }

    jObject2["state"] = speakerOnAutomationItem()->automation()->getLinesState();
    if (speakerOnAutomationItem()->insertCopiedList()) {

        jObject2["track_index"] = trackIndex;
        jObject2["sub_track_index"] = -1;

        jObject2["menu"] = "Mixer";
        jObject2["sub_menu"] = "SpeakerOn";
        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
        if (dont_copy == false)
            jArray.append(jObject2);
    }

    auto effect = ((SampleTrack*)track())->audioPort()->visrEfect();
    if (effect) {
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {

            if (effect->automationItem(properties[j])) {
                QJsonObject jObject2;
                jObject2["state"] = effect->automationItem(properties[j])->automation()->getLinesState();
                if (effect->automationItem(properties[j])->insertCopiedList()) {

                    jObject2["track_index"] = trackIndex;
                    jObject2["sub_track_index"] = -1;

                    jObject2["menu"] = "Visr";
                    jObject2["sub_menu"] = properties[j];
                    jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                    if (dont_copy == false)
                        jArray.append(jObject2);
                }
            }
        }
    }

    auto effectMap = ((SampleTrack*)track())->audioPort()->effects()->getEffectList();

    QMapIterator<QString, int> i(effectMap);
    while (i.hasNext()) {
        i.next();
        auto effect = ((SampleTrack*)track())->audioPort()->effects()->effect(i.value());
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {

            if (effect->automationItem(properties[j])) {
                QJsonObject jObject2;
                jObject2["state"] = effect->automationItem(properties[j])->automation()->getLinesState();
                if (effect->automationItem(properties[j])->insertCopiedList()) {

                    jObject2["track_index"] = trackIndex;
                    jObject2["sub_track_index"] = -1;

                    jObject2["menu"] = i.key();
                    jObject2["sub_menu"] = properties[j];
                    jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                    if (dont_copy == false)
                        jArray.append(jObject2);
                }
            }
        }
    }
}

void TrackItem::insertCurrentAutomationItem(QJsonArray& jArray, int trackIndex, bool dont_copy)
{

    if (automationItem()) {
        QJsonObject jObject2;
        jObject2["state"] = automationItem()->automation()->getLinesState();
        if (automationItem()->insertCopiedList()) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = -1;

            jObject2["menu"] = getAutomationMenuTitle();
            jObject2["sub_menu"] = automationSubMenuTitle();

            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            if (dont_copy == false)
                jArray.append(jObject2);
        }
    }
}

void TrackItem::insertCurrentAutomationItemInArea(QJsonArray& jArray, QString menu, QString submenu, int trackIndex, int laneIndex)
{
    if (submenu == "Volume") {
        QJsonObject jObject2;
        jObject2["state"] = volumeAutomationItem()->automation()->getLinesState();
        if (volumeAutomationItem()->insertCopiedList()) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = laneIndex;
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "Volume";

            jArray.append(jObject2);
        }

    } else if (submenu == "Pan") {
        QJsonObject jObject2;
        jObject2["state"] = panAutomationItem()->automation()->getLinesState();
        if (panAutomationItem()->insertCopiedList()) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = laneIndex;
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "Pan";

            jArray.append(jObject2);
        }

    } else if (submenu == "SpeakerOn") {
        QJsonObject jObject2;
        jObject2["state"] = speakerOnAutomationItem()->automation()->getLinesState();
        if (speakerOnAutomationItem()->insertCopiedList()) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = laneIndex;
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "SpeakerOn";

            jArray.append(jObject2);
        }

    } else {

        if (menu == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->automationItem(submenu)) {
                    QJsonObject jObject2;
                    jObject2["state"] = effect->automationItem(submenu)->automation()->getLinesState();
                    if (effect->automationItem(submenu)->insertCopiedList()) {

                        jObject2["track_index"] = trackIndex;
                        jObject2["sub_track_index"] = laneIndex;
                        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                        jObject2["menu"] = menu;
                        jObject2["sub_menu"] = submenu;

                        jArray.append(jObject2);
                    }
                }
            }
        }

        auto effects = ((SampleTrack*)track())->audioPort()->effects();

        if (effects->getEffect(menu)) {
            auto effect = effects->getEffect(menu);
            QJsonObject jObject2;

            if (effect->automationItem(submenu)) {
                jObject2["state"] = effect->automationItem(submenu)->automation()->getLinesState();
                if (effect->automationItem(submenu)->insertCopiedList()) {

                    jObject2["track_index"] = trackIndex;
                    jObject2["sub_track_index"] = laneIndex;
                    jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                    jObject2["menu"] = menu;
                    jObject2["sub_menu"] = submenu;

                    jArray.append(jObject2);
                }
            }
        }
    }
}

void TrackItem::duplicateAllAutomationItems(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, int trackIndex, int subTrackIndex)
{
    bool _empty = true;

    QJsonObject jObject2;
    jObject2["state"] = volumeAutomationItem()->getLinesState();

    if (volumeAutomationItem()->createCopyList(area)) {

        jObject2["track_index"] = trackIndex;
        jObject2["sub_track_index"] = subTrackIndex;
        jObject2["menu"] = "Mixer";
        jObject2["sub_menu"] = "Volume";

        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
        jArray.append(jObject2);
        list.append(AutomationList { trackIndex, subTrackIndex,
            "Mixer", "Volume",
            volumeAutomationItem()->automation()->copyList(), area });
    }
    jObject2["state"] = panAutomationItem()->getLinesState();
    if (panAutomationItem()->createCopyList(area)) {

        jObject2["track_index"] = trackIndex;
        jObject2["sub_track_index"] = subTrackIndex;
        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
        jArray.append(jObject2);

        jObject2["menu"] = "Mixer";
        jObject2["sub_menu"] = "Pan";

        list.append(AutomationList { trackIndex, subTrackIndex,
            "Mixer", "Pan",
            panAutomationItem()->automation()->copyList(), area });
    }
    jObject2["state"] = speakerOnAutomationItem()->getLinesState();
    if (speakerOnAutomationItem()->createCopyList(area)) {

        jObject2["track_index"] = trackIndex;
        jObject2["sub_track_index"] = subTrackIndex;
        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
        jArray.append(jObject2);

        jObject2["menu"] = "Mixer";
        jObject2["sub_menu"] = "SpeakerOn";

        list.append(AutomationList { trackIndex, subTrackIndex,
            "Mixer", "Pan",
            speakerOnAutomationItem()->automation()->copyList(), area });
    }
    auto effect = ((SampleTrack*)track())->audioPort()->visrEfect();
    if (effect) {
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {

            if (effect->automationItem(properties[j])) {
                jObject2["state"] = effect->automationItem(properties[j])->getLinesState();
                if (effect->automationItem(properties[j])->createCopyList(area)) {

                    jObject2["track_index"] = trackIndex;
                    jObject2["sub_track_index"] = subTrackIndex;
                    jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                    jArray.append(jObject2);

                    jObject2["menu"] = "Visr";
                    jObject2["sub_menu"] = properties[j];

                    list.append(AutomationList { trackIndex, subTrackIndex,
                        "Visr", properties[j],
                        effect->automation(properties[j])->copyList(), area });
                }
            }
        }
    }

    auto effectMap = ((SampleTrack*)track())->audioPort()->effects()->getEffectList();

    QMapIterator<QString, int> i(effectMap);
    while (i.hasNext()) {
        i.next();
        auto effect = ((SampleTrack*)track())->audioPort()->effects()->effect(i.value());
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {

            if (effect->automationItem(properties[j])) {
                jObject2["state"] = effect->automationItem(properties[j])->getLinesState();
                if (effect->automationItem(properties[j])->createCopyList(area)) {

                    jObject2["track_index"] = trackIndex;
                    jObject2["sub_track_index"] = subTrackIndex;
                    jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                    jArray.append(jObject2);

                    jObject2["menu"] = i.key();
                    jObject2["sub_menu"] = properties[j];

                    list.append(AutomationList { trackIndex, subTrackIndex,
                        i.key(), properties[j],
                        effect->automation(properties[j])->copyList(), area });
                }
            }
        }
    }
}

void TrackItem::duplicateCurrentAutomationItem(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, int trackIndex, int subTrackIndex)
{

    if (automationItem()) {
        QJsonObject jObject2;
        jObject2["state"] = automationItem()->getLinesState();
        if (automationItem()->createCopyList(area)) {
            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = subTrackIndex;
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;

            jObject2["menu"] = getAutomationMenuTitle();
            jObject2["sub_menu"] = automationSubMenuTitle();

            list.append(AutomationList { trackIndex, subTrackIndex,
                getAutomationMenuTitle(), automationSubMenuTitle(),
                automationItem()->automation()->copyList(), area });

            jArray.append(jObject2);
        }
    }
}

void TrackItem::duplicateCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, int trackIndex, int laneIndex, QString menu, QString submenu)
{

    if (submenu == "Volume") {

        QJsonObject jObject2;
        jObject2["state"] = volumeAutomationItem()->getLinesState();
        if (volumeAutomationItem()->createCopyList(area)) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = laneIndex;
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;

            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "Volume";

            jArray.append(jObject2);

            list.append(AutomationList { trackIndex, laneIndex,
                menu, submenu,
                volumeAutomationItem()->automation()->copyList(), area });
        }

    } else if (submenu == "Pan") {

        QJsonObject jObject2;
        jObject2["state"] = panAutomationItem()->getLinesState();
        if (panAutomationItem()->createCopyList(area)) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = laneIndex;
            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "Pan";
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jArray.append(jObject2);

            list.append(AutomationList { trackIndex, laneIndex,
                menu, submenu,
                panAutomationItem()->automation()->copyList(), area });
        }

    } else if (submenu == "SpeakerOn") {

        QJsonObject jObject2;
        jObject2["state"] = speakerOnAutomationItem()->getLinesState();
        if (speakerOnAutomationItem()->createCopyList(area)) {

            jObject2["track_index"] = trackIndex;
            jObject2["sub_track_index"] = laneIndex;
            jObject2["menu"] = "Mixer";
            jObject2["sub_menu"] = "SpeakerOn";
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            jArray.append(jObject2);

            list.append(AutomationList { trackIndex, laneIndex,
                menu, submenu,
                speakerOnAutomationItem()->automation()->copyList(), area });
        }

    } else {

        if (menu == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->automationItem(submenu)) {
                    QJsonObject jObject2;
                    jObject2["state"] = effect->automationItem(submenu)->getLinesState();
                    if (effect->automationItem(submenu)->createCopyList(area)) {
                        jObject2["track_index"] = trackIndex;
                        jObject2["sub_track_index"] = laneIndex;
                        jObject2["menu"] = menu;
                        jObject2["sub_menu"] = submenu;
                        jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                        jArray.append(jObject2);
                        list.append(AutomationList { trackIndex, laneIndex,
                            menu, submenu,
                            effect->automation(submenu)->copyList(), area });
                    }
                }
            }
        }

        auto effects = ((SampleTrack*)track())->audioPort()->effects();

        if (effects->getEffect(menu)) {
            auto effect = effects->getEffect(menu);
            if (effect->automationItem(submenu)) {
                QJsonObject jObject2;
                jObject2["state"] = effect->automationItem(submenu)->getLinesState();
                if (effect->automationItem(submenu)->createCopyList(area)) {
                    jObject2["track_index"] = trackIndex;
                    jObject2["sub_track_index"] = laneIndex;
                    jObject2["menu"] = menu;
                    jObject2["sub_menu"] = submenu;
                    jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
                    jArray.append(jObject2);
                    list.append(AutomationList { trackIndex, laneIndex,
                        menu, submenu,
                        effect->automation(submenu)->copyList(), area });
                }
            }
        }
    }
}

void TrackItem::movingAllAutomationItems(SelectedArea area, QVector<AutomationList>& list, int trackIndex)
{

    if (volumeAutomationItem()->automation()->contains(area) && volumeAutomationItem()->automation()->isAutomated()) {
        if (volumeAutomationItem()->createCopyList(area)) {

            list.append(AutomationList { trackIndex, -1,
                "Mixer", "Volume",
                volumeAutomationItem()->automation()->copyList(), area });
        }
    }

    if (panAutomationItem()->automation()->contains(area) && panAutomationItem()->automation()->isAutomated()) {
        if (panAutomationItem()->createCopyList(area)) {

            list.append(AutomationList { trackIndex, -1,
                "Mixer", "Pan",
                panAutomationItem()->automation()->copyList(), area });
        }
    }

    if (speakerOnAutomationItem()->automation()->contains(area) && speakerOnAutomationItem()->automation()->isAutomated()) {
        if (speakerOnAutomationItem()->createCopyList(area)) {

            list.append(AutomationList { trackIndex, -1,
                "Mixer", "SpeakerOn",
                speakerOnAutomationItem()->automation()->copyList(), area });
        }
    }

    auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
    if (effect) {
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {

            if (effect->automationItem(properties[j])) {
                if (effect->automationItem(properties[j])->automation()->contains(area) && effect->automationItem(properties[j])->automation()->isAutomated()) {
                    if (effect->automationItem(properties[j])->createCopyList(area)) {

                        list.append(AutomationList { trackIndex, -1,
                            "Visr", properties[j],
                            effect->automationItem(properties[j])->automation()->copyList(), area });
                    }
                }
            }
        }
    }

    auto effectMap = ((SampleTrack*)track())->audioPort()->effects()->getEffectList();

    QMapIterator<QString, int> i(effectMap);
    while (i.hasNext()) {
        i.next();
        auto effect = ((SampleTrack*)track())->audioPort()->effects()->effect(i.value());
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {

            if (effect->automationItem(properties[j])) {
                if (effect->automationItem(properties[j])->automation()->contains(area) && effect->automationItem(properties[j])->automation()->isAutomated()) {
                    if (effect->automationItem(properties[j])->createCopyList(area)) {

                        list.append(AutomationList { trackIndex, -1,
                            i.key(), properties[j],
                            effect->automationItem(properties[j])->automation()->copyList(), area });
                    }
                }
            }
        }
    }
}

void TrackItem::movingCurrentAutomationItem(SelectedArea area, QVector<AutomationList>& list, int trackIndex)
{

    if (automationItem()) {

        if (automationItem()->automation()->contains(area)) {
            if (automationItem()->createCopyList(area)) {

                list.append(AutomationList { trackIndex, -1,
                    getAutomationMenuTitle(), automationSubMenuTitle(),
                    automationItem()->automation()->copyList(), area });
            }
        }
    }
}

void TrackItem::movingCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, QString menu, QString submenu, int trackIndex, int laneIndex)
{

    if (submenu == "Volume") {

        if (volumeAutomationItem()->automation()->contains(area)) {
            if (volumeAutomationItem()->createCopyList(area)) {

                list.append(AutomationList { trackIndex, laneIndex,
                    menu, submenu,
                    volumeAutomationItem()->automation()->copyList(), area });
            }
        }

    } else if (submenu == "Pan") {

        if (panAutomationItem()->automation()->contains(area)) {
            if (panAutomationItem()->createCopyList(area)) {

                list.append(AutomationList { trackIndex, laneIndex,
                    menu, submenu,
                    panAutomationItem()->automation()->copyList(), area });
            }
        }

    } else if (submenu == "SpeakerOn") {

        if (speakerOnAutomationItem()->automation()->contains(area)) {
            if (speakerOnAutomationItem()->createCopyList(area)) {

                list.append(AutomationList { trackIndex, laneIndex,
                    menu, submenu,
                    speakerOnAutomationItem()->automation()->copyList(), area });
            }
        }

    } else {

        if (menu == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->automationItem(submenu)) {
                    if (effect->automationItem(submenu)->automation()->contains(area)) {
                        if (effect->automationItem(submenu)->createCopyList(area)) {

                            list.append(AutomationList { trackIndex, laneIndex,
                                menu, submenu,
                                effect->automation(submenu)->copyList(), area });
                        }
                    }
                }
            }
        }

        auto effects = ((SampleTrack*)track())->audioPort()->effects();

        if (effects->getEffect(menu)) {
            auto effect = effects->getEffect(menu);
            if (effect->automationItem(submenu)) {
                if (effect->automationItem(submenu)->automation()->contains(area)) {
                    if (effect->automationItem(submenu)->createCopyList(area)) {

                        list.append(AutomationList { trackIndex, laneIndex,
                            menu, submenu,
                            effect->automation(submenu)->copyList(), area });
                    }
                }
            }
        }
    }
}

void TrackItem::clearAutomationItems()
{
    volumeAutomationItem()->automation()->clearCopyList();
    panAutomationItem()->automation()->clearCopyList();

    auto effectMap = ((SampleTrack*)track())->audioPort()->effects()->getEffectList();

    QMapIterator<QString, int> i(effectMap);
    while (i.hasNext()) {
        i.next();
        auto effect = ((SampleTrack*)track())->audioPort()->effects()->effect(i.value());
        auto properties = effect->controls()->shortTitleList();
        for (int j = 0; j < properties.size(); j++) {

            if (effect->automationItem(properties[j])) {
                effect->automationItem(properties[j])->automation()->clearCopyList();
            }
        }
    }
}

// QSharedPointer<KeyFramesItem> TrackItem::keyFramesItem() const
//{
//     return _keyFramesItem;
// }

QMap<qint64, int> TrackItem::keyFrames() const
{
    return QMap<qint64, int>();
}

bool TrackItem::isMasterTrack() const
{
    return _isMasterTrack;
}

void TrackItem::setMasterTrack(bool isMasterTrack)
{
    _isMasterTrack = isMasterTrack;
}

void TrackItem::totalRowExpandedChanged()
{
    Q_EMIT rowsExpandedChanged(_trackIndex);
}

QString TrackItem::getAutomationMenuTitle() const
{
    return _automationMenuTitle1;
}

void TrackItem::setAutomationMenuTitle(const QString& automationMenuTitle)
{
    _automationMenuTitle1 = automationMenuTitle;
}

int TrackItem::getRowsExpandedCount() const
{
    return _rowsExpandedCount;
}

int TrackItem::getLastRowExpanded() const
{
    return _lastRowExpanded;
}

void TrackItem::setLastRowExpanded(int lastRowExpanded)
{
    _lastRowExpanded = lastRowExpanded;
}

QSharedPointer<AutomationLaneModel> TrackItem::automationLaneModel() const
{
    return _automationLaneModel;
}

AutomationLaneModel* TrackItem::getAutomationLaneModel() const
{

    auto obj = _automationLaneModel.data();
    QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
    return obj;
}

void TrackItem::setRowsExpandedCount(int count)
{

    if (count < 1) {
        count = 1;
    }
    _rowsExpandedCount = count;

    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();
        i.value()->setRowsExpandedCount(_rowsExpandedCount);
    }

    auto rect_ = rect();
    if (_rowsExpanded && automationLaneEnabled()) {
        rect_.setHeight(_rowsExpandedCount * _areaInfo->titleHeight() + automationLaneModel()->totalHeight() + _areaInfo->seperatorHeight());
    } else {

        rect_.setHeight(_rowsExpandedCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight());
    }
    setRect(rect_);

    Q_EMIT rowsExpandedChanged(_trackIndex);
}

int TrackItem::rowsExpandedCount() const
{
    return _rowsExpandedCount;
}

QColor TrackItem::getTitleColor() const
{
    return _titleColor;
}

void TrackItem::setTitleColor(const QColor& newTitleColor)
{
    _titleColor = newTitleColor;
}

bool TrackItem::isSelected() const
{
    return _isSelected;
}

void TrackItem::setSelected(bool isSelected)
{
    _isSelected = isSelected;
}

QSharedPointer<TruePeakModel> TrackItem::truePeakModel()
{
    return _truePeakModel;
}

TruePeakModel* TrackItem::getTruePeakModel()
{
    auto item = _truePeakModel.data();
    QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
    return item;
}

void TrackItem::updateTruePeak()
{

    if (((SampleTrack*)track())->audioPort()->bufferUsed() == false) {
        auto channels = 2;
        for (int i = 0; i < channels; i++) {

            float AverageMeterLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).AverageMeterLevel);
            float AverageMeterPeakLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).AverageMeterPeakLevel);
            float PeakMeterLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).PeakMeterLevel);
            float PeakMeterPeakLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).PeakMeterPeakLevel);
            float MaximumPeakLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).MaximumPeakLevel);
            float MaximumTruePeakLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).MaximumTruePeakLevel);

            AverageMeterLevel -= 1;
            AverageMeterPeakLevel -= 1;
            PeakMeterLevel -= 1;
            PeakMeterPeakLevel -= 1;
            MaximumPeakLevel -= 1;
            MaximumTruePeakLevel -= 1;

            if (AverageMeterLevel < -70) {
                AverageMeterLevel = -70;
            }
            if (AverageMeterPeakLevel < -70) {
                AverageMeterPeakLevel = -70;
            }
            if (PeakMeterLevel < -70) {
                PeakMeterLevel = -70;
            }
            if (PeakMeterPeakLevel < -70) {
                PeakMeterPeakLevel = -70;
            }
            if (MaximumPeakLevel < -70) {
                MaximumPeakLevel = -70;
            }
            if (MaximumTruePeakLevel < -70) {
                MaximumTruePeakLevel = -70;
            }

            _truePeakModel->insert(i,
                TruePeakModel::TruePeak { MathHelper1::decibel2yPixel(AverageMeterLevel), MathHelper1::decibel2yPixel(AverageMeterPeakLevel), MathHelper1::decibel2yPixel(PeakMeterLevel),
                    MathHelper1::decibel2yPixel(PeakMeterPeakLevel), MathHelper1::decibel2yPixel(MaximumPeakLevel), MathHelper1::decibel2yPixel(MaximumTruePeakLevel) });
        }
    } else {
        auto channels = 2;
        for (int i = 0; i < channels; i++) {
            float AverageMeterLevel = ((SampleTrack*)track())->truePeakMeter()->getLevels()->getAverageMeterLevel(i);
            float AverageMeterPeakLevel = ((SampleTrack*)track())->truePeakMeter()->getLevels()->getAverageMeterPeakLevel(i);
            float PeakMeterLevel = ((SampleTrack*)track())->truePeakMeter()->getLevels()->getPeakMeterLevel(i);
            float PeakMeterPeakLevel = ((SampleTrack*)track())->truePeakMeter()->getLevels()->getPeakMeterPeakLevel(i);
            float MaximumPeakLevel = ((SampleTrack*)track())->truePeakMeter()->getLevels()->getMaximumPeakLevel(i);
            float MaximumTruePeakLevel = ((SampleTrack*)track())->truePeakMeter()->getLevels()->getMaximumTruePeakLevel(i);

            if (AverageMeterLevel < -70) {
                AverageMeterLevel = -70;
            }
            if (AverageMeterPeakLevel < -70) {
                AverageMeterPeakLevel = -70;
            }
            if (PeakMeterLevel < -70) {
                PeakMeterLevel = -70;
            }
            if (PeakMeterPeakLevel < -70) {
                PeakMeterPeakLevel = -70;
            }
            if (MaximumPeakLevel < -70) {
                MaximumPeakLevel = -70;
            }
            if (MaximumTruePeakLevel < -70) {
                MaximumTruePeakLevel = -70;
            }

            if (AverageMeterLevel > 6) {
                AverageMeterLevel = 6;
            }
            if (AverageMeterPeakLevel > 6) {
                AverageMeterPeakLevel = 6;
            }
            if (PeakMeterLevel > 6) {
                PeakMeterLevel = 6;
            }
            if (PeakMeterPeakLevel > 6) {
                PeakMeterPeakLevel = 6;
            }
            if (MaximumPeakLevel > 6) {
                MaximumPeakLevel = 6;
            }
            if (MaximumTruePeakLevel > 6) {
                MaximumTruePeakLevel = 6;
            }

            _truePeakModel->insert((channels - 1) - i,
                TruePeakModel::TruePeak { MathHelper1::decibel2yPixel(AverageMeterLevel), MathHelper1::decibel2yPixel(AverageMeterPeakLevel), MathHelper1::decibel2yPixel(PeakMeterLevel),
                    MathHelper1::decibel2yPixel(PeakMeterPeakLevel), MathHelper1::decibel2yPixel(MaximumPeakLevel), MathHelper1::decibel2yPixel(MaximumTruePeakLevel) });
        }
    }

    Q_EMIT truePeakModelChanged();
}

QString TrackItem::currentClipItem() const
{
    return _currentClipItem;
}

void TrackItem::setCurrentClipItem(const QString& currentClipItem)
{
    _currentClipItem = currentClipItem;
}

int TrackItem::channelCount() const
{
    return 2;
}

void TrackItem::playBackUpdate()
{
    auto channels = 2;
    std::shared_ptr<MeterBallistics> levels;
    if (AudioManager::mixer() == Q_NULLPTR) {
        return;
    } else {
        levels = std::make_shared<MeterBallistics>(*AudioManager::mixer()->truePeakMeter()->getLevels());
    }

    if (AudioManager::getSong()->isPause() == true) {
        for (int i = 0; i < channels; i++) {
            float AverageMeterLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).AverageMeterLevel);
            float AverageMeterPeakLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).AverageMeterPeakLevel);
            float PeakMeterLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).PeakMeterLevel);
            float PeakMeterPeakLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).PeakMeterPeakLevel);
            float MaximumPeakLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).MaximumPeakLevel);
            float MaximumTruePeakLevel = MathHelper1::yPixel2Decibel(_truePeakModel->get(i).MaximumTruePeakLevel);

            AverageMeterLevel -= 1;
            AverageMeterPeakLevel -= 1;
            PeakMeterLevel -= 1;
            PeakMeterPeakLevel -= 1;
            MaximumPeakLevel -= 1;
            MaximumTruePeakLevel -= 1;

            if (AverageMeterLevel < -70) {
                AverageMeterLevel = -70;
            }
            if (AverageMeterPeakLevel < -70) {
                AverageMeterPeakLevel = -70;
            }
            if (PeakMeterLevel < -70) {
                PeakMeterLevel = -70;
            }
            if (PeakMeterPeakLevel < -70) {
                PeakMeterPeakLevel = -70;
            }
            if (MaximumPeakLevel < -70) {
                MaximumPeakLevel = -70;
            }
            if (MaximumTruePeakLevel < -70) {
                MaximumTruePeakLevel = -70;
            }

            _truePeakModel->insert(i,
                TruePeakModel::TruePeak { MathHelper1::decibel2yPixel(AverageMeterLevel), MathHelper1::decibel2yPixel(AverageMeterPeakLevel), MathHelper1::decibel2yPixel(PeakMeterLevel),
                    MathHelper1::decibel2yPixel(PeakMeterPeakLevel), MathHelper1::decibel2yPixel(MaximumPeakLevel), MathHelper1::decibel2yPixel(MaximumTruePeakLevel) });
        }
    } else {
        for (int i = 0; i < channels; i++) {
            float AverageMeterLevel = levels->getAverageMeterLevel(i);
            float AverageMeterPeakLevel = levels->getAverageMeterPeakLevel(i);
            float PeakMeterLevel = levels->getPeakMeterLevel(i);
            float PeakMeterPeakLevel = levels->getPeakMeterPeakLevel(i);
            float MaximumPeakLevel = levels->getMaximumPeakLevel(i);
            float MaximumTruePeakLevel = levels->getMaximumTruePeakLevel(i);

            if (AverageMeterLevel < -70) {
                AverageMeterLevel = -70;
            }
            if (AverageMeterPeakLevel < -70) {
                AverageMeterPeakLevel = -70;
            }
            if (PeakMeterLevel < -70) {
                PeakMeterLevel = -70;
            }
            if (PeakMeterPeakLevel < -70) {
                PeakMeterPeakLevel = -70;
            }
            if (MaximumPeakLevel < -70) {
                MaximumPeakLevel = -70;
            }
            if (MaximumTruePeakLevel < -70) {
                MaximumTruePeakLevel = -70;
            }

            if (AverageMeterLevel > 6) {
                AverageMeterLevel = 6;
            }
            if (AverageMeterPeakLevel > 6) {
                AverageMeterPeakLevel = 6;
            }
            if (PeakMeterLevel > 6) {
                PeakMeterLevel = 6;
            }
            if (PeakMeterPeakLevel > 6) {
                PeakMeterPeakLevel = 6;
            }
            if (MaximumPeakLevel > 6) {
                MaximumPeakLevel = 6;
            }
            if (MaximumTruePeakLevel > 6) {
                MaximumTruePeakLevel = 6;
            }

            _truePeakModel->insert((channels - 1) - i,
                TruePeakModel::TruePeak { MathHelper1::decibel2yPixel(AverageMeterLevel), MathHelper1::decibel2yPixel(AverageMeterPeakLevel), MathHelper1::decibel2yPixel(PeakMeterLevel),
                    MathHelper1::decibel2yPixel(PeakMeterPeakLevel), MathHelper1::decibel2yPixel(MaximumPeakLevel), MathHelper1::decibel2yPixel(MaximumTruePeakLevel) });
        }
    }
    Q_EMIT truePeakModelChanged();
}

void TrackItem::sltRowAdded(int index, int count, QString menu, QString subMenu)
{

    auto top = rect().top() + _rowsExpandedCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight();

    auto H = _automationLaneModel->totalRowCountBefore2(index) * _areaInfo->titleHeight() + index * _areaInfo->seperatorHeight();

    auto tempRect = rect();
    tempRect.setTop(top + H);
    tempRect.setHeight(_automationLaneModel->getItem(index)->automationLaneRowCount * _areaInfo->titleHeight());

    if (subMenu == "Volume") {

        _volumeAutomationItem->automation()->setSubTrackIndex(index);
        _volumeAutomationItem->setRect(tempRect);
        _volumeAutomationItem->automation()->update(tempRect);
        Q_EMIT _volumeAutomationItem->sigUpdate();
    } else if (subMenu == "Pan") {

        _panAutomationItem->automation()->setSubTrackIndex(index);
        _panAutomationItem->setRect(tempRect);
        _panAutomationItem->automation()->update(tempRect);

        Q_EMIT _panAutomationItem->sigUpdate();
    } else if (subMenu == "SpeakerOn") {

        _speakerOnAutomationItem->automation()->setSubTrackIndex(index);
        _speakerOnAutomationItem->setRect(tempRect);
        _speakerOnAutomationItem->automation()->update(tempRect);

        Q_EMIT _speakerOnAutomationItem->sigUpdate();

    } else if (subMenu == "KeyFrames") {

        _keyFramesAutomationItem->automation()->setSubTrackIndex(index);
        _keyFramesAutomationItem->setRect(tempRect);
        _keyFramesAutomationItem->automation()->update(tempRect);

        Q_EMIT _keyFramesAutomationItem->sigUpdate();

    } else {

        if (automationItem(menu, subMenu)) {
            automationItem(menu, subMenu)->automation()->setSubTrackIndex(index);
            automationItem(menu, subMenu)->setRect(tempRect);
            automationItem(menu, subMenu)->automation()->update(tempRect);

            Q_EMIT automationItem(menu, subMenu)->sigUpdate();
        }
    }

    auto rect_ = getTotalRect();
    if (_rowsExpanded && automationLaneEnabled()) {
        rect_.setHeight(_rowsExpandedCount * _areaInfo->titleHeight() + automationLaneModel()->totalHeight() + _areaInfo->seperatorHeight());
    } else {

        rect_.setHeight(_rowsExpandedCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight());
    }
    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();
        i.value()->setRect(rect_);
    }

    Q_EMIT sigAutomationLaneCountChanged(_trackIndex);
    Q_EMIT rowsExpandedChanged(_trackIndex);
}

void TrackItem::sltRowRemoved(int count, QString menu, QString subMenu)
{
    QRect tempRect = rect();
    tempRect.setTop(rect().top() + _areaInfo->titleHeight());
    tempRect.setHeight(_areaInfo->titleHeight() * (_rowsExpandedCount - 1));

    if (subMenu == "Volume") {

        _volumeAutomationItem->automation()->setSubTrackIndex(-1);
        _volumeAutomationItem->setRect(tempRect);
        _volumeAutomationItem->automation()->update(tempRect);
        _volumeAutomationItem->automation()->sigUpdate();

    } else if (subMenu == "Pan") {

        _panAutomationItem->setRect(tempRect);
        _panAutomationItem->automation()->update(tempRect);
        _panAutomationItem->automation()->setSubTrackIndex(-1);
        _panAutomationItem->automation()->sigUpdate();

    } else if (subMenu == "SpeakerOn") {

        _speakerOnAutomationItem->automation()->setSubTrackIndex(-1);
        _speakerOnAutomationItem->setRect(tempRect);
        _speakerOnAutomationItem->automation()->update(tempRect);
        _speakerOnAutomationItem->automation()->sigUpdate();

    } else if (subMenu == "KeyFrames") {

        _keyFramesAutomationItem->automation()->setSubTrackIndex(-1);
        _keyFramesAutomationItem->setRect(tempRect);
        _keyFramesAutomationItem->automation()->update(tempRect);

        _keyFramesAutomationItem->automation()->sigUpdate();

    } else {

        if (automationItem(menu, subMenu)) {

            automationItem(menu, subMenu)->automation()->setSubTrackIndex(-1);

            automationItem(menu, subMenu)->setRect(tempRect);
            automationItem(menu, subMenu)->automation()->update(tempRect);

            Q_EMIT automationItem(menu, subMenu)->sigUpdate();
        }
    }

    Q_EMIT sigAutomationLaneCountChanged(_trackIndex);
    Q_EMIT rowsExpandedChanged(_trackIndex);
    // setRowsExpandedCount(rowsExpandedCount() - count);
}
void TrackItem::sltRowChanged()
{
    Q_EMIT sigAutomationLaneCountChanged(_trackIndex);
    Q_EMIT rowsExpandedChanged(_trackIndex);
    // setRowsExpandedCount(rowsExpandedCount() - count);
}

void TrackItem::setMasterTrackVolume(float value)
{
    if (AudioManager::mixer() == Q_NULLPTR) {
        return;
    }

    AudioManager::mixer()->setMasterTrackVolume(value);
    Q_EMIT masterTrackVolumeChanged();
}

float TrackItem::masterTrackVolume() const
{
    if (AudioManager::mixer() == Q_NULLPTR) {
        return 0;
    }

    return AudioManager::mixer()->masterTrackVolume();
}

QString TrackItem::imageUrl() const
{
    return _imageUrl;
}

void TrackItem::setImageUrl(const QString& imageUrl)
{
    _imageUrl = imageUrl;
}

void TrackItem::setRowsExpanded(bool rowsExpanded)
{

    if (_rowsExpanded != rowsExpanded) {
        _rowsExpanded = rowsExpanded;

        Q_EMIT rowsExpandedChanged(_trackIndex);
    }
}

void TrackItem::updateRect()
{

    if (_rowsExpanded == true) {

        if (rowsExpandedCount() == 1) {
            // Back to Defaut

            setRowsExpandedCount(_areaInfo->minTitlebarCount());
        } else {
            // Back to Last Height

            setRowsExpandedCount(getLastRowExpanded());
        }
        Q_EMIT rowsExpandedChanged(0);

    } else {

        setLastRowExpanded(rowsExpandedCount());
        setRowsExpandedCount(1);
        Q_EMIT rowsExpandedChanged(0);
    }
}

bool TrackItem::rowsExpanded() const
{
    return _rowsExpanded;
}

int TrackItem::inDraggingCounter() const
{
    return _inDraggingCounter;
}

bool TrackItem::editName() const
{
    return _editName;
}

bool TrackItem::coverBound() const
{
    return _coverBound;
}

void TrackItem::setCoverBound(bool coverBound)
{
    _coverBound = coverBound;
}

bool TrackItem::selectTrackDependSolo() const
{
    return _selectTrackDependSolo;
}

bool TrackItem::isSolo() const
{
    return _track->isSolo();
}

bool TrackItem::isMuted() const
{
    return _track->isMuted();
}

bool TrackItem::partialySolo() const
{
    return _partialySolo;
}

bool TrackItem::selectSolo() const
{
    return _selectSolo;
}

bool TrackItem::isCollapsed() const
{
    return _isCollapsed;
}

QMap<QString, QSharedPointer<ClipItem>> TrackItem::getClipItems() const
{
    return _clipItems;
}

QSharedPointer<AutomationItem> TrackItem::volumeAutomationItem() const
{
    return _volumeAutomationItem;
}

QSharedPointer<AutomationItem> TrackItem::panAutomationItem() const
{
    return _panAutomationItem;
}

QSharedPointer<AutomationItem> TrackItem::speakerOnAutomationItem() const
{
    return _speakerOnAutomationItem;
}

QString TrackItem::hoverClipIndex() const
{
    return _hoverClipIndex;
}

QString TrackItem::selectedClipIndex() const
{
    return _selectedClipIndex;
}

TrackItem::IndicatorType TrackItem::indicatorEnabled() const
{
    return _indicatorEnabled;
}

void TrackItem::setIndicatorEnable(TrackItem::IndicatorType indicatorEnabled)
{
    _indicatorEnabled = indicatorEnabled;
}

void TrackItem::setIndicator(double currentIndicator)
{
    _currentIndicator = currentIndicator;

    auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
    auto effects = effectChain->getEffect(_automationMenuTitle1);

    _volumeAutomationItem->setCurrentIndicator(currentIndicator);
    _panAutomationItem->setCurrentIndicator(currentIndicator);
    _speakerOnAutomationItem->setCurrentIndicator(currentIndicator);
    _keyFramesAutomationItem->setCurrentIndicator(currentIndicator);

    if (_automationSubMenuTitle == "Volume") {

        if (_volumeAutomationItem->automation()->isAutomated())
            _volumeAutomationItem->setAutomatedNormalized(_volumeAutomationItem->automation()->getY(currentIndicator));
    }

    if (_automationSubMenuTitle == "Pan") {

        if (_panAutomationItem->automation()->isAutomated())
            _panAutomationItem->setAutomatedNormalized(_panAutomationItem->automation()->getY(currentIndicator));
    }

    if (_automationSubMenuTitle == "SpeakerOn") {

        if (_speakerOnAutomationItem->automation()->isAutomated())
            _speakerOnAutomationItem->setAutomatedNormalized(_speakerOnAutomationItem->automation()->getY(currentIndicator));
    }

    if (_automationSubMenuTitle == "KeyFrames") {

        if (_keyFramesAutomationItem->automation()->isAutomated())
            _keyFramesAutomationItem->setAutomatedNormalized(_keyFramesAutomationItem->automation()->getY(currentIndicator));
    }

    if (_automationMenuTitle1 == "Visr") {
        auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
        if (effect) {
            auto automationItem = effect->automationItem(_automationSubMenuTitle);

            automationItem->setCurrentIndicator(currentIndicator);
            if (automationItem->automation()->isAutomated())
                automationItem->setAutomatedNormalized(automationItem->automation()->getY(currentIndicator));
        }
    }

    if (effects != Q_NULLPTR) {
        if (effects->contains(_automationSubMenuTitle)) {
            auto automationItem = effects->automationItem(_automationSubMenuTitle);

            automationItem->setCurrentIndicator(currentIndicator);

            automationItem->setAutomatedNormalized(automationItem->automation()->getY(currentIndicator));
        }
    }
    for (int i = 0; i < _automationLaneModel->count(); i++) {

        if (_automationLaneModel->getItem(i)->automationLaneSubMenuTitle == "Volume") {

            _volumeAutomationItem->setCurrentIndicator(currentIndicator);

            if (_volumeAutomationItem->automation()->isAutomated())
                _volumeAutomationItem->setAutomatedNormalized(_volumeAutomationItem->automation()->getY(currentIndicator));

        } else if (_automationLaneModel->getItem(i)->automationLaneSubMenuTitle == "Pan") {

            _panAutomationItem->setCurrentIndicator(currentIndicator);
            if (_panAutomationItem->automation()->isAutomated())
                _panAutomationItem->setAutomatedNormalized(_panAutomationItem->automation()->getY(currentIndicator));
        } else if (_automationLaneModel->getItem(i)->automationLaneSubMenuTitle == "SpeakerOn") {

            _speakerOnAutomationItem->setCurrentIndicator(currentIndicator);

            if (_speakerOnAutomationItem->automation()->isAutomated())
                _speakerOnAutomationItem->setAutomatedNormalized(_speakerOnAutomationItem->automation()->getY(currentIndicator));

        } else if (_automationLaneModel->getItem(i)->automationLaneSubMenuTitle == "KeyFrames") {

            _keyFramesAutomationItem->setCurrentIndicator(currentIndicator);

            if (_keyFramesAutomationItem->automation()->isAutomated())
                _keyFramesAutomationItem->setAutomatedNormalized(_keyFramesAutomationItem->automation()->getY(currentIndicator));

        } else {

            if (_automationLaneModel->getItem(i)->automationLaneMenuTitle == "Visr") {
                auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
                if (effect) {
                    auto automationItem = effect->automationItem(_automationLaneModel->getItem(i)->automationLaneSubMenuTitle);

                    automationItem->setCurrentIndicator(currentIndicator);

                    if (automationItem->automation()->isAutomated())
                        automationItem->setAutomatedNormalized(automationItem->automation()->getY(currentIndicator));
                }
            }

            auto effects = effectChain->getEffect(_automationLaneModel->getItem(i)->automationLaneMenuTitle);

            if (effects != Q_NULLPTR) {

                if (effects->contains(_automationLaneModel->getItem(i)->automationLaneSubMenuTitle)) {

                    auto automationItem = effects->automationItem(_automationLaneModel->getItem(i)->automationLaneSubMenuTitle);
                    automationItem->setCurrentIndicator(currentIndicator);
                    if (automationItem->automation()->isAutomated())
                        automationItem->setAutomatedNormalized(automationItem->automation()->getY(currentIndicator));
                }
            }
        }
    }
}

double TrackItem::currentIndicator() const
{
    return _currentIndicator;
}

Track* TrackItem::track() const
{
    return _track;
}

int TrackItem::trackIndex() const
{
    return _trackIndex;
}

int TrackItem::mousePressEvent(QMouseEvent* event)
{

    _isSelected = true;
    Q_EMIT sigSelectedChanged(_trackIndex);

    _mutex.lock();
    bool status = false;
    _selectedClipIndex = "";

    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();

        status = i.value()->mousePressEvent(event);
        if (status) {
            _selectedClipIndex = i.key();
        }
    }

    _mutex.unlock();

    double currentPix = event->pos().x();

    double final_xpos = 0;
    double final_ypos = 0;
    final_xpos = _areaInfo->findSnap(currentPix);

    int automation_event = TrackItemEvent_TrackSelect;

    auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
    auto effects = effectChain->getEffect(_automationMenuTitle1);

    if (_automationSubMenuTitle == "Volume" && automationLaneEnabled()) {

        automation_event = _volumeAutomationItem->mousePressEvent(event);

        if (automation_event == AutomationItem::AutomationItemEvent_InnerSelect) {
            return TrackItemEvent_InnerSelect; // inner and point activated
        }
        if (automation_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
            return TrackItemEvent_OuterPointSelect; // outer activated
        }
    }

    if (_automationSubMenuTitle == "Pan" && automationLaneEnabled()) {

        automation_event = _panAutomationItem->mousePressEvent(event);

        if (automation_event == AutomationItem::AutomationItemEvent_InnerSelect) {
            return TrackItemEvent_InnerSelect; // inner and point activated
        }
        if (automation_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
            return TrackItemEvent_OuterPointSelect; // outer activated
        }
    }

    if (_automationSubMenuTitle == "SpeakerOn" && automationLaneEnabled()) {

        automation_event = _speakerOnAutomationItem->mousePressEvent(event);

        if (automation_event == AutomationItem::AutomationItemEvent_InnerSelect) {
            return TrackItemEvent_InnerSelect; // inner and point activated
        }
        if (automation_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
            return TrackItemEvent_OuterPointSelect; // outer activated
        }
    }

    if (_automationSubMenuTitle == "KeyFrames" && automationLaneEnabled()) {

        automation_event = _keyFramesAutomationItem->mousePressEvent(event);

        if (automation_event == AutomationItem::AutomationItemEvent_InnerSelect) {
            return TrackItemEvent_InnerSelect; // inner and point activated
        }
        if (automation_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
            return TrackItemEvent_OuterPointSelect; // outer activated
        }
    }

    if (_automationMenuTitle1 == "Visr" && automationLaneEnabled()) {
        auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
        if (effect) {
            auto automationItem = effect->automationItem(_automationSubMenuTitle);
            automation_event = automationItem->mousePressEvent(event);

            if (automation_event == AutomationItem::AutomationItemEvent_InnerSelect) {
                return TrackItemEvent_InnerSelect; // inner and point activated
            }
            if (automation_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
                return TrackItemEvent_OuterPointSelect; // outer activated
            }
        }
    }

    if (effects != Q_NULLPTR) {
        if (effects->contains(_automationSubMenuTitle) && automationLaneEnabled()) {
            auto automationItem = effects->automationItem(_automationSubMenuTitle);
            automation_event = automationItem->mousePressEvent(event);

            if (automation_event == AutomationItem::AutomationItemEvent_InnerSelect) {
                return TrackItemEvent_InnerSelect; // inner and point activated
            }
            if (automation_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
                return TrackItemEvent_OuterPointSelect; // outer activated
            }
        }
    }

    if (_automationLaneModel->count() > 0) {

        _automationLaneModel->setCurrentSelected(-1);
        for (int i = 0; i < _automationLaneModel->count(); i++) {

            if (_automationLaneModel->getItem(i)->automationLaneSubMenuTitle == "Volume") {

                int result_event = _volumeAutomationItem->mousePressEvent(event);

                if (result_event == AutomationItem::AutomationItemEvent_InnerSelect) {
                    automation_event = TrackItemEvent_InnerSelect;
                    return TrackItemEvent_InnerSelect;
                }
                if (result_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
                    automation_event = TrackItemEvent_OuterPointSelect;
                    return TrackItemEvent_OuterPointSelect;
                }
                if (result_event == AutomationItem::AutomationItemEvent_NotSelect) {
                    continue;
                }

                if (result_event == AutomationItem::AutomationItemEvent_Select) {
                    automation_event = TrackItemEvent_AutomationLineSelect;
                    _automationLaneModel->setCurrentSelected(i);
                    continue;
                }
            } else if (_automationLaneModel->getItem(i)->automationLaneSubMenuTitle == "Pan") {

                int result_event = _panAutomationItem->mousePressEvent(event);

                if (result_event == AutomationItem::AutomationItemEvent_InnerSelect) {
                    automation_event = TrackItemEvent_InnerSelect;
                    return TrackItemEvent_InnerSelect;
                }
                if (result_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
                    automation_event = TrackItemEvent_OuterPointSelect;
                    return TrackItemEvent_OuterPointSelect;
                }
                if (result_event == AutomationItem::AutomationItemEvent_NotSelect) {
                    continue;
                }

                if (result_event == AutomationItem::AutomationItemEvent_Select) {
                    automation_event = TrackItemEvent_AutomationLineSelect;
                    _automationLaneModel->setCurrentSelected(i);
                    continue;
                }

            } else if (_automationLaneModel->getItem(i)->automationLaneSubMenuTitle == "SpeakerOn") {

                int result_event = _speakerOnAutomationItem->mousePressEvent(event);

                if (result_event == AutomationItem::AutomationItemEvent_InnerSelect) {
                    automation_event = TrackItemEvent_InnerSelect;
                    return TrackItemEvent_InnerSelect;
                }
                if (result_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
                    automation_event = TrackItemEvent_OuterPointSelect;
                    return TrackItemEvent_OuterPointSelect;
                }
                if (result_event == AutomationItem::AutomationItemEvent_NotSelect) {
                    continue;
                }

                if (result_event == AutomationItem::AutomationItemEvent_Select) {
                    automation_event = TrackItemEvent_AutomationLineSelect;
                    _automationLaneModel->setCurrentSelected(i);
                    continue;
                }

            } else if (_automationLaneModel->getItem(i)->automationLaneSubMenuTitle == "KeyFrames") {

                int result_event = _keyFramesAutomationItem->mousePressEvent(event);

                if (result_event == AutomationItem::AutomationItemEvent_InnerSelect) {
                    automation_event = TrackItemEvent_InnerSelect;
                    return TrackItemEvent_InnerSelect;
                }
                if (result_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
                    automation_event = TrackItemEvent_OuterPointSelect;
                    return TrackItemEvent_OuterPointSelect;
                }
                if (result_event == AutomationItem::AutomationItemEvent_NotSelect) {
                    continue;
                }

                if (result_event == AutomationItem::AutomationItemEvent_Select) {
                    automation_event = TrackItemEvent_AutomationLineSelect;
                    _automationLaneModel->setCurrentSelected(i);
                    continue;
                }

            } else {

                if (_automationLaneModel->getItem(i)->automationLaneMenuTitle == "Visr") {
                    auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
                    if (effect) {
                        if (effect->contains(_automationLaneModel->getItem(i)->automationLaneSubMenuTitle)) {

                            auto automationItem = effect->automationItem(_automationLaneModel->getItem(i)->automationLaneSubMenuTitle);

                            int result_event = automationItem->mousePressEvent(event);

                            if (result_event == AutomationItem::AutomationItemEvent_InnerSelect) {
                                automation_event = TrackItemEvent_InnerSelect;
                                return TrackItemEvent_InnerSelect;
                            }
                            if (result_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
                                automation_event = TrackItemEvent_OuterPointSelect;
                                return TrackItemEvent_OuterPointSelect;
                            }

                            if (result_event == AutomationItem::AutomationItemEvent_NotSelect) {
                                continue;
                            }

                            if (result_event == AutomationItem::AutomationItemEvent_Select) {
                                automation_event = TrackItemEvent_AutomationLineSelect;
                                _automationLaneModel->setCurrentSelected(i);
                                continue;
                            }
                        }
                    }
                }

                auto effects = effectChain->getEffect(_automationLaneModel->getItem(i)->automationLaneMenuTitle);

                if (effects != Q_NULLPTR) {

                    if (effects->contains(_automationLaneModel->getItem(i)->automationLaneSubMenuTitle)) {

                        auto automationItem = effects->automationItem(_automationLaneModel->getItem(i)->automationLaneSubMenuTitle);

                        int result_event = automationItem->mousePressEvent(event);

                        if (result_event == AutomationItem::AutomationItemEvent_InnerSelect) {
                            automation_event = TrackItemEvent_InnerSelect;
                            return TrackItemEvent_InnerSelect;
                        }
                        if (result_event == AutomationItem::AutomationItemEvent_OuterPointSelect) {
                            automation_event = TrackItemEvent_OuterPointSelect;
                            return TrackItemEvent_OuterPointSelect;
                        }

                        if (result_event == AutomationItem::AutomationItemEvent_NotSelect) {
                            continue;
                        }

                        if (result_event == AutomationItem::AutomationItemEvent_Select) {
                            automation_event = TrackItemEvent_AutomationLineSelect;
                            _automationLaneModel->setCurrentSelected(i);
                            continue;
                        }
                    }
                }
            }
        }
    }

    if (_selectedClipIndex != "") {
        return TrackItemEvent_ClipSelect;
    }

    return automation_event;
}

int TrackItem::hoverMoveEvent(QHoverEvent* event)
{

    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    bool status = 0;
    _hoverClipIndex = "";
    while (i.hasNext()) {
        i.next();

        if (i.value()->hoverMoveEvent(event) == true) {
            status = 1;
            _hoverClipIndex = i.key();
        }
    }

    _areaInfo->setCurrentHoveTrackIndex(-1);
    if (_hoverClipIndex == "") {
        if (_volumeAutomationItem && _volumeAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "Volume") {

            status = _volumeAutomationItem->hoverMoveEvent(event);
        }
        if (_panAutomationItem && _panAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "Pan") {
            status = _panAutomationItem->hoverMoveEvent(event);
        }
        if (_speakerOnAutomationItem && _speakerOnAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "SpeakerOn") {
            status = _speakerOnAutomationItem->hoverMoveEvent(event);
        }
        if (_keyFramesAutomationItem && _keyFramesAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "KeyFrames") {
            status = _keyFramesAutomationItem->hoverMoveEvent(event);
        }

        if (_automationMenuTitle1 == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->contains(_automationSubMenuTitle)) {
                    auto automationItem = effect->automationItem(_automationSubMenuTitle);
                    status = automationItem->hoverMoveEvent(event);
                }
            }
        }

        auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
        auto effects = effectChain->getEffect(_automationMenuTitle1);
        if (effects) {
            if (effects->contains(_automationSubMenuTitle)) {
                auto automationItem = effects->automationItem(_automationSubMenuTitle);
                status = automationItem->hoverMoveEvent(event);
            }
        }
        if (_automationLaneModel->count() > 0 && automationLaneEnabled()) {

            bool status = false;
            for (int i = 0; i < _automationLaneModel->count(); i++) {
                auto item = _automationLaneModel->getItem(i);

                if (item->automationLaneSubMenuTitle == "Volume") {
                    if (_volumeAutomationItem->hoverMoveEvent(event))
                        status = true;
                } else if (item->automationLaneSubMenuTitle == "Pan") {
                    if (_panAutomationItem->hoverMoveEvent(event))
                        status = true;
                } else if (item->automationLaneSubMenuTitle == "SpeakerOn") {
                    if (_speakerOnAutomationItem->hoverMoveEvent(event))
                        status = true;
                } else if (item->automationLaneSubMenuTitle == "KeyFrames") {
                    if (_keyFramesAutomationItem->hoverMoveEvent(event))
                        status = true;
                } else {

                    if (item->automationLaneMenuTitle == "Visr") {
                        auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
                        if (effect) {
                            if (effect->contains(item->automationLaneSubMenuTitle)) {
                                auto automationItem = effect->automationItem(item->automationLaneSubMenuTitle);
                                if (automationItem->hoverMoveEvent(event))
                                    status = true;
                            }
                        }
                    }

                    auto effects = effectChain->getEffect(item->automationLaneMenuTitle);
                    if (effects) {

                        if (effects->contains(item->automationLaneSubMenuTitle)) {
                            auto automationItem = effects->automationItem(item->automationLaneSubMenuTitle);
                            if (automationItem->hoverMoveEvent(event))
                                status = true;
                        }
                    }
                }
            }
        }
    }

    Q_EMIT sigUpdate();
    return status;
}

int TrackItem::mouseReleaseEvent(QMouseEvent* event)
{

    if (_volumeAutomationItem && _volumeAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "Volume") {
        return _volumeAutomationItem->mouseReleaseEvent(event);
    }

    if (_panAutomationItem && _panAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "Pan") {
        return _panAutomationItem->mouseReleaseEvent(event);
    }

    if (_speakerOnAutomationItem && _speakerOnAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "SpeakerOn") {
        return _speakerOnAutomationItem->mouseReleaseEvent(event);
    }

    if (_keyFramesAutomationItem && _keyFramesAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "KeyFrames") {
        return _keyFramesAutomationItem->mouseReleaseEvent(event);
    }

    if (_automationMenuTitle1 == "Visr") {
        auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
        if (effect) {
            if (effect->contains(_automationSubMenuTitle)) {
                auto automationItem = effect->automationItem(_automationSubMenuTitle);
                if (automationItem->showEnvelopes() && automationLaneEnabled()) {
                    return automationItem->mouseReleaseEvent(event);
                }
            }
        }
    }

    auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
    auto effects = effectChain->getEffect(_automationMenuTitle1);
    if (effects) {
        if (effects->contains(_automationSubMenuTitle)) {
            auto automationItem = effects->automationItem(_automationSubMenuTitle);
            if (automationItem->showEnvelopes() && automationLaneEnabled()) {
                return automationItem->mouseReleaseEvent(event);
            }
        }
    }

    {
        auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();

        bool status = false;
        for (int i = 0; i < _automationLaneModel->count(); i++) {
            auto item = _automationLaneModel->getItem(i);

            if (item->automationLaneSubMenuTitle == "Volume") {
                if (_volumeAutomationItem->mouseReleaseEvent(event))
                    status = true;
            } else if (item->automationLaneSubMenuTitle == "Pan") {
                if (_panAutomationItem->mouseReleaseEvent(event))
                    status = true;
            } else if (item->automationLaneSubMenuTitle == "SpeakerOn") {
                if (_speakerOnAutomationItem->mouseReleaseEvent(event))
                    status = true;
            } else if (item->automationLaneSubMenuTitle == "KeyFrames") {
                if (_keyFramesAutomationItem->mouseReleaseEvent(event))
                    status = true;
            } else {

                if (item->automationLaneMenuTitle == "Visr") {
                    auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
                    if (effect) {

                        if (effect->contains(item->automationLaneSubMenuTitle)) {
                            auto automationItem = effect->automationItem(item->automationLaneSubMenuTitle);
                            if (automationItem->showEnvelopes() && automationLaneEnabled()) {
                                if (automationItem->mouseReleaseEvent(event))
                                    status = true;
                            }
                        }
                    }
                }

                auto effects = effectChain->getEffect(item->automationLaneMenuTitle);
                if (effects) {

                    if (effects->contains(item->automationLaneSubMenuTitle)) {
                        auto automationItem = effects->automationItem(item->automationLaneSubMenuTitle);
                        if (automationItem->showEnvelopes() && automationLaneEnabled()) {
                            if (automationItem->mouseReleaseEvent(event))
                                status = true;
                        }
                    }
                }
            }
        }
        return status;
    }

    return 0;
}

int TrackItem::mouseMoveEvent(QMouseEvent* event)
{

    if (_volumeAutomationItem && _volumeAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "Volume") {
        return _volumeAutomationItem->mouseMoveEvent(event);
    }
    if (_panAutomationItem && _panAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "Pan") {
        return _panAutomationItem->mouseMoveEvent(event);
    }
    if (_speakerOnAutomationItem && _speakerOnAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "SpeakerOn") {
        return _speakerOnAutomationItem->mouseMoveEvent(event);
    }
    if (_keyFramesAutomationItem && _keyFramesAutomationItem->showEnvelopes() && automationLaneEnabled() && _automationSubMenuTitle == "KeyFrames") {
        return _keyFramesAutomationItem->mouseMoveEvent(event);
    }

    if (_automationMenuTitle1 == "Visr") {
        auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
        if (effect) {

            if (effect->contains(_automationSubMenuTitle)) {
                auto automationItem = effect->automationItem(_automationSubMenuTitle);
                if (automationItem->showEnvelopes() && automationLaneEnabled()) {
                    return automationItem->mouseMoveEvent(event);
                }
            }
        }
    }

    auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
    auto effects = effectChain->getEffect(_automationMenuTitle1);
    if (effects) {
        if (effects->contains(_automationSubMenuTitle)) {
            auto automationItem = effects->automationItem(_automationSubMenuTitle);
            if (automationItem->showEnvelopes() && automationLaneEnabled()) {
                return automationItem->mouseMoveEvent(event);
            }
        }
    }

    {

        bool status = false;
        auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();

        for (int i = 0; i < _automationLaneModel->count(); i++) {
            auto item = _automationLaneModel->getItem(i);

            if (item->automationLaneSubMenuTitle == "Volume" && _volumeAutomationItem->showEnvelopes() && automationLaneEnabled()) {
                if (_volumeAutomationItem->mouseMoveEvent(event))
                    status = true;
            } else if (item->automationLaneSubMenuTitle == "Pan" && _panAutomationItem->showEnvelopes() && automationLaneEnabled()) {
                if (_panAutomationItem->mouseMoveEvent(event))
                    status = true;
            } else if (item->automationLaneSubMenuTitle == "SpeakerOn" && _speakerOnAutomationItem->showEnvelopes() && automationLaneEnabled()) {
                if (_speakerOnAutomationItem->mouseMoveEvent(event))
                    status = true;
            } else if (item->automationLaneSubMenuTitle == "KeyFrames" && _keyFramesAutomationItem->showEnvelopes() && automationLaneEnabled()) {
                if (_keyFramesAutomationItem->mouseMoveEvent(event))
                    status = true;
            } else {

                if (item->automationLaneMenuTitle == "Visr") {
                    auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
                    if (effect) {

                        auto automationItem = effect->automationItem(item->automationLaneSubMenuTitle);
                        if (automationItem->showEnvelopes() && automationLaneEnabled()) {
                            if (automationItem->mouseMoveEvent(event)) {
                                status = true;
                            }
                        }
                    }
                }

                auto effects = effectChain->getEffect(item->automationLaneMenuTitle);
                if (effects) {
                    if (effects->contains(item->automationLaneSubMenuTitle)) {
                        auto automationItem = effects->automationItem(item->automationLaneSubMenuTitle);
                        if (automationItem->showEnvelopes() && automationLaneEnabled()) {
                            if (automationItem->mouseMoveEvent(event)) {
                                status = true;
                            }
                        }
                    }
                }
            }
        }
        return status;
    }

    return 0;
}

int TrackItem::keyPressEvent(int modifier, int key)
{

    if (_rowsExpandedCount > 2) {
        if (_automationLaneModel->count() == 0) {
            if (_volumeAutomationItem && _volumeAutomationItem->showEnvelopes() && _automationSubMenuTitle == "Volume") {
                return _volumeAutomationItem->keyPressEvent(modifier, key);
            }
            if (_panAutomationItem && _panAutomationItem->showEnvelopes() && _automationSubMenuTitle == "Pan") {
                return _panAutomationItem->keyPressEvent(modifier, key);
            }
            if (_speakerOnAutomationItem && _speakerOnAutomationItem->showEnvelopes() && _automationSubMenuTitle == "SpeakerOn") {
                return _speakerOnAutomationItem->keyPressEvent(modifier, key);
            }
            if (_keyFramesAutomationItem && _keyFramesAutomationItem->showEnvelopes() && _automationSubMenuTitle == "KeyFrames") {
                return _keyFramesAutomationItem->keyPressEvent(modifier, key);
            }

        } else {

            bool status1 = _volumeAutomationItem->keyPressEvent(modifier, key);
            bool status2 = _panAutomationItem->keyPressEvent(modifier, key);
            bool status3 = _speakerOnAutomationItem->keyPressEvent(modifier, key);
            bool status4 = _keyFramesAutomationItem->keyPressEvent(modifier, key);

            return status1 || status2 || status3 || status4;
        }
    }
    return 0;
}

int TrackItem::keyReleaseEvent(int modifier, int key)
{

    if (_automationLaneModel->count() == 0) {
        if (_volumeAutomationItem && _volumeAutomationItem->showEnvelopes() && _automationMenuTitle1 == "Volume") {
            return _volumeAutomationItem->keyReleaseEvent(modifier, key);
        }
        if (_panAutomationItem && _panAutomationItem->showEnvelopes() && _automationMenuTitle1 == "Pan") {
            return _panAutomationItem->keyReleaseEvent(modifier, key);
        }
        if (_speakerOnAutomationItem && _speakerOnAutomationItem->showEnvelopes() && _automationMenuTitle1 == "SpeakerOn") {
            return _speakerOnAutomationItem->keyReleaseEvent(modifier, key);
        }
        if (_keyFramesAutomationItem && _keyFramesAutomationItem->showEnvelopes() && _automationMenuTitle1 == "KeyFrames") {
            return _keyFramesAutomationItem->keyReleaseEvent(modifier, key);
        }

    } else {
        bool status1 = _volumeAutomationItem->keyReleaseEvent(modifier, key);
        bool status2 = _panAutomationItem->keyReleaseEvent(modifier, key);
        bool status3 = _speakerOnAutomationItem->keyPressEvent(modifier, key);
        bool status4 = _keyFramesAutomationItem->keyPressEvent(modifier, key);

        return status1 || status2 || status3 || status4;
    }
    return 0;
}

// Automation* TrackItem::automation() const
//{

//    if (_automationSubMenuTitle == "Volume")
//        return _volumeAutomationItem->automation();
//    else if (_automationSubMenuTitle == "Pan")
//        return _panAutomationItem->automation();
//    else if (_automationSubMenuTitle == "Visr")
//        return _visrAutomationItem->automation();
//    else {
//        auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
//        auto effects = effectChain->getEffect(_automationMenuTitle1);
//        if (effects) {
//            if (effects->contains(_automationSubMenuTitle)) {
//                return effects->automationItem(_automationSubMenuTitle)->automation();
//            }
//        }
//    }
//    return Q_NULLPTR;
//}

QSharedPointer<AutomationItem> TrackItem::automationItem() const
{

    if (_automationSubMenuTitle == "Volume")
        return _volumeAutomationItem;
    else if (_automationSubMenuTitle == "Pan")
        return _panAutomationItem;
    else if (_automationSubMenuTitle == "SpeakerOn")
        return _speakerOnAutomationItem;
    else if (_automationSubMenuTitle == "KeyFrames")
        return _keyFramesAutomationItem;
    else {

        if (_automationMenuTitle1 == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {

                if (effect->contains(_automationSubMenuTitle)) {
                    return effect->automationItem(_automationSubMenuTitle);
                }
            }
        }

        auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
        auto effects = effectChain->getEffect(_automationMenuTitle1);

        if (effects) {
            if (effects->contains(_automationSubMenuTitle)) {
                return effects->automationItem(_automationSubMenuTitle);
            }
        }
    }
    return Q_NULLPTR;
}

QSharedPointer<AutomationItem> TrackItem::automationItem(QString menu, QString submenu) const
{
    if (submenu == "Volume") {
        return _volumeAutomationItem;
    } else if (submenu == "Pan") {
        return _panAutomationItem;
    } else if (submenu == "SpeakerOn") {
        return _speakerOnAutomationItem;
    } else if (submenu == "KeyFrames") {
        return _keyFramesAutomationItem;
    } else {

        if (menu == "Visr") {
            auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
            if (effect) {
                if (effect->contains(submenu)) {
                    return effect->automationItem(submenu);
                }
            }
        }

        auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
        auto effects = effectChain->getEffect(menu);
        if (effects) {
            if (effects->contains(submenu)) {
                return effects->automationItem(submenu);
            }
        }
    }
    return Q_NULLPTR;
}

// AutomationItem* TrackItem::automation(QString menu, QString submenu) const
//{
//     if (submenu == "Volume")
//         return _volumeAutomationItem;
//     else if (submenu == "Pan")
//         return _panAutomationItem;
//     else if (submenu == "Visr") {
//         auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
//         if (effect) {
//             if (effect->contains(submenu)) {
//                 return effect->automationItem(submenu)->automation();
//             }
//         }

//    } else {
//        auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
//        auto effects = effectChain->getEffect(menu);
//        if (effects) {
//            if (effects->contains(submenu)) {
//                return effects->automationItem(submenu);
//            }
//        }
//    }
//    return Q_NULLPTR;
//}

bool TrackItem::createCopyList(SelectedArea selectedArea)
{
    if (_automationLaneModel->count() == 0) {
        if (automationItem())
            return automationItem()->createCopyList(selectedArea);
    } else {

        bool status1 = _volumeAutomationItem->createCopyList(selectedArea);
        bool status2 = _panAutomationItem->createCopyList(selectedArea);
        bool status3 = _speakerOnAutomationItem->createCopyList(selectedArea);
        bool status4 = _keyFramesAutomationItem->createCopyList(selectedArea);

        return status1 || status2 || status3 || status4;
    }
}

int TrackItem::insertCopiedList()
{

    if (_automationLaneModel->count() == 0) {
        if (automationItem()->insertCopiedList()) {
            return -1;
        }
        return -2;
    } else {

        int index = -2;
        if (_volumeAutomationItem->insertCopiedList()) {
            if (_volumeAutomationItem->automation()->getSubTrackIndex() > index) {
                index = _volumeAutomationItem->automation()->getSubTrackIndex();
            }
        }
        if (_panAutomationItem->insertCopiedList()) {
            if (_panAutomationItem->automation()->getSubTrackIndex() > index) {
                index = _panAutomationItem->automation()->getSubTrackIndex();
            }
        }

        if (_speakerOnAutomationItem->insertCopiedList()) {
            if (_speakerOnAutomationItem->automation()->getSubTrackIndex() > index) {
                index = _speakerOnAutomationItem->automation()->getSubTrackIndex();
            }
        }

        if (_keyFramesAutomationItem->insertCopiedList()) {
            if (_keyFramesAutomationItem->automation()->getSubTrackIndex() > index) {
                index = _keyFramesAutomationItem->automation()->getSubTrackIndex();
            }
        }

        return index;
    }
}

void TrackItem::setCopyList(const QJsonArray copyList, int subTrackIndex)
{
    if (_automationLaneModel->count() == 0) {
        if (automationItem())
            automationItem()->automation()->setCopyList(copyList);
    } else {
        if (subTrackIndex < 0) {
            if (automationItem())
                automationItem()->automation()->setCopyList(copyList);
        } else {

            setCopyList2(copyList, _automationLaneModel->getItem(subTrackIndex)->automationLaneMenuTitle,
                _automationLaneModel->getItem(subTrackIndex)->automationLaneSubMenuTitle);
        }
    }
}

void TrackItem::setCopyList2(const QJsonArray copyList, QString menu, QString subMenu)
{

    if (automationItem(menu, subMenu))
        automationItem(menu, subMenu)->automation()->setCopyList(copyList);
}

bool TrackItem::cutCopiedList(SelectedArea selectedArea)
{
    if (_automationLaneModel->count() == 0) {
        if (automationItem())
            return automationItem()->cutCopiedList(selectedArea);
    } else {

        bool status1 = _volumeAutomationItem->cutCopiedList(selectedArea);
        bool status2 = _panAutomationItem->cutCopiedList(selectedArea);
        bool status3 = _speakerOnAutomationItem->cutCopiedList(selectedArea);
        bool status4 = _keyFramesAutomationItem->cutCopiedList(selectedArea);

        return status1 || status2 || status3 || status4;
    }
    return false;
}

void TrackItem::clearCopyList()
{

    clearAutomationItems();
}

void TrackItem::drawTitleLine(IPainter* painter)
{
    auto offset = 0;

    painter->beginPath();
    painter->setFillStyle("#000000");
    painter->setStrokeStyle("#000000");
    painter->drawLine(rect().x(), rect().y() + _areaInfo->titleHeight() + offset, rect().x() + rect().width(), rect().y() + _areaInfo->titleHeight() + offset);
    painter->stroke();
}

void TrackItem::draw(IPainter* painter)
{
    int offset = 0;
    int T = rect().top();
    int B = rect().bottom();
    auto lastWidth = _areaInfo->lastWidth();
    if (lastWidth <= 0) {
        return;
    }

    if (_rowsExpandedCount > 2) {

        QRect tempRect = _rect;
        tempRect.setTop(offset + rect().top() + _areaInfo->titleHeight());
        tempRect.setHeight(_areaInfo->titleHeight() * (_rowsExpandedCount - 1));

        if (_automationSubMenuTitle == "Volume" && _volumeAutomationItem->showEnvelopes() && automationLaneEnabled()) {

            _volumeAutomationItem->setRect(tempRect);
            _volumeAutomationItem->automation()->update(tempRect);
            _volumeAutomationItem->draw(painter);
            _volumeAutomationItem->setCurrentIndicator(_currentIndicator);
        } else if (_automationSubMenuTitle == "Pan" && _panAutomationItem->showEnvelopes() && automationLaneEnabled()) {

            _panAutomationItem->setRect(tempRect);
            _panAutomationItem->automation()->update(tempRect);
            _panAutomationItem->draw(painter);
            _panAutomationItem->setCurrentIndicator(_currentIndicator);
        } else if (_automationSubMenuTitle == "SpeakerOn" && _speakerOnAutomationItem->showEnvelopes() && automationLaneEnabled()) {

            _speakerOnAutomationItem->setRect(tempRect);
            _speakerOnAutomationItem->automation()->update(tempRect);
            _speakerOnAutomationItem->draw(painter);
            _speakerOnAutomationItem->setCurrentIndicator(_currentIndicator);
        } else if (_automationSubMenuTitle == "KeyFrames" && _keyFramesAutomationItem->showEnvelopes() && automationLaneEnabled()) {

            _keyFramesAutomationItem->setRect(tempRect);
            _keyFramesAutomationItem->automation()->update(tempRect);
            _keyFramesAutomationItem->draw(painter);
            _keyFramesAutomationItem->setCurrentIndicator(_currentIndicator);
        } else {
            if (_automationMenuTitle1 == "Visr") {
                auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
                if (effect) {

                    if (effect->contains(_automationSubMenuTitle) && automationLaneEnabled()) {
                        auto automationItem = effect->automationItem(_automationSubMenuTitle);

                        automationItem->setCurrentIndicator(_currentIndicator);

                        if (automationItem->showEnvelopes()) {
                            automationItem->setRect(tempRect);
                            automationItem->automation()->update(tempRect);
                            automationItem->draw(painter);
                        }
                    }
                }
            }

            auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();
            auto effects = effectChain->getEffect(_automationMenuTitle1);

            if (effects) {

                if (effects->contains(_automationSubMenuTitle) && automationLaneEnabled()) {
                    auto automationItem = effects->automationItem(_automationSubMenuTitle);

                    automationItem->setCurrentIndicator(_currentIndicator);

                    if (automationItem->showEnvelopes()) {
                        automationItem->setRect(tempRect);
                        automationItem->automation()->update(tempRect);
                        automationItem->draw(painter);
                    }
                }
            }
        }
    }

    if (_automationLaneModel->count() > 0 && _rowsExpandedCount > 2) {
        auto top = rect().top() + _rowsExpandedCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight();
        for (int i = 0; i < _automationLaneModel->count(); i++) {
            auto item = _automationLaneModel->getItem(i);

            auto H = _automationLaneModel->totalRowCountBefore2(i) * _areaInfo->titleHeight() + i * _areaInfo->seperatorHeight();

            auto tempRect = _rect;
            tempRect.setTop(offset + top + H);
            tempRect.setHeight(_automationLaneModel->getItem(i)->automationLaneRowCount * _areaInfo->titleHeight());

            _panAutomationItem->setCurrentIndicator(_currentIndicator);
            _volumeAutomationItem->setCurrentIndicator(_currentIndicator);

            if (item->automationLaneSubMenuTitle == "Pan" && _panAutomationItem->showEnvelopes() && automationLaneEnabled()) {

                _panAutomationItem->setRect(tempRect);
                _panAutomationItem->automation()->update(tempRect);
                _panAutomationItem->draw(painter);
            } else if (item->automationLaneSubMenuTitle == "Volume" && _volumeAutomationItem->showEnvelopes() && automationLaneEnabled()) {

                _volumeAutomationItem->setRect(tempRect);
                _volumeAutomationItem->automation()->update(tempRect);
                _volumeAutomationItem->draw(painter);
            } else if (item->automationLaneSubMenuTitle == "SpeakerOn" && _speakerOnAutomationItem->showEnvelopes() && automationLaneEnabled()) {

                _speakerOnAutomationItem->setRect(tempRect);
                _speakerOnAutomationItem->automation()->update(tempRect);
                _speakerOnAutomationItem->draw(painter);
            } else if (item->automationLaneSubMenuTitle == "KeyFrames" && _keyFramesAutomationItem->showEnvelopes() && automationLaneEnabled()) {

                _keyFramesAutomationItem->setRect(tempRect);
                _keyFramesAutomationItem->automation()->update(tempRect);
                _keyFramesAutomationItem->draw(painter);
            } else {

                if (item->automationLaneMenuTitle == "Visr") {
                    auto effect = ((SampleTrack*)_track)->audioPort()->visrEfect();
                    if (effect) {

                        if (effect->contains(item->automationLaneSubMenuTitle) && automationLaneEnabled()) {
                            auto automationItem = effect->automationItem(item->automationLaneSubMenuTitle);

                            automationItem->setCurrentIndicator(_currentIndicator);

                            if (automationItem->showEnvelopes()) {
                                automationItem->setRect(tempRect);
                                automationItem->automation()->update(tempRect);
                                automationItem->draw(painter);
                            }
                        }
                    }
                }

                auto effectChain = ((SampleTrack*)_track)->audioPort()->effects();

                auto effects = effectChain->getEffect(item->automationLaneMenuTitle);
                if (effects) {
                    if (effects->contains(item->automationLaneSubMenuTitle) && automationLaneEnabled()) {
                        auto automationItem = effects->automationItem(item->automationLaneSubMenuTitle);

                        automationItem->setCurrentIndicator(_currentIndicator);

                        if (automationItem->showEnvelopes()) {
                            automationItem->setRect(tempRect);
                            automationItem->automation()->update(tempRect);
                            automationItem->draw(painter);
                        }
                    }
                }
            }
        }
    }
}

void TrackItem::drawAutomationLane(IPainter* painter)
{

    auto offset = 0;

    painter->save();
    painter->beginPath();
    painter->setStrokeStyle("#2f3032");
    painter->setFillStyle("#2f3032");
    auto top = TrackItem::rect().y() + TrackItem::rect().height();
    for (int i = 0; i < _automationLaneModel->count(); i++) {
        auto H = _automationLaneModel->totalRowCountBefore2(i) * _areaInfo->titleHeight() + i * _areaInfo->seperatorHeight();
        painter->fillRect(rect().x(), offset + top + H + _automationLaneModel->getItem(i)->automationLaneRowCount * _areaInfo->titleHeight(), rect().width(), _areaInfo->seperatorHeight());
    }
    painter->fill();
    painter->restore();
}
void TrackItem::drawClips(IPainter* painter)
{
    _mutex.lock();

    if (_clipItems.contains(_selectedClipIndex)) {
        if (_clipItems[_selectedClipIndex])
            _clipItems[_selectedClipIndex]->draw(painter);
    }

    QMapIterator<QString, QSharedPointer<ClipItem>> i(_clipItems);
    while (i.hasNext()) {
        i.next();

        if (i.value() == nullptr)
            continue;

        if (i.key() == _selectedClipIndex)
            continue;

        i.value()->draw(painter);
    }

    _mutex.unlock();
}

void TrackItem::drawIndicator(IPainter* painter)
{
    auto offset = 0;

    int Y = rect().top() + offset;
    auto top = rect().top() + _rowsExpandedCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight() + offset;

    if (_indicatorEnabled == IndicatorType_Track) {
        painter->beginPath();
        painter->setStrokeStyle(_indicatorColor);

        double currentPix = ((_currentIndicator - _areaInfo->startTime()) / (double)_areaInfo->clipDuration()) * _areaInfo->lastWidth();

        painter->moveTo(currentPix, Y);
        painter->lineTo(currentPix, Y + rect().height());
        painter->stroke();
    }
    if (_indicatorEnabled == IndicatorType_Lane) {
        painter->beginPath();
        painter->setStrokeStyle(_indicatorColor);

        int index = _automationLaneModel->currentSelected();
        if (index >= 0) {
            double currentPix = ((_currentIndicator - _areaInfo->startTime()) / (double)_areaInfo->clipDuration()) * _areaInfo->lastWidth();
            auto H = _automationLaneModel->totalRowCountBefore2(index) * _areaInfo->titleHeight() + index * _areaInfo->seperatorHeight();

            Y = top + H;
            int B = Y + _automationLaneModel->getItem(index)->automationLaneRowCount * _areaInfo->titleHeight();

            painter->moveTo(currentPix, Y);
            painter->lineTo(currentPix, B);
            painter->stroke();
        }
    }
}

AutomationLaneModel::AutomationLaneModel(AreaInfo* areaInfo, TrackItem* trackItem, QObject* parent)
    : _areaInfo(areaInfo)
    , _trackItem(trackItem)
    , QAbstractListModel(parent)
{
    m_roleNames[ROLE_AutomationLaneIndex] = "AutomationLaneIndex";
    m_roleNames[ROLE_AutomationLaneMenuTitle] = "AutomationLaneMenuTitle";
    m_roleNames[ROLE_AutomationLaneSubMenuTitle] = "AutomationLaneSubMenuTitle";
    m_roleNames[ROLE_AutomationLaneRowCount] = "AutomationLaneRowCount";
    m_roleNames[ROLE_AutomationLaneHeight] = "AutomationLaneHeight";

    m_roleNames[ROLE_Value] = "value";
    m_roleNames[ROLE_Normalized] = "normalized";
    m_roleNames[ROLE_AutomatedNormalized] = "automatedNormalized";
    m_roleNames[ROLE_AutomatedValue] = "automatedValue";
    m_roleNames[ROLE_AutomatedActive] = "automatedActive";
    m_roleNames[ROLE_AutomatedEnabled] = "automatedEnabled";
    m_roleNames[ROLE_DefaultNormalized] = "defaultNormalized";

    m_roleNames[ROLE_Clear] = "clear";

    connect(trackItem->volumeAutomationItem().data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltAutomationTrackVolumeChanged);
    connect(trackItem->volumeAutomationItem().data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltAutomationTrackVolumeChanged);
    connect(trackItem->volumeAutomationItem()->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::volumeAutomatedLineChanged);
    connect(trackItem->volumeAutomationItem()->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltVolumeEnabledChanged);
    connect(trackItem->volumeAutomationItem()->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltAutomationTrackSpeakerOnChanged);

    connect(trackItem->panAutomationItem().data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltAutomationTrackPanChanged);
    connect(trackItem->panAutomationItem().data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltAutomationTrackPanChanged);
    connect(trackItem->panAutomationItem()->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::panAutomatedLineChanged);
    connect(trackItem->panAutomationItem()->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltPanEnabledChanged);
    connect(trackItem->panAutomationItem()->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltAutomationTrackSpeakerOnChanged);

    connect(trackItem->speakerOnAutomationItem().data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltAutomationTrackSpeakerOnChanged);
    connect(trackItem->speakerOnAutomationItem().data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltAutomationTrackSpeakerOnChanged);
    connect(trackItem->speakerOnAutomationItem()->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::speakerOnAutomatedLineChanged);
    connect(trackItem->speakerOnAutomationItem()->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltSpeakerOnEnabledChanged);
    connect(trackItem->speakerOnAutomationItem()->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltAutomationTrackSpeakerOnChanged);
}

AutomationLaneModel::~AutomationLaneModel()
{
    qDebug() << "~AutomationLaneModel()";
}

void AutomationLaneModel::insert(QString menu, QString subMenu, int index, int rowCount)
{

    m_automationLanes.insert(index, AutomationLane { m_automationLanes.size(), menu, subMenu, 3 });

    setIndex(menu, subMenu, index);

    auto effectItem = _trackItem->effectAutomationItem(menu, subMenu);

    if (effectItem) {
        disconnect(effectItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        disconnect(effectItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        disconnect(effectItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
        disconnect(effectItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
        disconnect(effectItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);

        connect(effectItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        connect(effectItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        connect(effectItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
        connect(effectItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
        connect(effectItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);
    }

    auto visrItem = _trackItem->visrAutomationItem(menu, subMenu);

    if (visrItem) {
        disconnect(visrItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        disconnect(visrItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        disconnect(visrItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
        disconnect(visrItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
        disconnect(visrItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltValueChanged);

        connect(visrItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        connect(visrItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        connect(visrItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
        connect(visrItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
        connect(visrItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);
    }

    Q_EMIT beginResetModel();
    Q_EMIT endResetModel();
    Q_EMIT totalHeightChanged();
    Q_EMIT sigRowAdded(m_automationLanes.size() - 1, rowCount, menu, subMenu);
    Q_EMIT countChanged();
}

void AutomationLaneModel::append(QString menu, QString subMenu, int rowCount)
{

    m_automationLanes.append(AutomationLane { m_automationLanes.size(), menu, subMenu, 3 });

    setIndex(menu, subMenu, m_automationLanes.size() - 1);

    auto effectItem = _trackItem->effectAutomationItem(menu, subMenu);

    if (effectItem) {
        disconnect(effectItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        disconnect(effectItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        disconnect(effectItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
        disconnect(effectItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
        disconnect(effectItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);

        connect(effectItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        connect(effectItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        connect(effectItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
        connect(effectItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
        connect(effectItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);
    }

    auto visrItem = _trackItem->visrAutomationItem(menu, subMenu);

    if (visrItem) {
        disconnect(visrItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        disconnect(visrItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        disconnect(visrItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
        disconnect(visrItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
        disconnect(visrItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltValueChanged);

        connect(visrItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        connect(visrItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
        connect(visrItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
        connect(visrItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
        connect(visrItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);
    }

    Q_EMIT beginResetModel();
    Q_EMIT endResetModel();
    Q_EMIT totalHeightChanged();
    Q_EMIT sigRowAdded(m_automationLanes.size() - 1, rowCount, menu, subMenu);
    Q_EMIT countChanged();
}

void AutomationLaneModel::remove(QString menu, QString subMenu)
{

    int index = getIndex(menu, subMenu);
    if (index >= 0) {
        remove(index);
    }
}

void AutomationLaneModel::remove(QString menu)
{

    int index = -1;
    QMapIterator<QString, int> i(mapSubMenu);
    while (i.hasNext()) {
        i.next();
        if (i.key().contains(menu)) {
            index = i.value();
            break;
        }
    }

    while (index >= 0) {
        remove(index);

        index = -1;
        QMapIterator<QString, int> i(mapSubMenu);
        while (i.hasNext()) {
            i.next();
            if (i.key().contains(menu)) {
                index = i.value();
                break;
            }
        }
    }
}

void AutomationLaneModel::remove(int index)
{

    if (index >= 0 && index < m_automationLanes.size()) {
        auto count = m_automationLanes[index].automationLaneRowCount;
        auto menu = m_automationLanes[index].automationLaneMenuTitle;
        auto subMenu = m_automationLanes[index].automationLaneSubMenuTitle;
        m_automationLanes.removeAt(index);

        removeIndex(menu, subMenu);

        QMap<QString, int>::iterator it = mapSubMenu.begin();
        while (it != mapSubMenu.end()) {
            if (it.value() > index) {
                // Decrease the index by one if it is greater than the removed row
                it.value() = it.value() - 1;
            }
            it++;
        }

        Q_EMIT beginResetModel();
        Q_EMIT endResetModel();

        Q_EMIT totalHeightChanged();
        Q_EMIT sigRowRemoved(count, menu, subMenu);
        Q_EMIT countChanged();
    }
}

bool AutomationLaneModel::isContained(QString menu, QString subMenu)
{

    return contains(menu, subMenu);
}

int AutomationLaneModel::count()
{
    return m_automationLanes.size();
}

int AutomationLaneModel::size()
{
    return m_automationLanes.size();
}

int AutomationLaneModel::totalHeight()
{
    int height = 0;

    for (int i = 0; i < m_automationLanes.size(); i++) {
        height += (m_automationLanes[i].automationLaneRowCount * _areaInfo->titleHeight()) + _areaInfo->seperatorHeight();
    }
    return height;
}

int AutomationLaneModel::getIndexGlobal(QPointF point)
{

    const int count = m_automationLanes.size();
    auto top = _trackItem->globalYPosition() + _trackItem->rowsExpandedCount() * _areaInfo->titleHeight() + _areaInfo->seperatorHeight();

    qreal firstTop = _trackItem->globalYPosition(); // top of first tempRect
    qreal lastBottom = _trackItem->globalYPosition() + _trackItem->totalRect().height(); // will compute

    for (int i = 0; i < m_automationLanes.size(); i++) {
        auto H = totalRowCountBefore2(i) * _areaInfo->titleHeight() + i * _areaInfo->seperatorHeight();
        auto tempRect = _trackItem->rect();
        tempRect.setTop(top + H);
        tempRect.setHeight(getItem(i)->automationLaneRowCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight());
        if (tempRect.contains(point.toPoint())) {
            return i;
        }
    }

    // Above first block
    if (point.y() < firstTop)
        return -1;

    // Below last block
    if (point.y() > lastBottom)
        return count;

    // Should not happen, but fallback
    return -1;
}

int AutomationLaneModel::getIndex(QPointF point)
{

    auto top = _trackItem->rect().top() + _trackItem->rowsExpandedCount() * _areaInfo->titleHeight() + _areaInfo->seperatorHeight();
    for (int i = 0; i < m_automationLanes.size(); i++) {
        auto H = totalRowCountBefore2(i) * _areaInfo->titleHeight() + i * _areaInfo->seperatorHeight();
        auto tempRect = _trackItem->rect();
        tempRect.setTop(top + H);
        tempRect.setHeight(getItem(i)->automationLaneRowCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight());
        if (tempRect.contains(point.toPoint())) {
            return i;
        }
    }
    return -1;
}

int AutomationLaneModel::totalRowCount()
{
    int count = 0;
    for (int i = 0; i < m_automationLanes.size(); i++) {
        count += (m_automationLanes[i].automationLaneRowCount);
    }
    return count;
}

int AutomationLaneModel::totalRowCountBefore(int _index)
{
    int count = 0;
    for (int i = 0; i <= _index; i++) {
        count += (m_automationLanes[i].automationLaneRowCount);
    }
    return count;
}
int AutomationLaneModel::totalRowCountBefore2(int _index)
{
    int count = 0;
    for (int i = 0; i < _index; i++) {
        count += (m_automationLanes[i].automationLaneRowCount);
    }
    return count;
}

void AutomationLaneModel::decrease(int i)
{
    if (m_automationLanes[i].automationLaneRowCount > 1) {
        m_automationLanes[i].automationLaneRowCount--;
        Q_EMIT sigRowChanged();
        Q_EMIT dataChanged(QAbstractListModel::index(i, 0), QAbstractListModel::index(i, 0), QVector<int>() << ROLE_AutomationLaneHeight);
        Q_EMIT totalHeightChanged();
    }
}

void AutomationLaneModel::increase(int i)
{
    m_automationLanes[i].automationLaneRowCount++;
    Q_EMIT sigRowChanged();
    Q_EMIT dataChanged(QAbstractListModel::index(i, 0), QAbstractListModel::index(i, 0), QVector<int>() << ROLE_AutomationLaneHeight);
    Q_EMIT totalHeightChanged();
}

void AutomationLaneModel::setLaneHeight(int _index, int posY)
{

    if (_index >= 0 && _index < m_automationLanes.size() - 1) {
        auto startY = totalRowCountBefore(_index) * _areaInfo->titleHeight() + (_trackItem->globalYPosition() + _trackItem->rect().height()) + _areaInfo->seperatorHeight();

        auto diff = posY - startY;

        if (diff > _areaInfo->titleHeight()) {

            m_automationLanes[_index].automationLaneRowCount++;

        } else if (diff < -_areaInfo->titleHeight()) {

            if (m_automationLanes[_index].automationLaneRowCount > 1) {
                m_automationLanes[_index].automationLaneRowCount--;
            }
        }

        Q_EMIT sigRowChanged();
        Q_EMIT dataChanged(QAbstractListModel::index(_index, 0), QAbstractListModel::index(_index, 0), QVector<int>() << ROLE_AutomationLaneHeight);
        Q_EMIT dataChanged(QAbstractListModel::index(_index + 1, 0), QAbstractListModel::index(_index + 1, 0), QVector<int>() << ROLE_AutomationLaneHeight);
        Q_EMIT dataChanged(QAbstractListModel::index(_index + 1, 0), QAbstractListModel::index(_index + 1, 0), QVector<int>() << ROLE_AutomationLaneRowCount);
        Q_EMIT dataChanged(QAbstractListModel::index(_index, 0), QAbstractListModel::index(_index, 0), QVector<int>() << ROLE_AutomationLaneRowCount);

    } else if (_index == m_automationLanes.size() - 1) {

        auto startY = totalRowCountBefore(_index) * _areaInfo->titleHeight() + (_trackItem->globalYPosition() + _trackItem->rect().height()) + _areaInfo->seperatorHeight();

        auto diff = posY - startY;

        if (diff > _areaInfo->titleHeight()) {

            m_automationLanes[_index].automationLaneRowCount++;

        } else if (diff < -_areaInfo->titleHeight()) {

            if (m_automationLanes[_index].automationLaneRowCount > 1) {
                m_automationLanes[_index].automationLaneRowCount--;
            }
        }

        Q_EMIT sigRowChanged();
        Q_EMIT dataChanged(QAbstractListModel::index(_index, 0), QAbstractListModel::index(_index, 0), QVector<int>() << ROLE_AutomationLaneHeight);
        Q_EMIT dataChanged(QAbstractListModel::index(_index, 0), QAbstractListModel::index(_index, 0), QVector<int>() << ROLE_AutomationLaneRowCount);
    }
}

void AutomationLaneModel::updateLanesHeight()
{
    for (int i = 0; i < m_automationLanes.size(); i++) {
        Q_EMIT dataChanged(QAbstractListModel::index(i, 0), QAbstractListModel::index(i, 0), QVector<int>() << ROLE_AutomationLaneHeight);
    }
}

int AutomationLaneModel::currentSelected() const
{
    return _currentSelected;
}

void AutomationLaneModel::setCurrentSelected(int newCurrentSelected)
{
    _currentSelected = newCurrentSelected;
}

int AutomationLaneModel::findIndex(QString menu, QString submenu)
{

    return getIndex(menu, submenu);
}

QHash<int, QByteArray> AutomationLaneModel::roleNames() const
{
    return m_roleNames;
}

int AutomationLaneModel::rowCount(const QModelIndex& parent) const
{
    return m_automationLanes.size();
}

QVariant AutomationLaneModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid())
        return QVariant();

    if (m_automationLanes.size() == 0 || index.row() < 0)
        return QVariant();

    AutomationLane value = m_automationLanes[index.row()];

    switch (role) {

    case ROLE_AutomationLaneIndex:
        return value.automationLaneIndex;

    case ROLE_AutomationLaneSubMenuTitle:
        return value.automationLaneSubMenuTitle;
    case ROLE_AutomationLaneMenuTitle:
        return value.automationLaneMenuTitle;
    case ROLE_AutomationLaneRowCount:
        return value.automationLaneRowCount;
    case ROLE_AutomationLaneHeight:
        return m_automationLanes[index.row()].automationLaneRowCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight();

    case ROLE_Value:

        if (value.automationLaneSubMenuTitle == "None") {
            return 0;
        } else if (value.automationLaneSubMenuTitle == "Volume") {

            return _trackItem->volumeAutomationItem()->value();
        } else if (value.automationLaneSubMenuTitle == "Pan") {
            return _trackItem->panAutomationItem()->value();
        } else if (value.automationLaneSubMenuTitle == "SpeakerOn") {
            return _trackItem->speakerOnAutomationItem()->value();
        } else {
            if (value.automationLaneMenuTitle == "Visr") {
                if (_trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)) {
                    return _trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->value();
                }
            } else {
                if (_trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)) {
                    return _trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->value();
                }
            }
            return 0;
        }

    case ROLE_AutomatedValue:
        if (value.automationLaneSubMenuTitle == "None") {
            return 0;
        } else if (value.automationLaneSubMenuTitle == "Volume") {
            return _trackItem->volumeAutomationItem()->automatedValue();
        } else if (value.automationLaneSubMenuTitle == "Pan") {
            return _trackItem->panAutomationItem()->automatedValue();
        } else if (value.automationLaneSubMenuTitle == "SpeakerOn") {
            return _trackItem->speakerOnAutomationItem()->automatedValue();
        } else {
            if (value.automationLaneMenuTitle == "Visr") {
                if (_trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automatedValue();

            } else {
                if (_trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automatedValue();
            }
            return 0;
        }

    case ROLE_AutomatedNormalized:
        if (value.automationLaneSubMenuTitle == "None") {
            return 0;
        } else if (value.automationLaneSubMenuTitle == "Volume") {
            return _trackItem->volumeAutomationItem()->automation()->automatedNormalized();
        } else if (value.automationLaneSubMenuTitle == "Pan") {
            return _trackItem->panAutomationItem()->automation()->automatedNormalized();
        } else if (value.automationLaneSubMenuTitle == "SpeakerOn") {
            return _trackItem->speakerOnAutomationItem()->automation()->automatedNormalized();
        } else {
            if (value.automationLaneMenuTitle == "Visr") {
                if (_trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->automatedNormalized();
            } else {
                if (_trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->automatedNormalized();
            }
            return 0;
        }
    case ROLE_Normalized:
        if (value.automationLaneSubMenuTitle == "None") {
            return 0;
        } else if (value.automationLaneSubMenuTitle == "Volume") {
            return _trackItem->volumeAutomationItem()->automation()->normalized();
        } else if (value.automationLaneSubMenuTitle == "Pan") {
            return _trackItem->panAutomationItem()->automation()->normalized();
        } else if (value.automationLaneSubMenuTitle == "SpeakerOn") {
            return _trackItem->speakerOnAutomationItem()->automation()->normalized();
        } else {
            if (value.automationLaneMenuTitle == "Visr") {
                if (_trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->normalized();
            } else {
                if (_trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->normalized();
            }
            return 0;
        }
    case ROLE_AutomatedActive:
        if (value.automationLaneSubMenuTitle == "None") {
            return 0;
        } else if (value.automationLaneSubMenuTitle == "Volume") {
            return _trackItem->volumeAutomationItem()->automation()->isAutomated();
        } else if (value.automationLaneSubMenuTitle == "Pan") {
            return _trackItem->panAutomationItem()->automation()->isAutomated();
        } else if (value.automationLaneSubMenuTitle == "SpeakerOn") {
            return _trackItem->speakerOnAutomationItem()->automation()->isAutomated();
        } else {
            if (value.automationLaneMenuTitle == "Visr") {
                if (_trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->isAutomated();

            } else {
                if (_trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->isAutomated();
            }
            return 0;
        }
    case ROLE_AutomatedEnabled:

        if (value.automationLaneSubMenuTitle == "None") {
            return 0;
        } else if (value.automationLaneSubMenuTitle == "Volume") {
            return _trackItem->volumeAutomationItem()->automation()->enabled();
        } else if (value.automationLaneSubMenuTitle == "Pan") {
            return _trackItem->panAutomationItem()->automation()->enabled();
        } else if (value.automationLaneSubMenuTitle == "SpeakerOn") {
            return _trackItem->speakerOnAutomationItem()->automation()->enabled();
        } else {
            if (value.automationLaneMenuTitle == "Visr") {
                if (_trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->enabled();
            } else {
                if (_trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->enabled();
            }
            return 0;
        }

    case ROLE_DefaultNormalized:

        if (value.automationLaneSubMenuTitle == "None") {
            return 0;
        } else if (value.automationLaneSubMenuTitle == "Volume") {
            return _trackItem->volumeAutomationItem()->automation()->defaultNormalized();
        } else if (value.automationLaneSubMenuTitle == "Pan") {
            return _trackItem->panAutomationItem()->automation()->defaultNormalized();
        } else if (value.automationLaneSubMenuTitle == "SpeakerOn") {
            return _trackItem->speakerOnAutomationItem()->automation()->defaultNormalized();
        } else {
            if (value.automationLaneMenuTitle == "Visr") {
                if (_trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->visrAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->defaultNormalized();
            } else {
                if (_trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle))
                    return _trackItem->effectAutomationItem(value.automationLaneMenuTitle, value.automationLaneSubMenuTitle)->automation()->defaultNormalized();
            }
            return 0;
        }
    }
    return QVariant();
}

void AutomationLaneModel::sltEnableChanged()
{

    auto _sender = (Automation*)sender();

    int i = getIndex(_sender->type(), _sender->subType());

    if (i >= 0 && i < m_automationLanes.size()) {
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedEnabled);
    }
}
void AutomationLaneModel::sltLineChanged()
{

    auto _sender = (Automation*)sender();

    int i = getIndex(_sender->type(), _sender->subType());

    _trackItem->automationItem(_sender->type(), _sender->subType())->automation()->setEnabled1(true);

    if (i >= 0 && i < m_automationLanes.size()) {
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedActive);
    }
    if (!_sender->isAutomated()) {
        _sender->setAutomatedNormalized(_sender->normalized());
    }
}
void AutomationLaneModel::sltValueChanged()
{

    auto _sender = (AutomationItem*)sender();

    int i = getIndex(_sender->automation()->type(), _sender->automation()->subType());

    if (i >= 0 && i < m_automationLanes.size()) {

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Value);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Normalized);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedValue);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedNormalized);
    }
}

void AutomationLaneModel::sltNormalizedValueChanged()
{

    auto _sender = (Automation*)sender();

    int i = getIndex(_sender->type(), _sender->subType());

    if (i >= 0 && i < m_automationLanes.size()) {

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Value);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Normalized);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedValue);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedNormalized);
    }
}

bool AutomationLaneModel::setData(const QModelIndex& index, const QVariant& value, int role)
{

    if (m_automationLanes.size() == 0 || index.row() < 0 || index.row() > m_automationLanes.size())
        return false;

    auto item = m_automationLanes[index.row()];

    if (index.isValid() && role == ROLE_Clear) {

        if (value.toBool() == true) {

            auto automation = _trackItem->automationItem(item.automationLaneMenuTitle, item.automationLaneSubMenuTitle)->automation();

            QJsonArray array;
            QJsonObject jObject2;
            jObject2["track_index"] = _trackItem->trackIndex();
            jObject2["sub_track_index"] = automation->getSubTrackIndex();
            jObject2["menu"] = automation->type();
            jObject2["sub_menu"] = automation->subType();
            jObject2["state"] = automation->getLinesState();
            jObject2["operation"] = ActionStack::Operation_RefreshAutomationPoint;
            array.append(jObject2);
            ActionStack::instance()->pushInUndo(array);

            automation->clear();

            Q_EMIT sigValueChanged();
            return true;
        }
        return true;
    }
    if (index.isValid() && role == ROLE_AutomationLaneSubMenuTitle) {

        if (m_automationLanes[index.row()].automationLaneSubMenuTitle != value.toString()) {

            removeIndex(m_automationLanes[index.row()].automationLaneMenuTitle, m_automationLanes[index.row()].automationLaneSubMenuTitle);

            setIndex(m_automationLanes[index.row()].automationLaneMenuTitle, m_automationLanes[index.row()].automationLaneSubMenuTitle, index.row());

            m_automationLanes[index.row()].automationLaneSubMenuTitle = value.toString();

            auto effectItem = _trackItem->effectAutomationItem(m_automationLanes[index.row()].automationLaneMenuTitle, value.toString());

            if (effectItem) {
                disconnect(effectItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
                disconnect(effectItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
                disconnect(effectItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
                disconnect(effectItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
                disconnect(effectItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);

                connect(effectItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
                connect(effectItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
                connect(effectItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
                connect(effectItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
                connect(effectItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);
            }

            auto visrItem = _trackItem->visrAutomationItem(m_automationLanes[index.row()].automationLaneMenuTitle, value.toString());
            if (visrItem) {
                disconnect(visrItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
                disconnect(visrItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
                disconnect(visrItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
                disconnect(visrItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
                disconnect(visrItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);

                connect(visrItem.data(), &AutomationItem::sigAutomatedNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
                connect(visrItem.data(), &AutomationItem::sigNormalizedChanged, this, &AutomationLaneModel::sltValueChanged);
                connect(visrItem->automation().data(), &Automation::sigLinesChanged, this, &AutomationLaneModel::sltLineChanged);
                connect(visrItem->automation().data(), &Automation::enabledChanged, this, &AutomationLaneModel::sltEnableChanged);
                connect(visrItem->automation().data(), &Automation::valueChanged, this, &AutomationLaneModel::sltNormalizedValueChanged);
            }

            Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_AutomationLaneSubMenuTitle);
            Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_Value);
            Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_Normalized);
            Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_AutomatedEnabled);

            Q_EMIT sigValueChanged();
        }

        return true;
    }
    if (index.isValid() && role == ROLE_Normalized) {

        if (item.automationLaneSubMenuTitle == "Volume") {
            _trackItem->volumeAutomationItem()->setNormalized(value.toDouble());
        } else if (item.automationLaneSubMenuTitle == "Pan") {
            _trackItem->panAutomationItem()->setNormalized(value.toDouble());
        } else if (item.automationLaneSubMenuTitle == "SpeakerOn") {
            _trackItem->speakerOnAutomationItem()->setNormalized(value.toDouble());
        } else {
            if (item.automationLaneMenuTitle == "Visr") {
                if (_trackItem->visrAutomationItem(item.automationLaneMenuTitle, item.automationLaneSubMenuTitle))
                    _trackItem->visrAutomationItem(item.automationLaneMenuTitle, item.automationLaneSubMenuTitle)->setNormalized(value.toDouble());
            } else {
                if (_trackItem->effectAutomationItem(item.automationLaneMenuTitle, item.automationLaneSubMenuTitle))
                    _trackItem->effectAutomationItem(item.automationLaneMenuTitle, item.automationLaneSubMenuTitle)->setNormalized(value.toDouble());
            }
        }

        Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_Normalized);
        Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_Value);

        Q_EMIT sigValueChanged();
        return true;
    }
    if (index.isValid() && role == ROLE_AutomatedEnabled) {

        if (item.automationLaneSubMenuTitle == "Volume") {

            _trackItem->volumeAutomationItem()->setEnabled(value.toBool());
        } else if (item.automationLaneSubMenuTitle == "Pan") {
            _trackItem->panAutomationItem()->setEnabled(value.toBool());
        } else if (item.automationLaneSubMenuTitle == "SpeakerOn") {
            _trackItem->speakerOnAutomationItem()->setEnabled(value.toBool());
        } else {
            if (item.automationLaneMenuTitle == "Visr") {
                if (_trackItem->visrAutomationItem(item.automationLaneMenuTitle, item.automationLaneSubMenuTitle))
                    _trackItem->visrAutomationItem(item.automationLaneMenuTitle, item.automationLaneSubMenuTitle)->setEnabled(value.toBool());
            } else {
                if (_trackItem->effectAutomationItem(item.automationLaneMenuTitle, item.automationLaneSubMenuTitle))
                    _trackItem->effectAutomationItem(item.automationLaneMenuTitle, item.automationLaneSubMenuTitle)->setEnabled(value.toBool());
            }
        }

        Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), 0), QVector<int>() << ROLE_AutomatedEnabled);

        Q_EMIT sigValueChanged();
        return true;
    }
}

void AutomationLaneModel::sltAutomationTrackSpeakerOnChanged()
{
    int i = findIndex("Mixer", "SpeakerOn");
    if (i >= 0 && i < m_automationLanes.size()) {

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedNormalized);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedValue);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Normalized);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Value);
    }
}

void AutomationLaneModel::speakerOnAutomatedLineChanged()
{

    auto _sender = (Automation*)sender();

    int i = findIndex("Mixer", "SpeakerOn");
    if (i >= 0 && i < m_automationLanes.size()) {

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedActive);
    }
    if (!_sender->isAutomated()) {
        _sender->setAutomatedNormalized(_sender->normalized());
    }
}

void AutomationLaneModel::sltSpeakerOnEnabledChanged()
{
    int i = findIndex("Mixer", "SpeakerOn");
    if (i >= 0 && i < m_automationLanes.size()) {

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedEnabled);
    }
}

void AutomationLaneModel::sltAutomationTrackVolumeChanged()
{

    int i = findIndex("Mixer", "Volume");
    if (i >= 0 && i < m_automationLanes.size()) {
        if (i >= 0 && i < m_automationLanes.size()) {
            Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedNormalized);
            Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedValue);
            Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Normalized);
            Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Value);
        }
    }
}
void AutomationLaneModel::sltAutomationTrackPanChanged()
{

    int i = findIndex("Mixer", "Pan");
    if (i >= 0 && i < m_automationLanes.size()) {

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedNormalized);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedValue);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Normalized);
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Value);
    }
}
void AutomationLaneModel::volumeAutomatedLineChanged()
{

    auto _sender = (Automation*)sender();

    int i = findIndex("Mixer", "Volume");

    if (i >= 0 && i < m_automationLanes.size()) {

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedActive);
    }
    if (!_sender->isAutomated()) {

        _sender->setAutomatedNormalized(_sender->normalized());
    }
}
void AutomationLaneModel::panAutomatedLineChanged()
{

    auto _sender = (Automation*)sender();

    int i = findIndex("Mixer", "Pan");
    if (i >= 0 && i < m_automationLanes.size()) {

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedActive);
    }
    if (!_sender->isAutomated()) {
        _sender->setAutomatedNormalized(_sender->normalized());
    }
}

void AutomationLaneModel::sltVolumeEnabledChanged()
{

    int i = findIndex("Mixer", "Volume");
    if (i >= 0 && i < m_automationLanes.size()) {
        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedEnabled);
    }
}
void AutomationLaneModel::sltPanEnabledChanged()
{

    int i = findIndex("Mixer", "Pan");
    if (i >= 0 && i < m_automationLanes.size()) {

        Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_AutomatedEnabled);
    }
}
