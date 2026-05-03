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

QVariantMap Scene3DController::entityPositionMap(int trackIndex) const
{
    const QVector3D p = m_positions.value(trackIndex);
    QVariantMap m;
    m.insert(QStringLiteral("x"), double(p.x()));
    m.insert(QStringLiteral("y"), double(p.y()));
    m.insert(QStringLiteral("z"), double(p.z()));
    return m;
}

QList<int> Scene3DController::entityIds() const
{
    return m_positions.keys();
}

QString Scene3DController::entityColor(int trackIndex) const
{
    return m_colors.value(trackIndex, QColor("#888888")).name();
}

bool Scene3DController::entityVisible(int trackIndex) const
{
    return m_visible.value(trackIndex, true);
}

void Scene3DController::entityMovedInScene(int trackIndex, double x, double y, double z)
{
    m_positions.insert(trackIndex, QVector3D(float(x), float(y), float(z)));
    emit entityPositionChanged(trackIndex, x, y, z);
    emit entityMovedFromScene(trackIndex, x, y, z);
}

void Scene3DController::entityClicked(int trackIndex)
{
    setSelected(trackIndex);
}

void Scene3DController::log(const QString& msg) const
{
    qDebug().noquote() << "SCENE-LOG:" << msg;
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
        emit entityAdded(trackIndex, x, y, z);
    emit entityPositionChanged(trackIndex, x, y, z);
}

void Scene3DController::setEntityColor(int trackIndex, const QColor& color)
{
    if (m_colors.contains(trackIndex) && m_colors.value(trackIndex) == color)
        return;
    m_colors.insert(trackIndex, color);
    emit entityColorChanged(trackIndex, color.name());
}

void Scene3DController::setEntityVisible(int trackIndex, bool visible)
{
    if (m_visible.contains(trackIndex) && m_visible.value(trackIndex) == visible)
        return;
    m_visible.insert(trackIndex, visible);
    emit entityVisibilityChanged(trackIndex, visible);
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
    m_colors.clear();
    m_visible.clear();
    m_selectedTrack = -1;
    emit sceneReset();
}
