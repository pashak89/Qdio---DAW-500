#include "eventitempainter.h"
#include "eventitem.h"

EventItemPainter::EventItemPainter()
{
    m_activeItem = -1;
    m_pressedItem = -1;
    m_hovered = false;

    m_hoverColor = "#f0ffff";
}

void EventItemPainter::synchronize(QNanoQuickItem* item)
{
    // Setting values here synchronized
    EventItem* realItem = static_cast<EventItem*>(item);
    if (realItem) {
        m_items = realItem->m_items;
        m_activeItem = realItem->m_activeItem;
        m_pressedItem = realItem->m_pressedItem;
        m_hoverEnabled = realItem->mouseHoverEventsEnabled();
        m_hovered = realItem->m_hovered;

        m_hoverColor = realItem->m_hoverColor;
        m_unHoverColor = realItem->m_unHoverColor;

        value = realItem->value();

        radius = realItem->radius();
        // geometery of slider
        diameter = realItem->knobeDiameter();
        borderKnobe = realItem->knobeBorder();
        sliderWidth = realItem->sliderWidth();

        // colors of slider
        valuecolor = QNanoColor::fromQColor(realItem->valueColor());
        emptyColor = QNanoColor::fromQColor(realItem->emptyColor());
        brKnobeColor = QNanoColor::fromQColor(realItem->brKnobeColor());
        flKnobeColor = QNanoColor::fromQColor(realItem->flKnobeColor());
    }
}

void EventItemPainter::paint(QNanoPainter* p)
{
    // Background
    QNanoColor hoverColor = QNanoColor::fromQColor(m_hoverColor);
    QNanoColor unHoverColor = QNanoColor::fromQColor(m_unHoverColor);
    p->beginPath();
    QNanoColor fillColor(m_hovered ? hoverColor : m_hoverEnabled ? unHoverColor
                                                                 : "#00000000");
    p->setFillStyle(fillColor);
    p->roundedRect(0, 0, width(), height(), radius);
    p->setLineWidth(0);
    p->fill();
    p->stroke();

    // Draw boxes
    if (!m_hoverEnabled) {
        int r = diameter / 2;
        yRelease = (24 * 6 - value) / (24 * 6 + 70) * (height() - diameter - borderKnobe) - r;
        QRectF box = m_items.at(0);
        // background slider
        p->beginPath();
        p->setFillStyle(emptyColor);
        p->roundedRect(
            box.x() + width() / 2 - sliderWidth / 2,
            box.y(),
            sliderWidth,
            box.height(),
            radius);
        p->fill();
        p->stroke();

        // filler slider
        p->beginPath();
        p->setFillStyle(valuecolor);
        p->roundedRect(
            box.x() + width() / 2 - sliderWidth / 2,
            box.y() + yRelease + diameter + borderKnobe,
            sliderWidth,
            box.height() - yRelease - diameter - borderKnobe,
            radius);
        p->setLineWidth(0);
        p->fill();
        p->stroke();

        // corner circle
        p->beginPath();
        p->setFillStyle(flKnobeColor);
        p->setStrokeStyle(brKnobeColor);
        p->setLineWidth(borderKnobe);
        p->circle(
            box.x() + width() / 2,
            box.y() + yRelease + diameter + borderKnobe / 2,
            r);
        p->stroke();
        p->fill();
    }
}
