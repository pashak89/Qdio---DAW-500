#ifndef EVENTITEM_H
#define EVENTITEM_H

#include "qnanoquickitem.h"
#include <QCursor>
#include <QKeyEvent>

class EventItem : public QNanoQuickItem {
    Q_OBJECT
    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)

    Q_PROPERTY(bool isHover READ isHover WRITE setIsHover NOTIFY isHoverChanged)
    Q_PROPERTY(QString hoverColor READ hoverColor WRITE setHoverColor NOTIFY hoverColorChanged)
    Q_PROPERTY(QString unHoverColor READ unHoverColor WRITE setUnHoverColor NOTIFY unHoverColorChanged)

    Q_PROPERTY(bool is2Click READ is2Click WRITE setIs2Click NOTIFY is2ClickChanged)
    Q_PROPERTY(bool isPress READ isPress WRITE setIsPress NOTIFY isPressChanged)
    Q_PROPERTY(bool isMoving READ isMoving WRITE setIsMoving NOTIFY isMovingChanged)
    Q_PROPERTY(bool isRelease READ isRelease WRITE setIsRelease NOTIFY isReleaseChanged)

    Q_PROPERTY(int vPos READ vPos WRITE setVPos NOTIFY vPosChanged)
    Q_PROPERTY(double vScale READ vScale WRITE setVScale NOTIFY vScaleChanged)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool didFocus READ didFocus WRITE setDidFocus NOTIFY didFocusChanged)

    Q_PROPERTY(int knobeDiameter READ knobeDiameter WRITE setKnobeDiameter NOTIFY knobeDiameterChanged)
    Q_PROPERTY(int knobeBorder READ knobeBorder WRITE setKnobeBorder NOTIFY knobeBorderChanged)
    Q_PROPERTY(int sliderWidth READ sliderWidth WRITE setSliderWidth NOTIFY sliderWidthChanged)

    //-- colors of slider
    Q_PROPERTY(QColor valueColor READ valueColor WRITE setValueColor NOTIFY valueColorChanged)
    Q_PROPERTY(QColor emptyColor READ emptyColor WRITE setEmptyColor NOTIFY emptyColorChanged)
    Q_PROPERTY(QColor brKnobeColor READ brKnobeColor WRITE setBrKnobeColor NOTIFY brKnobeColorChanged)
    Q_PROPERTY(QColor flKnobeColor READ flKnobeColor WRITE setFlKnobeColor NOTIFY flKnobeColorChanged)

    Q_PROPERTY(double itemY READ itemY WRITE setItemY NOTIFY itemYChanged)

public:
    EventItem(QQuickItem* parent = nullptr);

    // Reimplement
    QNanoQuickItemPainter* createItemPainter() const;

    void hideCursor() { setCursor(Qt::BlankCursor); }

    //-- color sof slider
    QColor valueColor() const;
    void setValueColor(const QColor value);

    QColor emptyColor() const;
    void setEmptyColor(const QColor value);

    QColor brKnobeColor() const;
    void setBrKnobeColor(const QColor value);

    QColor flKnobeColor() const;
    void setFlKnobeColor(const QColor value);

    //-- Geometery od slider
    int knobeDiameter() const;
    void setKnobeDiameter(const int value);

    int knobeBorder() const;
    void setKnobeBorder(const int value);

    int sliderWidth() const;
    void setSliderWidth(const int value);

    // Geometery of Shape
    double radius() const;
    void setRadius(const double value);

    //-- Mouse Area:
    bool is2Click() const;
    void setIs2Click(const bool value);

    bool isPress() const;
    void setIsPress(const bool value);

    bool isMoving() const;
    void setIsMoving(const bool value);

    bool isRelease() const;
    void setIsRelease(const bool value);
    //-- Mouse Area.

    //-- calculate:
    int vPos() const;
    void setVPos(const int& vv);
    double vScale() const;
    void setVScale(const double& vs);
    double value() const;
    void setValue(const double& vv);
    double didFocus() const;
    void setDidFocus(const bool& df);

    //-- Hover functions:
    bool isHover() const { return m_isHover; }
    void setIsHover(const bool value)
    {
        m_isHover = value;
        Q_EMIT isHoverChanged();
        update();
    }

    QString hoverColor() const { return m_hoverColor; }
    void setHoverColor(const QString value)
    {
        m_hoverColor = value;
        Q_EMIT hoverColorChanged();
        update();
    }

    QString unHoverColor() const { return m_unHoverColor; }
    void setUnHoverColor(const QString value)
    {
        m_unHoverColor = value;
        Q_EMIT unHoverColorChanged();
        update();
    }
    //-- Hover functions.

    double itemY() const { return m_itemY; }
    void setItemY(const double value)
    {
        m_itemY = value;
        Q_EMIT itemYChanged();
        update();
    }

public Q_SLOTS:
    void generateRandomItems();

protected:
    // Reimplement from QQuickItem
#if (QT_VERSION >= 0x060000)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) Q_DECL_OVERRIDE;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) Q_DECL_OVERRIDE;
#endif
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void hoverEnterEvent(QHoverEvent* event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QHoverEvent* event) Q_DECL_OVERRIDE;
    void hoverMoveEvent(QHoverEvent* event) Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent* e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent* e) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void isHoverChanged();
    void hoverColorChanged();
    void unHoverColorChanged();

    void is2ClickChanged();
    void isPressChanged();
    void isMovingChanged();
    void isReleaseChanged();

    void vPosChanged();
    void vScaleChanged();
    void valueChanged();
    void didFocusChanged();

    void knobeDiameterChanged();
    void knobeBorderChanged();
    void sliderWidthChanged();

    void radiusChanged();

    void valueColorChanged();
    void emptyColorChanged();
    void brKnobeColorChanged();
    void flKnobeColorChanged();

    void itemYChanged();

private:
    friend class EventItemPainter;

    QList<QRectF> m_items;
    int m_activeItem;
    int m_pressedItem;
    bool m_hovered;
    QPointF m_pressPos;

    bool m_isHover;
    QString m_hoverColor;
    QString m_unHoverColor;

    bool m_is2Click;
    bool m_isPress;
    bool m_isMoving;
    bool m_isRelease;

    int m_vPos;
    double m_vScale;
    double m_vValue;
    bool m_didFocus;

    bool didStart = false;
    int y_pos = 0;

    double myHeight;

    double max = 24 * 6;
    double min = -70;
    double step = 0.01;
    double sense = 20;

    double saveX = 0;
    double saveY = 0;

    double range = (max - min) / step;

    double lastPos;
    double pressPos;

    bool isIgnore = false;

    double scaleY = 1;

    double pix2Value();

    double yRelease;

    double m_itemY;

    //-- geometery of slider
    int m_knobeDiameter;
    int m_knobeBorder;
    int m_sliderWidth;

    // geometery of Shape
    double m_radius;

    // colors of slider
    QColor m_valuecolor;
    QColor m_emptyColor;
    QColor m_brKnobeColor;
    QColor m_flKnobeColor;
};

#endif // EVENTITEM_H
