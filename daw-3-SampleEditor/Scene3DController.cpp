#include "Scene3DController.h"

#include "core/objectautomation.h"
#include <QtDebug>
#include <algorithm>

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

void Scene3DController::attachAutomation(int trackIndex, ObjectPosAutomation* automation)
{
    if (!automation) return;
    if (m_automations.value(trackIndex) == automation) return;
    m_automations[trackIndex] = automation;
    // Whenever the automation mutates (add/remove/move/tangent/interp), re-push.
    QObject::connect(automation, &ObjectPosAutomation::changed, this,
        [this, trackIndex]() {
            pushKeyframesFor(trackIndex);
            pushPathFor(trackIndex);
        });
    pushKeyframesFor(trackIndex);
    pushPathFor(trackIndex);
}

void Scene3DController::pushKeyframesFor(int trackIndex)
{
    auto* oa = m_automations.value(trackIndex, nullptr);
    if (!oa) return;
    emit keyframesCleared(trackIndex);
    const auto& keys = oa->keys();
    for (auto it = keys.constBegin(); it != keys.constEnd(); ++it) {
        const ObjectKeyFrame& kf = it.value();
        const QString id = QStringLiteral("k_%1_%2").arg(trackIndex).arg(it.key());
        emit keyframeAdded(trackIndex, id,
                           double(kf.pos.x()), double(kf.pos.y()), double(kf.pos.z()),
                           int(kf.interp));
    }
}

void Scene3DController::pushPathFor(int trackIndex)
{
    auto* oa = m_automations.value(trackIndex, nullptr);
    if (!oa || oa->isEmpty()) {
        emit pathSampled(trackIndex, QVariantList{});
        return;
    }
    const QList<qint64> times = oa->keyTimes();
    const qint64 t0 = times.first();
    const qint64 t1 = times.last();
    QVariantList flat;
    if (t1 <= t0) {
        QVector3D p = oa->evaluate(t0);
        flat << double(p.x()) << double(p.y()) << double(p.z());
        emit pathSampled(trackIndex, flat);
        return;
    }
    constexpr int N = 128;
    flat.reserve(N * 3);
    for (int i = 0; i < N; ++i) {
        const double u = double(i) / double(N - 1);
        const qint64 t = t0 + qint64(u * double(t1 - t0));
        QVector3D p = oa->evaluate(t);
        flat << double(p.x()) << double(p.y()) << double(p.z());
    }
    emit pathSampled(trackIndex, flat);
}
