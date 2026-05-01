#include "automationshapeitem.h"

#include "automationitem.h"
#include "core/cliparea.h"
#include "core/global.h"
#include <QQuickWindow>
#include <QSGSimpleRectNode>
#include <QSGVertexColorMaterial>
#include <QtMath>

AutomationShapeItem::AutomationShapeItem(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
    setZ(12e6); // ensure on top (adjust as you like)
}

ClipArea* AutomationShapeItem::clipArea() const
{
    return _clipArea;
}

void AutomationShapeItem::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;
}

AreaInfo* AutomationShapeItem::areaInfo() const
{
    return _areaInfo;
}

void AutomationShapeItem::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;

    connect(_clipArea->tracksModel(), &TracksModel::sigClipChanged, this, &AutomationShapeItem::update);
    connect(_clipArea->tracksModel(), &TracksModel::sigUpdate, this, &AutomationShapeItem::update);

    connect(_areaInfo, &AreaInfo::playheadMarkerChanged, this, &AutomationShapeItem::update);

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);

    connect(_tracksModel, &TracksModel::sigUpdate, this, &AutomationShapeItem::sltModelUpdate);
}

void AutomationShapeItem::sltModelUpdate()
{

    if (rootNode == nullptr) {
        return;
    }

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    auto _automationMenuTitle1 = trackItem->getAutomationMenuTitle();
    auto _automationSubMenuTitle = trackItem->automationSubMenuTitle();

    if (_automationMenuTitle1 == "Mixer" && _automationSubMenuTitle == "Volume") {

        sltUpdateAutomation(trackItem->volumeAutomationItem().get());
        connect(trackItem->volumeAutomationItem().get(), &AutomationItem::sigUpdate, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
        connect(trackItem->volumeAutomationItem().get(), &AutomationItem::sigLineChanged, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
    }

    if (_automationMenuTitle1 == "Mixer" && _automationSubMenuTitle == "Pan") {

        sltUpdateAutomation(trackItem->panAutomationItem().get());
        connect(trackItem->panAutomationItem().get(), &AutomationItem::sigUpdate, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
        connect(trackItem->panAutomationItem().get(), &AutomationItem::sigLineChanged, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
    }

    if (_automationMenuTitle1 == "Mixer" && _automationSubMenuTitle == "SpeakerOn") {

        sltUpdateAutomation(trackItem->speakerOnAutomationItem().get());
        connect(trackItem->speakerOnAutomationItem().get(), &AutomationItem::sigUpdate, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
        connect(trackItem->speakerOnAutomationItem().get(), &AutomationItem::sigLineChanged, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
    }

    if (_automationMenuTitle1 == "AudioObject" && _automationSubMenuTitle == "KeyFrames") {

        sltUpdateAutomation(trackItem->keyFramesAutomationItem().get());
        connect(trackItem->keyFramesAutomationItem().get(), &AutomationItem::sigUpdate, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
        connect(trackItem->keyFramesAutomationItem().get(), &AutomationItem::sigLineChanged, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
    }

    if (_automationMenuTitle1 == "Visr") {
        auto effect = ((SampleTrack*)trackItem->track())->audioPort()->visrEfect();
        if (effect) {

            if (effect->contains(_automationSubMenuTitle) && automationLaneEnabled()) {

                auto automationItem = effect->automationItem(_automationSubMenuTitle);

                auto key = automationItem->automation()->type() + "," + automationItem->automation()->subType();
                if (!_automations.contains(key)) {
                    auto automation = new AutomationNode(key, window(), _areaInfo, _clipArea, trackItem, automationItem);
                    _automations.insert(key, automation);

                    sltUpdateAutomation(automationItem.get());
                    connect(automationItem.get(), &AutomationItem::sigUpdate, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
                    connect(automationItem.get(), &AutomationItem::sigLineChanged, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);

                    rootNode->appendChildNode(automation);
                }

                auto autom = _automations[key];

                if (autom && effect->contains(_automationSubMenuTitle) && automationLaneEnabled()) {
                    auto automationItem = effect->automationItem(_automationSubMenuTitle);

                    if (automationItem->showEnvelopes()) {
                        autom->setShow(true);
                    } else {
                        autom->setShow(false);
                    }
                }
            }
        }
    }

    auto effectChain = ((SampleTrack*)trackItem->track())->audioPort()->effects();
    auto effects = effectChain->getEffect(_automationMenuTitle1);

    if (effects) {

        if (effects->contains(_automationSubMenuTitle) && automationLaneEnabled()) {

            auto automationItem = effects->automationItem(_automationSubMenuTitle);

            auto key = automationItem->automation()->type() + "," + automationItem->automation()->subType();

            if (!_automations.contains(key)) {
                auto automation = new AutomationNode(key, window(), _areaInfo, _clipArea, trackItem, automationItem);
                _automations.insert(key, automation);

                sltUpdateAutomation(automationItem.get());
                connect(automationItem.get(), &AutomationItem::sigUpdate, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);
                connect(automationItem.get(), &AutomationItem::sigLineChanged, this, &AutomationShapeItem::sltUpdate, Qt::UniqueConnection);

                rootNode->appendChildNode(automation);
            }

            auto autom = _automations[key];

            if (autom && effects->contains(_automationSubMenuTitle) && automationLaneEnabled()) {
                auto automationItem = effects->automationItem(_automationSubMenuTitle);

                if (automationItem->showEnvelopes()) {
                    autom->setShow(true);
                } else {
                    autom->setShow(false);
                }
            }
        }
    }
}
void AutomationShapeItem::sltUpdateAutomation(AutomationItem* automationItem)
{

    auto key = automationItem->automation()->type() + "," + automationItem->automation()->subType();

    if (!_automations.contains(key)) {
        return;
    }

    auto automationNode = _automations[key];

    auto _tracksModel = _clipArea->tracksModel();
    auto _trackItem = _tracksModel->trackItem(m_trackIndex);

    auto _automationLaneModel = _trackItem->automationLaneModel();
    auto _rowsExpandedCount = _trackItem->rowsExpandedCount();

    auto index = automationItem->automation()->getSubTrackIndex();
    auto subIndex = _automationLaneModel->getIndex(automationItem->automation()->type(), automationItem->automation()->subType());

    if (index == -1) {
        auto tempRect = boundingRect().toRect();
        tempRect.setTop(boundingRect().top() + _areaInfo->titleHeight());
        tempRect.setHeight(_areaInfo->titleHeight() * (_trackItem->rowsExpandedCount() - 1));

        automationItem->setRect(tempRect);
        automationItem->automation()->update(tempRect);
        automationItem->setCurrentIndicator(_trackItem->currentIndicator());

    } else {

        if (subIndex >= 0) {
            auto top = boundingRect().top() + _rowsExpandedCount * _areaInfo->titleHeight() + _areaInfo->seperatorHeight();

            auto H = _automationLaneModel->totalRowCountBefore2(subIndex) * _areaInfo->titleHeight() + subIndex * _areaInfo->seperatorHeight();

            auto tempRect = boundingRect().toRect();
            tempRect.setTop(top + H);
            tempRect.setHeight(_automationLaneModel->getItem(subIndex)->automationLaneRowCount * _areaInfo->titleHeight());

            automationItem->setRect(tempRect);
            automationItem->automation()->update(tempRect);
            automationItem->setCurrentIndicator(_trackItem->currentIndicator());
        } else {
            return;
        }
    }

    auto lines = automationItem->automation()->getFinalLines();
    auto selectedLines = automationItem->automation()->getSelectedLines();

    {
        QMutexLocker lock(&m_mutex);
        m_pendingUpserts.clear();
        m_pendingPoints.clear();
        m_pendingSelectedLines.clear();
        for (int i = 0; i < lines.size(); i++) {

            auto color = "#FFA500";
            if (lines[i].hovred() && lines[i].enabled())
                color = "#00ff66"; // hovered + enabled
            else if (lines[i].hovred() && !lines[i].enabled())
                color = "#E0A870"; // hovered + disabled  (example color)
            else if (!lines[i].hovred() && lines[i].enabled())
                color = "#FFA500"; // not hovered + enabled
            else
                color = "#D3D3D3"; // not hovered + disabled (example color)

            if (i < lines.size() - 1) {

                m_pendingPoints.push_back(ColoredPoint { lines[i].fp2(), color, lines[i].p2()->hovred() });
            }

            m_pendingUpserts.push_back(ColoredLine { QLineF(lines[i].fp1(), lines[i].fp2()), color, lines[i].curveM(), lines[i].curveN(), lines[i].startX(), lines[i].endX() });
        }

        for (int i = 0; i < selectedLines.size(); i++) {

            auto color = "#FFA500";
            if (selectedLines[i].hovred() && selectedLines[i].enabled())
                color = "#00ff66"; // hovered + enabled
            else if (selectedLines[i].hovred() && !selectedLines[i].enabled())
                color = "#E0A870"; // hovered + disabled  (example color)
            else if (!selectedLines[i].hovred() && selectedLines[i].enabled())
                color = "#FFA500"; // not hovered + enabled
            else
                color = "#D3D3D3"; // not hovered + disabled (example color)

            m_pendingSelectedLines.push_back(ColoredLine { QLineF(selectedLines[i].fp1(), selectedLines[i].fp2()), color, selectedLines[i].curveM(), selectedLines[i].curveN(), selectedLines[i].startX(), selectedLines[i].endX() });
        }

        if (automationNode) {

            automationNode->applyPending(m_pendingUpserts, m_pendingPoints, m_pendingSelectedLines);
        }
    }

    update();
}
void AutomationShapeItem::sltUpdate()
{

    AutomationItem* automationItem = qobject_cast<AutomationItem*>(sender());
    if (!automationItem)
        return; // safety check

    sltUpdateAutomation(automationItem);
}

bool AutomationShapeItem::automationLaneEnabled()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->automationLaneEnabled();
}

QSharedPointer<AutomationLaneModel> AutomationShapeItem::automationLaneModel()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->automationLaneModel();
}

int AutomationShapeItem::trackIndex() const
{
    return m_trackIndex;
}

void AutomationShapeItem::setTrackIndex(int index)
{
    m_trackIndex = index;
}

static void addQuad(QSGGeometry::Point2D* v, quint16* idx,
    int quadIndex, const QPointF& a, const QPointF& b, float halfT)
{
    // Build a rectangle centered on segment a->b, thickness = 2*halfT
    QVector2D dir(b - a);
    dir.normalize();
    QVector2D n(-dir.y(), dir.x()); // left-normal
    QVector2D off = n * halfT;

    int vbase = quadIndex * 4;
    int ibase = quadIndex * 6;

    v[vbase + 0].set(a.x() + off.x(), a.y() + off.y());
    v[vbase + 1].set(b.x() + off.x(), b.y() + off.y());
    v[vbase + 2].set(b.x() - off.x(), b.y() - off.y());
    v[vbase + 3].set(a.x() - off.x(), a.y() - off.y());

    // two triangles
    idx[ibase + 0] = vbase + 0;
    idx[ibase + 1] = vbase + 1;
    idx[ibase + 2] = vbase + 2;
    idx[ibase + 3] = vbase + 0;
    idx[ibase + 4] = vbase + 2;
    idx[ibase + 5] = vbase + 3;
}

QSGNode* AutomationShapeItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{

    rootNode = oldNode ? oldNode : new QSGNode;

    if (_clipArea == nullptr) {
        return rootNode;
    }

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    auto volumeAutomation = trackItem->volumeAutomationItem();
    auto panAutomation = trackItem->panAutomationItem();
    auto speakerOn = trackItem->speakerOnAutomationItem();
    auto keyFrames = trackItem->keyFramesAutomationItem();
    auto _automationMenuTitle1 = trackItem->getAutomationMenuTitle();
    auto _automationSubMenuTitle = trackItem->automationSubMenuTitle();

    if (!_volumeAutomationNode) {
        _volumeAutomationNode = new AutomationNode("Mixer,Volume", window(), _areaInfo, _clipArea, trackItem, volumeAutomation);
        _automations.insert("Mixer,Volume", _volumeAutomationNode);
        rootNode->appendChildNode(_volumeAutomationNode);
    }

    if (!_panAutomationNode) {
        _panAutomationNode = new AutomationNode("Mixer,Pan", window(), _areaInfo, _clipArea, trackItem, panAutomation);
        _automations.insert("Mixer,Pan", _panAutomationNode);
        rootNode->appendChildNode(_panAutomationNode);
    }

    if (!_speakerAutomationNode) {
        _speakerAutomationNode = new AutomationNode("Mixer,SpeakerOn", window(), _areaInfo, _clipArea, trackItem, speakerOn);
        _automations.insert("Mixer,SpeakerOn", _speakerAutomationNode);
        rootNode->appendChildNode(_speakerAutomationNode);
    }

    if (!_keyFramesAutomationNode) {
        _keyFramesAutomationNode = new AutomationNode("AudioObject,KeyFrames", window(), _areaInfo, _clipArea, trackItem, keyFrames);
        _automations.insert("AudioObject,KeyFrames", _keyFramesAutomationNode);
        rootNode->appendChildNode(_keyFramesAutomationNode);
    }

    if (volumeAutomation->showEnvelopes() == false || !trackItem->automationLaneEnabled() || trackItem->rowsExpandedCount() <= 2) {
        _volumeAutomationNode->setShow(false);
    } else {
        _volumeAutomationNode->setShow(true);
    }

    if (panAutomation->showEnvelopes() == false || !trackItem->automationLaneEnabled() || trackItem->rowsExpandedCount() <= 2) {
        _panAutomationNode->setShow(false);
    } else {
        _panAutomationNode->setShow(true);
    }

    if (speakerOn->showEnvelopes() == false || !trackItem->automationLaneEnabled() || trackItem->rowsExpandedCount() <= 2) {
        _speakerAutomationNode->setShow(false);
    } else {
        _speakerAutomationNode->setShow(true);
    }

    if (keyFrames->showEnvelopes() == false || !trackItem->automationLaneEnabled() || trackItem->rowsExpandedCount() <= 2) {
        _keyFramesAutomationNode->setShow(false);
    } else {
        _keyFramesAutomationNode->setShow(true);
    }

    _volumeAutomationNode->updateGeometryIfNeeded(_volumeAutomationNode->automationItem()->rect());
    _panAutomationNode->updateGeometryIfNeeded(_panAutomationNode->automationItem()->rect());
    _keyFramesAutomationNode->updateGeometryIfNeeded(_keyFramesAutomationNode->automationItem()->rect());
    _speakerAutomationNode->updateGeometryIfNeeded(_speakerAutomationNode->automationItem()->rect());

    QMapIterator<QString, AutomationNode*> i(_automations);
    while (i.hasNext()) {
        i.next();

        i.value()->updateGeometryIfNeeded(i.value()->automationItem()->rect());
    }

    return rootNode;
}

CircleNode::CircleNode()
{
    setFlag(QSGNode::OwnsGeometry, true);
    setFlag(QSGNode::OwnsMaterial, true);

    auto* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0, 0);
    g->setDrawingMode(QSGGeometry::DrawTriangles);
    setGeometry(g);

    m_mat = new QSGFlatColorMaterial();
    setMaterial(m_mat);
}

void CircleNode::setCenter(const QPointF& c)
{
    if (c == m_center)
        return;
    m_center = c;
    m_dirty = true;
}
void CircleNode::setRadius(float r)
{
    if (qFuzzyCompare(r, m_radius))
        return;
    m_radius = r;
    m_dirty = true;
}
void CircleNode::setColor(const QColor& c)
{
    if (c == m_color)
        return;
    m_color = c;
    m_dirty = true;
}

void CircleNode::updateGeometryIfNeeded()
{

    rebuild();
}

void CircleNode::rebuild()
{
    if (m_radius <= 0.f) {
        auto* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0, 0);
        setGeometry(g);
        markDirty(QSGNode::DirtyGeometry);
        return;
    }

    // Choose resolution: decent quality without overdraw
    const int segments = qBound(16, int(m_radius * 0.75), 96); // heuristic
    // center + segments+1 (closing vertex)
    const int vCount = 1 + segments + 1;
    const int triCount = segments; // triangle fan
    auto* geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vCount, triCount * 3);
    geom->setDrawingMode(QSGGeometry::DrawTriangles);

    auto* v = geom->vertexDataAsPoint2D();
    quint16* idx = reinterpret_cast<quint16*>(geom->indexData());

    // center
    v[0].set(m_center.x(), m_center.y());

    // ring
    const float twoPi = 6.28318530718f;
    for (int i = 0; i <= segments; ++i) {
        float t = (float(i) / segments) * twoPi;
        float x = m_center.x() + m_radius * std::cos(t);
        float y = m_center.y() + m_radius * std::sin(t);
        v[1 + i].set(x, y);
    }

    // indices for triangle fan: (0, i, i+1)
    int k = 0;
    for (int i = 1; i <= segments; ++i) {
        idx[k++] = 0;
        idx[k++] = i;
        idx[k++] = i + 1;
    }

    setGeometry(geom);
    m_mat->setColor(m_color);
    markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
}

float CircleNode::radius() const
{
    return m_radius;
}

AutomationNode::AutomationNode(QString name, QQuickWindow* window, AreaInfo* areaInfo, ClipArea* clipArea, QSharedPointer<TrackItem> trackItem, QSharedPointer<AutomationItem> automationItem)
{
    _window = window;
    _name = name;
    _areaInfo = areaInfo;
    _clipArea = clipArea;
    _trackItem = trackItem;
    _automationItem = automationItem;

    setFlag(QSGNode::OwnsGeometry, true);
    setFlag(QSGNode::OwnsMaterial, true);

    auto* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0, 0);
    g->setDrawingMode(QSGGeometry::DrawTriangles);
    setGeometry(g);

    m_mat = new QSGFlatColorMaterial();
    setMaterial(m_mat);
}

void AutomationNode::updateGeometryIfNeeded(QRectF boundingRect)
{
    QVector<ColoredLine> localReplace;
    QVector<ColoredLine> localSelectedLinesReplace;
    QVector<ColoredPoint> localPoints;

    doReplace = false;

    { // minimal lock
        QMutexLocker lk(&m_mutex);

        if (m_replaceAllPending && m_dirty2) {
            doReplace = true;
            localReplace.swap(m_replaceAllBuffer);
            localPoints.swap(m_replaceAllPoints);
            localSelectedLinesReplace.swap(m_replaceAllSelectedLines);
            m_replaceAllPending = false;
        }
        m_dirty2 = false;
    }

    if (doReplace) {

        m_lines.swap(localReplace);
        m_points.swap(localPoints);
        m_selectedLines.swap(localSelectedLinesReplace);
        m_geomDirty = true;
    }

    _boundingRect = boundingRect;
    rebuild();
}

QSharedPointer<AutomationItem> AutomationNode::automationItem() const
{
    return _automationItem;
}

QPointF AutomationNode::drawLevel(QSize imgSize, QPointF correctedPoint)
{

    QRectF imgRect = QRectF(0, 0, imgSize.width(), imgSize.height());
    imgRect = imgRect.marginsAdded(QMarginsF(2.0, 2.0, 2.0, 2.0));

    auto offsetX = ScreenInterface::scaleSize2(60, _areaInfo->zoomFactor());
    auto offsetY = ScreenInterface::scaleSize2(60, _areaInfo->zoomFactor());

    if (correctedPoint.x() < _boundingRect.width() / 2) {
        correctedPoint.setX(correctedPoint.x() + offsetX);
    } else {
        correctedPoint.setX(correctedPoint.x() - offsetX - imgRect.width());
    }

    if (correctedPoint.x() < 0) {
        correctedPoint.setX(5.0);
    }

    if (correctedPoint.y() - offsetY < _boundingRect.top()) {
        correctedPoint.setY(_boundingRect.top() + 5.0);
    } else {
        correctedPoint.setY(correctedPoint.y() - offsetY);
    }

    if (correctedPoint.y() + imgRect.height() > _boundingRect.bottom()) {
        correctedPoint.setY(_boundingRect.bottom() - imgRect.height() - 2.0);
    }

    if (correctedPoint.x() > _boundingRect.width()) {
        correctedPoint.setX(_boundingRect.width() - offsetX - 5.0);
    }

    return correctedPoint;
}

void AutomationNode::rebuild()
{

    auto _automationLaneModel = _trackItem->automationLaneModel();

    bool conatins = false;

    auto key = _trackItem->getAutomationMenuTitle().toLower() + "," + _trackItem->automationSubMenuTitle().toLower();
    if (key == _name.toLower()) {
        conatins = true;
    }

    if (_automationLaneModel->contains(_name.split(",")[0], _name.split(",")[1])) {
        conatins = true;
    }

    if (conatins == false) {
        _show = false;
    }

    if (!_dashedNode) {
        _dashedNode = new DashedLineNode(_areaInfo, _automationItem);
        appendChildNode(_dashedNode);
    }

    if (!_redDashedNode) {
        _redDashedNode = new DashedLineNode(_areaInfo, _automationItem);
        _redDashedNode->setShow(false);

        appendChildNode(_redDashedNode);
    }

    if (!_linesNode) {
        _linesNode = new ColoredLinesNode();
        _linesNode->setJointMarker(ColoredLinesNode::JointMarker::Hollow);
        appendChildNode(_linesNode);
    }
    _linesNode->setBoundingRect(_boundingRect);

    if (!_selectedLinesNode) {
        _selectedLinesNode = new ColoredLinesNode();
        _selectedLinesNode->setJointMarker(ColoredLinesNode::JointMarker::Hollow);
        appendChildNode(_selectedLinesNode);
    }
    _selectedLinesNode->setBoundingRect(_boundingRect);

    if (!_circleNode) {
        _circleNode = new CircleMarkersNode();
        appendChildNode(_circleNode);
    }

    if (!_hoverCircleNode) {
        _hoverCircleNode = new CircleNode();
        appendChildNode(_hoverCircleNode);
    }

    if (!_labelBoxNode) {
        _labelBoxNode = new LabelBoxNode(_window, _areaInfo, _clipArea);
        _labelBoxNode->rebuild();
        appendChildNode(_labelBoxNode);
    }

    if (m_lines.size() == 0) {
        _show = false;
    }

    if (_show == false) {
        _hoverCircleNode->setRadius(0);
        _dashedNode->setShow(false);
        _dashedNode->rebuild(_boundingRect);

        _linesNode->setShow(false);
        _linesNode->rebuild();

        _selectedLinesNode->setShow(false);
        _selectedLinesNode->rebuild();

        _labelBoxNode->setVisible(false);
        _labelBoxNode->rebuild();

        _circleNode->setShow(false);
        _circleNode->rebuild();

        _redDashedNode->setShow(false);
        _redDashedNode->rebuild(_boundingRect);
        return;
    }

    if (_automationItem->automation()->getAsyncClear()) {
        _automationItem->automation()->clear();
    }

    if (_automationItem->saveingLastTemplatesLines()) {
        _automationItem->setSaveingLastTemplatesLines(false);
        _automationItem->setSelectedLinesMoving(false);
        _automationItem->setSelectedPointsMoving(false);
        _automationItem->setLiveMovingEnabled(false);

        _automationItem->automation()->saveTemplateLine();
        _automationItem->automation()->createSelectedPoints(_areaInfo->selectionArea()->area().left(), _areaInfo->selectionArea()->area().right());
    }

    if (qAbs(_automationItem->lastStartMarker() - _areaInfo->selectionArea()->area().left()) > 0.0001
        || qAbs(_automationItem->lastEndMarker() - _areaInfo->selectionArea()->area().right()) > 0.001) {

        _automationItem->setLastEndMarker(_areaInfo->selectionArea()->area().right());
        _automationItem->setLastStartMarker(_areaInfo->selectionArea()->area().left());

        _automationItem->automation()->saveTemplateLine();
        _automationItem->automation()->createSelectedPoints(_areaInfo->selectionArea()->area().left(), _areaInfo->selectionArea()->area().right());
    }

    if (_automationItem->automation()->getSelectedRangeMoveEnabled() == false) {
        _automationItem->setCheckStrachingActive(true);

        _automationItem->checkEvents();

    } else {

        if (_automationItem->checkStrachingActive() == true) {
            _automationItem->setCheckStrachingActive(false);
        }

        _automationItem->setPointActive(false);
        _automationItem->setInnerActive(false);
        _automationItem->setOuterActive(false);
        _automationItem->setCurrentRangeIndex(-1);
    }

    if (_automationItem->selectAllRangeEvent()) {
        _automationItem->setSelectAllRangeEvent(false);
        _automationItem->automation()->selectAll();
    }

    if (m_lines.size() == 1) {

        auto dashedPoint = _automationItem->automation()->dashedLineHovred();
        _dashedNode->setHasGap(dashedPoint.isNull());

        auto normalizedLine = QLineF(QPointF(_boundingRect.left(),
                                         _automationItem->automation()->getYPixel(_automationItem->automation()->normalized())),
            QPointF(_boundingRect.right(), _automationItem->automation()->getYPixel(_automationItem->automation()->normalized())));

        // _dashedNode->setColor(_dashedLine.color());
        _dashedNode->setLine(normalizedLine);
        _dashedNode->setShow(true);
        _dashedNode->rebuild(_boundingRect);

        _hoverCircleNode->setCenter(dashedPoint);
        _hoverCircleNode->setRadius(4);

        if (dashedPoint.x() <= 0 || dashedPoint.x() >= _boundingRect.width() || dashedPoint.y() < _boundingRect.top() || dashedPoint.y() >= _boundingRect.bottom()) {
            _hoverCircleNode->setRadius(0);
        } else {
            if (_automationItem->mouseHover() && _areaInfo->currentHoveTrackIndex() >= 0) {
                _hoverCircleNode->setRadius(4);
            } else {
                _hoverCircleNode->setRadius(0);
            }
        }
        _hoverCircleNode->updateGeometryIfNeeded();

        _redDashedNode->setShow(false);
        _redDashedNode->rebuild(_boundingRect);

        _circleNode->setShow(false);
        _circleNode->rebuild();

        _linesNode->setShow(false);
        _linesNode->rebuild();

        _selectedLinesNode->setShow(false);
        _selectedLinesNode->rebuild();

    } else {

        _dashedNode->setShow(false);
        _dashedNode->rebuild(_boundingRect);

        _linesNode->setShow(true);
        _linesNode->setLines(m_lines);
        _linesNode->rebuild();

        _circleNode->setShow(true);
        _circleNode->setPoints(m_points);
        _circleNode->rebuild();

        _selectedLinesNode->setShow(true);
        _selectedLinesNode->setLines(m_selectedLines);
        _selectedLinesNode->rebuild();

        auto correctedPoint = _automationItem->hitHoverPoint();

        auto status = _automationItem->hitHoverState();

        _hoverCircleNode->setCenter(correctedPoint);

        bool statusOk = (status == Automation::OnLine || status == Automation::OnPoint);
        bool pointOk = correctedPoint.x() >= 0 && correctedPoint.x() < _boundingRect.width() && correctedPoint.y() >= _boundingRect.top() && correctedPoint.y() < _boundingRect.bottom();

        _hoverCircleNode->setRadius(statusOk && pointOk ? 4 : 0);
        _hoverCircleNode->updateGeometryIfNeeded();

        if (_automationItem->automation()->enabled() == false) {

            auto normalizedLine = QLineF(QPointF(_boundingRect.left(),
                                             _automationItem->automation()->getYPixel(_automationItem->automation()->normalized())),
                QPointF(_boundingRect.right(), _automationItem->automation()->getYPixel(_automationItem->automation()->normalized())));

            _redDashedNode->setLine(normalizedLine);
            _redDashedNode->setShow(true);
            _redDashedNode->setColor(QColor("#ff0000"));
            _redDashedNode->rebuild(_boundingRect);
        } else {

            _redDashedNode->setShow(false);
            _redDashedNode->rebuild(_boundingRect);
        }
    }

    if (_areaInfo->altKeyPressed() == false) {

        if (_automationItem->automation()->fixedVertical() == false) {

            _labelBoxNode->setLabel(_automationItem->currentValue());

            if (_automationItem->hitHoverState() != Automation::None) {

                auto img = _labelBoxNode->renderToSize(_automationItem->currentValue());

                auto correctedPoint = _automationItem->hitHoverPoint();

                auto point = drawLevel(img, correctedPoint);

                _labelBoxNode->setPosition(point);

                _labelBoxNode->setVisible(true);
            } else if (_automationItem->hitPressedState() != Automation::None) {

                auto img = _labelBoxNode->renderToSize(_automationItem->currentValue());

                auto correctedPoint = _automationItem->hitPressedPoint();

                auto point = drawLevel(img, correctedPoint);

                _labelBoxNode->setPosition(point);

                _labelBoxNode->setVisible(true);
            } else {
                _labelBoxNode->setVisible(false);
            }

            _labelBoxNode->rebuild();
        } else {
            _labelBoxNode->setVisible(false);
            _labelBoxNode->rebuild();
        }

    } else {
        _labelBoxNode->setVisible(false);
        _labelBoxNode->rebuild();
    }
}

bool AutomationNode::show() const
{
    return _show;
}

void AutomationNode::setShow(bool newShow)
{
    _show = newShow;
}

void AutomationNode::applyPending(QVector<ColoredLine> m_pendingUpserts, QVector<ColoredPoint> m_pendingPoints, QVector<ColoredLine> m_pendingSelectedLines)
{
    {
        QMutexLocker lk(&m_mutex);
        m_replaceAllPending = true;
        m_replaceAllSelectedLines = std::move(m_pendingSelectedLines);
        m_replaceAllBuffer = std::move(m_pendingUpserts);
        m_replaceAllPoints = std::move(m_pendingPoints);

        m_dirty2 = true;
    }
}

// ============================================================================
// Helpers
// ============================================================================
static inline bool samePt(const QPointF& a, const QPointF& b, qreal eps = 1e-3)
{
    return std::abs(a.x() - b.x()) <= eps && std::abs(a.y() - b.y()) <= eps;
}

// ============================================================================
// ColoredLinesNode implementation
// ============================================================================
inline ColoredLinesNode::ColoredLinesNode()
{

    setFlag(QSGNode::OwnsGeometry, true);
    setFlag(QSGNode::OwnsMaterial, true);

    m_geom = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 0, 0);
    m_geom->setDrawingMode(QSGGeometry::DrawLines);
    setGeometry(m_geom);

    m_mat = new QSGVertexColorMaterial();
    m_mat->setFlag(QSGMaterial::Blending, true);
    setMaterial(m_mat);

    //    setThickness(2.f);
    //    setPixelSnap(true);
    //    setSuperellipseSteps(96);

    //    setJointMarker(ColoredLinesNode::JointMarker::Hollow);
    //    setJointRadius(4.f);
    //    setJointRingWidth(2.f);
    //    useLineColorForJoint(true);
    //    setUseJointBgEraser(true);
    //    setJointBackgroundColor(QColor("#202020"));
}

inline void ColoredLinesNode::setLines(const QVector<ColoredLine>& lines)
{
    m_lines = lines;
}

inline void ColoredLinesNode::setThickness(float t)
{
    m_thickness = qMax(0.f, t);
}
static QVector<QPointF> buildCurvePoints(const QPointF& a,
    const QPointF& b,
    double m_in,
    double n_in,
    int width,
    int steps)
{
    QVector<QPointF> pts;

    if (steps < 2)
        steps = 2;

    // Straight "curve"
    if (qFuzzyCompare(n_in, 1.0) && qFuzzyCompare(m_in, 1.0)) {
        pts << a << b;
        return pts;
    }

    const double n = std::abs(n_in);
    const double m = std::abs(m_in);

    QSet<int> xList;

    // Start with first point
    pts.push_back(a);
    xList.insert(qRound(a.x()));

    if (n_in < 0) {
        const double cx = a.x();
        const double cy = b.y();
        const double A = a.y() - cy; // vertical semi-axis
        const double B = cx - b.x(); // horizontal semi-axis

        if (std::abs(A) < 1e-6 || std::abs(B) < 1e-6) {
            // Just a straight segment
            pts.clear();
            pts << a << b;
            return pts;
        }

        // interior points (skip i=0 and i=steps: they are a and b)
        for (int i = 1; i < steps; ++i) {
            const double t = double(i) / steps;
            const double x = a.x() + (b.x() - a.x()) * t;

            if (x < -1)
                continue;
            if (x > width + 1)
                continue;

            const double xn = std::pow(std::abs((x - cx) / B), n);
            const double yv = std::max(0.0, 1.0 - xn);
            const double y = A * std::pow(yv, 1.0 / m) + cy;

            const int xi = qRound(x);
            if (xList.contains(xi))
                continue;
            xList.insert(xi);

            const QPointF p(x, y);
            if (!std::isfinite(p.x()) || !std::isfinite(p.y())) {
                pts.clear();
                pts << a << b;
                return pts;
            }

            pts.push_back(p);
        }
    } else {
        const double cx = b.x();
        const double cy = a.y();
        const double A = a.x() - cx; // horizontal semi-axis
        const double B = cy - b.y(); // vertical semi-axis

        if (std::abs(A) < 1e-6 || std::abs(B) < 1e-6) {
            // Just a straight segment
            pts.clear();
            pts << a << b;
            return pts;
        }

        // interior points (skip i=0 and i=steps: they are a and b)
        for (int i = 1; i < steps; ++i) {
            const double t = double(i) / steps;
            const double x = a.x() + (b.x() - a.x()) * t;

            if (x < -1)
                continue;
            if (x > width + 1)
                continue;

            const double xn = std::pow(std::abs((x - cx) / A), n);
            const double yv = std::max(0.0, 1.0 - xn);
            const double y = -B * std::pow(yv, 1.0 / m) + cy;

            const int xi = qRound(x);
            if (xList.contains(xi))
                continue;
            xList.insert(xi);

            const QPointF p(x, y);
            if (!std::isfinite(p.x()) || !std::isfinite(p.y())) {
                pts.clear();
                pts << a << b;
                return pts;
            }

            pts.push_back(p);
        }
    }

    // Ensure last point is b
    if (pts.isEmpty() || pts.back() != b)
        pts.push_back(b);

    return pts;
}
inline void addQuad2(QSGGeometry::ColoredPoint2D*& v,
    const QPointF& a,
    const QPointF& b,
    const QColor& color,
    float halfWidth)
{
    // Compute unit normal to segment (a->b)
    const QVector2D ab(b - a);
    const float len = ab.length();
    if (len <= 0.00001f)
        return; // safety: caller usually filters, but double-guard

    const float invLen = 1.0f / len;
    // Left-hand normal (CCW): (-y, x)
    const float nx = -ab.y() * invLen;
    const float ny = ab.x() * invLen;

    const QPointF off(nx * halfWidth, ny * halfWidth);

    // Quad corners in CCW order
    const QPointF p0 = a - off;
    const QPointF p1 = a + off;
    const QPointF p2 = b + off;
    const QPointF p3 = b - off;

    const uchar R = uchar(color.red());
    const uchar G = uchar(color.green());
    const uchar B = uchar(color.blue());
    const uchar A = uchar(color.alpha());

    // Triangle 1: p0, p1, p2
    v[0].x = p0.x();
    v[0].y = p0.y();
    v[0].r = R;
    v[0].g = G;
    v[0].b = B;
    v[0].a = A;
    v[1].x = p1.x();
    v[1].y = p1.y();
    v[1].r = R;
    v[1].g = G;
    v[1].b = B;
    v[1].a = A;
    v[2].x = p2.x();
    v[2].y = p2.y();
    v[2].r = R;
    v[2].g = G;
    v[2].b = B;
    v[2].a = A;

    // Triangle 2: p2, p3, p0
    v[3].x = p2.x();
    v[3].y = p2.y();
    v[3].r = R;
    v[3].g = G;
    v[3].b = B;
    v[3].a = A;
    v[4].x = p3.x();
    v[4].y = p3.y();
    v[4].r = R;
    v[4].g = G;
    v[4].b = B;
    v[4].a = A;
    v[5].x = p0.x();
    v[5].y = p0.y();
    v[5].r = R;
    v[5].g = G;
    v[5].b = B;
    v[5].a = A;

    v += 6; // advance write pointer by the number of vertices we wrote
}
static inline bool isDegenerate(const QPointF& p1, const QPointF& p2)
{
    return qFuzzyCompare(p1.x(), p2.x()) && qFuzzyCompare(p1.y(), p2.y());
}

void ColoredLinesNode::rebuild()
{
    // Handle hidden/zero-thickness quickly
    if (!_show || m_thickness <= 0.f || m_lines.isEmpty()) {
        if (!m_geom) {
            m_geom = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 0, 0);
            setGeometry(m_geom);
        }
        m_geom->allocate(0, 0);
        m_geom->setDrawingMode(QSGGeometry::DrawTriangles);
        markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
        return;
    }

    // pass 1: count total quads
    int totalQuads = 0;
    for (const auto& s : m_lines) {
        const QPointF p1 = s.line.p1();
        const QPointF p2 = s.line.p2();

        double startX = s.startX;
        double endX = s.endX;
        if (isDegenerate(p1, p2))
            continue;

        if (s.curve_m == 1.0 && s.curve_n == 1.0) {
            totalQuads += 1;
        } else {
            const QVector<QPointF> pts = buildCurvePoints(p2, p1, s.curve_m, s.curve_n, _boundingRect.width(), qAbs(p2.x() - p1.x()));
            QVector<QPointF> filtered;
            filtered.reserve(pts.size());

            for (const QPointF& pt : pts) {
                if (pt.x() < startX || pt.x() > endX) {
                    continue;
                }

                filtered.append(pt);
            }

            totalQuads += std::max(0, filtered.size() - 1);
        }
    }

    const int vCount = totalQuads * 6; // 6 verts per quad (2 triangles)
    if (!m_geom) {
        m_geom = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), vCount, 0);
        setGeometry(m_geom);
    } else {
        m_geom->allocate(vCount, 0);
    }
    m_geom->setDrawingMode(QSGGeometry::DrawTriangles);

    // pass 2: fill vertices
    auto* v = reinterpret_cast<QSGGeometry::ColoredPoint2D*>(m_geom->vertexData());
    const float halfW = 0.5f * m_thickness;

    for (const auto& s : m_lines) {
        const QPointF p1 = s.line.p1();
        const QPointF p2 = s.line.p2();

        double startX = s.startX;
        double endX = s.endX;

        if (isDegenerate(p1, p2))
            continue;

        if (s.curve_m == 1.0 && s.curve_n == 1.0) {
            addQuad2(v, p1, p2, s.color, halfW);
        } else {
            const QVector<QPointF> pts = buildCurvePoints(p2, p1, s.curve_m, s.curve_n, _boundingRect.width(), qAbs(p2.x() - p1.x()));
            for (int i = 0; i + 1 < pts.size(); ++i) {
                const QPointF& a = pts[i];
                const QPointF& b = pts[i + 1];

                if (a.x() >= startX && a.x() <= endX && b.x() >= startX && b.x() <= endX) {
                    addQuad2(v, a, b, s.color, halfW);
                }
            }
        }
    }

    markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
}

bool ColoredLinesNode::show() const
{
    return _show;
}

void ColoredLinesNode::setShow(bool newShow)
{
    _show = newShow;
}

DashedLineNode::DashedLineNode(AreaInfo* areaInfo, QSharedPointer<AutomationItem> automationItem)
{
    _automationItem = automationItem;
    _areaInfo = areaInfo;

    geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0, 0);
    geom->setDrawingMode(QSGGeometry::DrawTriangles);
    setGeometry(geom);
    m_mat = new QSGFlatColorMaterial();
    m_mat->setColor(QColor("#eeeeee"));
    setMaterial(m_mat);
}

void DashedLineNode::rebuild(QRectF _boundingRect)
{

    if (_show == false) {
        geom->allocate(0, 0);
        markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
        return;
    }

    qreal m_thickness { 1.0 };
    qreal m_dash { 8.0 };
    qreal m_gap { 6.0 };

    const QPointF A = m_line.p1();
    const QPointF B = m_line.p2();
    const QVector2D AB(B - A);
    const float L = AB.length();

    if (m_hasGap == false) {
        m_gap = 0;
    }

    const float pattern = m_dash + qMax(0.0, m_gap);
    const int dashCount = qFloor(L / pattern) + ((int)fmod(L, pattern) > 0 ? 1 : 0);

    QSGGeometry* geom = geometry();

    geom->allocate(dashCount * 4, dashCount * 6); // 2 verts per segment
    geom->setDrawingMode(QSGGeometry::DrawTriangles);
    m_mat->setColor(m_color);

    auto* v = geom->vertexDataAsPoint2D();
    auto* idx = reinterpret_cast<quint16*>(geom->indexData());

    const QVector2D dir = AB.normalized();
    float cursor = 0.f;
    int qi = 0;
    for (; qi < dashCount; ++qi) {
        float dashStart = cursor;
        float dashEnd = qMin(cursor + (float)m_dash, L);
        if (dashStart >= L)
            break;

        QPointF p0 = A + (dir * dashStart).toPointF();
        QPointF p1 = A + (dir * dashEnd).toPointF();

        addQuad(v, idx, qi, p0, p1, float(m_thickness * 0.5));
        cursor += pattern;
    }

    if (qi != dashCount) {
        geom->allocate(qi * 4, qi * 6);
    }

    markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
}

bool DashedLineNode::show() const
{
    return _show;
}

void DashedLineNode::setShow(bool newShow)
{
    _show = newShow;
}

QColor DashedLineNode::color() const
{
    return m_color;
}

void DashedLineNode::setColor(const QColor& newColor)
{
    m_color = newColor;
}

QLineF DashedLineNode::line() const
{
    return m_line;
}

void DashedLineNode::setLine(const QLineF& newLine)
{
    m_line = newLine;
}

bool DashedLineNode::hasGap() const
{
    return m_hasGap;
}

void DashedLineNode::setHasGap(bool newHasGap)
{
    m_hasGap = newHasGap;
}

LabelBoxNode::LabelBoxNode(QQuickWindow* win, AreaInfo* areaInfo, ClipArea* clipArea)
    : m_window(win)
{

    _areaInfo = areaInfo;
    _clipArea = clipArea;

    m_bg = new QSGSimpleRectNode();
    appendChildNode(m_bg);

    m_textNode = new QSGSimpleTextureNode();
    appendChildNode(m_textNode);
}

QString LabelBoxNode::loadSourceSansPro()
{
    static QString cached;
    if (!cached.isEmpty())
        return cached;

    const int id = QFontDatabase::addApplicationFont(":/resources/font/SourceSansPro-Regular.ttf");
    if (id < 0) {
        qDebug() << "Failed to load SourceSansPro-Regular.ttf from resources";
        cached = QLatin1String("Sans Serif"); // fallback
        return cached;
    }
    const QStringList fams = QFontDatabase::applicationFontFamilies(id);
    if (fams.isEmpty()) {
        qDebug() << "Font loaded but no families exposed";
        cached = QLatin1String("Sans Serif");
        return cached;
    }
    cached = fams.first(); // e.g. "Source Sans Pro"
    return cached;
}

QImage LabelBoxNode::renderToImage(QString m_text)
{
    QFont m_font(loadSourceSansPro());
    auto m_color = Qt::white;

    m_font.setPixelSize(ScreenInterface::scaleSize2(28, _areaInfo->zoomFactor()));
    m_font.setKerning(false);

    QFontMetrics fm(m_font);
    QRect rect = fm.boundingRect(m_text);
    QSize pxSize(rect.size()); // add padding

    // Render into an image that is exactly the rect width/height in device px
    QImage img(QSize(pxSize.width(), fm.height()), QImage::Format_ARGB32_Premultiplied);
    img.setDevicePixelRatio(m_window->effectiveDevicePixelRatio());
    img.fill(Qt::transparent);

    {

        QPainter p(&img);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setRenderHint(QPainter::TextAntialiasing, true);
        p.setFont(m_font);

        p.setPen(m_color);

        p.drawText(QRect(0, 0, img.width(), fm.height()), Qt::AlignLeft | Qt::AlignVCenter, m_text);
    }

    return img;
}

QSize LabelBoxNode::renderToSize(QString m_text)
{
    QFont m_font(loadSourceSansPro());
    auto m_color = Qt::white;

    m_font.setPixelSize(ScreenInterface::scaleSize2(28, _areaInfo->zoomFactor()));
    m_font.setKerning(false);

    QFontMetrics fm(m_font);
    QRect rect = fm.boundingRect(m_text);
    QSize pxSize(rect.size()); // add padding

    return QSize(pxSize.width(), fm.height());
}

void LabelBoxNode::rebuild()
{

    if (!m_window) {
        // Inert: draw nothing but keep structure valid
        m_bg->setRect(QRectF());
        m_textNode->setRect(QRectF());
        return;
    }

    // Ensure the texture node ALWAYS has a valid texture bound
    ensureDummyTextureBound(); // binds 1x1 transparent if needed

    if (!m_dirty)
        return;
    m_dirty = false;

    if (!m_visible || m_label.isEmpty()) {
        // Hidden: zero rects, keep (dummy) texture bound
        m_bg->setRect(QRectF());
        m_textNode->setRect(QRectF());
        return;
    }

    QImage img = renderToImage(m_label);

    // ---- 2) Build/Update the texture node for text ----
    // recycle texture if the same size, otherwise recreate
    if (!m_texture || m_texture->textureSize() != img.size()) {
        // Create a fresh texture from the window (valid inside updatePaintNode)
        QSGTexture* tex = m_window->createTextureFromImage(img);
        tex->setFiltering(QSGTexture::Linear);

        m_texture.reset(tex); // keep it alive on the node
        m_textNode->setTexture(m_texture.data());
        m_textNode->setOwnsTexture(false); // QSharedPointer manages lifetime
    } else {
        // Recreate to update pixels (simplest portable way)
        QSGTexture* tex = m_window->createTextureFromImage(img);
        tex->setFiltering(QSGTexture::Linear);
        m_texture.reset(tex);
        m_textNode->setTexture(m_texture.data());
        m_textNode->setOwnsTexture(false);
    }

    // ---- 3) Compute geometry rects (background + text) ----
    // Background rect: pad around the text
    const QRectF textLogicalRect(0, 0, img.width(), img.height());
    const QRectF bgRect = textLogicalRect.marginsAdded(QMarginsF(m_padding, m_padding, m_padding, m_padding));

    // Position them at m_pos (your x,y)
    const QRectF bgAtPos = bgRect.translated(m_pos);
    const QRectF textAtPos = textLogicalRect.translated(m_pos);

    // ---- 4) Apply to child nodes ----
    // Background
    m_bg->setRect(bgAtPos);
    m_bg->setColor(m_bgColor);

    // Text texture node
    m_textNode->setRect(textAtPos);
    // Note: texture's source rect is the full image by default

    // Slight Z order: make sure text is above background
    m_bg->setFlag(QSGNode::OwnedByParent);
    m_textNode->setFlag(QSGNode::OwnedByParent);

    markDirty(QSGNode::DirtyGeometry);
}
static inline void writeColored(QSGGeometry::ColoredPoint2D& v,
    float x, float y,
    const QColor& c)
{
    v.x = x;
    v.y = y;
    v.r = uchar(c.red());
    v.g = uchar(c.green());
    v.b = uchar(c.blue());
    v.a = uchar(c.alpha());
}

void CircleMarkersNode::rebuild()
{

    if (!m_show) {

        m_geom->allocate(0, 0);
        markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
        return;
    }

    const qreal EPS = 1e-3;
    QVector<CircleMarker> markers;
    auto addJoint = [&](const QPointF& p, const QColor& c, bool fill) {
        for (auto& jt : markers) {
            if (samePt(jt.p, p, EPS)) {
                jt.color = c; // last one wins; or comment this line for "first wins"
                return;
            }
        }
        markers.push_back({ p, c, fill });
    };

    for (const auto& s : m_points) {

        addJoint(s.line, s.fill ? Qt::white : s.color, s.fill);
    }

    auto segs = 64;
    bool pixelSnap = false;

    if (segs < 8)
        segs = 8;

    // PASS 1: count verts/indices exactly
    int totalVerts = 0;
    int totalIdxs = 0;

    for (const auto& m : markers) {

        if (m.fill) {
            // filled disk: 1 center + (segs+1) rim verts; segs * 3 indices
            totalVerts += (segs + 2);
            totalIdxs += (segs * 3);
        } else {
            // ring: two rims (outer + inner), (segs+1)*2 verts; segs*2 triangles -> segs*6 indices
            totalVerts += 2 * (segs + 1);
            totalIdxs += segs * 6;
        }
    }

    m_geom->allocate(totalVerts, totalIdxs);

    auto* v = reinterpret_cast<QSGGeometry::ColoredPoint2D*>(m_geom->vertexData());
    auto* idx = reinterpret_cast<quint16*>(m_geom->indexData());

    int vOfs = 0;
    int iOfs = 0;
    float r = 4.0;
    float stroke = 1.f;
    for (const auto& m : markers) {

        const QColor col = m.color;

        // (optional) pixel snap in item coordinates
        const float cx = pixelSnap ? float(std::round(m.p.x()) + 0.5) : float(m.p.x());
        const float cy = pixelSnap ? float(std::round(m.p.y()) + 0.5) : float(m.p.y());

        if (m.fill) {
            // Filled fan
            writeColored(v[vOfs + 0], cx, cy, col);

            for (int i = 0; i <= segs; ++i) {
                const float t = float(i) / segs * float(M_PI) * 2.f;
                const float ct = std::cos(t), st = std::sin(t);
                writeColored(v[vOfs + 1 + i], cx + r * ct, cy + r * st, col);
            }
            for (int i = 0; i < segs; ++i) {
                idx[iOfs + 3 * i + 0] = quint16(vOfs + 0);
                idx[iOfs + 3 * i + 1] = quint16(vOfs + 1 + i);
                idx[iOfs + 3 * i + 2] = quint16(vOfs + 2 + i);
            }
            vOfs += (segs + 2);
            iOfs += (segs * 3);
        } else {
            // Ring as triangle strip (two fans stitched into quads)
            const float ro = 4.0;
            const float ri = qMax(0.0f, ro - stroke);

            // vertices: outer[0..segs], inner[0..segs]
            for (int i = 0; i <= segs; ++i) {
                const float t = float(i) / segs * float(M_PI) * 2.f;
                const float ct = std::cos(t), st = std::sin(t);

                writeColored(v[vOfs + i], cx + ro * ct, cy + ro * st, col);
                writeColored(v[vOfs + (segs + 1) + i], cx + ri * ct, cy + ri * st, col);
            }
            // indices: per segment, two triangles forming a quad
            for (int i = 0; i < segs; ++i) {
                const quint16 o0 = quint16(vOfs + i);
                const quint16 o1 = quint16(vOfs + i + 1);
                const quint16 i0 = quint16(vOfs + (segs + 1) + i);
                const quint16 i1 = quint16(vOfs + (segs + 1) + i + 1);

                // tri 1: o0, i0, o1
                idx[iOfs + 6 * i + 0] = o0;
                idx[iOfs + 6 * i + 1] = i0;
                idx[iOfs + 6 * i + 2] = o1;
                // tri 2: o1, i0, i1
                idx[iOfs + 6 * i + 3] = o1;
                idx[iOfs + 6 * i + 4] = i0;
                idx[iOfs + 6 * i + 5] = i1;
            }
            vOfs += 2 * (segs + 1);
            iOfs += segs * 6;
        }
    }

    // done
    markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
}

bool CircleMarkersNode::show() const
{
    return m_show;
}

void CircleMarkersNode::setShow(bool newShow)
{
    m_show = newShow;
}
