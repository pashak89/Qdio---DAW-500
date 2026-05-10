/* Copyright (C) 2024, Abbas Aliakbari - All Rights Reserved
 * SPDX-License-Identifier: MIT
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: a.aliakbari91@gmail.com
 *
 * Written by Abbas Aliakbari
 *
 * @section DESCRIPTION
 *
 *
 */
#pragma once

#include <QtCore/QObject>
#include <QSet>
#include "core/objectautomation.h"

#include <Qt/QEngine.h>

#include <Core/Serializers/Serialize.h>
#include <GFX/DrawablesManager.h>
#include <GFX/GCore/GL/RendererGL.h>
#include <GFX/GCore/GL/VertexArrayGL.h>
#include <GFX/GeometryModel.h>
#include <GFX/Primitives/Cube.h>
#include <Scene/Components/AnimationComponent.h>
#include <Scene/Components/ModelComponent.h>
#include <Scene/Components/ShaderComponent.h>
#include <Scene/DEntity.h>
#include <Scene/Node.hpp>

#include <QObject>
#include <Qt/QEngine.h>

#include <EngineLib/Engine/Managers/AssetsManager.h>
#include <Scene/DEntity.h>
#include <Scene/SceneNode.h>
#include <UI/AnimationManager.h>

using namespace DGE::QtWrapper;
using namespace DGE::Scene;

struct SpatialFrame {
    float x = 0;
    float y = 0;
    float z = 0;
    float radius = 0.5f;
};

class ObjectCreator : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool autoKeyMode READ autoKeyMode WRITE setAutoKeyMode NOTIFY autoKeyModeChanged)

public:
    explicit ObjectCreator(AnimationManager* animationManager = Q_NULLPTR, QEngineRef engineRef = Q_NULLPTR, QObject* parent = nullptr);
    Q_INVOKABLE void createObject(quint64 trackIndex, bool enabled);
    bool rotateFunction(GCore::InputParameters param);
    bool updateSize(GCore::InputParameters param);

    void onMouseReleased();
    void onMousePressed();

private:
    void createHandlers();

public slots:

    void onStartedEngine();
    void onAddingObject(int trackIndex, bool enabled);
    void addKeyFrame(int trackindex, quint64 time, int type);
    void editKeyFrame(int trackindex, quint64 lastTime, quint64 newTime, int type);

    void editKeyFrameWithPosition(int trackIndex, QString status);

    void removeKeyFrame(int trackindex, quint64 time);

    void keyFrameChanged(int trackIndex, QMap<quint64, int> list);

    void keyFrameClear(int trackIndex);

    Q_INVOKABLE void setKeyFrames(int trackIndex, QMap<qint64, int> keyFrames);
    Q_INVOKABLE void setCurrentTime(qint64 time);
    Q_INVOKABLE QVariantList getSpatialFrames() const;

    // Live transform — sphere moves visually while user drags. Never writes
    // a keyframe. Call this on every pointer-move during a drag.
    Q_INVOKABLE void setObjectLocationLive(int trackIndex, double x, double y, double z);
    // Commit transform — fires once on mouse release / transform finalization.
    // In Auto Key mode: update existing kf at current time, OR create one.
    // In Manual Key mode: only updates the live position (no kf write).
    Q_INVOKABLE void commitObjectLocation(int trackIndex, double x, double y, double z);
    // Back-compat shim — forwards to setObjectLocationLive. Will be removed
    // once all callers migrate to the live/commit split.
    Q_INVOKABLE void setObjectLocation(int trackIndex, double x, double y, double z);
    Q_INVOKABLE void setKeyframeLaneActive(int trackIndex, bool active);
    void syncObjectKeyframes(int trackIndex, QList<qint64> times);

    ObjectPosAutomation* ensureAutomation(int trackIndex);
    void setKeyFrameInterp(int trackIndex, qint64 time, int interp);

    // Auto Key Mode — when true (default), commit operations write keyframes
    // at the current playhead. When false, transforms are visual-only and the
    // user must explicitly create keyframes via the lane or a shortcut.
    bool autoKeyMode() const { return m_autoKeyMode; }
    void setAutoKeyMode(bool on);
    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void save();
    Q_INVOKABLE void load(QStringList objects);

signals:
    void sigKeyFrameEdited(int trackIndex, QString status);
    void sigAutomationCreated(int trackIndex, ObjectPosAutomation* automation);
    // Emitted when ObjectCreator auto-records a keyframe (e.g., user drags the
    // sphere with the keyframe lane active). main.cpp routes this to AreaInfo
    // so the timeline lane gets an orange marker too.
    void sigAutoRecordedKeyFrame(int trackIndex, quint64 time, int type);

    void sigObjectMoved(int trackIndex, double x, double y, double z);
    void sigSaveObjectMove(int trackIndex, double x, double y, double z);
    void sigCurrentTimeChanged(qint64 time);
    void autoKeyModeChanged(bool on);
    void sigEngineStarted();
    void sigSaveObjectsReady(QStringList result);
    void sigCurrentStatus(int trackIndex, QString currentChanges);

private slots:

    void keyframeItemsListChanged(QList<KeyFrameItem*> keyframeItemsList);
    void currentTimeChanged(quint64 currentTime);

    void onEntityMoved(Scene::DEntityWRef entity, Vector3D movement);

    void canPlayChanged();

    void animationChanged();

protected:
    QString m_latestChanges;
    QString m_currentChanes;

    double m_objectX;
    double m_objectY;
    double m_objectZ;
    int m_entityMovingKey = -1;
    bool m_entityMoved = false;
    bool m_animationMoved = false;
    bool m_isCreatingTestObject = false;
    DGE::QtWrapper::QEngineRef m_engineRef;
    DGE::Core::System::ContextRef m_context;
    DGE::Scene::SceneNodeRef m_scene;
    DGE::Managers::AssetsManagerWRef m_assetsManager;
    QMap<int, DGE::Scene::DEntity*> m_objects;
    QMap<int, AnimationTrackItem*> _tracklist;
    QMap<int, QMap<quint64, QJsonObject>> _lastKeyFrames;

    // Per-track 3D position keyframes — drives scene3D during playback
    QMap<int, QVector3D>        m_lastPositions;
    QMap<int, ObjectPosAutomation*> m_objectAutomations;
    QSet<int>                    m_keyframeLaneActive;   // legacy — to be removed
    qint64                       m_currentTime = 0;
    bool                         m_autoKeyMode = true;

    // Per-track spatial state snapshot — written by UI thread at 60Hz, read by JS bridge
    QHash<int, SpatialFrame> m_spatialFrames;

    QList<qint64> removing_list;
    bool _is_undoing = false;
    QList<quint64> _undo_redo;

    QString _beforeAnimationChanged;

    ShaderResourceRef m_shaderResource;
    AnimationManager* m_animationManager;
};
