#include "rubberbanditem.h"
#include "core/cliparea.h"
#include "core/global.h"
#include "trackitem.h"

#include <QQuickWindow>
#include <QSGSimpleRectNode>
#include <QtMath>

namespace {
// A container node holding: [0] fill rect, [1..4] border rects
struct RubberBandNode : public QSGNode {
    QSGSimpleRectNode* fill = nullptr;
    QSGSimpleRectNode* bTop = nullptr;
    QSGSimpleRectNode* bBottom = nullptr;
    QSGSimpleRectNode* bLeft = nullptr;
    QSGSimpleRectNode* bRight = nullptr;
};
}

RubberBandItem::RubberBandItem(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
    setAcceptHoverEvents(false);
    setZ(0.5e6);
}

ClipArea* RubberBandItem::clipArea() const
{
    return _clipArea;
}

void RubberBandItem::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;
}

int RubberBandItem::trackIndex() const
{
    return _trackIndex;
}

void RubberBandItem::setTrackIndex(int index)
{
    _trackIndex = index;
}

AreaInfo* RubberBandItem::areaInfo() const
{
    return _areaInfo;
}

void RubberBandItem::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;

    connect(_areaInfo, &AreaInfo::timeSelectionChanged, this, &RubberBandItem::update);
    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &RubberBandItem::update);
}

QRect RubberBandItem::drawSelectedArea()
{

    auto offset = 0;

    if (_areaInfo == Q_NULLPTR)
        return QRect();

    auto _lastWidth = _areaInfo->lastWidth();
    auto _startTime = _areaInfo->startTime();
    auto _clipDuration = _areaInfo->clipDuration();

    auto _selectedArea = _areaInfo->selectionArea()->area();

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);

    if (_selectedArea.left() >= 0 && _selectedArea.right() >= 0 && _selectedArea.right() != _selectedArea.left()) {

        double startX = ((_selectedArea.left() - _startTime) / (double)_clipDuration) * _lastWidth;
        double endX = ((_selectedArea.right() - _startTime) / (double)_clipDuration) * _lastWidth;
        if (_selectedArea.left() < _selectedArea.right()) {
            startX = ((_selectedArea.left() - _startTime) / (double)_clipDuration) * _lastWidth;
            endX = ((_selectedArea.right() - _startTime) / (double)_clipDuration) * _lastWidth;
        } else {
            endX = ((_selectedArea.left() - _startTime) / (double)_clipDuration) * _lastWidth;
            startX = ((_selectedArea.right() - _startTime) / (double)_clipDuration) * _lastWidth;
        }

        QRect drawAreaTotal = QRect(startX, boundingRect().top() + offset, (endX - startX), boundingRect().height());
        QRect drawArea = QRect(startX, boundingRect().top() + offset, (endX - startX), boundingRect().height());
        QVector<QRect> drawAreas;

        for (int i = 0; i < automationLaneModel()->count(); i++) {

            auto top = boundingRect().top() + rowsExpandedCount() * _areaInfo->titleHeight() + _areaInfo->seperatorHeight();
            auto H = automationLaneModel()->totalRowCountBefore2(i) * _areaInfo->titleHeight() + i * _areaInfo->seperatorHeight();
            auto tempRect = boundingRect();
            tempRect.setTop(top + H + offset);
            tempRect.setHeight(automationLaneModel()->getItem(i)->automationLaneRowCount * _areaInfo->titleHeight());

            auto _drawArea = QRect(startX, tempRect.top(), (endX - startX), tempRect.height());
            drawAreas.append(_drawArea);
        }

        if (!(_areaInfo->selectionArea()->isClipSelectedAtLast || _areaInfo->selectionArea()->isClipSelectedAtFirst)) {
            if (automationLaneEnabled()) {

                drawArea = QRect(startX, boundingRect().top() + _areaInfo->titleHeight() + offset, (endX - startX), boundingRect().height() - _areaInfo->titleHeight());
            }
        }
        if (automationLaneEnabled()) {
            if (_areaInfo->selectionArea()->isClipSelectedAtLast == false) {

                drawArea = QRect(startX, boundingRect().top() + _areaInfo->titleHeight() + offset, (endX - startX), boundingRect().height() - _areaInfo->titleHeight());
            }
        } else {

            if (!(_areaInfo->selectionArea()->isClipSelectedAtLast || _areaInfo->selectionArea()->isClipSelectedAtFirst)) {

                drawArea = QRect();
            }
        }

        if (_areaInfo->selectionArea()->firstTrackIndex() <= _trackIndex && _trackIndex <= _areaInfo->selectionArea()->lastTrackIndex()) {

            if (_areaInfo->selectionArea()->firstTrackIndex() == _trackIndex && _areaInfo->selectionArea()->lastTrackIndex() != _trackIndex) {

                if (automationLaneModel()->count() > 0 && automationLaneEnabled()) {

                    if (_areaInfo->selectionArea()->isClipSelectedAtLast == true && _areaInfo->selectionArea()->isClipSelectedAtFirst == true) {

                        return drawAreaTotal;
                    } else {

                        if (_areaInfo->selectionArea()->_firstSubTrackIndex == -1) {
                            if (!drawArea.isNull()) {
                                return drawArea;
                            }
                        }

                        if (drawAreas.size() > 0) {
                            if (_areaInfo->selectionArea()->_firstSubTrackIndex >= 0 && _areaInfo->selectionArea()->_firstSubTrackIndex < drawAreas.size()) {
                                if (!drawAreas[_areaInfo->selectionArea()->_firstSubTrackIndex].isNull()) {
                                    return drawAreas[_areaInfo->selectionArea()->_firstSubTrackIndex];
                                }
                            }
                        }
                    }

                } else {
                    if (!drawArea.isNull()) {
                        return drawArea;
                    }
                }

            } else if (_areaInfo->selectionArea()->lastTrackIndex() == _trackIndex && _areaInfo->selectionArea()->firstTrackIndex() != _trackIndex) {

                if (automationLaneModel()->count() > 0 && automationLaneEnabled()) {

                    if (_areaInfo->selectionArea()->isClipSelectedAtLast == true || _areaInfo->selectionArea()->isClipSelectedAtFirst == true) {
                        return drawAreaTotal;
                    } else {

                        if (_areaInfo->selectionArea()->_lastSubTrackIndex == -1) {
                            if (!drawArea.isNull()) {
                                return drawArea;
                            }
                        }

                        if (drawAreas.size() > 0) {
                            if (_areaInfo->selectionArea()->_lastSubTrackIndex >= 0 && _areaInfo->selectionArea()->_lastSubTrackIndex < drawAreas.size()) {
                                if (!drawAreas[_areaInfo->selectionArea()->_lastSubTrackIndex].isNull()) {
                                    return drawAreas[_areaInfo->selectionArea()->_lastSubTrackIndex];
                                }
                            }
                        }
                    }

                } else {

                    if (!drawArea.isNull()) {
                        return drawArea;
                    }
                }

            } else if (_areaInfo->selectionArea()->lastTrackIndex() == _trackIndex && _areaInfo->selectionArea()->firstTrackIndex() == _trackIndex) {

                if (automationLaneModel()->count() > 0 && automationLaneEnabled()) {

                    if (_areaInfo->selectionArea()->isClipSelectedAtLast == true || _areaInfo->selectionArea()->isClipSelectedAtFirst == true) {
                        return drawAreaTotal;
                    } else {

                        if (_areaInfo->selectionArea()->_firstSubTrackIndex == -1 || _areaInfo->selectionArea()->_lastSubTrackIndex == -1) {
                            if (!drawArea.isNull()) {
                                return drawArea;
                            }
                        }

                        for (int i = 0; i < automationLaneModel()->count(); i++) {
                            if (i >= _areaInfo->selectionArea()->_firstSubTrackIndex && i <= _areaInfo->selectionArea()->_lastSubTrackIndex) {

                                if (!drawAreas[i].isNull()) {
                                    return drawAreas[i];
                                }
                            }
                        }
                    }

                } else {
                    if (!drawArea.isNull()) {
                        return drawArea;
                    }
                }

            } else {

                if (_areaInfo->selectionArea()->isClipSelectedAtLast == true || _areaInfo->selectionArea()->isClipSelectedAtFirst == true) {
                    return drawAreaTotal;
                } else {
                    if (!drawArea.isNull()) {
                        return drawArea;
                    }

                    for (int i = 0; i < automationLaneModel()->count(); i++) {
                        if (!drawAreas[i].isNull()) {
                            return drawAreas[i];
                        }
                    }
                }
            }
        }
    }
    return QRect();
}

void RubberBandItem::setFillColor(const QColor& c)
{
    if (m_fill == c)
        return;
    m_fill = c;
    emit fillColorChanged();
    update();
}

void RubberBandItem::setBorderColor(const QColor& c)
{
    if (m_border == c)
        return;
    m_border = c;
    emit borderColorChanged();
    update();
}

void RubberBandItem::setBorderWidth(qreal w)
{
    if (qFuzzyCompare(m_borderWidth, w))
        return;
    m_borderWidth = qMax(0.0, w);
    emit borderWidthChanged();
    update();
}

void RubberBandItem::setActive(bool on)
{
    if (m_active == on)
        return;
    m_active = on;
    emit activeChanged();
    update();
}

int RubberBandItem::lastRowExpanded()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);
    return trackItem->getLastRowExpanded();
}

int RubberBandItem::rowsExpandedCount()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);
    return trackItem->rowsExpandedCount();
}

bool RubberBandItem::automationLaneEnabled()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);
    return trackItem->automationLaneEnabled();
}

QSharedPointer<AutomationLaneModel> RubberBandItem::automationLaneModel()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);
    return trackItem->automationLaneModel();
}

static QSGSimpleRectNode* makeRectNode(const QRectF& r, const QColor& c)
{
    auto* n = new QSGSimpleRectNode(r, c);
    n->setFlag(QSGNode::OwnsMaterial, false);
    return n;
}

QSGNode* RubberBandItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{

    auto* root = static_cast<RubberBandNode*>(oldNode);

    if (!root) {
        root = new RubberBandNode;
        root->setFlag(QSGNode::OwnedByParent, true);

        root->fill = makeRectNode({}, m_fill);
        //        root->bTop = makeRectNode({}, m_border);
        //        root->bBottom = makeRectNode({}, m_border);
        //        root->bLeft = makeRectNode({}, m_border);
        //        root->bRight = makeRectNode({}, m_border);

        root->appendChildNode(root->fill);
        //        root->appendChildNode(root->bTop);
        //        root->appendChildNode(root->bBottom);
        //        root->appendChildNode(root->bLeft);
        //        root->appendChildNode(root->bRight);
    }

    // Update colors (in case changed)
    root->fill->setColor(m_fill);
    //    root->bTop->setColor(m_border);
    //    root->bBottom->setColor(m_border);
    //    root->bLeft->setColor(m_border);
    //    root->bRight->setColor(m_border);

    QRectF r = drawSelectedArea();

    //_areaInfo->setSelectionAreaRect(r);

    if (r.x() < 0) {
        r.setX(0);
    }

    if (r.width() < 0) {
        r.setWidth(0);
    }

    root->fill->setRect(r);

    //    // Border rects (four thin rectangles)
    //    const qreal w = qMax(1.0, m_borderWidth);
    //    root->bTop->setRect(QRectF(r.left(), r.top(), r.width(), w));
    //    root->bBottom->setRect(QRectF(r.left(), r.bottom() - w, r.width(), w));
    //    root->bLeft->setRect(QRectF(r.left(), r.top(), w, r.height()));
    //    root->bRight->setRect(QRectF(r.right() - w, r.top(), w, r.height()));

    return root;
}

QRectF RubberBandItem::normalizedRect(const QPointF& a, const QPointF& b) const
{
    const qreal l = qMin(a.x(), b.x());
    const qreal t = qMin(a.y(), b.y());
    const qreal r = qMax(a.x(), b.x());
    const qreal btm = qMax(a.y(), b.y());
    return QRectF(QPointF(l, t), QPointF(r, btm));
}
