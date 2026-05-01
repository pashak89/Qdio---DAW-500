#ifndef IPAINTERITEM_H
#define IPAINTERITEM_H

#include "qnanoquickitem.h"
#include "qnanoquickitempainter.h"

#include <QStack>
#include <QtMath>
#include <qpainter.h>
#define SQLRT4_3 1.15470054
#define SQLRT3_4 0.8660254037
class AreaInfo;
class IPainter : public QNanoPainter {

public:
    void drawText(float x, float y, QString& text, double maxWidth)
    {
        fillText(text, x, y, maxWidth);
    }

    void drawText(float x, float y, QString& text)
    {
        fillText(text, x, y);
    }

    void drawText(QRect rect, QString& text)
    {

        fillText(text, rect);
    }
    void drawTriangle(QPointF center, int sideLength)
    {

        // Calculate the height of the equilateral triangle
        const double height = (sqrt(3) / 2) * sideLength;

        // Calculate the vertices of the equilateral triangle
        QPointF p1(center.x() - sideLength / 2.0, center.y() + height / 3);
        QPointF p2(center.x() + sideLength / 2.0, center.y() + height / 3);
        QPointF p3(center.x(), center.y() - 2 * height / 3);

        // Move to the first point
        moveTo(p1);

        // Draw lines between the points
        lineTo(p2);
        lineTo(p3);
        lineTo(p1);
        fill();
    }
    void drawPolygon(QPolygonF polygn)
    {
        save();

        moveTo(polygn.at(0));
        for (int i = 0; i < polygn.size(); i++) {
            lineTo(polygn.at(i));
        }
        stroke();
    }

    void drawDiamond(double x, double y, int sideLength)
    {

        drawDiamond(QPointF(x, y), sideLength);
    }

    void drawDiamond(QPointF center, int sideLength)
    {

        // Calculate the half-diagonals of the diamond
        const double halfDiagonal1 = sideLength * sqrt(2) / 2; // Half of the vertical diagonal
        const double halfDiagonal2 = sideLength / 2; // Half of the horizontal diagonal

        // Calculate the vertices of the diamond
        QPoint p1(center.x(), center.y() - halfDiagonal1); // Top vertex
        QPoint p2(center.x() + halfDiagonal2, center.y()); // Right vertex
        QPoint p3(center.x(), center.y() + halfDiagonal1); // Bottom vertex
        QPoint p4(center.x() - halfDiagonal2, center.y()); // Left vertex

        // Move to the first point
        moveTo(p1);

        // Draw lines between the points
        lineTo(p2);
        lineTo(p3);
        lineTo(p4);
        lineTo(p1);
    }

    void drawTriangle(float x0, float y0, float height)
    {
        moveTo(x0, y0);
        lineTo(x0 - (SQLRT4_3 * height) / 2, y0 - height);
        lineTo(x0 + (SQLRT4_3 * height) / 2, y0 - height);
        lineTo(x0, y0);
        fill();
    }
    void drawTriangleRight(float x0, float y0, float height)
    {

        moveTo(x0, y0);
        lineTo(x0 - (SQLRT4_3 * height) / 2, y0 - height);
        lineTo(x0, y0 - height);
        lineTo(x0, y0);
        fill();
    }
    void drawTriangleLeft(float x0, float y0, float height)
    {
        moveTo(x0, y0);
        lineTo(x0 + (SQLRT4_3 * height) / 2, y0 - height);
        lineTo(x0, y0 - height);
        lineTo(x0, y0);
        fill();
    }
    void drawTriangleBraceRight(QNanoPainter* p,
        float x0, float y0, float height,
        const QColor& fillColor,
        const QColor& lineColor,
        float lineWidth = 1.0f)
    {
        p->beginPath();
        p->setAntialias(1);
        p->moveTo(x0, y0);
        p->lineTo(x0 - (SQLRT3_4 * height), y0 + height / 2.0f);
        p->lineTo(x0, y0 + height);
        p->closePath();

        // Fill
        p->setFillStyle(QNanoColor::fromQColor(fillColor));
        p->fill();

        // Stroke
        p->setStrokeStyle(QNanoColor::fromQColor(lineColor));
        p->setLineWidth(lineWidth);
        p->stroke();
    }

    void drawTriangleBraceLeft(QNanoPainter* p,
        float x0, float y0, float height,
        const QColor& fillColor,
        const QColor& lineColor,
        float lineWidth = 1.0f)
    {
        p->beginPath();
        p->setAntialias(1);
        p->moveTo(x0, y0);
        p->lineTo(x0 + (SQLRT3_4 * height), y0 + height / 2.0f);
        p->lineTo(x0, y0 + height);
        p->closePath();

        // Fill
        p->setFillStyle(QNanoColor::fromQColor(fillColor));
        p->fill();

        // Stroke
        p->setStrokeStyle(QNanoColor::fromQColor(lineColor));
        p->setLineWidth(lineWidth);
        p->stroke();
    }

    void drawLine(float x0, float y0, float x1, float y1)
    {
        moveTo(x0, y0);
        lineTo(x1, y1);
    }
    int sgn(float x)
    {
        return ((x > 0) - (x < 0)) * 1;
    }
    QVector<QLineF> drawSuperellipse(QPoint a, QPoint b, float n, float m, float width)
    {

        QVector<QLineF> poly;
        if (n < 0) {
            n = qAbs(n);
            m = qAbs(m);

            int centerX = a.x();
            int centerY = b.y();

            float A = a.y() - centerY;
            float B = centerX - b.x();
            moveTo(b);

            QPointF f = QPointF(b);
            float sp = b.x();
            float ep = centerX;
            if (sp < 0)
                sp = 0;
            if (ep > width) {
                ep = width;
            }

            if (qAbs(sp - ep) <= 0) {

                poly.append(QLineF(a, b));
                lineTo(a);
            } else {
                for (int x = sp; x <= ep; x++) {

                    float y = A * qPow(1 - qPow(qAbs((x - centerX) / (float)B), n), 1.0 / (float)m) + centerY; // solve for y
                    lineTo(x, y);

                    poly.append(QLineF(f, QPointF(x, y)));
                    f = QPointF(x, y);
                }
            }

        } else {
            n = qAbs(n);
            m = qAbs(m);

            int centerX = b.x();
            int centerY = a.y();

            float A = a.x() - centerX;
            float B = centerY - b.y();

            moveTo(a);

            QPointF f = QPointF(a);
            float sp = a.x();
            float ep = centerX;
            if (sp > width)
                sp = width;
            if (ep < 0) {
                ep = 0;
            }

            if (qAbs(sp - ep) <= 0) {
                poly.append(QLineF(a, b));
                lineTo(b);
            } else {
                for (int x = sp; x >= ep; x--) {

                    float y = -B * qPow(1 - qPow(qAbs((x - centerX) / (float)A), n), 1.0 / (float)m) + centerY; // solve for y
                    lineTo(x, y);

                    poly.append(QLineF(f, QPointF(x, y)));
                    f = QPointF(x, y);
                }
            }
        }

        return poly;
    }

    QVector<QLine> drawSuperellipse(QPoint a, QPoint b, float n, float m, float width, int k)
    {

        QVector<QLine> poly;
        if (n < 0) {
            n = qAbs(n);
            m = qAbs(m);
            int centerX = a.x();
            int centerY = b.y();

            QPoint bb = QPoint(b.x() + 20, b.y());
            QPoint aa = QPoint(a.x(), a.y() + 20);

            float A = aa.y() - centerY;
            float B = centerX - bb.x();

            moveTo(b);

            QPoint f = QPoint(b);
            float sp = b.x();
            float ep = centerX;
            if (sp < 0)
                sp = 0;
            if (ep > width) {
                ep = width;
            }
            for (int x = sp; x <= ep; x++) {

                float y = (A)*qPow(1 - qPow(qAbs((x - centerX) / (float)(B)), n), 1.0 / (float)m) + centerY; // solve for y
                lineTo(x, y);

                poly.append(QLine(f, QPoint(x, y)));
                f = QPoint(x, y);
            }
        } else {
            n = qAbs(n);
            m = qAbs(m);

            int centerX = b.x();
            int centerY = a.y();

            QPoint bb = QPoint(b.x(), b.y() - 20);
            QPoint aa = QPoint(a.x() - 20, a.y());

            float A = aa.x() - centerX;
            float B = centerY - bb.y();

            moveTo(a);

            QPoint f = QPoint(a);
            float sp = aa.x();
            float ep = centerX;
            if (sp > width)
                sp = width;
            if (ep < 0) {
                ep = 0;
            }
            for (int x = sp; x >= ep; x--) {

                float y = -(B)*qPow(1 - qPow(qAbs((x - centerX) / (float)(A)), n), 1.0 / (float)m) + centerY; // solve for y
                lineTo(x, y);

                poly.append(QLine(f, QPoint(x, y)));
                f = QPoint(x, y);
            }
        }

        return poly;
    }
    void drawRect(float x0, float y0, float x1, float y1)
    {
        moveTo(x0, y0);
        lineTo(x0, y1);
        lineTo(x1, y1);
        lineTo(x1, y0);
        lineTo(x0, y0);
    }
    void drawRectTop(float x0, float y0, float y1, float height)
    {
        float a = height / (1.0 + SQLRT3_4);
        moveTo(x0 + (SQLRT4_3 * height) / 2, y0 - height);
        lineTo(x0 + (SQLRT4_3 * height) / 2, y1);
        lineTo(x0 - (SQLRT4_3 * height) / 2, y1);
        lineTo(x0 - (SQLRT4_3 * height) / 2, y0 - height);
        lineTo(x0 + (SQLRT4_3 * height) / 2, y0 - height);
    }

    float distance(const QPointF& pt1, const QPointF& pt2)
    {
        float hd = (pt1.x() - pt2.x()) * (pt1.x() - pt2.x());
        float vd = (pt1.y() - pt2.y()) * (pt1.y() - pt2.y());
        return qSqrt(hd + vd);
    }

    QPointF getLineStart(const QPointF& pt1, const QPointF& pt2)
    {
        QPointF pt;
        float rat = 1.0 / distance(pt1, pt2);
        if (rat > 0.5) {
            rat = 0.5;
        }
        pt.setX((1.0 - rat) * pt1.x() + rat * pt2.x());
        pt.setY((1.0 - rat) * pt1.y() + rat * pt2.y());
        return pt;
    }

    QPointF getLineEnd(const QPointF& pt1, const QPointF& pt2)
    {
        QPointF pt;
        float rat = 1.0 / distance(pt1, pt2);
        if (rat > 0.5) {
            rat = 0.5;
        }
        pt.setX(rat * pt1.x() + (1.0 - rat) * pt2.x());
        pt.setY(rat * pt1.y() + (1.0 - rat) * pt2.y());
        return pt;
    }

    void fillRect(float x0, float y0, float width, float height, QBrush brush)
    {
        QNanoPainter::setFillStyle(QNanoColor(brush.color().red(), brush.color().green(), brush.color().blue()));
        QNanoPainter::fillRect(x0, y0, width, height);
    }

    void drawRotatedTriangle(
        float x, float y, // top-left reference (like rect)
        float width, float height, // bounding box
        float angleDeg,
        const QColor& fillColor,
        const QColor& strokeColor,
        float strokeWidth)
    {
        // Save current state
        QNanoPainter::save();

        // Move origin to triangle center
        float cx = x + width * 0.5f;
        float cy = y + height * 0.5f;
        QNanoPainter::translate(cx, cy);

        // Rotate (degrees)
        QNanoPainter::rotate(angleDeg);

        // Draw triangle centered at origin
        QNanoPainter::beginPath();
        QNanoPainter::moveTo(0.0f, -height * 0.5f); // top
        QNanoPainter::lineTo(-width * 0.5f, height * 0.5f); // bottom-left
        QNanoPainter::lineTo(width * 0.5f, height * 0.5f); // bottom-right
        QNanoPainter::closePath();

        // Fill
        QNanoPainter::setFillStyle(QNanoColor::fromQColor(fillColor));
        QNanoPainter::fill();

        // Stroke
        QNanoPainter::setStrokeStyle(QNanoColor::fromQColor(strokeColor));
        QNanoPainter::setLineWidth(strokeWidth);
        QNanoPainter::stroke();

        // Restore painter state
        QNanoPainter::restore();
    }

    void fillRect(float x0, float y0, float width, float height, QColor fillColor, QColor strokeColor)
    {

        QNanoPainter::beginPath();
        QNanoPainter::rect(x0, y0, width, height);

        // Fill color (inside)
        QNanoPainter::setFillStyle(QNanoColor::fromQColor(fillColor)); // Blue
        QNanoPainter::fill();

        // Stroke color (border)
        QNanoPainter::setStrokeStyle(QNanoColor::fromQColor(strokeColor)); // Red
        QNanoPainter::setLineWidth(1);
        QNanoPainter::stroke();
    }

    void fillRect(float x0, float y0, float width, float height)
    {
        QNanoPainter::fillRect(x0, y0, width, height);
    }
    void fillRect(QRectF area)
    {
        QNanoPainter::fillRect(area.x(), area.y(), area.width(), area.height());
    }

    void drawDashedLine(float x0, float y0, float x1, float y1, float dashLength)
    {
        // Calculate the length of the line
        double dx = x1 - x0;
        double dy = y1 - y0;
        double lineLength = std::sqrt(dx * dx + dy * dy);

        // Calculate the number of dashes
        int numDashes = static_cast<int>(lineLength / dashLength);

        // Calculate the dash increments
        double dashX = dx / numDashes;
        double dashY = dy / numDashes;

        // Draw the line with dashes
        for (int i = 0; i < numDashes; i++) {
            if (i % 2 == 0) {

                moveTo(x0 + i * dashX, y0 + i * dashY);
            } else {

                lineTo(x0 + i * dashX, y0 + i * dashY);
            }
        }
    }
    //    void drawDashedLine(float x0, float y0, float width, float height, float dashLength)
    //    {
    //        qDebug() << x0 << y0 << width << height;
    //        int numLines = 10;
    //        float m = (height) / (width);
    //        float y1;
    //        float x1 = x0;
    //        moveTo(x0, y0);

    //        for (int i = 0; i < numLines; i++) {
    //            x1 += (10 - dashLength);
    //            y1 = m * (x1 - x0) + y0;
    //            lineTo(x1, y1);
    //            x1 += dashLength;
    //            y1 = m * (x1 - x0) + y0;
    //            moveTo(x1, y1);
    //        }
    //    }
};

class INanoPainterItem : public QNanoQuickItemPainter {

public:
    explicit INanoPainterItem() { }

    // QQuickPaintedItem interface
public:
    virtual void painter(IPainter* painter)
    {
        QNanoFont font;
        font.setPixelSize(13);
        painter->setFont(font);

        painter->setFillStyle(QNanoColor("#004C4C4C"));
        painter->fillRect(0, 0, width(), height());

        QNanoFont font1(QNanoFont::DEFAULT_FONT_NORMAL);
        font1.setPixelSize(10);
        painter->setFont(font1);
        painter->setFillStyle("#000000");
        painter->setStrokeStyle("#000000");
        painter->beginPath();

        painter->setLineWidth(1);
        painter->setLineCap(IPainter::CAP_ROUND);
        painter->setLineJoin(IPainter::JOIN_ROUND);

        painter->beginPath();
        draw(painter);

        // painter->stroke();
    }
    virtual void draw(IPainter* painter) = 0;

    // QQuickPaintedItem interface
public:
    void paint(QNanoPainter* painter) override
    {
        INanoPainterItem::painter((IPainter*)painter);
    }
    // QNanoQuickItemPainter interface
protected:
    void synchronize(QNanoQuickItem* item) override
    {
        Q_UNUSED(item)
    }
};

class INanoItem : public QNanoQuickItem {
    Q_OBJECT
public:
    INanoItem(QQuickItem* parent = Q_NULLPTR);

    Q_INVOKABLE void update();

signals:
    void beatBarChanged();
};

class IPainterItem : public QObject {
    Q_OBJECT
    Q_PROPERTY(QRect rect READ rect WRITE setRect NOTIFY sigRectChanged)
public:
    IPainterItem()
    {
    }
    virtual void draw(IPainter* painter) = 0;
    virtual int mousePressEvent(QMouseEvent* event) { return 0; }
    virtual int mouseMoveEvent(QMouseEvent* event) { return 0; }
    virtual int mouseReleaseEvent(QMouseEvent* event) { return 0; }
    virtual int hoverMoveEvent(QHoverEvent* event) { return 0; }
    virtual int mouseDoubleClickEvent(QMouseEvent* event) { return 0; }
    virtual int hoverEnterEvent(QHoverEvent* event) { return 0; }
    virtual int hoverLeaveEvent(QHoverEvent* event) { return 0; }
    virtual int keyPressEvent(int modifier, int key) { return 0; }
    virtual int keyReleaseEvent(int modifier, int key) { return 0; }
    virtual int dropEvent(QDropEvent* event) { return 0; }
    virtual int dragMoveEvent(QDragMoveEvent* event) { return 0; }
    virtual int dragEnterEvent(QDragEnterEvent* event) { return 0; }

    virtual bool undo() { return false; }
    virtual bool redo() { return false; }

    virtual QRect rect() const;
    virtual void setRect(const QRect& rect);

    bool dirty() const;
    void setDirty(bool dirty);

    bool visible() const;
    void setVisible(bool visible);

protected:
    QRect _rect;
    bool _containMouse = false;

    int width;
    int height;
    int X;
    int Y;

    bool _dirty = false;
    bool _visible = false;

signals:
    void sigSetCursor(const QCursor& cursor);
    void sigCursorPosition(QPoint point);
    void sigUpdate();
    void sigLockUpdate(int id = -1);
    void sigFreeUpdate(int id = -1);
    void sigRectChanged();
};

#endif // IPAINTERITEM_H
