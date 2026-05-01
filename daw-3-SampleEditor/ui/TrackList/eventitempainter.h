#ifndef EVENTITEMPAINTER_H
#define EVENTITEMPAINTER_H

#include "qnanoquickitempainter.h"

class EventItemPainter : public QNanoQuickItemPainter {
public:
    EventItemPainter();
    // Reimplement
    void synchronize(QNanoQuickItem* item);
    void paint(QNanoPainter* p);

private:
    QList<QRectF> m_items;
    int m_activeItem;
    int m_pressedItem;
    bool m_hoverEnabled;
    bool m_hovered;

    QString m_hoverColor;
    QString m_unHoverColor;

    double value;

    double yRelease;

    /// geometery of slider
    int diameter;
    int sliderWidth;
    int borderKnobe;

    // geometery of shape
    double radius;

    // colors of slider
    QNanoColor valuecolor;
    QNanoColor emptyColor;
    QNanoColor brKnobeColor;
    QNanoColor flKnobeColor;
};

#endif // EVENTITEMPAINTER_H
