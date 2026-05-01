#include "MouseZone.h"

MouseZone::MouseZone(QQuickItem* parent)
    : QQuickItem(parent)
    , m_isRound(false)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(QQuickItem::ItemHasContents);

    setVPos(0);
    didStart = true;
}

QSGNode* MouseZone::updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData* updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData)
    QSGGeometryNode* root = static_cast<QSGGeometryNode*>(oldNode);

    if (!root) {
        root = new QSGGeometryNode;
        QSGGeometry* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 3);
        geometry->setDrawingMode(GL_TRIANGLE_FAN);
        geometry->vertexDataAsPoint2D()[0].set(width() / 2, 0);
        geometry->vertexDataAsPoint2D()[1].set(width(), height());
        geometry->vertexDataAsPoint2D()[2].set(0, height());

        root->setGeometry(geometry);
        root->setFlag(QSGNode::OwnsGeometry);
        root->setFlag(QSGNode::OwnsMaterial);

        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(Qt::transparent);
        root->setMaterial(material);
    }

    // initialize..
    if (didStart) {
        setDidFocus(false);
        min = minValue();
        max = maxValue();
        range = (max - min) / step;

        scaleY = qRound(range / height() / 100);
        //        qDebug()<< "in initial scaleY: " << scaleY;

        didRound = isRound();

        //        qDebug()<< "didround: "<< didRound;

        //        y_pos = qRound((max-defValue())/(max-min)*scaleY*height());
        y_pos = defValue() * scaleY;

        didStart = false;
    }

    //--------------------------------------- fake slider:
    //
    //    if(isfake() && y_pos > 0 && !didFake){
    //        scaleY += 10;
    //        didFake = true;
    //    }
    //    else if(isfake() && y_pos <= 0 && didFake){
    //        scaleY += -10;
    //        didFake = false;
    //    }
    //
    //--------------------------------------- fake slider.
    setVValue(pix2Value());
    //    qDebug()<< "in Geometery";
    return root;
}

void MouseZone::mousePressEvent(QMouseEvent* event)
{
    setFocus(true);
    setDidFocus(true);
    setCursor(Qt::BlankCursor);
    pressPos = lastPos = event->pos().y();
    //    y_pos = event->pos().y() *scaleY;

    y_pos = pix2Value() * scaleY;

    if (isfake() && y_pos > 0)
        y_pos = y_pos * 6;

    saveX = event->globalPos().x();
    saveY = event->globalPos().y();
    qDebug() << "y_pos/scaleY in press: " << y_pos / scaleY;
    setVPos(y_pos);
    update();
}

void MouseZone::mouseDoubleClickEvent(QMouseEvent*)
{
    //    y_pos = qRound((max-defValue())/(max-min)*scaleY*height());
    y_pos = defValue() * scaleY;
    setVPos(y_pos);
    pix2Value();
    //    qDebug() << "2 clicks";
}

void MouseZone::mouseMoveEvent(QMouseEvent* event)
{
    if (isIgnore) {
        isIgnore = false;
        return;
    }

    int diff = qAbs(pressPos - event->pos().y());
    if (diff >= 100) {
        QCursor().setPos(QCursor().pos().x(), mapToGlobal(QPoint(event->pos().x(), pressPos)).y());
        //    y_pos += (event->pos().y() - lastPos);
        y_pos += (-event->pos().y() + lastPos);
        //        y_pos = qBound( 0.0 , (double)y_pos , height() * scaleY);
        y_pos = qBound(min * scaleY, (double)y_pos, max * scaleY);

        setVPos(y_pos);
        lastPos = pressPos;
        update();
        isIgnore = true;
        return;
    }

    //    y_pos += (event->pos().y() - lastPos);
    y_pos += (-event->pos().y() + lastPos);

    //    y_pos = qBound( 0.0 , (double)y_pos , height() * scaleY);
    y_pos = qBound(min * scaleY, (double)y_pos, max * scaleY);

    setVPos(y_pos);
    //    qDebug()<< "y_pos/scaleY in move: " << y_pos/scaleY;// <<", last pos: "<< lastPos << ", e.pos: "<< event->pos().y();

    lastPos = event->pos().y();

    update();
}

void MouseZone::mouseReleaseEvent(QMouseEvent*)
{
    setCursor(Qt::ArrowCursor);
    isIgnore = true;
    QCursor().setPos(saveX, saveY);
    setDidFocus(false);
}

void MouseZone::hoverEnterEvent(QHoverEvent* event)
{
    qDebug() << "LOG: hoverEnterEvent";
    Q_UNUSED(event);
    m_hovered = true;
    update();
}

void MouseZone::hoverLeaveEvent(QHoverEvent* event)
{
    qDebug() << "LOG: hoverLeaveEvent";
    Q_UNUSED(event);
    m_hovered = false;
    update();
}

void MouseZone::hoverMoveEvent(QHoverEvent* event)
{
    qDebug() << "LOG: hoverMoveEvent";
    Q_UNUSED(event);
}

void MouseZone::keyPressEvent(QKeyEvent* e)
{
    if (e->modifiers() == Qt::ShiftModifier) {
        didRound = false;

        double maxLastPos = height() * scaleY;
        scaleY *= sense;
        double maxPos = height() * scaleY;
        y_pos = y_pos * maxPos / maxLastPos;
    }
}

void MouseZone::keyReleaseEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Shift) {
        didRound = isRound();

        double maxLastPos = height() * scaleY;
        scaleY /= sense;
        double maxPos = height() * scaleY;
        y_pos = y_pos * maxPos / maxLastPos;
    }
}

double MouseZone::pix2Value()
{
    double result = y_pos / scaleY; //(min - max) * (y_pos/scaleY) / height() + max;
    //    qDebug()<< "                                                        result: "<< result;

    double r1 = didRound ? qRound(result) : isRound() ? round2Tenth(result)
                                                      : result;
    if (isfake() && r1 > 0)
        r1 = r1 / 6;
    return r1;
}

//-----------------------------------------------===============-------
//                                                QML Property:
bool MouseZone::isRound() const
{
    return m_isRound;
}

void MouseZone::setIsRound(const bool& isR)
{
    if (m_isRound != isR) {
        m_isRound = isR;
        update();
        isRoundChanged();
    }
}

int MouseZone::vPos() const
{
    return m_vPos;
}

void MouseZone::setVPos(const int& vv)
{
    if (m_vPos != vv) {
        m_vPos = vv;
        update();
        vPosChanged();
    }
}

bool MouseZone::isfake() const
{
    return m_isfake;
}

void MouseZone::setIsfake(const bool& isF)
{
    if (m_isfake != isF) {
        m_isfake = isF;
        update();
        isfakeChanged();
    }
}

double MouseZone::vValue() const
{
    return m_vValue;
}

void MouseZone::setVValue(const double& vv)
{
    if (m_vValue != vv) {
        m_vValue = vv;
        update();
        vValueChanged();
    }
}

double MouseZone::minValue() const
{
    return m_minValue;
}

void MouseZone::setMinValue(const double& mv)
{
    if (m_minValue != mv) {
        m_minValue = mv;
        update();
        minValueChanged();
    }
}

double MouseZone::maxValue() const
{
    return m_maxValue;
}

void MouseZone::setMaxValue(const double& mv)
{
    if (m_maxValue != mv) {
        m_maxValue = mv;
        update();
        maxValueChanged();
    }
}

double MouseZone::defValue() const
{
    return m_defValue;
}

void MouseZone::setDefValue(const double& dv)
{
    if (m_defValue != dv) {
        m_defValue = dv;
        update();
        defValueChanged();
    }
}

double MouseZone::didFocus() const
{
    return m_didFocus;
}

void MouseZone::setDidFocus(const bool& df)
{
    if (m_didFocus != df) {
        m_didFocus = df;
        update();
        didFocusChanged();
    }
}
