#include "objectpositionitem.h"
#include "trackitem.h"

ObjectPositionItem::ObjectPositionItem(int trackIndex, AreaInfo* areaInfo)
    : _areaInfo(areaInfo)
    , _trackIndex(trackIndex)
    , _enabled(true)
{
    connect(&_checkPressAndHold, &QTimer::timeout, this, &ObjectPositionItem::checkPressAndHoldTimeout);
    _checkPressAndHold.setInterval(300);
}

void ObjectPositionItem::update(QRect windowRect)
{
    _windowRect = windowRect;
}

bool ObjectPositionItem::enabled() const
{
    return _enabled;
}

void ObjectPositionItem::setEnabled(bool value)
{
    _enabled = value;
}

double ObjectPositionItem::time2Pixel(double time)
{

    return ((time - _areaInfo->startTime()) / _areaInfo->clipDuration() * _areaInfo->lastWidth());
}

double ObjectPositionItem::pixel2Time(double pixel)
{

    return _areaInfo->startTime() + pixel / (double)_areaInfo->lastWidth() * _areaInfo->clipDuration();
}

double ObjectPositionItem::value() const
{
    return _value;
}

int ObjectPositionItem::findPoint(QPointF point)
{
    for (int i = 0; i < _points.size(); i++) {

        auto x1 = time2Pixel(point.x());
        auto x2 = time2Pixel(_points[i].x());
        if (qAbs(x1 - x2) < 4 && qAbs(point.y() - _points[i].y()) < 0.05) {
            return i;
        }
    }
    return -1;
}

int ObjectPositionItem::findLine(double time)
{
    auto _lines = getLines();
    for (int i = 0; i < _lines.size(); i++) {
        if (time >= _lines[i].x1() && time <= _lines[i].x2()) {
            return i;
        }
    }
    return 0;
}

void ObjectPositionItem::checkPressAndHoldTimeout()
{

    _checkPressAndHold.stop();
    if (_mousePressed) {

        _mouseMoved = true;
        auto _polyInners = lineAreas();

        auto time = pixel2Time(_mousePressedX);
        double Y = (_mousePressedY - _windowRect.top()) / (double)(_windowRect.height());

        for (int i = 0; i < _polyInners.size(); i++) {
            if (_polyInners[i].containsPoint(QPointF(time, Y), Qt::WindingFill)) {
                QPointF mapped = mapPointToLine(i, QPointF(_mousePressedX, _mousePressedY));

                auto index = findPoint(mapped);

                if (index >= 0) {
                    _points.removeAt(index);
                    break;
                } else {
                    _points.insert(i, mapped);
                    _currentPointIndex = i;
                    break;
                }
            }
        }
    }
}

QVector<QLineF> ObjectPositionItem::getLines()
{

    QVector<QLineF> lines;
    if (_points.size() > 0) {
        lines.append(QLineF(QPointF(0, _points.first().y()), QPointF(_points.first().x(), _points.first().y())));

        if (_points.size() > 1) {
            for (int i = 0; i < _points.size() - 1; i++) {
                lines.append(QLineF(QPointF(_points[i].x(), _points[i].y()), QPointF(_points[i + 1].x(), _points[i + 1].y())));
            }
        }

        lines.append(QLineF(QPointF(_points.last().x(), _points.last().y()), QPointF(_areaInfo->clipDuration(), _points.last().y())));

    } else {

        lines.append(QLineF(QPointF(0, 0.5), QPointF(_areaInfo->clipDuration(), 0.5)));
    }

    return lines;
}

QPointF ObjectPositionItem::mapPointToLine(int index, QPointF pixelPoint)
{

    auto time = pixel2Time(pixelPoint.x());
    double Y = (pixelPoint.y() - _windowRect.top()) / (double)(_windowRect.height());

    QPointF point = QPointF(time, Y);
    auto lines = getLines();

    QPointF mapped = MathHelper1::mapPointToLine(lines[index].p1(), lines[index].p2(), point);

    return mapped;
}

QVector<QPolygonF> ObjectPositionItem::lineAreas()
{
    QVector<QPolygonF> polyInners;
    QPolygonF _polyOuter;
    QPolygonF _polyInner;
    auto lines = getLines();
    for (int i = 0; i < lines.size(); i++) {
        MathHelper1::lineAreas2(lines[i].p1(), lines[i].p2(), _polyInner, _polyOuter);
        polyInners.append(_polyInner);
    }

    return polyInners;
}

void ObjectPositionItem::draw(IPainter* painter)
{

    if (_enabled == false)
        return;

    drawLine(painter);
    drawKeyPoints(painter);
    if (_mouseMoved == false) {
        drawKeyHoverPoint(painter);
    }
}

int ObjectPositionItem::hoverMoveEvent(QHoverEvent* event)
{

    if (_enabled == false)
        return 0;

    _mouseHoverX = event->pos().x();
    _mouseHoverY = event->pos().y();
    return 0;
}

int ObjectPositionItem::mousePressEvent(QMouseEvent* event)
{

    if (_enabled == false)
        return 0;

    _mousePressed = true;
    _mouseMoved = false;
    _mousePressedX = event->pos().x();
    _mousePressedY = event->pos().y();

    auto time = pixel2Time(_mousePressedX);
    double Y = (_mousePressedY - _windowRect.top()) / (double)(_windowRect.height());

    auto index = findLine(time);

    _value = MathHelper1::lineEquationXtoY(getLines()[index], _mousePressedX) * (_maxValue - _minValue);
    Q_EMIT sigValueChanged(_trackIndex);

    auto _polyInners = lineAreas();

    for (int i = 0; i < _polyInners.size(); i++) {

        if (_polyInners[i].containsPoint(QPointF(time, Y), Qt::WindingFill)) {
            QPointF mapped = mapPointToLine(i, QPointF(_mousePressedX, _mousePressedY));
            _currentPointIndex = findPoint(mapped);
            if (_currentPointIndex < 0) {
                _checkPressAndHold.start();
            }

            return 1;
        }
    }

    return 0;
}

int ObjectPositionItem::mouseReleaseEvent(QMouseEvent* event)
{

    if (_enabled == false)
        return 0;

    _checkPressAndHold.stop();
    if (_mouseMoved == false && _mousePressed == true) {

        auto _polyInners = lineAreas();

        auto time = pixel2Time(_mousePressedX);
        double Y = (_mousePressedY - _windowRect.top()) / (double)(_windowRect.height());

        for (int i = 0; i < _polyInners.size(); i++) {
            if (_polyInners[i].containsPoint(QPointF(time, Y), Qt::WindingFill)) {
                QPointF mapped = mapPointToLine(i, QPointF(_mousePressedX, _mousePressedY));

                auto index = findPoint(mapped);

                if (index >= 0) {
                    _points.removeAt(index);
                    break;
                } else {
                    _points.insert(i, mapped);
                    break;
                }
            }
        }
    }

    _mousePressed = false;
    _mouseMoved = false;
    return 0;
}

int ObjectPositionItem::mouseMoveEvent(QMouseEvent* event)
{

    if (_enabled == false)
        return 0;

    if (_mousePressed && _currentPointIndex >= 0 && _points.size() > 0) {

        auto point = _points[_currentPointIndex];

        auto time = pixel2Time(event->pos().x());
        double Y = (event->pos().y() - _windowRect.top()) / (double)(_windowRect.height());

        if (time != point.x() || Y != point.y()) {

            if (Y < 0) {
                Y = 0;
            }
            if (Y > 1) {
                Y = 1;
            }
            _mouseMoved = true;
            point.setX(time);
            point.setY(Y);

            _points[_currentPointIndex] = point;

            _value = Y * (_maxValue - _minValue);

            Q_EMIT sigValueChanged(_trackIndex);

            return 1;
        }
    }
    return 0;
}

void ObjectPositionItem::drawLine(IPainter* painter)
{
    painter->beginPath();
    painter->setFillStyle("#eeeeee");
    painter->setStrokeStyle("#eeeeee");
    painter->setLineWidth(1);
    auto lines = getLines();

    for (int i = 0; i < lines.size(); i++) {
        double X1 = time2Pixel(lines[i].p1().x());
        double Y1 = lines[i].p1().y() * _windowRect.height() + _windowRect.top();

        double X2 = time2Pixel(lines[i].p2().x());
        double Y2 = lines[i].p2().y() * _windowRect.height() + _windowRect.top();

        painter->drawLine(X1, Y1, X2, Y2);
    }

    painter->stroke();
}

void ObjectPositionItem::drawKeyPoints(IPainter* painter)
{

    for (int i = 0; i < _points.size(); i++) {
        painter->beginPath();
        painter->setFillStyle("#eeeeee");
        painter->setStrokeStyle("#eeeeee");

        auto point = _points[i];

        if (_mouseMoved && i == _currentPointIndex) {
            painter->setFillStyle("#ff3333");
            painter->setStrokeStyle("#ff3333");
        }

        double X1 = time2Pixel(point.x());
        double Y1 = point.y() * _windowRect.height() + _windowRect.top();

        painter->circle(X1, Y1, 4);
        painter->fill();
    }
}

void ObjectPositionItem::drawKeyHoverPoint(IPainter* painter)
{

    auto _polyInners = lineAreas();
    for (int i = 0; i < _polyInners.size(); i++) {

        auto time = pixel2Time(_mouseHoverX);
        double Y = (_mouseHoverY - _windowRect.top()) / (double)(_windowRect.height());

        qDebug() << "p1" << _windowRect << time << Y << _mouseHoverY;
        if (_polyInners[i].containsPoint(QPointF(time, Y), Qt::WindingFill)) {

            painter->beginPath();

            QPointF mapped = mapPointToLine(i, QPointF(_mouseHoverX, _mouseHoverY));
            auto index = findPoint(QPointF(time, Y));

            qDebug() << "p2" << _windowRect << time << Y;
            if (index >= 0) {

                mapped = _points[index];

                double X1 = time2Pixel(mapped.x());
                double Y1 = mapped.y() * _windowRect.height() + _windowRect.top();

                painter->setFillStyle("#ff3333");
                painter->setStrokeStyle("#ff3333");

                painter->circle(X1, Y1, 4);
                painter->fill();

            } else {
                double X1 = time2Pixel(mapped.x());
                double Y1 = mapped.y() * _windowRect.height() + _windowRect.top();

                painter->setFillStyle("#eeeeee");
                painter->setStrokeStyle("#eeeeee");
                painter->circle(X1, Y1, 4);
                painter->fill();
            }
        }
    }
}
