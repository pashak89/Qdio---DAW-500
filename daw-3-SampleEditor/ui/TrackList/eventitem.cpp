#include "eventitem.h"
#include "eventitempainter.h"
#include <QCursor>
#include <QtMath>

#if (QT_VERSION >= 0x050150) // Qt5.15 -- qrand() deprecated ; Qt6 -- qrand() is gone.
#include <QRandomGenerator>
#endif /* (QT_VERSION >= 0x050150) */

EventItem::EventItem(QQuickItem* parent)
    : QNanoQuickItem(parent)
{
    m_activeItem = -1;
    m_pressedItem = -1;
    m_hovered = false;

    setVPos(0);
    didStart = true;
}

QNanoQuickItemPainter* EventItem::createItemPainter() const
{
    return new EventItemPainter();
}

#if (QT_VERSION >= 0x060000)
void EventItem::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
#else
void EventItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
#endif
{
#if (QT_VERSION >= 0x060000) // Qt6 -- renamed to geometryChange()
    QQuickItem::geometryChange(newGeometry, oldGeometry);
#else // Qt5 -- had geometryChanged()
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
#endif /* (QT_VERSION >= 0x060000) */
    if (width() > 0 && height() > 0) {
        generateRandomItems();
    }

    if (didStart && !m_hovered) {
        scaleY = (range / height()) / 30;

        y_pos = qRound(height() * max * scaleY / (max - min));
        //        qDebug()<< "in start y_pose = "<< y_pos;
        setVPos(y_pos);
        pix2Value();
        setVScale(scaleY);
        setDidFocus(false);
        setValue(0);
        myHeight = height();
        didStart = false;
    }
    if (!didStart && height() != myHeight && !m_hovered) {
        scaleY = (range / height()) / 30;
        setVScale(scaleY);
        myHeight = height();
    }
}

void EventItem::mousePressEvent(QMouseEvent* event)
{

    setIsPress(true);

    setIs2Click(false);
    setIsMoving(false);
    setIsRelease(false);

    if (!m_hovered) {
        setFocus(true);
        setDidFocus(true);
        hideCursor();
        setCursor(Qt::BlankCursor);
        pressPos = lastPos = event->pos().y();

        y_pos = event->pos().y() * scaleY;
        saveX = event->globalPos().x();

        setVPos(y_pos);
        setValue(qRound(pix2Value()));
    }
    update();
}

void EventItem::mouseReleaseEvent(QMouseEvent* event)
{

    Q_UNUSED(event);
    if (!mouseHoverEventsEnabled()) {
        m_activeItem = -1;
    }

    if (!m_hovered) {
        yRelease = (24 * 6 - value()) / (24 * 6 + 70) * (height() - knobeDiameter() - knobeBorder()) - knobeDiameter() / 2;

        setCursor(Qt::ArrowCursor);
        isIgnore = true;
        if (is2Click())
            QCursor().setPos(saveX, saveY);
        else
            QCursor().setPos(saveX, yRelease + itemY());
        setDidFocus(false);
    }

    setIsRelease(true);

    setIs2Click(false);
    setIsMoving(false);
    setIsPress(false);

    update();
}

void EventItem::mouseDoubleClickEvent(QMouseEvent* event)
{

    setIs2Click(true);

    setIsRelease(false);
    setIsMoving(false);
    setIsPress(false);

    if (!m_hovered) {
        setFocus(true);
        y_pos = height() * max * scaleY / (max - min);
        setVPos(y_pos);
        pix2Value();

        saveX = event->globalPos().x();
        saveY = event->globalPos().y();

        setValue(qRound(pix2Value()));
    }

    update();
}

void EventItem::mouseMoveEvent(QMouseEvent* event)
{

    setIsMoving(true);

    setIsRelease(false);
    setIs2Click(false);
    setIsPress(false);

    if (!m_hovered) {

        if (isIgnore) {
            isIgnore = false;
            return;
        }

        int diff = qAbs(pressPos - event->pos().y());
        if (diff >= 100) {
            QCursor().setPos(QCursor().pos().x(), mapToGlobal(QPoint(event->pos().x(), pressPos)).y());
            y_pos += (event->pos().y() - lastPos);
            y_pos = qBound(0.0, (double)y_pos, height() * scaleY);
            setVPos(y_pos);
            lastPos = pressPos;
            update();
            isIgnore = true;
            return;
        }

        y_pos += (event->pos().y() - lastPos);

        y_pos = qBound(0.0, (double)y_pos, height() * scaleY);
        setVPos(y_pos);
        pix2Value();

        lastPos = event->pos().y();

        setValue(qRound(pix2Value()));
    }

    update();
}

void EventItem::keyPressEvent(QKeyEvent* e)
{

    if (e->modifiers() == Qt::ShiftModifier && !m_hovered) {
        double maxLastPos = height() * scaleY;
        scaleY *= sense;
        setVScale(scaleY);
        double maxPos = height() * scaleY;
        y_pos = y_pos * maxPos / maxLastPos;
        setVPos(y_pos);
    }
}

void EventItem::keyReleaseEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Shift && !m_hovered) {
        double maxLastPos = height() * scaleY;
        scaleY /= sense;
        setVScale(scaleY);
        double maxPos = height() * scaleY;
        y_pos = y_pos * maxPos / maxLastPos;
        setVPos(y_pos);
    }
}

void EventItem::hoverEnterEvent(QHoverEvent* event)
{

    Q_UNUSED(event);
    m_hovered = true;
    setIsHover(true);
    update();
}

void EventItem::hoverLeaveEvent(QHoverEvent* event)
{

    Q_UNUSED(event);
    m_hovered = false;
    setIsHover(false);
    update();
}

void EventItem::hoverMoveEvent(QHoverEvent* event)
{

    Q_UNUSED(event);
    update();
}

#if (QT_VERSION >= 0x050150) // Qt5.15 -- qrand() deprecated ; Qt6 -- qrand() is gone.
#define QRAND() QRandomGenerator::global()->generate()
#else
#define QRAND() qrand()
#endif /* (QT_VERSION >= 0x050150) */

void EventItem::generateRandomItems()
{
    m_items.clear();
    double w = width();
    double h = height();
    double x = 0;
    double y = 0;
    m_items << QRectF(x, y, w, h);
    update();
}

// mouse area:
bool EventItem::is2Click() const { return m_is2Click; }
void EventItem::setIs2Click(const bool value)
{
    m_is2Click = value;
    Q_EMIT is2ClickChanged();
    update();
}

bool EventItem::isPress() const { return m_isPress; }
void EventItem::setIsPress(const bool value)
{
    m_isPress = value;
    Q_EMIT isPressChanged();
    update();
}

bool EventItem::isMoving() const { return m_isMoving; }
void EventItem::setIsMoving(const bool value)
{
    m_isMoving = value;
    Q_EMIT isMovingChanged();
    update();
}

bool EventItem::isRelease() const { return m_isRelease; }
void EventItem::setIsRelease(const bool value)
{
    m_isRelease = value;
    Q_EMIT isReleaseChanged();
    update();
}

//-- Calculate:
int EventItem::vPos() const { return m_vPos; }
void EventItem::setVPos(const int& vv)
{
    if (m_vPos != vv) {
        m_vPos = vv;
        Q_EMIT vPosChanged();
        update();
    }
}

double EventItem::vScale() const { return m_vScale; }
void EventItem::setVScale(const double& vs)
{
    if (m_vScale != vs) {
        m_vScale = vs;
        Q_EMIT vScaleChanged();
        update();
    }
}

double EventItem::value() const { return m_vValue; }
void EventItem::setValue(const double& vv)
{
    if (m_vValue != vv) {
        m_vValue = vv;
        Q_EMIT valueChanged();
        update();
    }
}

double EventItem::didFocus() const { return m_didFocus; }
void EventItem::setDidFocus(const bool& df)
{
    if (m_didFocus != df) {
        m_didFocus = df;
        Q_EMIT didFocusChanged();
        update();
    }
}

double EventItem::pix2Value()
{
    double result = (min - max) * (y_pos / scaleY) / height() + max;
    return result;
}

//-- geometery of slider
int EventItem::knobeDiameter() const { return m_knobeDiameter; }
void EventItem::setKnobeDiameter(const int value)
{
    m_knobeDiameter = value;
    Q_EMIT knobeDiameterChanged();
    update();
}

int EventItem::knobeBorder() const { return m_knobeBorder; }
void EventItem::setKnobeBorder(const int value)
{
    m_knobeBorder = value;
    Q_EMIT knobeBorderChanged();
    update();
}

int EventItem::sliderWidth() const { return m_sliderWidth; }
void EventItem::setSliderWidth(const int value)
{
    m_sliderWidth = value;
    Q_EMIT sliderWidthChanged();
    update();
}

// geometery of shape
double EventItem::radius() const { return m_radius; }
void EventItem::setRadius(const double value)
{
    m_radius = value;
    Q_EMIT radiusChanged();
    update();
}

// Colors of slider
QColor EventItem::valueColor() const { return m_valuecolor; }
void EventItem::setValueColor(const QColor value)
{
    m_valuecolor = value;
    Q_EMIT valueColorChanged();
    update();
}

QColor EventItem::emptyColor() const { return m_emptyColor; }
void EventItem::setEmptyColor(const QColor value)
{
    m_emptyColor = value;
    Q_EMIT emptyColorChanged();
    update();
}

QColor EventItem::brKnobeColor() const { return m_brKnobeColor; }
void EventItem::setBrKnobeColor(const QColor value)
{
    m_brKnobeColor = value;
    Q_EMIT brKnobeColorChanged();
    update();
}

QColor EventItem::flKnobeColor() const { return m_flKnobeColor; }
void EventItem::setFlKnobeColor(const QColor value)
{
    m_flKnobeColor = value;
    Q_EMIT flKnobeColorChanged();
    update();
}
