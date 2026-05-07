#pragma once

#include <QColor>
#include <QHash>
#include <QObject>
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

    // Called by QML when the user drags an entity in the scene.
    Q_INVOKABLE void entityMovedInScene(int trackIndex, double x, double y, double z);

    // Called by QML when the user clicks a sphere (raycast hit).
    Q_INVOKABLE void entityClicked(int trackIndex);
    Q_INVOKABLE void log(const QString& msg) const;

    // Phase 3: live tangent drag from JS — kfId="k_<track>_<time>", side="in"|"out"
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

    // Reverse sync: emitted on user drag, consumed by ObjectCreator.
    void entityMovedFromScene(int trackIndex, double x, double y, double z);

    // Phase 1: keyframe / path signals consumed by Three.js viewports
    void keyframesCleared(int trackIndex);
    void keyframeAdded(int trackIndex, QString kfId, double x, double y, double z, int interp);
    void pathSampled(int trackIndex, QVariantList xyzFlat);

    // Phase 2: Bezier tangent handle signals
    void tangentsForSelected(QString kfId,
                             double kx, double ky, double kz,
                             double inX, double inY, double inZ,
                             double outX, double outY, double outZ);
    void tangentsCleared();

private:
    QHash<int, QVector3D> m_positions;
    QHash<int, QColor>    m_colors;
    QHash<int, bool>      m_visible;
    QHash<int, ObjectPosAutomation*> m_automations;
    qint64 m_playhead { 0 };
    int    m_selectedTrack { -1 };
};
