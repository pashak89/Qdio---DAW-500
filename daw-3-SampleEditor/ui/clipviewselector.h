//#ifndef STARTENDRECORD_H
//#define STARTENDRECORD_H

//#include <QPainter>
//#include <QQuickPaintedItem>
//#include <QTimer>

//#include "core/beatbar.h"
//#include "ui/beatbaritem.h"
//#include "ui/clipitem.h"
//#include "ui/ipainteritem.h"
//#include "ui/plotitem.h"

//class ClipViewSelectorItem;
//class ClipViewSelectorPainterItem : public INanoPainterItem {

//    // INanoPainterItem interface
//public:
//    ClipViewSelectorPainterItem(SampleEditor* sampleEditor);
//    void draw(IPainter* painter);

//private:
//    SampleEditor* _sampleEditor = nullptr;

//    bool _greenWindowZoomStop = false;

//    double _preDuration = 0;

//    double _startPix = 0;
//    double _endPix = 0;

//    double _lastStartX;
//    double _lastEndX;
//    bool _zoomEndded = false;
//    double _coeffStartX = 1;
//    double _coeffEndX = 1;
//};

//class ClipViewSelectorItem : public INanoItem {
//    Q_OBJECT
//public:
//    explicit ClipViewSelectorItem(QQuickItem* parent = nullptr);

//    Q_INVOKABLE void update();

//protected:
//    QNanoQuickItemPainter* createItemPainter() const;

//private:
//    SampleEditor* _sampleEditor = nullptr;
//    bool _resizeStart = false;
//    bool _resizeEnd = false;
//    double _mousePressRatio;
//    double _startPix;
//    double _endPix;

//    bool _mouseMove = false;
//    bool _mouseReleased = false;
//    double _lastCursorPosX = 0;
//    double _lastCursorPosY = 0;
//    double _startPositionX = 0;
//    double _startPositionY = 0;
//    double _currentPosistionY = 0;
//    double _currentPosistionX = 0;

//    QTimer _timerWidth;
//    int _lastWidth = 0;

//protected:
//    void mousePressEvent(QMouseEvent* event);
//    void mouseMoveEvent(QMouseEvent* event);
//    void mouseReleaseEvent(QMouseEvent* event);

//    void hoverMoveEvent(QHoverEvent* event);

//private Q_SLOTS:
//    void sltSampleChanged();
//    void sltTimerUpdated();

//Q_SIGNALS:
//    void sigUpdate();
//    void sigWidthChanged();
//};

//#endif // STARTENDRECORD_H
