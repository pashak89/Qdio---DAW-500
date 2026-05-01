#ifndef MOUSEZONE_H
#define MOUSEZONE_H

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

class MouseZone : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(bool isRound READ isRound WRITE setIsRound NOTIFY isRoundChanged)
    Q_PROPERTY(int vPos READ vPos WRITE setVPos NOTIFY vPosChanged)
    Q_PROPERTY(bool isfake READ isfake WRITE setIsfake NOTIFY isfakeChanged)
    Q_PROPERTY(double vValue READ vValue WRITE setVValue NOTIFY vValueChanged)

    Q_PROPERTY(double minValue READ minValue WRITE setMinValue NOTIFY minValueChanged)
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged)
    Q_PROPERTY(double defValue READ defValue WRITE setDefValue NOTIFY defValueChanged)

    Q_PROPERTY(bool didFocus READ didFocus WRITE setDidFocus NOTIFY didFocusChanged)
public:
    explicit MouseZone(QQuickItem* parent = nullptr);

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData) Q_DECL_OVERRIDE;

    bool isRound() const;
    void setIsRound(const bool& isR);

    int vPos() const;
    void setVPos(const int& vv);
    bool isfake() const;
    void setIsfake(const bool& isF);
    double vValue() const;
    void setVValue(const double& vv);

    double minValue() const;
    void setMinValue(const double& mv);
    double maxValue() const;
    void setMaxValue(const double& mv);
    double defValue() const;
    void setDefValue(const double& dv);

    double didFocus() const;
    void setDidFocus(const bool& df);

    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent* e) Q_DECL_OVERRIDE;

    void hoverEnterEvent(QHoverEvent* event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QHoverEvent* event) Q_DECL_OVERRIDE;
    void hoverMoveEvent(QHoverEvent* event) Q_DECL_OVERRIDE;

private:
    bool m_isRound;
    int m_vPos;
    bool m_isfake;
    double m_vValue;
    bool m_didFocus;

    bool m_hovered;

    bool didRound = false;
    bool didFake = false;

    double m_minValue;
    double m_maxValue;
    double m_defValue;

    bool didStart = false;
    double y_pos = 0;

    QRectF scene;

    double max = 24 * 6;
    double min = -70;
    double step = 0.01;
    double sense = 20;

    double saveX = 0;
    double saveY = 0;

    double range = 1;

    double lastPos;
    double pressPos;

    bool isIgnore = false;

    double scaleY = 1;

    double pix2Value();

    double round2Tenth(double value)
    {
        value = qRound(value * 10);
        value = value / 10;
        return value;
    }

signals:
    void isRoundChanged();
    void vPosChanged();
    void isfakeChanged();
    void vValueChanged();

    void minValueChanged();
    void maxValueChanged();
    void defValueChanged();

    void didFocusChanged();
};

#endif // MOUSEZONE_H
