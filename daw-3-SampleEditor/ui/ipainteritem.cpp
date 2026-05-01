#include "ipainteritem.h"

INanoItem::INanoItem(QQuickItem* parent)
    : QNanoQuickItem(parent)

{
}

void INanoItem::update()
{
    QNanoQuickItem::update();
}

QRect IPainterItem::rect() const
{
    return _rect;
}

void IPainterItem::setRect(const QRect& rect)
{
    _rect = rect;
    width = _rect.width();
    height = rect.height();
    X = rect.x();
    Y = rect.y();
    Q_EMIT sigRectChanged();
}

bool IPainterItem::dirty() const
{
    return _dirty;
}

void IPainterItem::setDirty(bool dirty)
{
    _dirty = dirty;
}

bool IPainterItem::visible() const
{
    return _visible;
}

void IPainterItem::setVisible(bool visible)
{
    _visible = visible;
}
