#include "EngineHelper.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <cstdio>

namespace {
static void kfLog(const QString& msg)
{
    FILE* f = std::fopen("C:\\temp\\kf\\kf_debug.log", "a");
    if (f) {
        const QString line = QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "  " + msg + "\n";
        const QByteArray ba = line.toUtf8();
        std::fwrite(ba.constData(), 1, ba.size(), f);
        std::fflush(f);
        std::fclose(f);
    }
}
}

#include <Core/BaseTypes/DMetaHelper.h>
#include <Core/BaseTypes/DPath.h>
#include <Core/IO/LogSystem/Log.h>
#include <Core/IO/LogSystem/LogMacrosDef.h>
#include <Core/Math/Vector3D.h>
#include <Core/System/DEvent.h>
#include <Core/System/Functor.h>
#include <Engine/Managers/Environment3DManager.h>
#include <EngineCore/Engine.h>
#include <GFX/DrawablesManager.h>
#include <GFX/GCore/Element.h>
#include <GFX/GCore/ElementDrawable.h>
#include <GFX/GCore/RenderPipelineDef.h>
#include <GFX/GeometryModel.h>
#include <GFX/LodModel.h>
#include <GFX/Primitives/Arrow.h>
#include <GFX/Primitives/Capsule.h>
#include <GFX/Primitives/Cube.h>
#include <GFX/Primitives/Cylinder.h>
#include <GFX/Primitives/Line.h>
#include <GFX/Primitives/Sphere.h>
#include <GFX/Primitives/ThreeAxes.h>
#include <GFX/Primitives/Triangle.h>
#include <GFX/Resource/ShaderResource.h>
#include <Scene/Components/ModelComponent.h>
#include <Scene/Components/MouseController.h>
#include <Scene/Components/ShaderComponent.h>
#include <Scene/Components/SolidRenderer.h>
#include <Scene/SceneHelper.h>
#include <type_traits>
using namespace DGE;
using namespace DGE::Scene;
using namespace DGE::GFX;
using namespace DGE::Core;
using namespace Core::Math;

ObjectCreator::ObjectCreator(AnimationManager* animationManager, DGE::QtWrapper::QEngineRef engineRef, QObject* parent)
    : QObject { parent }
    , m_engineRef(engineRef)
    , m_animationManager(animationManager)
{
    connect(m_animationManager, &AnimationManager::currentTimeChanged, this, &ObjectCreator::currentTimeChanged);
    kfLog("=== ObjectCreator constructed ===");
}

void ObjectCreator::canPlayChanged()
{
    qDebug() << "canPlayChanged" << m_animationManager->canPlay();
}
void ObjectCreator::onEntityMoved(Scene::DEntityWRef entity, Vector3D movement)
{
    auto selectedEntity = GFX::Viewport::currentSelectedEntity();
    if (!selectedEntity)
        return;

    m_entityMoved = true;
    //    if (m_entityMovingKey >= 0 && m_entityMoved == false) {
    //        m_entityMoved = true;

    //        auto pos = m_objects[m_entityMovingKey]->localPosition();
    //        m_objectX = pos[0];
    //        m_objectY = pos[1];
    //        m_objectZ = pos[2];
    //    }

    auto pos = selectedEntity->localPosition();

    if (entity.lock()->name().contains("ArrowX")) {
        // Q_EMIT sigObjectMovedX(foundKey, pos[0]);
        selectedEntity->move({ movement.x(), 0, 0 });
    } else if (entity.lock()->name().contains("ArrowY")) {
        // Q_EMIT sigObjectMovedY(foundKey, pos[1]);
        selectedEntity->move({ 0, movement.y(), 0 });
    } else if (entity.lock()->name().contains("ArrowZ")) {
        // Q_EMIT sigObjectMovedZ(foundKey, pos[2]);
        selectedEntity->move({ 0, 0, movement.z() });
    }
}

void ObjectCreator::createHandlers()
{
    if (1) {
        const bool isVisibleBoundingBox = false;
        const float axisScale = 1.2f;
        const float distancePivot = axisScale / 2.0f;

        auto moveArrowNode = m_scene->createChild();
        moveArrowNode->setName("MovingHandlerGizmo");
        moveArrowNode->setHasRenderState(true);
        moveArrowNode->setAttachParentLocationOnly(true);
        moveArrowNode->initialized() += D_PFUNC(
            [moveArrowNode](Core::Utility::Interfaces::InitializeParams) {
                auto renderState = moveArrowNode->renderState();
                if (renderState) {
                    renderState->setDepthWrite(true);
                    renderState->setDepthTest(true, GCore::CompareMode::Always);
                    renderState->setBlendMode(true, GCore::BlendMode::Replace, false);
                    renderState->setDepthValueType(GCore::DepthValueType::PinToFront);
                }
                return true;
            });
        auto mouseComponent = moveArrowNode->createComponent<MouseController>();
        mouseComponent->setUseChildrenOnly(true);
        DConnect(mouseComponent, entityMoved, this, &ObjectCreator::onEntityMoved);

        Environment3DManager::setMovingArrows(moveArrowNode);
        tupleVars(arrowX) = Scene::SceneHelper::createEntityNode<
            GFX::Primitives::Arrow, GFX::GeometryModel, false, false, true,
            isVisibleBoundingBox>(moveArrowNode, m_shaderResource, 1, 0.1f, true);
        arrowXEntity->setPivot({ distancePivot, 0, 0 });
        arrowXEntity->setName("ArrowX");
        arrowXEntity->setCurrentColor(Color(250, 125, 125, 255));
        arrowXEntity->setSelectionColor(Color(200, 50, 50, 255));
        arrowXEntity->setBoundingBoxScale(Vector3D(axisScale));
        arrowXEntity->setIsRandomColoring(false);
        arrowXEntity->setHasBoundaries(true);

        tupleVars(arrowY) = Scene::SceneHelper::createEntityNode<
            GFX::Primitives::Arrow, GFX::GeometryModel, false, false, true,
            isVisibleBoundingBox>(moveArrowNode, m_shaderResource, 1, 0.1f, true, 1);
        arrowYEntity->setPivot({ 0, distancePivot, 0 });
        arrowYEntity->setCurrentColor(Color(125, 225, 125, 255));
        arrowYEntity->setSelectionColor(Color(50, 200, 50, 255));
        arrowYEntity->setBoundingBoxScale(Vector3D(axisScale));
        arrowYEntity->setIsRandomColoring(false);
        arrowYEntity->setName("ArrowY");
        arrowYEntity->setHasBoundaries(true);

        tupleVars(arrowZ) = Scene::SceneHelper::createEntityNode<
            GFX::Primitives::Arrow, GFX::GeometryModel, false, false, true,
            isVisibleBoundingBox>(moveArrowNode, m_shaderResource, 1, 0.1f, true, 2);
        arrowZEntity->setPivot({ 0, 0, distancePivot });
        arrowZEntity->setCurrentColor(Color(125, 125, 250, 255));
        arrowZEntity->setSelectionColor(Color(50, 50, 200, 255));
        arrowZEntity->setBoundingBoxScale(Vector3D(axisScale));
        arrowZEntity->setIsRandomColoring(false);
        arrowZEntity->setName("ArrowZ");
        arrowZEntity->setHasBoundaries(true);
        moveArrowNode->setEnable(false);

        auto miniViewNode = m_scene->createChild(3);
        miniViewNode->assignTag("Viewport#1#DefaultView#ViewTag");
        miniViewNode->assignTag("Viewport#3#DefaultView#ViewTag");
        miniViewNode->assignTag("Viewport#5#DefaultView#ViewTag");
        tupleVars(triangleMiniView) = Scene::SceneHelper::createEntityNode<
            GFX::Primitives::ThreeAxes, GFX::GeometryModel, false>(miniViewNode, m_shaderResource,
            1, 0.25f, true);
    }
}

void ObjectCreator::createObject(quint64 trackIndex, bool enabled)
{

    if (!m_scene) {
        return;
    }

    if (m_objects.contains(trackIndex)) {
        m_objects[trackIndex]->setEnable(enabled);
        return;
    } else {
        if (enabled == false) {
            return;
        }
    }

    auto sceneNode = m_scene->createChild();

    static const DString cubeName = "CubeEntity";
    static auto updateFunction = D_PFUNC(this, &ObjectCreator::updateSize);
    auto defaultShaderResource = m_assetsManager.lock()->loadAsset<ShaderResource>(
        DGE::Managers::AssetsManager::defaultAssetsPath()
        + "Shaders/GL/default-solid.shader");

    auto rotatePtrFunc = D_PFUNC(this, &ObjectCreator::rotateFunction);

    tupleVars(object) = Scene::SceneHelper::createEntityNode<
        GFX::Primitives::Sphere, GFX::GeometryModel, true, false,
        true>(sceneNode, defaultShaderResource, 0.2);

    DGE::GFX::Viewport::setSelectedEntity(objectEntity);
    m_animationManager->addTrack();
    m_animationManager->updateCanPlay();

    objectEntity->setLocalPosition(0, 0, 0);

    m_objects.insert(trackIndex, objectEntity.get());

    auto trackList = m_animationManager->trackItemsList();

    for (int i = 0; i < trackList.size(); i++) {
        if (trackList[i]->isSelected()) {
            _tracklist.insert(trackIndex, trackList[i]);

            connect(trackList[i], &AnimationTrackItem::keyframeItemsListChanged, this, &ObjectCreator::keyframeItemsListChanged);
            connect(trackList[i], &AnimationTrackItem::animationChanged, this, &ObjectCreator::animationChanged);
            break;
        }
    }

    DConnect(objectEntity, positionChanged, [this, objectEntity](glm::vec3 value) {
        int foundKey = m_objects.values().indexOf(objectEntity.get());

        if (foundKey >= 0) {
            auto pos = objectEntity->localPosition();
            Q_EMIT sigObjectMoved(foundKey, value[0], value[1], value[2]);
        }
    });

    DConnect(objectEntity,
        updated,
        [this](GCore::InputParameters param) {
            auto elapsed = param.m_elapsedTime.count();
            if (param.m_thisNode) {

                auto selfNode = std::dynamic_pointer_cast<Node>(param.m_thisNode);

                if (selfNode) {

                    float rotate = selfNode->localRotation().y;
                    rotate = remainder(rotate + elapsed * randomNum(0.0, 0.1), 360.0f);
                    selfNode->setLocalRotation(glm::vec3(0.0, rotate, 0.0));

                } else {
                }
            }

            return true;
        });

    //    objectEntity->setLocalPosition(0, 0, 0);
}

bool ObjectCreator::rotateFunction(GFX::GCore::InputParameters param)
{
    using namespace DGE::Scene;
    using namespace DGE::GFX;
    using namespace DGE::Core;
    try {
        auto paramList = any_cast<InitializeParams>(param.m_initializeParameters);
        auto elapsed = param.m_elapsedTime.count();

        if (param.m_thisNode) {
            try {
                auto selfNode = std::dynamic_pointer_cast<DEntity>(param.m_thisNode);
                static float rotate = 0;
                rotate = remainder(rotate + elapsed * 0.1f, 360.0f);
                selfNode->setLocalRotation(glm::vec3(0.0, rotate, 0.0));
            } catch (...) {
                LogError(DST("Unexpected error occurred in casting std::any type: \n "
                             "Real Type is: ")
                    + param.m_thisNode->typeInfo().toString());
            }
        }
    } catch (...) {
        LogError("Unexpected error occurred.");
    }

    return true;
}

bool ObjectCreator::updateSize(GFX::GCore::InputParameters param)
{
    using namespace DGE::Scene;
    using namespace DGE::GFX;
    using namespace DGE::Core;
    try {
        auto paramList = any_cast<InitializeParams>(param.m_initializeParameters);
        auto elapsed = param.m_elapsedTime.count();
        if (paramList.contains(DrawablesManager::s_currentFrameNumber)) {
            auto currentFrameNumber = param.m_frameNumber;
            //			if (currentFrameNumber > 100)
            //				qApp->exit();
        }
        if (param.m_currentMesh) {
            try {
                auto cube = std::dynamic_pointer_cast<GFX::Primitives::Cube>(
                    any_cast<GFX::GCore::Interfaces::IGObjectDrawableRef>(param.m_currentMesh));
                static int defaultStep = 20;
                static int increasedSize = -defaultStep;
                if (increasedSize > 0) {
                    increasedSize -= elapsed * 0.001f;
                    if (increasedSize > 0)
                        cube->setWidth(cube->width() + elapsed * 0.001f);
                    else
                        increasedSize = -defaultStep;
                } else {
                    increasedSize += elapsed * 0.001f;
                    if (increasedSize < 0)
                        cube->setWidth(cube->width() - elapsed * 0.001f);
                    else
                        increasedSize = defaultStep;
                }
            } catch (...) {
                LogError(DST("Unexpected error occurred in casting std::any type: \n "
                             "Real Type is: ")
                    + param.m_currentMesh->typeInfo().toString());
            }
        }
    } catch (...) {
        LogError("Unexpected error occurred.");
    }

    return true;
}

void ObjectCreator::onStartedEngine()
{
    qDebug() << "onStartedEngine";
    m_context = System::Context::getInstance();
    m_assetsManager = m_context->getRegisteredSubsystem<DGE::Managers::AssetsManager>();

    auto inputManager
        = m_context->getRegisteredSubsystem<DGE::Managers::InputEventsManager>().lock();
    if (inputManager) {

        DConnect(inputManager, mouseReleased, this, &ObjectCreator::onMouseReleased);
        DConnect(inputManager, mousePressed, this, &ObjectCreator::onMousePressed);
    }

    if (!m_assetsManager.expired()) {
        {
            m_scene = SceneNode::getInstance();
            m_scene->setName("MainScene");

            auto sceneNode = m_scene->createChild(1);

            static const DString cubeName = "CubeEntity";
            static auto updateFunction = D_PFUNC(this, &ObjectCreator::updateSize);
            auto defaultShaderResource = m_assetsManager.lock()->loadAsset<ShaderResource>(
                DGE::Managers::AssetsManager::defaultAssetsPath()
                + "Shaders/GL/default-solid.shader");

            if (1) {
                const bool isVisibleBoundingBox = false;
                const float axisScale = 1.2f;
                const float distancePivot = axisScale / 2.0f;

                auto moveArrowNode = sceneNode->createChild();
                moveArrowNode->setName("MovingHandlerGizmo");
                moveArrowNode->setHasRenderState(true);
                moveArrowNode->setAttachParentLocationOnly(true);
                moveArrowNode->initialized() += D_PFUNC(
                    [moveArrowNode](Core::Utility::Interfaces::InitializeParams) {
                        auto renderState = moveArrowNode->renderState();
                        if (renderState) {
                            renderState->setDepthWrite(true);
                            renderState->setDepthTest(true, GCore::CompareMode::Always);
                            renderState->setBlendMode(true, GCore::BlendMode::Replace, false);
                            renderState->setDepthValueType(GCore::DepthValueType::PinToFront);
                        }
                        return true;
                    });
                auto mouseComponent = moveArrowNode->createComponent<MouseController>();
                mouseComponent->setUseChildrenOnly(true);
                DConnect(mouseComponent, entityMoved, this, &ObjectCreator::onEntityMoved);
                Environment3DManager::setMovingArrows(moveArrowNode);
                tupleVars(arrowX) = Scene::SceneHelper::createEntityNode<
                    GFX::Primitives::Arrow, GFX::GeometryModel, false, false, true,
                    isVisibleBoundingBox>(moveArrowNode, defaultShaderResource, 1, 0.1f, true);
                arrowXEntity->setPivot({ distancePivot, 0, 0 });
                arrowXEntity->setName("ArrowX");
                arrowXEntity->setCurrentColor(Color(250, 125, 125, 255));
                arrowXEntity->setSelectionColor(Color(200, 50, 50, 255));
                arrowXEntity->setBoundingBoxScale(Vector3D(axisScale));
                // arrowXEntity->setLocalScale(0.1, 0.1, 0.1);
                arrowXEntity->setIsRandomColoring(false);
                arrowXEntity->setHasBoundaries(true);
                arrowXEntity->setIsHoverable(true);
                auto solidRendererX = arrowXEntity->getComponent<Scene::SolidRenderer>();
                if (solidRendererX) {
                    // solidRendererX->setIsFixedSize(true);
                }
                // arrow1->setPosition(-5, 3, -5);
                tupleVars(arrowY) = Scene::SceneHelper::createEntityNode<
                    GFX::Primitives::Arrow, GFX::GeometryModel, false, false, true,
                    isVisibleBoundingBox>(moveArrowNode, defaultShaderResource, 1, 0.1f, true, 1);
                arrowYEntity->setPivot({ 0, distancePivot, 0 });
                arrowYEntity->setCurrentColor(Color(125, 225, 125, 255));
                arrowYEntity->setSelectionColor(Color(50, 200, 50, 255));
                arrowYEntity->setBoundingBoxScale(Vector3D(axisScale));
                // arrowYEntity->setLocalScale(0.1, 0.1, 0.1);
                arrowYEntity->setIsRandomColoring(false);
                arrowYEntity->setName("ArrowY");
                arrowYEntity->setHasBoundaries(true);
                arrowYEntity->setIsHoverable(true);
                auto solidRendererY = arrowYEntity->getComponent<Scene::SolidRenderer>();
                if (solidRendererY) {
                    // solidRendererY->setIsFixedSize(true);
                }
                // arrow2->setPosition(-5, 3, -5);
                tupleVars(arrowZ) = Scene::SceneHelper::createEntityNode<
                    GFX::Primitives::Arrow, GFX::GeometryModel, false, false, true,
                    isVisibleBoundingBox>(moveArrowNode, defaultShaderResource, 1, 0.1f, true, 2);
                arrowZEntity->setPivot({ 0, 0, distancePivot });
                arrowZEntity->setCurrentColor(Color(125, 125, 250, 255));
                arrowZEntity->setSelectionColor(Color(50, 50, 200, 255));
                arrowZEntity->setBoundingBoxScale(Vector3D(axisScale));
                // arrowZEntity->setLocalScale(0.1, 0.1, 0.1);
                arrowZEntity->setIsRandomColoring(false);
                arrowZEntity->setName("ArrowZ");
                arrowZEntity->setHasBoundaries(true);
                arrowZEntity->setIsHoverable(true);
                auto solidRendererZ = arrowZEntity->getComponent<Scene::SolidRenderer>();
                if (solidRendererZ) {
                    // solidRendererZ->setIsFixedSize(true);
                }
                moveArrowNode->setEnable(false);

                auto miniViewNode = m_scene->createChild(3);
                miniViewNode->assignTag("Viewport#1#DefaultView#ViewTag");
                miniViewNode->assignTag("Viewport#3#DefaultView#ViewTag");
                miniViewNode->assignTag("Viewport#5#DefaultView#ViewTag");
                tupleVars(triangleMiniView) = Scene::SceneHelper::createEntityNode<
                    GFX::Primitives::ThreeAxes, GFX::GeometryModel, false>(miniViewNode,
                    defaultShaderResource, 1,
                    0.25f, true);
            }
        }
    }
    Q_EMIT sigEngineStarted();
}

void ObjectCreator::onAddingObject(int trackIndex, bool enabled)
{

    QMetaObject::invokeMethod(this, "createObject", Q_ARG(quint64, trackIndex), Q_ARG(bool, enabled));
}

void ObjectCreator::editKeyFrameWithPosition(int trackindex, QString status)
{
    QJsonParseError parseError;
    QByteArray byteArray = status.toUtf8();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);

    if (parseError.error == QJsonParseError::NoError && doc.isArray()) {

        _is_undoing = true;
        QJsonArray objs = doc.array();
        //        for (int i = 0; i < objs.size(); i++) {
        //            QJsonObject obj = objs[i].toObject();
        //            _undo_redo.append(quint64(obj["time"].toDouble()));
        //            qDebug() << "editKeyFrameWithPosition" << objs << quint64(obj["time"].toDouble());
        //        }
        _tracklist[trackindex]->removeAllKeyFrames();

        for (int i = 0; i < objs.size(); i++) {

            QJsonObject obj = objs[i].toObject();

            if (obj.contains("bx")) {
                _tracklist[trackindex]->addKeyFrame(quint64(obj["time"].toDouble()),
                    DGE::Core::Math::Vector3D { float(obj["x"].toDouble()), float(obj["y"].toDouble()), float(obj["z"].toDouble()) },
                    (DGE::Core::Animation::TransitionType)obj["type"].toInt(),
                    DGE::Core::Math::Vector3D { float(obj["bx"].toDouble()), float(obj["by"].toDouble()), float(obj["bz"].toDouble()) },
                    DGE::Core::Math::Vector3D { float(obj["ex"].toDouble()), float(obj["ey"].toDouble()), float(obj["ez"].toDouble()) });
            } else {
                _tracklist[trackindex]->addKeyFrame(quint64(obj["time"].toDouble()), DGE::Core::Math::Vector3D { float(obj["x"].toDouble()), float(obj["y"].toDouble()), float(obj["z"].toDouble()) },
                    (DGE::Core::Animation::TransitionType)obj["type"].toInt());
            }
            if (m_objects.contains(trackindex)) {
                m_objects[trackindex]->setLocalPosition(obj["x"].toDouble(), obj["y"].toDouble(), obj["z"].toDouble());
                m_objects[trackindex]->requestToUpdate();
            }
        }
        _is_undoing = false;
    }
}

void ObjectCreator::addKeyFrame(int trackindex, quint64 time, int type)
{
    if (_tracklist.contains(trackindex)) {
        if (_tracklist[trackindex]->containsKeyFrame(time)) {
            return;
        }
        _tracklist[trackindex]->addKeyFrame(time, (KeyFrameItem::TransitionType)type);
        setCurrentTime(time);
        m_animationManager->updateCanPlay();
    }

    // Capture the current 3D position into our ObjectPosAutomation.
    // Prefer the live entity localPosition so each keyframe captures wherever
    // the object actually is right now, not whatever was last cached.
    QVector3D pos;
    if (m_objects.contains(trackindex) && m_objects[trackindex]) {
        auto p = m_objects[trackindex]->localPosition();
        pos = QVector3D(float(p[0]), float(p[1]), float(p[2]));
    } else {
        pos = m_lastPositions.value(trackindex, QVector3D());
    }
    m_lastPositions[trackindex] = pos;

    ensureAutomation(trackindex);
    const KeyInterp interp = (type == 2) ? KeyInterp::Hold
                           : (type == 0) ? KeyInterp::Bezier
                           : KeyInterp::Linear;
    m_objectAutomations[trackindex]->addKey(qint64(time), pos, interp);
}

void ObjectCreator::editKeyFrame(int trackindex, quint64 lastTime, quint64 newTime, int type)
{

    removeKeyFrame(trackindex, lastTime);
    addKeyFrame(trackindex, newTime, type);
}

void ObjectCreator::removeKeyFrame(int trackindex, quint64 time)
{
    if (removing_list.contains(time)) {
        return;
    }
    setCurrentTime(time);
    if (_tracklist.contains(trackindex)) {
        double last = -1;
        if (_lastKeyFrames[trackindex].size() > 0)
            last = _lastKeyFrames[trackindex].lastKey();
        _tracklist[trackindex]->removeKeyFrame(time);
        if (last >= 0)
            setCurrentTime(last);
    }

    // Mirror removal into ObjectPosAutomation
    if (m_objectAutomations.contains(trackindex))
        m_objectAutomations[trackindex]->removeKey(qint64(time));
}

void ObjectCreator::keyFrameChanged(int trackIndex, QMap<quint64, int> list)
{
    if (_tracklist.contains(trackIndex)) {
        QMapIterator<quint64, int> i(list);
        while (i.hasNext()) {
            i.next();

            if (_tracklist[trackIndex]->containsKeyFrame(i.value())) {
                continue;
            } else {
                addKeyFrame(trackIndex, i.key(), i.value());
            }
        }

        auto lastKeyFrames = _lastKeyFrames[trackIndex];
        QMapIterator<quint64, QJsonObject> j(lastKeyFrames);
        while (j.hasNext()) {
            j.next();

            if (list.contains(j.key())) {
                continue;
            } else {
                removeKeyFrame(trackIndex, j.key());
            }
        }
    }
}

void ObjectCreator::setKeyFrames(int trackIndex, QMap<qint64, int> keyFrames)
{

    if (_tracklist.contains(trackIndex)) {

        auto keyframeItemsList = _tracklist[trackIndex]->keyframeItemsList();
        for (int i = 0; i < keyframeItemsList.size(); i++) {

            _tracklist[trackIndex]->removeKeyFrame(keyframeItemsList[i]->time());
        }

        QMapIterator<qint64, int> i(keyFrames);
        while (i.hasNext()) {
            i.next();

            QJsonObject obj;

            obj["time"] = double((quint64)i.key());
            obj["type"] = i.value();

            _lastKeyFrames[trackIndex].insert(i.key(), obj);
            addKeyFrame(trackIndex, i.key(), i.value());
        }
    }
}

void ObjectCreator::setCurrentTime(qint64 time)
{
    m_currentTime = time;
    m_animationManager->setCurrentTime(time);

    // Evaluate all per-track ObjectPosAutomations and push positions to scene3D
    for (auto it = m_objectAutomations.constBegin(); it != m_objectAutomations.constEnd(); ++it) {
        ObjectPosAutomation* oa = it.value();
        if (oa && !oa->isEmpty()) {
            QVector3D p = oa->evaluate(time);
            kfLog(QString("eval track=%1 t=%2 keys=%3 pos=(%4,%5,%6)")
                .arg(it.key()).arg(time).arg(oa->count())
                .arg(p.x()).arg(p.y()).arg(p.z()));
            emit sigObjectMoved(it.key(), double(p.x()), double(p.y()), double(p.z()));
        }
    }
}

void ObjectCreator::syncObjectKeyframes(int trackIndex, QList<qint64> times)
{
    // Read current entity position; new keys capture this so motion is non-zero
    QVector3D pos;
    bool fromEntity = false;
    if (m_objects.contains(trackIndex) && m_objects[trackIndex]) {
        auto p = m_objects[trackIndex]->localPosition();
        pos = QVector3D(float(p[0]), float(p[1]), float(p[2]));
        fromEntity = true;
    } else {
        pos = m_lastPositions.value(trackIndex, QVector3D());
    }
    m_lastPositions[trackIndex] = pos;
    {
        QStringList ts;
        for (qint64 t : times) ts << QString::number(t);
        kfLog(QString("syncObjectKeyframes track=%1 times=[%2] live-pos=(%3,%4,%5) fromEntity=%6")
            .arg(trackIndex).arg(ts.join(','))
            .arg(pos.x()).arg(pos.y()).arg(pos.z())
            .arg(fromEntity ? "yes" : "no"));
    }

    ObjectPosAutomation* oa = ensureAutomation(trackIndex);

    // Detect kf "moves" on the timeline lane (lane is fixedVertical, so only
    // the time changed). The lane can emit multiple simultaneous moves in one
    // sigLinesChanged when snap-quantization shifts neighbors, so we generalize:
    // when |removed| == |added|, pair each removed time with its nearest added
    // time (greedy by absolute delta) and transfer pos/interp/tangents to the
    // new time. This preserves each kf's 3D position — only the timing changes.
    {
        const QList<qint64> existing = oa->keyTimes();
        QSet<qint64> existingSet;
        for (qint64 t : existing) existingSet.insert(t);
        QSet<qint64> wantS;
        for (qint64 t : times) wantS.insert(t);
        QList<qint64> removed = (existingSet - wantS).values();
        QList<qint64> added   = (wantS - existingSet).values();
        if (!removed.isEmpty() && removed.size() == added.size()) {
            // Greedy nearest-pair matching: for each removed time, pair with the
            // closest unpaired added time.
            QVector<bool> usedAdded(added.size(), false);
            QList<QPair<qint64, qint64>> pairs;
            pairs.reserve(removed.size());
            for (qint64 oldT : removed) {
                int bestIdx = -1;
                qint64 bestDelta = std::numeric_limits<qint64>::max();
                for (int i = 0; i < added.size(); ++i) {
                    if (usedAdded[i]) continue;
                    const qint64 d = std::llabs(added[i] - oldT);
                    if (d < bestDelta) { bestDelta = d; bestIdx = i; }
                }
                if (bestIdx < 0) break;
                usedAdded[bestIdx] = true;
                pairs.append({oldT, added[bestIdx]});
            }
            if (pairs.size() == removed.size()) {
                // Identity-preserving move: each kf has a stable id stored in
                // ObjectKeyFrame::id. Look up the id at each old time BEFORE
                // mutating, then call moveTimeById() — the kf object is the
                // SAME object at the new time (pos/interp/tangents/id intact).
                QList<QString> ids;
                ids.reserve(pairs.size());
                bool allFound = true;
                for (const auto& p : pairs) {
                    const ObjectKeyFrame* k = oa->key(p.first);
                    if (!k || k->id.isEmpty()) { allFound = false; break; }
                    ids.append(k->id);
                }
                if (allFound) {
                    for (int i = 0; i < pairs.size(); ++i) {
                        const qint64 oldT = pairs[i].first;
                        const qint64 newT = pairs[i].second;
                        const ObjectKeyFrame* k = oa->keyById(ids[i]);
                        kfLog(QString("  move-by-id id=%1 t=%2 -> %3 pos=(%4,%5,%6) interp=%7")
                            .arg(ids[i]).arg(oldT).arg(newT)
                            .arg(k ? k->pos.x() : 0.0).arg(k ? k->pos.y() : 0.0).arg(k ? k->pos.z() : 0.0)
                            .arg(k ? int(k->interp) : -1));
                        oa->moveTimeById(ids[i], newT);
                    }
                    setCurrentTime(m_currentTime);
                    return;
                }
            }
        }
    }

    // Add any new times with the live position
    QSet<qint64> wantSet;
    for (qint64 t : times) {
        wantSet.insert(t);
        if (!oa->hasKey(t)) {
            kfLog(QString("  add key t=%1 pos=(%2,%3,%4)")
                .arg(t).arg(pos.x()).arg(pos.y()).arg(pos.z()));
            oa->addKey(t, pos, KeyInterp::Linear);
        } else {
            QVector3D ep = oa->key(t)->pos;
            kfLog(QString("  keep key t=%1 existing-pos=(%2,%3,%4)")
                .arg(t).arg(ep.x()).arg(ep.y()).arg(ep.z()));
        }
    }
    // Remove any times no longer present in the lane
    QList<qint64> existing = oa->keyTimes();
    for (qint64 t : existing) {
        if (!wantSet.contains(t)) {
            kfLog(QString("  remove key t=%1").arg(t));
            oa->removeKey(t);
        }
    }

    // Re-evaluate current playhead so the scene reflects the new automation
    setCurrentTime(m_currentTime);
}

void ObjectCreator::setKeyFrameInterp(int trackIndex, qint64 time, int interp)
{
    kfLog(QString("setKeyFrameInterp track=%1 time=%2 interp=%3")
        .arg(trackIndex).arg(time).arg(interp));
    auto* oa = ensureAutomation(trackIndex);
    oa->setInterp(time, KeyInterp(interp));

    // Seed Catmull-Rom tangents when switching to Bezier with zero handles.
    if (KeyInterp(interp) == KeyInterp::Bezier) {
        const ObjectKeyFrame* kf = oa->key(time);
        if (kf && kf->tangentIn.isNull() && kf->tangentOut.isNull()) {
            const QList<qint64> times = oa->keyTimes();
            const int idx = times.indexOf(time);
            const bool hasPrev = (idx > 0);
            const bool hasNext = (idx < times.size() - 1);
            const QVector3D selfPos = kf->pos;
            const QVector3D prevPos = hasPrev ? oa->key(times[idx - 1])->pos : selfPos;
            const QVector3D nextPos = hasNext ? oa->key(times[idx + 1])->pos : selfPos;
            QVector3D tOut, tIn;
            if (hasPrev && hasNext) {
                tOut = (nextPos - prevPos) * 0.25f;
                tIn  = (prevPos - nextPos) * 0.25f;
            } else if (hasNext) {
                tOut = (nextPos - selfPos) * 0.33f;
                tIn  = -tOut;
            } else if (hasPrev) {
                tIn  = (prevPos - selfPos) * 0.33f;
                tOut = -tIn;
            }
            if (!tOut.isNull() || !tIn.isNull())
                oa->setTangents(time, tIn, tOut);
        }
    }
}

ObjectPosAutomation* ObjectCreator::ensureAutomation(int trackIndex)
{
    auto it = m_objectAutomations.find(trackIndex);
    if (it != m_objectAutomations.end())
        return it.value();
    auto* oa = new ObjectPosAutomation(this);
    m_objectAutomations[trackIndex] = oa;
    emit sigAutomationCreated(trackIndex, oa);
    return oa;
}

void ObjectCreator::setKeyframeLaneActive(int trackIndex, bool active)
{
    if (active)
        m_keyframeLaneActive.insert(trackIndex);
    else
        m_keyframeLaneActive.remove(trackIndex);
}

void ObjectCreator::onMouseReleased()
{

    qDebug() << "m_entityMovingKey" << m_entityMovingKey << m_animationMoved << m_entityMoved;
    if (m_entityMovingKey >= 0 && m_entityMoved) {

        qDebug() << "trackIndex1" << m_entityMovingKey << m_objectX << m_objectY << m_objectZ;
        Q_EMIT sigSaveObjectMove(m_entityMovingKey, m_objectX, m_objectY, m_objectZ);
        m_entityMoved = false;
        m_entityMovingKey = -1;
    }
    if (m_entityMovingKey >= 0 && m_animationMoved) {

        Q_EMIT sigKeyFrameEdited(m_entityMovingKey, _beforeAnimationChanged);
        _beforeAnimationChanged = "";
        m_animationMoved = false;
        m_entityMovingKey = -1;
    }
}

void ObjectCreator::onMousePressed()
{
    auto selectedEntity = GFX::Viewport::currentSelectedEntity();
    if (!selectedEntity)
        return;

    m_entityMovingKey = -1;
    QMap<int, DGE::Scene::DEntity*>::const_iterator it = m_objects.constBegin();
    while (it != m_objects.constEnd()) {
        if (it.value() == selectedEntity.get()) {
            m_entityMovingKey = it.key();
            break;
        }
        ++it;
    }

    if (m_entityMovingKey >= 0) {
        auto pos = m_objects[m_entityMovingKey]->localPosition();
        m_objectX = pos[0];
        m_objectY = pos[1];
        m_objectZ = pos[2];
        qDebug() << "trackIndex" << m_entityMovingKey << m_objectX << m_objectY << m_objectZ;
    }

    QJsonArray objs;
    auto last = _lastKeyFrames[m_entityMovingKey];
    QMapIterator<quint64, QJsonObject> i(last);
    while (i.hasNext()) {
        i.next();
        objs.append(i.value());
    }
    QJsonDocument doc(objs);
    _beforeAnimationChanged = doc.toJson(QJsonDocument::Compact);
}

void ObjectCreator::setObjectLocation(int trackIndex, double x, double y, double z)
{
    kfLog(QString("setObjectLocation track=%1 pos=(%2,%3,%4) laneActive=%5 currentTime=%6")
        .arg(trackIndex).arg(x).arg(y).arg(z)
        .arg(m_keyframeLaneActive.contains(trackIndex) ? "yes" : "no")
        .arg(m_currentTime));
    const QVector3D pos{ float(x), float(y), float(z) };
    m_lastPositions[trackIndex] = pos;
    if (m_objects.contains(trackIndex)) {
        m_objects[trackIndex]->setLocalPosition(x, y, z);
        m_objects[trackIndex]->requestToUpdate();
    }
    // Auto-record: a 3D drag is always the user expressing "this object is at
    // position P at time T". Establish (or update) a keyframe at the current
    // playhead time on every drag. The duplicate guard prevents lane-marker spam
    // during a continuous drag at a single playhead frame.
    ObjectPosAutomation* oa = ensureAutomation(trackIndex);
    const bool alreadyAtT = oa->hasKey(m_currentTime);
    oa->addKey(m_currentTime, pos, KeyInterp::Linear);
    if (!alreadyAtT) {
        kfLog(QString("  auto-record FIRE sigAutoRecordedKeyFrame t=%1").arg(m_currentTime));
        emit sigAutoRecordedKeyFrame(trackIndex, quint64(m_currentTime), 1 /*Linear*/);
    }
}

void ObjectCreator::play()
{
    m_animationManager->play();
}

void ObjectCreator::stop()
{
    m_animationManager->stop();
}

void ObjectCreator::save()
{

    QStringList results;
    for (int i = 0; i < _tracklist.size(); i++) {

        auto p1 = _tracklist[i]->animationController()->toAnimationTemplate(false);
        Core::Serializers::JsonSerializer serializer;
        auto resultJson = serializer.serialize(p1);

        QByteArray byteArray(reinterpret_cast<const char*>(resultJson.data()), static_cast<int>(resultJson.size()));

        // Step 2: Parse JSON
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteArray, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "JSON parse error:" << parseError.errorString();
            return;
        }

        if (jsonDoc.isObject()) {
            auto _savedObjects = jsonDoc.object();
            QJsonDocument doc(_savedObjects);
            results.append(doc.toJson(QJsonDocument::Compact));
        } else if (jsonDoc.isArray()) {
            qDebug() << "Parsed JSON array";
        } else {
            qDebug() << "Parsed JSON is neither object nor array";
        }
    }
    Q_EMIT sigSaveObjectsReady(results);
}

void ObjectCreator::load(QStringList objects)
{

    for (int i = 0; i < objects.size(); i++) {

        if (objects[i].isNull() || objects[i].isEmpty() || objects[i] == "{}") {
            continue;
        }

        Core::Serializers::JsonSerializer serializer;
        QByteArray byteArray = objects[i].toLatin1();
        auto resultJson = std::vector<uint8_t>(byteArray.begin(), byteArray.end());
        auto animTemplate2 = std::make_shared<Scene::Resource::AnimationTemplate<Math::Vector3D>>("");
        serializer.deserialize(animTemplate2, resultJson);
        animTemplate2->reloadData();

        if (_tracklist[i]) {

            if (_tracklist[i]->animationController()) {

                _tracklist[i]->removeAllKeyFrames();
                auto keys = animTemplate2->keyFrames();

                for (auto key : keys) {
                    float controlPoints[6] = { 0 };
                    if (key->transformer()) {
                        auto transformersData = key->transformer()->data();
                        uint8 counter = 0;
                        for (auto controlPoint : transformersData) {
                            controlPoints[counter++] = controlPoint[0];
                            controlPoints[counter++] = controlPoint[1];
                        }
                    }

                    _tracklist[i]
                        ->addKeyFrame(key->frameTime().toMilliSecond(), key->value(), key->transitionType(),
                            DGE::Core::Math::Vector3D { controlPoints[0], controlPoints[2], controlPoints[4] },
                            DGE::Core::Math::Vector3D { controlPoints[1], controlPoints[3],
                                controlPoints[5] });
                }
            }
        }
    }
}

void ObjectCreator::keyFrameClear(int trackIndex)
{
    if (_tracklist.contains(trackIndex)) {

        auto last = _lastKeyFrames[trackIndex];

        _lastKeyFrames[trackIndex].clear();

        QMapIterator<quint64, QJsonObject> i(last);
        while (i.hasNext()) {
            i.next();
            _tracklist[trackIndex]->removeKeyFrame(i.key());
        }

        m_animationManager->updateCanPlay();
    }
}

void ObjectCreator::pause()
{
    m_animationManager->pause();
}

void ObjectCreator::animationChanged()
{
    AnimationTrackItem* item = (AnimationTrackItem*)sender();
    QMapIterator<int, AnimationTrackItem*> i(_tracklist);
    int track_index = -1;
    while (i.hasNext()) {
        i.next();
        if (i.value() == item) {
            track_index = i.key();
            break;
        }
    }

    auto keyframeItemsList = _tracklist[track_index]->keyframeItemsList();
    auto last = _lastKeyFrames[track_index];
    for (int i = 0; i < keyframeItemsList.size(); i++) {

        QJsonObject obj;

        if (keyframeItemsList[i]->beginPathController()) {
            obj["bx"] = keyframeItemsList[i]->beginPathController()->controlBegin().x();
            obj["by"] = keyframeItemsList[i]->beginPathController()->controlBegin().y();
            obj["bz"] = keyframeItemsList[i]->beginPathController()->controlBegin().z();
        }

        if (keyframeItemsList[i]->beginPathController()) {
            obj["ex"] = keyframeItemsList[i]->beginPathController()->controlEnd().x();
            obj["ey"] = keyframeItemsList[i]->beginPathController()->controlEnd().y();
            obj["ez"] = keyframeItemsList[i]->beginPathController()->controlEnd().z();
        }

        obj["x"] = keyframeItemsList[i]->value().x();
        obj["y"] = keyframeItemsList[i]->value().y();
        obj["z"] = keyframeItemsList[i]->value().z();

        obj["time"] = double(keyframeItemsList[i]->time());
        obj["type"] = keyframeItemsList[i]->transitionType();

        last.insert(keyframeItemsList[i]->time(), obj);
    }

    qDebug() << "animationChanged";
    m_animationMoved = true;
    _lastKeyFrames.insert(track_index, last);

    {
        QJsonArray objs;
        auto last = _lastKeyFrames[track_index];
        QMapIterator<quint64, QJsonObject> i(last);
        while (i.hasNext()) {
            i.next();
            objs.append(i.value());
        }
        QJsonDocument doc(objs);
        auto status = doc.toJson(QJsonDocument::Compact);

        Q_EMIT sigCurrentStatus(track_index, status);
    }
}
void ObjectCreator::keyframeItemsListChanged(QList<KeyFrameItem*> keyframeItemsList)
{

    qDebug() << "keyframeItemsListChanged";

    AnimationTrackItem* item = (AnimationTrackItem*)sender();
    QMapIterator<int, AnimationTrackItem*> i(_tracklist);
    int track_index = -1;
    while (i.hasNext()) {
        i.next();
        if (i.value() == item) {
            track_index = i.key();
            break;
        }
    }

    {
        QJsonArray objs;

        auto last = _lastKeyFrames[track_index];

        QMapIterator<quint64, QJsonObject> i(last);
        while (i.hasNext()) {
            i.next();

            objs.append(i.value());
        }

        QJsonDocument doc(objs);
        auto status = doc.toJson(QJsonDocument::Compact);

        if (_is_undoing == false) {
            qDebug() << "sigKeyFrameEdited" << track_index << objs;
            Q_EMIT sigKeyFrameEdited(track_index, status);
        }
    }

    if (track_index >= 0) {

        removing_list.clear();

        auto last = _lastKeyFrames[track_index];

        QMapIterator<quint64, QJsonObject> i(last);

        while (i.hasNext()) {
            i.next();

            if (!item->containsKeyFrame(i.key())) {
                removing_list.append(i.key());
            }
        }

        for (int i = 0; i < removing_list.size(); i++) {
            if (last.contains(removing_list[i]))
                last.remove(removing_list[i]);
        }
        for (int i = 0; i < keyframeItemsList.size(); i++) {
            if (!last.contains(keyframeItemsList[i]->time())) {

                QJsonObject obj;

                if (keyframeItemsList[i]->beginPathController()) {
                    obj["bx"] = keyframeItemsList[i]->beginPathController()->controlBegin().x();
                    obj["by"] = keyframeItemsList[i]->beginPathController()->controlBegin().y();
                    obj["bz"] = keyframeItemsList[i]->beginPathController()->controlBegin().z();
                }

                if (keyframeItemsList[i]->beginPathController()) {
                    obj["ex"] = keyframeItemsList[i]->beginPathController()->controlEnd().x();
                    obj["ey"] = keyframeItemsList[i]->beginPathController()->controlEnd().y();
                    obj["ez"] = keyframeItemsList[i]->beginPathController()->controlEnd().z();
                }

                obj["x"] = keyframeItemsList[i]->value().x();
                obj["y"] = keyframeItemsList[i]->value().y();
                obj["z"] = keyframeItemsList[i]->value().z();

                obj["time"] = double(keyframeItemsList[i]->time());
                obj["type"] = keyframeItemsList[i]->transitionType();

                last.insert(keyframeItemsList[i]->time(), obj);
            }
        }

        _lastKeyFrames.insert(track_index, last);
    }

    {
        QJsonArray objs;
        auto last = _lastKeyFrames[track_index];
        QMapIterator<quint64, QJsonObject> i(last);
        while (i.hasNext()) {
            i.next();
            objs.append(i.value());
        }
        QJsonDocument doc(objs);
        auto status = doc.toJson(QJsonDocument::Compact);

        if (_is_undoing == false) {
            qDebug() << "sigKeyFrameEdited1" << track_index << status;
        }
        Q_EMIT sigCurrentStatus(track_index, status);
    }

    // qDebug() << "m_currentChanes" << m_currentChanes;
    // Q_EMIT sigSaveObjectMove(track_index, m_currentChanes, getCurrentChanged(true));
}

void ObjectCreator::currentTimeChanged(quint64 currentTime)
{
    // Q_EMIT sigCurrentTimeChanged(currentTime);
    //     QMap<int, DGE::Scene::DEntity*>::const_iterator it = m_objects.constBegin();
    //     while (it != m_objects.constEnd()) {
    //         auto pos = it.value()->localPosition();
    //         Q_EMIT sigObjectMovedX(it.key(), pos[0]);
    //         Q_EMIT sigObjectMovedY(it.key(), pos[1]);
    //         Q_EMIT sigObjectMovedZ(it.key(), pos[2]);
    //         ++it;
    //     }
}
