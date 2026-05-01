#include "clipitem.h"
#include "core/sampleeditor.h"
#include "core/warp.h"
#include <QCursor>
#include <QDir>
#include <QMimeData>
#include <QTime>
#include <QtMath>
#include <audio/audioengine.h>
#include <audio/engine.h>
#include <audio/samplebuffer.h>
#include <audio/sampleclip.h>
#include <audio/song.h>
#include <core/cliparea.h>

ClipItem::ClipItem(int trackIndex, QColor titleColor, QString clipIndex, AreaInfo* areaInfo, SampleClip* sampleClip)
    : IPainterItem()
    , _areaInfo(areaInfo)
    , _sampleClip(sampleClip)

{
    if (clipIndex == "")
        _clipIndex = QUuid::createUuid().toString();
    else
        _clipIndex = clipIndex;

    _baseColor = QColor(titleColor.red(), titleColor.green(), titleColor.blue());

    _trackIndex = trackIndex;
    _sampleClip->setClipIndex(_clipIndex);

    _titleHeight = areaInfo->titleHeight();
    _seperatorHeight = areaInfo->seperatorHeight();
    _rowsExpandedCount = _areaInfo->minTitlebarCount();
}

ClipItem::ClipItem(ClipItem& item, Track* track, bool isFake)
    : _trackIndex(item.trackIndex())
    , _sampleClip(item._sampleClip->clone(track, isFake))
    , _areaInfo(item.areaInfo())
    , _baseColor(item._baseColor)
    , _seperatorHeight(item.rowsExpandedCount())

{

    _clipIndex = QUuid::createUuid().toString();
    _sampleClip->setClipIndex(_clipIndex);
    _rect = item.rect();

    _titleHeight = _areaInfo->titleHeight();
    _seperatorHeight = _areaInfo->seperatorHeight();
}

void ClipItem::setParameters(double startTime, double endTime, double startOffset, double endOffset)
{

    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

    sampleClip()->setStartTime(startTime);
    sampleClip()->setEndTime(endTime);

    sampleClip()->sampleBuffer()->setOffsetStartFrame(startOffset);
    sampleClip()->sampleBuffer()->setOffsetEndFrame(endOffset);

    sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
    sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
}

ClipItem::~ClipItem()
{

    m_mutex.lock();

    if (_sampleClip) {
        delete _sampleClip;
        _sampleClip = Q_NULLPTR;
    }

    m_mutex.unlock();
}

void ClipItem::setBaseColor(QColor titleColor)
{
    _baseColor = titleColor;
}

void ClipItem::setAutomationLaneModel(QSharedPointer<AutomationLaneModel> automationLaneModel)
{
    _automationLaneModel = automationLaneModel;
}

void ClipItem::setRowsExpandedCount(int count)
{
    _rowsExpandedCount = count;
}

void ClipItem::setAutomationLaneEnabled(bool status)
{
    _automationLaneEnabled = status;
}

void ClipItem::draw(IPainter* painter)
{
    return;
}

void ClipItem::draw(QVector<QVector<QPointF>>& minPoints,
    QVector<QVector<QPointF>>& maxPoints,
    QVector<QVector<QPointF>>& points)
{

    auto _startTime = _areaInfo->startTime();
    auto _clipDuration = _areaInfo->clipDuration();

    auto _lastWidth = _areaInfo->lastWidth();

    double endTime = _startTime + _clipDuration;
    double currentFileDuration = _areaInfo->endTime() - _areaInfo->startTime();

    int channels = _sampleClip->sampleBuffer()->channels();

    double timePerPixel = _lastWidth / _clipDuration;

    if (currentFileDuration > 0 && channels > 0) {

        double pix1 = ((_sampleClip->startTime() - _startTime) * timePerPixel);
        double pix2 = ((_sampleClip->endTime() - _startTime) * timePerPixel);

        m_baseFileName = QFileInfo(_sampleClip->sampleBuffer()->audioFile()).baseName();

        auto start_pix_text = pix1 + ScreenInterface::scaleSize2(5, _areaInfo->zoomFactor());

        if (start_pix_text < 0) {
            start_pix_text = ScreenInterface::scaleSize2(5, _areaInfo->zoomFactor());
        }

        int H = (_rowsExpandedCount * _titleHeight + _seperatorHeight - _areaInfo->titleHeight()) / channels;

        double diff = _lastStartffset - _startTime;

        double pix = static_cast<double>(diff * timePerPixel);

        if (pix >= 1 || pix <= -1) {
            _lastStartffset = _startTime;
        }

        _lastDuration = currentFileDuration;

        for (int i = 0; i < _sampleClip->sampleBuffer()->channels(); i++) {
            int Y = i * H + (_rect.y() + _areaInfo->titleHeight());

            double start = _sampleClip->sampleBuffer()->offsetStartFrame() / (float)AudioManager::audioEngine()->outputSampleRate() * 1000.0;
            double end = _sampleClip->sampleBuffer()->offsetEndFrame() / (float)AudioManager::audioEngine()->outputSampleRate() * 1000.0;

            double pix1 = ((_sampleClip->startTime() - _startTime) * timePerPixel);
            double pix2 = ((_sampleClip->endTime() - _startTime) * timePerPixel);

            if (_startTime >= _sampleClip->startTime()) {
                start = _startTime - _sampleClip->startTime() + _sampleClip->sampleBuffer()->offsetStartFrame() / (float)AudioManager::audioEngine()->outputSampleRate() * 1000.0;
            } else {
                start = _sampleClip->sampleBuffer()->offsetStartFrame() / (float)AudioManager::audioEngine()->outputSampleRate() * 1000.0;
            }

            if (endTime <= _sampleClip->endTime()) {
                end = (_sampleClip->sampleBuffer()->offsetEndFrame() / (float)AudioManager::audioEngine()->outputSampleRate() * 1000.0) - (_sampleClip->endTime() - endTime);
            } else {
                end = _sampleClip->sampleBuffer()->offsetEndFrame() / (float)AudioManager::audioEngine()->outputSampleRate() * 1000.0;
            }

            QVector<QPointF> minPoint;
            QVector<QPointF> maxPoint;
            QVector<QPointF> point;

            if (pix1 < 0 && pix2 < 0) {
                minPoints.append(minPoint);
                maxPoints.append(maxPoint);
                points.append(point);
                continue;
            }

            if (pix1 > _lastWidth && pix2 > _lastWidth) {
                minPoints.append(minPoint);
                maxPoints.append(maxPoint);
                points.append(point);
                continue;
            }

            double p0, p1;

            if (pix2 > _lastWidth)
                p1 = _lastWidth;
            else
                p1 = pix2;

            p0 = pix1;

            if (p0 < 0)
                p0 = 0;

            QRectF rect(p0, Y, p1 - p0, H);

            QVector<float> arr_min;
            QVector<float> arr_max;

            arr_min.resize(p1 - p0);
            arr_max.resize(p1 - p0);

            if (arr_min.size() <= 0) {
                minPoints.append(minPoint);
                maxPoints.append(maxPoint);
                points.append(point);
                continue;
            }

            _sampleClip->sampleBuffer()->get(rect, _lastWidth, i,
                start, end, arr_max.data(), arr_min.data());

            _sampleClip->sampleBuffer()->visualize2(
                rect.toRect(), i,
                start, end,
                arr_max.data(), arr_min.data(), arr_min.size(), minPoint, maxPoint, point);

            minPoints.append(minPoint);
            maxPoints.append(maxPoint);
            points.append(point);
        }

        if (_automationLaneModel && _automationLaneEnabled) {

            int baseTop = _rect.top() + _rowsExpandedCount * _titleHeight + _seperatorHeight;

            for (int index = 0; index < _automationLaneModel->count(); index++) {
                auto H = (_automationLaneModel->getItem(index)->automationLaneRowCount * _titleHeight) / channels;
                auto H_before = _automationLaneModel->totalRowCountBefore2(index) * _titleHeight + index * _seperatorHeight;
                auto Top = baseTop + H_before;

                for (int i = 0; i < _sampleClip->sampleBuffer()->channels(); i++) {
                    int Y = i * H + Top;

                    double start = _sampleClip->sampleBuffer()->offsetStartFrame() / AudioManager::audioEngine()->outputSampleRate() * 1000.0;
                    double end = _sampleClip->sampleBuffer()->offsetEndFrame() / AudioManager::audioEngine()->outputSampleRate() * 1000.0;

                    double pix1 = ((_sampleClip->startTime() - _startTime) * timePerPixel);
                    double pix2 = ((_sampleClip->endTime() - _startTime) * timePerPixel);

                    if (_startTime >= _sampleClip->startTime()) {
                        start = _startTime - _sampleClip->startTime() + _sampleClip->sampleBuffer()->offsetStartFrame() / AudioManager::audioEngine()->outputSampleRate() * 1000.0;
                    } else {
                        start = _sampleClip->sampleBuffer()->offsetStartFrame() / AudioManager::audioEngine()->outputSampleRate() * 1000.0;
                    }

                    if (endTime <= _sampleClip->endTime()) {
                        end = (_sampleClip->sampleBuffer()->offsetEndFrame() / AudioManager::audioEngine()->outputSampleRate() * 1000.0) - (_sampleClip->endTime() - endTime);
                    } else {
                        end = _sampleClip->sampleBuffer()->offsetEndFrame() / AudioManager::audioEngine()->outputSampleRate() * 1000.0;
                    }

                    QVector<QPointF> minPoint;
                    QVector<QPointF> maxPoint;
                    QVector<QPointF> point;

                    if (pix1 < 0 && pix2 < 0) {
                        minPoints.append(minPoint);
                        maxPoints.append(maxPoint);
                        points.append(point);
                    }

                    if (pix1 > _lastWidth && pix2 > _lastWidth) {
                        minPoints.append(minPoint);
                        maxPoints.append(maxPoint);
                        points.append(point);
                    }

                    double p0, p1;

                    if (pix2 > _lastWidth)
                        p1 = _lastWidth;
                    else
                        p1 = pix2;

                    p0 = pix1;

                    if (p0 < 0)
                        p0 = 0;

                    QRectF rect(p0, Y, p1 - p0, H);

                    QVector<float> arr_min;
                    QVector<float> arr_max;

                    arr_min.resize(p1 - p0);
                    arr_max.resize(p1 - p0);

                    if (arr_min.size() <= 0) {
                        minPoints.append(minPoint);
                        maxPoints.append(maxPoint);
                        points.append(point);
                    }

                    _sampleClip->sampleBuffer()->get(rect, _lastWidth, i,
                        start, end, arr_max.data(), arr_min.data());

                    _sampleClip->sampleBuffer()->visualize2(
                        rect.toRect(), i,
                        start, end,
                        arr_max.data(), arr_min.data(), arr_min.size(), minPoint, maxPoint, point);

                    minPoints.append(minPoint);
                    maxPoints.append(maxPoint);
                    points.append(point);
                }
            }
        }
    }
}

int ClipItem::rowsExpandedCount() const
{
    return _rowsExpandedCount;
}

QString ClipItem::baseFileName() const
{
    return m_baseFileName;
}

QColor ClipItem::baseColor() const
{
    return _baseColor;
}

AreaInfo* ClipItem::areaInfo() const
{
    return _areaInfo;
}

void ClipItem::setMouseAction(const MouseAction& mouseAction)
{
    _mouseHoverAction = mouseAction;
}

void ClipItem::setPressAndHold(const MouseAction& pressAndHold)
{
    _pressAndHoldAction = pressAndHold;
}

ClipItem::MouseAction ClipItem::pressAndHold() const
{
    return _pressAndHoldAction;
}

ClipItem::MouseAction ClipItem::mouseAction() const
{
    return _mouseHoverAction;
}

QString ClipItem::clipIndex() const
{
    return _clipIndex;
}

int ClipItem::trackIndex() const
{
    return _trackIndex;
}

void ClipItem::setClipIndex(QString clipIndex)
{
    _clipIndex = clipIndex;
}

void ClipItem::setTrackIndex(int trackIndex)
{
    _trackIndex = trackIndex;
}

SampleClip* ClipItem::sampleClip() const
{

    return _sampleClip;
}

bool ClipItem::containsPixel(double pixel)
{

    auto _startTime = _areaInfo->startTime();

    double timePerPixel = _areaInfo->timePerPixel();

    double pix1 = ((_sampleClip->startTime() - _startTime) * timePerPixel);
    double pix2 = ((_sampleClip->endTime() - _startTime) * timePerPixel);

    if (pixel > pix1 && pixel < pix2) {
        return true;
    }
    return false;
}

bool ClipItem::containsTime(double time)
{

    auto startTime = _sampleClip->startTime();
    auto endTime = _sampleClip->endTime();

    if (time > startTime && time < endTime) {
        return true;
    }
    return false;
}

int ClipItem::mousePressEvent(QMouseEvent* event)
{

    if (_sampleClip == nullptr)
        return 0;

    auto _startTime = _areaInfo->startTime();

    double timePerPixel = _areaInfo->timePerPixel();

    double pix1 = ((_sampleClip->startTime() - _startTime) * timePerPixel);
    double pix2 = ((_sampleClip->endTime() - _startTime) * timePerPixel);
    QRect rect(pix1, _rect.y(), pix2 - pix1, _areaInfo->titleHeight());

    if (!rect.contains(event->pos())) {
        return 0;
    }

    _pressAndHoldAction = _mouseHoverAction;

    return 1;
}

int ClipItem::mouseMoveEvent(QMouseEvent* event)
{

    if (_sampleClip == nullptr)
        return 0;

    auto _startTime = _areaInfo->startTime();
    auto _clipDuration = _areaInfo->clipDuration();
    auto _lastWidth = _areaInfo->lastWidth();
    if (_pressAndHoldAction == MouseAction_ResizeRight) {

        double timePerPixel = _lastWidth / _clipDuration;
        double final_xpos = 0;
        double final_ypos = 0;
        final_xpos = _areaInfo->findSnap(event->pos().x());

        auto _shadowEndTime = (final_xpos)*1.0 / timePerPixel + _startTime;

        auto startFrame = _sampleClip->startTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        auto endFrame = _shadowEndTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

        double diff = _shadowEndTime - _sampleClip->endTime();
        auto duration = (_sampleClip->sampleBuffer()->offsetEndFrame() - _sampleClip->sampleBuffer()->offsetStartFrame()) / AudioManager::audioEngine()->outputSampleRate() * 1000.0;
        _sampleClip->setFake(true);
        if ((_sampleClip->sampleBuffer()->offsetEndFrame() / AudioManager::audioEngine()->outputSampleRate() * 1000.0) + diff > _sampleClip->currentFileDuration()) {

            if (startFrame > endFrame) {

            } else {
                endFrame = (_sampleClip->startTime() + duration) / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                _sampleClip->sampleBuffer()->setBaseStartFrame(startFrame);
                _sampleClip->sampleBuffer()->setBaseEndFrame(endFrame);

                _sampleClip->sampleBuffer()->setOffsetEndFrame(_sampleClip->currentFileDuration() / 1000.0 * AudioManager::audioEngine()->outputSampleRate());

                _sampleClip->setEndTime((_sampleClip->startTime() - (_sampleClip->sampleBuffer()->offsetStartFrame() / AudioManager::audioEngine()->outputSampleRate() * 1000.0))
                    + _sampleClip->currentFileDuration());
            }

        } else {

            if (startFrame > endFrame) {

            } else {
                _sampleClip->sampleBuffer()->setBaseStartFrame(startFrame);
                _sampleClip->sampleBuffer()->setBaseEndFrame(endFrame);

                _sampleClip->sampleBuffer()->setOffsetEndFrame(_sampleClip->sampleBuffer()->offsetEndFrame() + diff / 1000.0 * AudioManager::audioEngine()->outputSampleRate());
                _sampleClip->setEndTime(_shadowEndTime);
            }
        }
        return 1;
    }

    if (_pressAndHoldAction == MouseAction_ResizeLeft) {

        double timePerPixel = _lastWidth / _clipDuration;
        double final_xpos = 0;
        double final_ypos = 0;
        final_xpos = _areaInfo->findSnap(event->pos().x());

        auto _shadowStartTime = (final_xpos)*1.0 / timePerPixel + _startTime;

        auto startFrame = _shadowStartTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        auto endFrame = _sampleClip->endTime() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        _sampleClip->setFake(true);
        double diff = _shadowStartTime - _sampleClip->startTime();
        auto duration = (_sampleClip->sampleBuffer()->offsetEndFrame() - _sampleClip->sampleBuffer()->offsetStartFrame()) / AudioManager::audioEngine()->outputSampleRate() * 1000.0;

        if (((_sampleClip->sampleBuffer()->offsetStartFrame() + diff / AudioManager::audioEngine()->outputSampleRate() * 1000.0)) <= 0) {

            if (startFrame > endFrame) {

            } else {
                auto startFrame = (_sampleClip->endTime() - duration) / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                _sampleClip->sampleBuffer()->setBaseStartFrame(startFrame);
                _sampleClip->sampleBuffer()->setBaseEndFrame(endFrame);

                _sampleClip->sampleBuffer()->setOffsetStartFrame(0);

                _sampleClip->setStartTime(_sampleClip->endTime() - (_sampleClip->sampleBuffer()->offsetEndFrame() / AudioManager::audioEngine()->outputSampleRate() * 1000.0));

                _sampleClip->setStartTimeOffset(0);
            }

        } else {

            if (startFrame > endFrame) {

            } else {
                _sampleClip->sampleBuffer()->setBaseStartFrame(startFrame);
                _sampleClip->sampleBuffer()->setBaseEndFrame(endFrame);

                _sampleClip->sampleBuffer()->setOffsetStartFrame(_sampleClip->sampleBuffer()->offsetStartFrame() + diff / 1000.0 * AudioManager::audioEngine()->outputSampleRate());
                _sampleClip->setStartTime(_shadowStartTime);

                _sampleClip->setStartTimeOffset(_sampleClip->sampleBuffer()->offsetStartFrame() / 256.0);
            }
        }
        return 1;
    }

    return 0;
}

int ClipItem::hoverMoveEvent(QHoverEvent* event)
{

    if (_sampleClip == nullptr) {
        return 0;
    }

    auto _startTime = _areaInfo->startTime();
    auto _clipDuration = _areaInfo->clipDuration();
    auto _lastWidth = _areaInfo->lastWidth();

    if (_clipDuration <= 0) {
        return 0;
    }

    double timePerPixel = _lastWidth / _clipDuration;
    double pix1 = ((_sampleClip->startTime() - _startTime) * timePerPixel);
    double pix2 = ((_sampleClip->endTime() - _startTime) * timePerPixel);

    QRect rect(pix1, _rect.y(), pix2 - pix1, _areaInfo->titleHeight());

    QRect rightExtend(pix2 - _areaInfo->titleHeight(), _rect.y(), _areaInfo->titleHeight(), _areaInfo->titleHeight());
    QRect leftExtend(pix1, _rect.y(), _areaInfo->titleHeight(), _areaInfo->titleHeight());

    _mouseHoverAction = MouseAction_None;

    if (rightExtend.contains(event->pos())) {
        _mouseHoverAction = MouseAction_ResizeRight;

        return 1;
    } else if (leftExtend.contains(event->pos())) {

        _mouseHoverAction = MouseAction_ResizeLeft;
        return 1;
    } else {
        if (!rect.contains(event->pos())) {

            _mouseHoverAction = MouseAction_None;

        } else {

            _mouseHoverAction = MouseAction_Move;
            return 1;
        }
    }

    return 0;
}

int ClipItem::mouseReleaseEvent(QMouseEvent* event)
{

    if (_sampleClip == nullptr) {

        return 0;
    }

    Q_EMIT sigSetCursor(QCursor(Qt::ArrowCursor));

    if (_pressAndHoldAction == ClipItem::MouseAction_ResizeLeft
        || _pressAndHoldAction == ClipItem::MouseAction_ResizeRight) {

        qint64 startFrame = _sampleClip->sampleBuffer()->baseStartFrame();
        qint64 endFrame = _sampleClip->sampleBuffer()->baseEndFrame();

        _sampleClip->setFake(false);
        _sampleClip->movePosition(startFrame / 256.0);
        _sampleClip->changeLength((endFrame - startFrame) / 256.0);
        _sampleClip->sampleChange();
        _pressAndHoldAction = ClipItem::MouseAction_None;

        return 1;
    }

    _pressAndHoldAction = ClipItem::MouseAction_None;

    return 0;
}
