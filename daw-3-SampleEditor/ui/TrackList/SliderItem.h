#ifndef SLIDERITEM_H
#define SLIDERITEM_H

#include <QCursor>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>
#include <QQuickItem>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QtQml>
#include <QtQuick>

class SliderItem : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(int vPos READ vPos WRITE setVPos NOTIFY vPosChanged)
    Q_PROPERTY(double vScale READ vScale WRITE setVScale NOTIFY vScaleChanged)
    Q_PROPERTY(double vValue READ vValue WRITE setVValue NOTIFY vValueChanged)
    Q_PROPERTY(bool didFocus READ didFocus WRITE setDidFocus NOTIFY didFocusChanged)
public:
    explicit SliderItem(QQuickItem* parent = nullptr);

    void hideCursor()
    {
        setCursor(Qt::BlankCursor);
    }

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData);

    QColor color() const;
    void setColor(const QColor& color);

    int vPos() const;
    void setVPos(const int& vv);
    double vScale() const;
    void setVScale(const double& vs);
    double vValue() const;
    void setVValue(const double& vv);
    double didFocus() const;
    void setDidFocus(const bool& df);

    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);

private:
    QColor m_color;
    int m_vPos;
    double m_vScale;
    double m_vValue;
    bool m_didFocus;
    bool m_needUpdate;

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

signals:
    void colorChanged();
    void vPosChanged();
    void vScaleChanged();
    void vValueChanged();
    void didFocusChanged();
};

#endif // SLIDERITEM_H
