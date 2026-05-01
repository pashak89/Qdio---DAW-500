#ifndef KEYFRAMESITEM_H
#define KEYFRAMESITEM_H

#include <QCursor>
#include <QPainter>
#include <QPainterPath>
#include <QQuickPaintedItem>
#include <QTimer>

#include "core/automation.h"
#include "core/beatbar.h"
#include "core/mathhelper.h"
#include "ui/ipainteritem.h"
#include "ui/plotitem.h"
class TrackItem;
class KeyFramesItem : public IPainterItem {
    Q_OBJECT

    enum KeyFramesType {
        KeyFramesType_Circle = 0,
        KeyFramesType_Dimond = 1,
        KeyFramesType_Triangle = 2,
    };

public:
    struct KeyFramesPoint {
        QPointF point;
        int type;
        double posX;
        double posY;
        double posZ;
    };
    explicit KeyFramesItem(int trackIndex, AreaInfo* areaInfo);
    bool keyFramesEnabled() const;
    void setKeyFramesEnabled(bool newKeyFramesEnabled);

    void setObjectPosition(double x, double y, double z);

    void addKeyFrame(qint64 time, int type, double x, double y, double z);
    void addKeyFrame(qint64 time, int type);
    void removeKeyFrame(qint64 time);

    QMap<qint64, int> keyFrames() const;
    QMap<qint64, double> objectPositionX() const;
    QMap<qint64, double> objectPositionY() const;
    QMap<qint64, double> objectPositionZ() const;

    bool contains(SelectedArea selectedArea);

    QList<KeyFramesPoint> createCopyList(SelectedArea selectedArea);

private:
    int _lastInsertedIndex = -1;

    AreaInfo* _areaInfo;
    int _trackIndex;
    bool _keyFramesEnabled = true;

    int _mouseHoverX;
    int _mouseHoverY;

    int _mousePressedX;
    int _mousePressedY;

    int _currentPointIndex = -1;
    int _nextPointIndex = -1;
    int _lastPointIndex = -1;

    bool _mousePressed = false;
    bool _mouseMoved = false;

    QList<KeyFramesPoint> _points;

    int _keyFrameType = 0;

    double m_objectPositionX;
    double m_objectPositionY;
    double m_objectPositionZ;

    // IPainterItem interface
public:
    void draw(IPainter* painter);
    int hoverMoveEvent(QHoverEvent* event);
    int mousePressEvent(QMouseEvent* event);
    int mouseMoveEvent(QMouseEvent* event);
    int mouseReleaseEvent(QMouseEvent* event);

    int keyFrameType() const;
    void setKeyFrameType(int newKeyFrameType);

    void saveInsertUndoItems(double time, int type, double posX, double posY, double posZ);
    void saveRemoveUndoItems(double time, int type, double posX, double posY, double posZ);

    void saveObjectUndoItems(double posX, double posY, double posZ);

    QJsonObject applyUndo(QJsonObject obj);

    QJsonObject save();
    void load(QJsonObject obj);

private:
    int insertToPoints(QPointF point, int type, double x, double y, double z);
    void drawLine(IPainter* painter);
    void drawKeyPoints(IPainter* painter);
    void drawKeyHoverPoint(IPainter* painter);
    int findPoint(QPointF point);

    QPointF mapPointToLine(QPointF point);
    QPolygonF lineArea();
    QLineF getLine();
    double time2Pixel(double time);
    double pixel2Time(double pixel);
    void findSnap(QVector<QPair<double, double>> _snapPoints, double posX, double posY, double& final_xpos, double& final_ypos);
    // IPainterItem interface
public:
    void setRect(const QRect& rect);
};

#endif // KEYFRAMESITEM_H
