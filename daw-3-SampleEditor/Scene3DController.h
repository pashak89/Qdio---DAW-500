#pragma once

#include <QHash>
#include <QObject>
#include <QVector3D>

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

    // Called by QML when the user drags an entity in the scene.
    Q_INVOKABLE void entityMovedInScene(int trackIndex, double x, double y, double z);

    // Called by QML when the user clicks a sphere (raycast hit).
    Q_INVOKABLE void entityClicked(int trackIndex);

public slots:
    void setPlayhead(qint64 time);
    void setEntityPosition(int trackIndex, double x, double y, double z);
    void setSelected(int trackIndex);
    void resetScene();

signals:
    void playheadMoved(qint64 time);
    void entityAdded(int trackIndex, QVector3D position);
    void entityPositionChanged(int trackIndex, QVector3D position);
    void sceneReset();
    void trackSelected(int trackIndex);

    // Reverse sync: emitted on user drag, consumed by ObjectCreator.
    void entityMovedFromScene(int trackIndex, double x, double y, double z);

private:
    QHash<int, QVector3D> m_positions;
    qint64 m_playhead { 0 };
    int    m_selectedTrack { -1 };
};
