#include "SliderItem.h"

SliderItem::SliderItem(QQuickItem* parent)
    : QQuickItem(parent)
    , m_color(Qt::red)
    , m_needUpdate(true)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(QQuickItem::ItemHasContents);

    setVPos(0);
    didStart = true;
}

QSGNode* SliderItem::updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData* updatePaintNodeData)
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
    }

    if (m_needUpdate) {
        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(m_color);
        root->setMaterial(material);
        m_needUpdate = false;
    }

    if (didStart) {
        scaleY = (range / height()) / 30;

        y_pos = qRound(height() * max * scaleY / (max - min));

        setVPos(y_pos);
        pix2Value();
        setVScale(scaleY);
        setDidFocus(false);

        myHeight = height();
        didStart = false;
    }
    if (!didStart && height() != myHeight) {
        scaleY = (range / height()) / 30;
        setVScale(scaleY);
        myHeight = height();
    }

    y_pos = vPos();
    setVValue(qRound(pix2Value()));
    return root;
}

QColor SliderItem::color() const
{
    return m_color;
}

void SliderItem::setColor(const QColor& color)
{
    if (m_color != color) {
        m_color = color;
        m_needUpdate = true;
        update();
        colorChanged();
    }
}

int SliderItem::vPos() const
{
    return m_vPos;
}

void SliderItem::setVPos(const int& vv)
{
    if (m_vPos != vv) {
        m_vPos = vv;
        m_needUpdate = true;
        update();
        vPosChanged();
    }
}

double SliderItem::vScale() const
{
    return m_vScale;
}

void SliderItem::setVScale(const double& vs)
{
    if (m_vScale != vs) {
        m_vScale = vs;
        m_needUpdate = true;
        update();
        vScaleChanged();
    }
}

double SliderItem::vValue() const
{
    return m_vValue;
}

void SliderItem::setVValue(const double& vv)
{
    if (m_vValue != vv) {
        m_vValue = vv;
        m_needUpdate = true;
        update();
        vValueChanged();
    }
}

double SliderItem::didFocus() const
{
    return m_didFocus;
}

void SliderItem::setDidFocus(const bool& df)
{
    if (m_didFocus != df) {
        m_didFocus = df;
        m_needUpdate = true;
        update();
        didFocusChanged();
    }
}

void SliderItem::mousePressEvent(QMouseEvent* event)
{
    setFocus(true);
    setDidFocus(true);
    hideCursor();
    setCursor(Qt::BlankCursor);
    pressPos = lastPos = event->pos().y();
    y_pos = event->pos().y() * scaleY;
    saveX = event->globalPos().x();
    saveY = event->globalPos().y();

    setVPos(y_pos);
    update();
}

void SliderItem::mouseDoubleClickEvent(QMouseEvent*)
{
    setFocus(true);
    y_pos = height() * max * scaleY / (max - min);
    setVPos(y_pos);
    pix2Value();
}

void SliderItem::mouseMoveEvent(QMouseEvent* event)
{
    if (isIgnore) {
        isIgnore = false;
        return;
    }

    int diff = qAbs(pressPos - event->pos().y());
    if (diff >= 100) {
        QCursor().setPos(QCursor().pos().x(), mapToGlobal(QPoint(event->pos().x(), pressPos)).y());
        y_pos += (event->pos().y() - lastPos);
        y_pos = qBound(0.0, (double)y_pos, height() * scaleY);
        setVPos(y_pos);
        lastPos = pressPos;
        update();
        isIgnore = true;
        return;
    }

    y_pos += (event->pos().y() - lastPos);

    y_pos = qBound(0.0, (double)y_pos, height() * scaleY);
    setVPos(y_pos);
    pix2Value();

    lastPos = event->pos().y();

    update();
}

void SliderItem::mouseReleaseEvent(QMouseEvent*)
{
    setCursor(Qt::ArrowCursor);
    isIgnore = true;
    QCursor().setPos(saveX, saveY);
    setDidFocus(false);
}

void SliderItem::keyPressEvent(QKeyEvent* e)
{

    if (e->modifiers() == Qt::ShiftModifier) {
        double maxLastPos = height() * scaleY;
        scaleY *= sense;
        setVScale(scaleY);
        double maxPos = height() * scaleY;
        y_pos = y_pos * maxPos / maxLastPos;
        setVPos(y_pos);
    }
}

void SliderItem::keyReleaseEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Shift) {
        double maxLastPos = height() * scaleY;
        scaleY /= sense;
        setVScale(scaleY);
        double maxPos = height() * scaleY;
        y_pos = y_pos * maxPos / maxLastPos;
        setVPos(y_pos);
    }
}

double SliderItem::pix2Value()
{
    double result = (min - max) * (y_pos / scaleY) / height() + max;
    return result;
}
