#pragma once

#include <QColor>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QString>
#include <QVariantList>
#include <QVector3D>

class ObjectPosAutomation;

// Bridges DAW timeline state to the Three.js WebEngine viewports embedded in main.qml.
// Signals flow C++ -> QML/JS via QWebChannel. Q_INVOKABLE methods flow QML/JS -> C++.
class Scene3DController : public QObject {
    Q_OBJECT
    Q_PROPERTY(qint64 playhead READ playhead NOTIFY playheadMoved)
    Q_PROPERTY(int selectedTrack READ selectedTrack NOTIFY trackSelected)

public:
    explicit Scene3DController(QObject* parent = nullptr);

    qint64 playhead() const { return m_playhead; }
    int selectedTrack() const { return m_selectedTrack; }

    Q_INVOKABLE QVector3D entityPosition(int trackIndex) const;
    Q_INVOKABLE QList<int> entityIds() const;
    Q_INVOKABLE QString entityColor(int trackIndex) const;
    Q_INVOKABLE bool entityVisible(int trackIndex) const;

    // ── Live + Commit transform API (professional commit-only animation) ──
    // Called by JS on every pointer-move during a drag. Updates only the live
    // sphere position — no keyframe writes.
    Q_INVOKABLE void entityMovedLiveInScene(int trackIndex, double x, double y, double z);
    // Called by JS once on mouse release / drag end. The C++ side decides
    // (per Auto Key Mode) whether to write/update a keyframe.
    Q_INVOKABLE void entityCommittedInScene(int trackIndex, double x, double y, double z);
    // Back-compat alias for the previous always-on auto-record entry. Forwards
    // to the live path so legacy callers no longer write kfs during drag.
    Q_INVOKABLE void entityMovedInScene(int trackIndex, double x, double y, double z);

    // Called by QML when the user clicks a sphere (raycast hit).
    Q_INVOKABLE void entityClicked(int trackIndex);
    // Called by JS when the user clicks a kf marker in the 3D/2D viewport.
    // Decodes kfId → time, emits kfClickedInScene so main.cpp can route to
    // lane selection + tangent display.
    Q_INVOKABLE void keyframeClickedInScene(int trackIndex, const QString& kfId);
    Q_INVOKABLE void log(const QString& msg) const;

    // Tangent drag — live preview vs commit. kfId is the kf's stable UUID.
    Q_INVOKABLE void tangentChangedLiveInScene(const QString& kfId, const QString& side,
                                               double x, double y, double z);
    Q_INVOKABLE void tangentCommittedInScene(const QString& kfId, const QString& side,
                                             double x, double y, double z);
    // Back-compat alias.
    Q_INVOKABLE void tangentChangedInScene(const QString& kfId, const QString& side,
                                           double x, double y, double z);

public slots:
    void setPlayhead(qint64 time);
    void setEntityPosition(int trackIndex, double x, double y, double z);
    void setEntityColor(int trackIndex, const QColor& color);
    void setEntityVisible(int trackIndex, bool visible);
    void setSelected(int trackIndex);
    void resetScene();

    // Phase 1: per-track keyframe / path push to JS
    void attachAutomation(int trackIndex, ObjectPosAutomation* automation);
    void pushKeyframesFor(int trackIndex);
    void pushPathFor(int trackIndex);

    // Phase 2: push Bezier tangent handles for a specific keyframe
    void setSelectedKeyframe(int trackIndex, qint64 time);
    void clearTangentSelection();

signals:
    void playheadMoved(qint64 time);
    void entityAdded(int trackIndex, double x, double y, double z);
    void entityPositionChanged(int trackIndex, double x, double y, double z);
    void entityColorChanged(int trackIndex, QString colorHex);
    void entityVisibilityChanged(int trackIndex, bool visible);
    void sceneReset();
    void trackSelected(int trackIndex);

    // Reverse sync: live drag → ObjectCreator::setObjectLocationLive
    void entityMovedFromSceneLive(int trackIndex, double x, double y, double z);
    // Reverse sync: drag release → ObjectCreator::commitObjectLocation
    void entityCommittedFromScene(int trackIndex, double x, double y, double z);
    // Back-compat — same as entityMovedFromSceneLive (forwarded). Will be
    // removed once the JS migration completes.
    void entityMovedFromScene(int trackIndex, double x, double y, double z);

    // Phase 1 / R6: keyframe / path signals consumed by Three.js viewports.
    //
    // Reconciliation model (avoids GPU thrash during high-frequency tangent drags):
    //   keyframesCleared    — full reset (track removal / scene reset)
    //   keyframeAdded       — kf with this id NEW; JS creates marker mesh
    //   keyframeUpdated     — kf already known; JS updates pos / replaces geometry if interp differs
    //   keyframeRemoved     — kf gone; JS disposes mesh
    void keyframesCleared(int trackIndex);
    void keyframeAdded(int trackIndex, QString kfId, double x, double y, double z, int interp);
    void keyframeUpdated(int trackIndex, QString kfId, double x, double y, double z, int interp);
    void keyframeRemoved(int trackIndex, QString kfId);
    void pathSampled(int trackIndex, QVariantList xyzFlat);

    // Phase 2: Bezier tangent handle signals
    void tangentsForSelected(QString kfId,
                             double kx, double ky, double kz,
                             double inX, double inY, double inZ,
                             double outX, double outY, double outZ);
    void tangentsCleared();

    // User clicked a kf marker in the 3D/2D viewport. Routed in main.cpp
    // to update the lane's selection halo + tangent display.
    void kfClickedInScene(int trackIndex, qint64 time);

private:
    QHash<int, QVector3D> m_positions;
    QHash<int, QColor>    m_colors;
    QHash<int, bool>      m_visible;
    QHash<int, ObjectPosAutomation*> m_automations;
    // Per-track snapshot of (kfId → publishedKey-state) — lets pushKeyframesFor
    // emit narrow add / update / remove diffs instead of clear-then-readd-all.
    struct PublishedKey {
        qint64 time = 0;
        double x = 0, y = 0, z = 0;
        int interp = 1;
    };
    QHash<int, QHash<QString, PublishedKey>> m_publishedKeys;
    qint64 m_playhead { 0 };
    int    m_selectedTrack { -1 };
};
