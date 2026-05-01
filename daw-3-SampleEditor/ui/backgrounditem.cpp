#include "backgrounditem.h"

BackgroundItem::BackgroundItem()
    : IPainterItem()
{
    _brush = "#4F4F4F";
}

void BackgroundItem::setBrush(const QString& brush)
{
    _brush = brush;
}

void BackgroundItem::draw(IPainter* painter)
{

    painter->setStrokeStyle(QNanoColor(_brush.toLatin1().data()));
    painter->fillRect(0, 0, _rect.width(), _rect.height(), QBrush(QColor(_brush.toLatin1().data())));
}
