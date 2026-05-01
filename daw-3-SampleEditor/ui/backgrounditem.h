#ifndef BACKGROUNDITEM_H
#define BACKGROUNDITEM_H

#include <QPainter>
#include <QtMath>

#include <QGuiApplication>
#include <QQuickPaintedItem>

#include "core/beatbar.h"
#include "ui/ipainteritem.h"

class BackgroundItem : public IPainterItem {

public:
    explicit BackgroundItem();

public:
    QString _brush;
    void draw(IPainter* painter);
    void setBrush(const QString &brush);
};
#endif // BACKGROUNDITEM_H
