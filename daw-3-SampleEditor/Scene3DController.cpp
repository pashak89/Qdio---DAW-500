#include "Scene3DController.h"

#include <QtDebug>

Scene3DController::Scene3DController(QObject* parent)
    : QObject(parent)
{
}

QVector3D Scene3DController::entityPosition(int trackIndex) const
{
    return m_positions.value(trackIndex);
}

QList<int> Scene3DController::entityIds() const
{
    return m_positions.keys();
}

void Scene3DController::entityMovedInScene(int trackIndex, double x, double y, double z)
{
    m_positions.insert(trackIndex, QVector3D(float(x), float(y), float(z)));
    emit entityMovedFromScene(trackIndex, x, y, z);
}

void Scene3DController::entityClicked(int trackIndex)
{
    setSelected(trackIndex);
}

void Scene3DController::setPlayhead(qint64 time)
{
    if (m_playhead == time)
        return;
    m_playhead = time;
    emit playheadMoved(time);
}

void Scene3DController::setEntityPosition(int trackIndex, double x, double y, double z)
{
    const QVector3D pos{float(x), float(y), float(z)};
    const bool isNew = !m_positions.contains(trackIndex);
    m_positions.insert(trackIndex, pos);
    if (isNew)
        emit entityAdded(trackIndex, pos);
    emit entityPositionChanged(trackIndex, pos);
}

void Scene3DController::setSelected(int trackIndex)
{
    if (m_selectedTrack == trackIndex)
        return;
    m_selectedTrack = trackIndex;
    emit trackSelected(trackIndex);
}

void Scene3DController::resetScene()
{
    m_positions.clear();
    m_selectedTrack = -1;
    emit sceneReset();
}
