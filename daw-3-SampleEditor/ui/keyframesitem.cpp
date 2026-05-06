#include "keyframesitem.h"

KeyFramesItem::KeyFramesItem(int trackIndex, AreaInfo* areaInfo)
    : _areaInfo(areaInfo)
    , _trackIndex(trackIndex)
    , _keyFramesEnabled(false)
    , _automation(new ObjectPosAutomation(this))
{
}

bool KeyFramesItem::contains(SelectedArea selectedArea)
{

    int _subTrackIndex = -1;
    bool case1 = (_trackIndex > selectedArea.firstTrackIndex() && _trackIndex < selectedArea.lastTrackIndex());
    bool case2 = (_trackIndex == selectedArea.firstTrackIndex() && selectedArea.firstTrackIndex() != selectedArea.lastTrackIndex()
        && _subTrackIndex >= selectedArea._firstSubTrackIndex);
    bool case3 = (_trackIndex == selectedArea.lastTrackIndex() && selectedArea.firstTrackIndex() != selectedArea.lastTrackIndex()
        && _subTrackIndex <= selectedArea._lastSubTrackIndex);
    bool case4 = (selectedArea.firstTrackIndex() == _trackIndex && selectedArea.firstTrackIndex() == selectedArea.lastTrackIndex()
        && _subTrackIndex >= selectedArea._firstSubTrackIndex && _subTrackIndex <= selectedArea._lastSubTrackIndex);

    if (case1 || case2 || case3 || case4) {

        return true;
    } else {

        return false;
    }
}

QList<KeyFramesItem::KeyFramesPoint> KeyFramesItem::createCopyList(SelectedArea selectedArea)
{

    QList<KeyFramesItem::KeyFramesPoint> _list;

    auto _startMarker = selectedArea.area().left();
    auto _endMarker = selectedArea.area().right();

    int _subTrackIndex = -1;

    if ((_trackIndex > selectedArea.firstTrackIndex() && _trackIndex < selectedArea.lastTrackIndex())
        || (_trackIndex == selectedArea.firstTrackIndex() && selectedArea.firstTrackIndex() != selectedArea.lastTrackIndex() && _subTrackIndex >= selectedArea._firstSubTrackIndex)
        || (_trackIndex == selectedArea.lastTrackIndex() && selectedArea.firstTrackIndex() != selectedArea.lastTrackIndex() && _subTrackIndex <= selectedArea._lastSubTrackIndex)
        || (selectedArea.firstTrackIndex() == _trackIndex && selectedArea.firstTrackIndex() == selectedArea.lastTrackIndex() && _subTrackIndex >= selectedArea._firstSubTrackIndex
            && _subTrackIndex <= selectedArea._lastSubTrackIndex)) {

    } else {

        return QList<KeyFramesItem::KeyFramesPoint>();
    }

    if (qAbs(_startMarker - _endMarker) <= 0.0)
        return QList<KeyFramesItem::KeyFramesPoint>();

    if (_points.size() == 0)
        return QList<KeyFramesItem::KeyFramesPoint>();

    int sIndex = -1;
    int eIndex = _points.size();

    for (int i = 0; i < _points.size(); i++) {
        if (_startMarker < _points[i].point.x()) {
            sIndex = i;
            break;
        }
    }

    for (int i = _points.size() - 1; i >= 0; i--) {
        if (_endMarker > _points[i].point.x()) {
            sIndex = i;
            break;
        }
    }

    if (sIndex == -1 && eIndex == _points.size()) {
        return _points;
    }

    for (int i = 0; i < _points.size(); i++) {
        if (_startMarker < _points[i].point.x()) {
            sIndex = i;
            break;
        }
    }

    if (sIndex < 0)
        sIndex = 0;

    if (eIndex >= _points.size())
        eIndex == _points.size() - 1;

    for (int i = sIndex; i < eIndex; i++) {
        _list.append(_points[i]);
    }

    return _list;
}

void KeyFramesItem::saveInsertUndoItems(double time, int type, double posX, double posY, double posZ)
{
}

void KeyFramesItem::saveObjectUndoItems(double posX, double posY, double posZ)
{
}

void KeyFramesItem::saveRemoveUndoItems(double time, int type, double posX, double posY, double posZ)
{
}

QJsonObject KeyFramesItem::applyUndo(QJsonObject obj)
{

    qDebug() << "applyUndo" << obj;
    QJsonObject jObject2;

    return jObject2;
}

QJsonObject KeyFramesItem::save()
{
    QJsonObject jObject2;
    jObject2["track_index"] = _trackIndex;
    QJsonArray jsonArray;
    for (int i = 0; i < _points.size(); i++) {
        QJsonObject jsonObj;
        jsonObj["time"] = _points[i].point.x();
        jsonObj["type"] = _points[i].type;
        jsonObj["posx"] = _points[i].posX;
        jsonObj["posy"] = _points[i].posY;
        jsonObj["posz"] = _points[i].posZ;
        jsonArray.append(jsonObj);
    }
    jObject2["state"] = jsonArray;
    // Persist bezier tangents and interp types
    jObject2["automation"] = _automation->save();
    return jObject2;
}

void KeyFramesItem::load(QJsonObject obj)
{
    _points.clear();
    _automation->blockSignals(true);
    Q_EMIT sigUpdate();
    Q_EMIT _areaInfo->sigKeyFrameClear(_trackIndex);

    QJsonArray array = obj["state"].toArray();
    for (int i = 0; i < array.size(); i++) {
        QJsonObject jsonObj = array[i].toObject();
        addKeyFrame(jsonObj["time"].toDouble(), jsonObj["type"].toInt(),
                    jsonObj["posx"].toDouble(), jsonObj["posy"].toDouble(), jsonObj["posz"].toDouble());
    }

    // Restore bezier tangents if present (written by the new save path)
    if (obj.contains("automation"))
        _automation->load(obj["automation"].toObject());

    _automation->blockSignals(false);
    Q_EMIT _automation->changed();
}

int KeyFramesItem::insertToPoints(QPointF point, int type, double x, double y, double z)
{

    int _index = -1;
    if (_points.size() == 0) {
        if (type == 0)
            _points.append(KeyFramesPoint { point, KeyFramesType_Bezier, x, y, z });
        else
            _points.append(KeyFramesPoint { point, KeyFramesType_Linear, x, y, z });

        return _points.size() - 1;
    }
    for (int i = _points.size() - 1; i >= 0; --i) {
        if (point.x() > _points[i].point.x()) {
            if (_index == -1) {

                if (type == 0)
                    _points.append(KeyFramesPoint { point, KeyFramesType_Bezier, x, y, z });
                else
                    _points.append(KeyFramesPoint { point, KeyFramesType_Linear, x, y, z });

                return _points.size() - 1;
            } else {
                if (type == 0)
                    _points.insert(_index, KeyFramesPoint { point, KeyFramesType_Bezier, x, y, z });
                else
                    _points.insert(_index, KeyFramesPoint { point, KeyFramesType_Linear, x, y, z });

                return _index;
            }
        } else {
            _index = i;
        }
    }

    if (type == 0)
        _points.prepend(KeyFramesPoint { point, KeyFramesType_Bezier, x, y, z });
    else
        _points.prepend(KeyFramesPoint { point, KeyFramesType_Linear, x, y, z });

    return 0;
}

bool KeyFramesItem::keyFramesEnabled() const
{
    return _keyFramesEnabled;
}

void KeyFramesItem::setKeyFramesEnabled(bool newKeyFramesEnabled)
{
    _keyFramesEnabled = newKeyFramesEnabled;
}

void KeyFramesItem::setObjectPosition(double x, double y, double z)
{
    m_objectPositionX = x;
    m_objectPositionY = y;
    m_objectPositionZ = z;

    qDebug() << "setObjectPosition" << m_objectPositionX << m_objectPositionY << m_objectPositionZ;
    //    saveObjectUndoItems(x, y, z);
}

void KeyFramesItem::addKeyFrame(qint64 time, int type, double x, double y, double z)
{
    auto line = getLine();
    _lastInsertedIndex = insertToPoints(QPointF(time, line.p1().y()), type, x, y, z);

    // Keep ObjectPosAutomation in sync
    const KeyInterp interp = (type == KeyFramesType_Bezier) ? KeyInterp::Bezier
                           : (type == KeyFramesType_Hold)   ? KeyInterp::Hold
                           : KeyInterp::Linear;
    _automation->addKey(time, QVector3D(float(x), float(y), float(z)), interp);

    Q_EMIT sigUpdate();
}

void KeyFramesItem::addKeyFrame(qint64 time, int type)
{
    addKeyFrame(time, type, m_objectPositionX, m_objectPositionY, m_objectPositionZ);
}

void KeyFramesItem::removeKeyFrame(qint64 time)
{
    auto line = getLine();
    int index = findPoint(QPointF(time, line.p1().y()));

    if (index >= 0) {
        _points.takeAt(index);
        _automation->removeKey(time);
        Q_EMIT sigUpdate();
    }
}

QMap<qint64, int> KeyFramesItem::keyFrames() const
{
    QMap<qint64, int> list;
    for (int i = 0; i < _points.size(); i++) {
        list.insert(qint64(_points[i].point.x()), _points[i].type);
    }
    return list;
}

QMap<qint64, double> KeyFramesItem::objectPositionX() const
{
    QMap<qint64, double> list;
    for (int i = 0; i < _points.size(); i++) {
        list.insert(qint64(_points[i].point.x()), _points[i].posX);
    }

    return list;
}

QMap<qint64, double> KeyFramesItem::objectPositionY() const
{
    QMap<qint64, double> list;
    for (int i = 0; i < _points.size(); i++) {
        list.insert(qint64(_points[i].point.x()), _points[i].posY);
    }

    return list;
}

QMap<qint64, double> KeyFramesItem::objectPositionZ() const
{
    QMap<qint64, double> list;
    for (int i = 0; i < _points.size(); i++) {
        list.insert(qint64(_points[i].point.x()), _points[i].posZ);
    }

    return list;
}

int KeyFramesItem::keyFrameType() const
{
    return _keyFrameType;
}

void KeyFramesItem::setKeyFrameType(int newKeyFrameType)
{
    _keyFrameType = newKeyFrameType;
}

int KeyFramesItem::findPoint(QPointF point)
{
    for (int i = 0; i < _points.size(); i++) {

        if (qAbs(point.x() - _points[i].point.x()) < 100) {
            return i;
        }
    }
    return -1;
}

QPointF KeyFramesItem::mapPointToLine(QPointF point)
{
    auto line = getLine();
    QPointF mapped = MathHelper1::mapPointToLine(line.p1(), line.p2(), point);
    return mapped;
}

QPolygonF KeyFramesItem::lineArea()
{
    QPolygonF _polyInner;
    QPolygonF _polyOuter;
    auto line = getLine();
    MathHelper1::lineAreas(line.p1(), line.p2(), _polyInner, _polyOuter, 10, 20);
    return _polyInner;
}

QLineF KeyFramesItem::getLine()
{
    auto p1 = QPointF(pixel2Time(_rect.left()), (_rect.top() + _rect.bottom()) / 2 + _areaInfo->titleHeight());
    auto p2 = QPointF(pixel2Time(_rect.right()), (_rect.top() + _rect.bottom()) / 2 + _areaInfo->titleHeight());

    return QLineF(p1, p2);
}

void KeyFramesItem::draw(IPainter* painter)
{
    if (_keyFramesEnabled == false)
        return;

    drawLine(painter);
    drawKeyPoints(painter);
    drawBezierHandles(painter);
    if (_mouseMoved == false) {
        drawKeyHoverPoint(painter);
    }
}

void KeyFramesItem::drawBezierHandles(IPainter* painter)
{
    for (int i = 0; i < _points.size(); i++) {
        if (_points[i].type != KeyFramesType_Bezier)
            continue;

        const ObjectKeyFrame* kf = _automation->key(qint64(_points[i].point.x()));
        if (!kf)
            continue;

        const float kx = float(time2Pixel(_points[i].point.x()));
        const float ky = float(_points[i].point.y());

        // tangentOut handle (outgoing, to the right visually)
        // Represent as a screen-space offset: use tangentOut.x() as time-pixels, .y() as vertical pixels
        const float ohx = kx + float(kf->tangentOut.x());
        const float ohy = ky - float(kf->tangentOut.y());

        // tangentIn handle (incoming, stored on this keyframe but shown to the left)
        const float ihx = kx - float(kf->tangentIn.x());
        const float ihy = ky + float(kf->tangentIn.y());

        // Draw dashed lines from key to handles
        painter->setStrokeStyle("#aaaaaa");
        painter->setLineWidth(1);
        painter->beginPath();
        painter->moveTo(kx, ky);
        painter->lineTo(ohx, ohy);
        painter->stroke();

        painter->beginPath();
        painter->moveTo(kx, ky);
        painter->lineTo(ihx, ihy);
        painter->stroke();

        // Draw handle dots
        painter->setFillStyle("#ffcc00");
        painter->setStrokeStyle("#ffcc00");
        painter->beginPath();
        painter->circle(ohx, ohy, 4);
        painter->fill();

        painter->beginPath();
        painter->circle(ihx, ihy, 4);
        painter->fill();
    }
}

int KeyFramesItem::findHandle(QPointF screenPos, HandleSide& side)
{
    const float hitRadius = 8.f;
    for (int i = 0; i < _points.size(); i++) {
        if (_points[i].type != KeyFramesType_Bezier)
            continue;
        const ObjectKeyFrame* kf = _automation->key(qint64(_points[i].point.x()));
        if (!kf) continue;

        const float kx = float(time2Pixel(_points[i].point.x()));
        const float ky = float(_points[i].point.y());

        const float ohx = kx + float(kf->tangentOut.x());
        const float ohy = ky - float(kf->tangentOut.y());
        if (QVector2D(screenPos.x() - ohx, screenPos.y() - ohy).length() < hitRadius) {
            side = HandleSide::Out;
            return i;
        }

        const float ihx = kx - float(kf->tangentIn.x());
        const float ihy = ky + float(kf->tangentIn.y());
        if (QVector2D(screenPos.x() - ihx, screenPos.y() - ihy).length() < hitRadius) {
            side = HandleSide::In;
            return i;
        }
    }
    side = HandleSide::None;
    return -1;
}

int KeyFramesItem::hoverMoveEvent(QHoverEvent* event)
{
    if (_keyFramesEnabled == false)
        return 0;

    _mouseHoverX = event->pos().x();
    _mouseHoverY = event->pos().y();
    return 0;
}

int KeyFramesItem::mousePressEvent(QMouseEvent* event)
{
    if (_keyFramesEnabled == false)
        return 0;

    _mousePressed = true;
    _mouseMoved = false;
    _mousePressedX = event->pos().x();
    _mousePressedY = event->pos().y();
    _handleDragPoint = -1;
    _handleDragSide  = HandleSide::None;

    // Check bezier handle hit first
    const QPointF screenPos(event->pos().x(), event->pos().y());
    HandleSide hside = HandleSide::None;
    int hi = findHandle(screenPos, hside);
    if (hi >= 0) {
        _handleDragPoint = hi;
        _handleDragSide  = hside;
        return 1;
    }

    auto _polyInner = lineArea();
    if (_polyInner.containsPoint(QPointF(pixel2Time(_mousePressedX), _mousePressedY), Qt::WindingFill)) {
        QPointF mapped = mapPointToLine(QPointF(pixel2Time(_mousePressedX), _mousePressedY));
        _currentPointIndex = findPoint(mapped);

        _nextPointIndex = _points.size();
        _lastPointIndex = -1;
        if (_currentPointIndex >= 0) {
            _nextPointIndex = _currentPointIndex + 1;
            _lastPointIndex = _currentPointIndex - 1;
        }
        return 1;
    }
    return 0;
}

double KeyFramesItem::time2Pixel(double time)
{

    return ((time - _areaInfo->startTime()) / _areaInfo->clipDuration() * _areaInfo->lastWidth());
}

double KeyFramesItem::pixel2Time(double pixel)
{

    return _areaInfo->startTime() + pixel / (double)_areaInfo->lastWidth() * _areaInfo->clipDuration();
}

void KeyFramesItem::setRect(const QRect& rect)
{

    IPainterItem::setRect(rect);
}

int KeyFramesItem::mouseReleaseEvent(QMouseEvent* event)
{

    if (_keyFramesEnabled == false)
        return 0;

    // Right-click on existing keyframe: cycle interpolation type Hold→Linear→Bezier→Hold
    if (event->button() == Qt::RightButton && _mouseMoved == false) {
        auto _polyInner = lineArea();
        if (_polyInner.containsPoint(QPointF(pixel2Time(_mousePressedX), _mousePressedY), Qt::WindingFill)) {
            QPointF mapped = mapPointToLine(QPointF(pixel2Time(_mousePressedX), _mousePressedY));
            auto index = findPoint(mapped);
            if (index >= 0) {
                const int nextType = (_points[index].type + 1) % 3;
                _points[index].type = nextType;
                const KeyInterp nextInterp = (nextType == KeyFramesType_Bezier) ? KeyInterp::Bezier
                                           : (nextType == KeyFramesType_Hold)   ? KeyInterp::Hold
                                           : KeyInterp::Linear;
                const qint64 kfTime = qint64(_points[index].point.x());
                _automation->setInterp(kfTime, nextInterp);
                Q_EMIT _areaInfo->sigInterpChanged(_trackIndex, kfTime, int(nextInterp));
                _mousePressed = false;
                _mouseMoved   = false;
                Q_EMIT sigUpdate();
                return 1;
            }
        }
        _mousePressed = false;
        _mouseMoved   = false;
        return 0;
    }

    if (_mouseMoved == false && _mousePressed == true) {

        auto _polyInner = lineArea();
        if (_polyInner.containsPoint(QPointF(pixel2Time(_mousePressedX), _mousePressedY), Qt::WindingFill)) {
            QPointF mapped = mapPointToLine(QPointF(pixel2Time(_mousePressedX), _mousePressedY));
            auto index = findPoint(mapped);

            if (index >= 0) {

                saveRemoveUndoItems(mapped.x(), _keyFrameType, _points[index].posX, _points[index].posY, _points[index].posZ);

                Q_EMIT _areaInfo->sigRemoveKeyFrame(_trackIndex, _points[index].point.x());

                _points.removeAt(index);

            } else {

                saveInsertUndoItems(mapped.x(), _keyFrameType, m_objectPositionX, m_objectPositionY, m_objectPositionZ);

                _lastInsertedIndex = insertToPoints(mapped, _keyFrameType, m_objectPositionX, m_objectPositionY, m_objectPositionZ);

                Q_EMIT _areaInfo->sigAddKeyFrame(_trackIndex, mapped.x(), _keyFrameType);
            }
        }
    }
    if (_mouseMoved) {

        auto mapped = _points[_currentPointIndex].point;
        m_objectPositionX = _points[_currentPointIndex].posX;
        m_objectPositionY = _points[_currentPointIndex].posY;
        m_objectPositionZ = _points[_currentPointIndex].posZ;
        saveInsertUndoItems(mapped.x(), _keyFrameType, m_objectPositionX, m_objectPositionY, m_objectPositionZ);
    }

    _mousePressed = false;
    _mouseMoved = false;

    return 0;
}

void KeyFramesItem::findSnap(QVector<QPair<double, double>> _snapPoints, double posX, double posY, double& final_xpos, double& final_ypos)
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

int KeyFramesItem::mouseMoveEvent(QMouseEvent* event)
{
    if (_keyFramesEnabled == false)
        return 0;

    // Handle bezier tangent dragging
    if (_mousePressed && _handleDragPoint >= 0) {
        const float kx = float(time2Pixel(_points[_handleDragPoint].point.x()));
        const float ky = float(_points[_handleDragPoint].point.y());
        const float dx = float(event->pos().x()) - kx;
        const float dy = ky - float(event->pos().y());  // invert Y so up = positive

        const qint64 time = qint64(_points[_handleDragPoint].point.x());
        const ObjectKeyFrame* kf = _automation->key(time);
        if (kf) {
            QVector3D tOut = kf->tangentOut;
            QVector3D tIn  = kf->tangentIn;
            if (_handleDragSide == HandleSide::Out) {
                tOut = QVector3D(dx, dy, tOut.z());
            } else {
                tIn = QVector3D(-dx, -dy, tIn.z());
            }
            _automation->setTangents(time, tIn, tOut);
            Q_EMIT sigUpdate();
        }
        _mouseMoved = true;
        return 1;
    }

    if (_mousePressed) {

        if (_currentPointIndex == -1) {
            auto _polyInner = lineArea();
            if (_polyInner.containsPoint(QPointF(pixel2Time(_mousePressedX), _mousePressedY), Qt::WindingFill)) {
                QPointF mapped = mapPointToLine(QPointF(pixel2Time(_mousePressedX), _mousePressedY));

                if (_lastInsertedIndex >= 0) {
                    saveInsertUndoItems(mapped.x(), _keyFrameType, _points[_lastInsertedIndex].posX, _points[_lastInsertedIndex].posY, _points[_lastInsertedIndex].posZ);
                } else {
                    saveInsertUndoItems(mapped.x(), _keyFrameType, m_objectPositionX, m_objectPositionY, m_objectPositionZ);
                }

                _currentPointIndex = insertToPoints(mapped, _keyFrameType, m_objectPositionX, m_objectPositionY, m_objectPositionZ);
                _lastInsertedIndex = _currentPointIndex;

                _nextPointIndex = _currentPointIndex + 1;
                _lastPointIndex = _currentPointIndex - 1;

                Q_EMIT _areaInfo->sigAddKeyFrame(_trackIndex, mapped.x(), _keyFrameType);
            }
            _mousePressed = false;
        }

        double final_xpos = 0;
        double final_ypos = 0;
        final_xpos = _areaInfo->findSnap(event->pos().x());

        auto point = _points[_currentPointIndex].point;
        if (qAbs(pixel2Time(final_xpos) - point.x()) > 10) {
            _mouseMoved = true;

            if (_points.size() >= 2) {

                if (_nextPointIndex < _points.size() && _lastPointIndex >= 0) {
                    if (pixel2Time(final_xpos) < _points[_nextPointIndex].point.x() && pixel2Time(final_xpos) > _points[_lastPointIndex].point.x()) {
                        point.setX(pixel2Time(final_xpos));
                    }
                }
                if (_nextPointIndex < _points.size() && _lastPointIndex < 0) {
                    if (pixel2Time(final_xpos) < _points[_nextPointIndex].point.x()) {
                        point.setX(pixel2Time(final_xpos));
                    }
                }
                if (_nextPointIndex >= _points.size() && _lastPointIndex >= 0) {
                    if (pixel2Time(final_xpos) > _points[_lastPointIndex].point.x()) {
                        point.setX(pixel2Time(final_xpos));
                    }
                }
            } else {
                point.setX(pixel2Time(final_xpos));
            }

            Q_EMIT _areaInfo->sigEditKeyFrame(_trackIndex, _points[_currentPointIndex].point.x(), point.x(), _points[_currentPointIndex].type);
            _points[_currentPointIndex].point = point;

            Q_EMIT sigUpdate();

            return 1;
        }
    }
    return 0;
}

void KeyFramesItem::drawLine(IPainter* painter)
{
    painter->beginPath();
    painter->setFillStyle("#eeeeee");
    painter->setStrokeStyle("#eeeeee");
    painter->setLineWidth(1);
    auto line = getLine();

    painter->drawLine(time2Pixel(line.p1().x()), line.p1().y(), time2Pixel(line.p2().x()), line.p2().y());
    painter->stroke();
}

void KeyFramesItem::drawKeyPoints(IPainter* painter)
{

    for (int i = 0; i < _points.size(); i++) {
        painter->beginPath();
        painter->setFillStyle("#eeeeee");
        painter->setStrokeStyle("#eeeeee");

        auto point = _points[i].point;

        if (_mouseMoved && i == _currentPointIndex) {
            painter->setFillStyle("#ff3333");
            painter->setStrokeStyle("#ff3333");
        }
        const float px = float(time2Pixel(point.x()));
        const float py = float(point.y());
        if (_points[i].type == KeyFramesType_Hold) {
            painter->fillRect(px - 5, py - 5, 10, 10);
        } else if (_points[i].type == KeyFramesType_Bezier) {
            painter->circle(px, py, 4);
            painter->fill();
        } else {  // Linear (diamond)
            painter->drawDiamond(QPointF(px, py), 8);
            painter->fill();
        }
    }
}

void KeyFramesItem::drawKeyHoverPoint(IPainter* painter)
{

    auto _polyInner = lineArea();
    if (_polyInner.containsPoint(QPointF(pixel2Time(_mouseHoverX), _mouseHoverY), Qt::WindingFill)) {
        painter->beginPath();

        QPointF mapped = mapPointToLine(QPointF(pixel2Time(_mouseHoverX), _mouseHoverY));
        auto index = findPoint(mapped);

        if (index >= 0) {
            mapped = _points[index].point;
            painter->setFillStyle("#ff3333");
            painter->setStrokeStyle("#ff3333");

            const float hx = float(time2Pixel(mapped.x()));
            const float hy = float(mapped.y());
            if (_points[index].type == KeyFramesType_Hold) {
                painter->fillRect(hx - 5, hy - 5, 10, 10);
            } else if (_points[index].type == KeyFramesType_Bezier) {
                painter->circle(hx, hy, 4);
                painter->fill();
            } else {  // Linear
                painter->drawDiamond(QPointF(hx, hy), 8);
                painter->fill();
            }

        } else {
            painter->setFillStyle("#eeeeee");
            painter->setStrokeStyle("#eeeeee");
            painter->circle(time2Pixel(mapped.x()), mapped.y(), 4);
            painter->fill();
        }
    }
}
