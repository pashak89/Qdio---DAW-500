#include "Scene3DController.h"

#include "core/objectautomation.h"
#include <QtDebug>
#include <QDateTime>
#include <algorithm>
#include <cstdio>

namespace {
void scnLog(const QString& s) {
    if (FILE* f = std::fopen("C:\\temp\\kf\\kf_debug.log", "a")) {
        const QString line = QDateTime::currentDateTime().toString("hh:mm:ss.zzz")
                             + "  " + s + "\n";
        const QByteArray ba = line.toUtf8();
        std::fwrite(ba.constData(), 1, ba.size(), f);
        std::fflush(f);
        std::fclose(f);
    }
}
}

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

void Scene3DController::entityMovedLiveInScene(int trackIndex, double x, double y, double z)
{
    // Drag-frame: visual position only, no keyframe mutation.
    m_positions.insert(trackIndex, QVector3D(float(x), float(y), float(z)));
    emit entityPositionChanged(trackIndex, x, y, z);
    emit entityMovedFromSceneLive(trackIndex, x, y, z);
}

void Scene3DController::entityCommittedInScene(int trackIndex, double x, double y, double z)
{
    // Drag release: the single point at which animation data may be written
    // (subject to Auto Key Mode in ObjectCreator).
    m_positions.insert(trackIndex, QVector3D(float(x), float(y), float(z)));
    scnLog(QString("[Scene3D] entityCommittedInScene track=%1 pos=(%2,%3,%4)")
        .arg(trackIndex).arg(x).arg(y).arg(z));
    emit entityCommittedFromScene(trackIndex, x, y, z);
}

void Scene3DController::entityMovedInScene(int trackIndex, double x, double y, double z)
{
    // Back-compat: legacy JS that calls this from drag-frame events should
    // behave as a live update only. Emit the legacy signal too so any old
    // listener (none expected) still fires; primary path is the live signal.
    entityMovedLiveInScene(trackIndex, x, y, z);
    emit entityMovedFromScene(trackIndex, x, y, z);
}

void Scene3DController::entityClicked(int trackIndex)
{
    setSelected(trackIndex);
}

void Scene3DController::keyframeClickedInScene(int trackIndex, const QString& kfId)
{
    auto it = m_automations.constFind(trackIndex);
    if (it == m_automations.constEnd() || !it.value()) return;
    const qint64 t = it.value()->timeForId(kfId);
    if (t < 0) return;
    emit kfClickedInScene(trackIndex, t);
}

void Scene3DController::log(const QString& msg) const
{
    qDebug().noquote() << "SCENE-LOG:" << msg;
    scnLog(msg);   // also persist into the unified kf_debug.log for cross-side tracing
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

void Scene3DController::setPlaybackState(bool playing)
{
    if (m_playing == playing) return;
    m_playing = playing;
    scnLog(QString("[Scene3D] setPlaybackState playing=%1").arg(playing ? "true" : "false"));
    emit playbackStateChanged(playing);
}

void Scene3DController::resetScene()
{
    m_positions.clear();
    m_colors.clear();
    m_visible.clear();
    m_publishedKeys.clear();   // JS will get keyframesCleared per track via the reset
    m_selectedTrack = -1;
    emit sceneReset();
}

void Scene3DController::attachAutomation(int trackIndex, ObjectPosAutomation* automation)
{
    if (!automation) return;
    const bool reattach = (m_automations.value(trackIndex) == automation);
    scnLog(QString("[Scene3D] attachAutomation track=%1 automation=0x%2 reattach=%3")
        .arg(trackIndex).arg(quintptr(automation), 0, 16).arg(reattach ? "yes" : "no"));
    if (reattach) return;
    m_automations[trackIndex] = automation;
    // Whenever the automation mutates (add/remove/move/tangent/interp), re-push.
    QObject::connect(automation, &ObjectPosAutomation::changed, this,
        [this, trackIndex]() {
            scnLog(QString("[Scene3D] automation.changed → re-push track=%1").arg(trackIndex));
            pushKeyframesFor(trackIndex);
            pushPathFor(trackIndex);
        });
    pushKeyframesFor(trackIndex);
    pushPathFor(trackIndex);
}

void Scene3DController::pushKeyframesFor(int trackIndex)
{
    auto* oa = m_automations.value(trackIndex, nullptr);
    if (!oa) {
        scnLog(QString("[Scene3D→JS] pushKeyframesFor track=%1 NO_AUTOMATION").arg(trackIndex));
        return;
    }

    // R6: emit narrow diffs instead of clear+re-add-all. The published snapshot
    // tells us which kf ids JS already knows about. Each call emits at most:
    //   N keyframeAdded   (for ids new this round)
    //   M keyframeUpdated (for ids whose pos/interp changed since last push)
    //   K keyframeRemoved (for ids gone)
    // unchanged kfs produce zero traffic — critical for tangent-drag perf.
    auto& published = m_publishedKeys[trackIndex];
    QSet<QString> seen;
    const auto& keys = oa->keys();
    for (auto it = keys.constBegin(); it != keys.constEnd(); ++it) {
        const ObjectKeyFrame& kf = it.value();
        if (kf.id.isEmpty()) continue;   // safety
        seen.insert(kf.id);
        const double x = kf.pos.x();
        const double y = kf.pos.y();
        const double z = kf.pos.z();
        const int    ip = int(kf.interp);
        auto pit = published.find(kf.id);
        if (pit == published.end()) {
            scnLog(QString("[Scene3D→JS] kfAdded id=%1 t=%2 pos=(%3,%4,%5) interp=%6")
                .arg(kf.id).arg(kf.time).arg(x).arg(y).arg(z).arg(ip));
            emit keyframeAdded(trackIndex, kf.id, x, y, z, ip);
            published.insert(kf.id, PublishedKey{kf.time, x, y, z, ip});
        } else if (pit->x != x || pit->y != y || pit->z != z || pit->interp != ip || pit->time != kf.time) {
            scnLog(QString("[Scene3D→JS] kfUpdated id=%1 t=%2 pos=(%3,%4,%5) interp=%6")
                .arg(kf.id).arg(kf.time).arg(x).arg(y).arg(z).arg(ip));
            emit keyframeUpdated(trackIndex, kf.id, x, y, z, ip);
            *pit = PublishedKey{kf.time, x, y, z, ip};
        }
        // else: identical to last push — skip (this is the perf win).
    }

    // Anything still in `published` but not in `seen` was removed.
    for (auto it = published.begin(); it != published.end();) {
        if (!seen.contains(it.key())) {
            scnLog(QString("[Scene3D→JS] kfRemoved id=%1").arg(it.key()));
            emit keyframeRemoved(trackIndex, it.key());
            it = published.erase(it);
        } else {
            ++it;
        }
    }
}

void Scene3DController::setSelectedKeyframe(int trackIndex, qint64 time)
{
    auto* oa = m_automations.value(trackIndex, nullptr);
    if (!oa) {
        scnLog(QString("[Scene3D→JS] tangentsCleared (no automation track=%1)").arg(trackIndex));
        emit tangentsCleared(); return;
    }
    const ObjectKeyFrame* kf = oa->key(time);
    if (!kf || kf->interp != KeyInterp::Bezier) {
        scnLog(QString("[Scene3D→JS] tangentsCleared (no-key-or-not-bezier track=%1 time=%2)")
            .arg(trackIndex).arg(time));
        emit tangentsCleared(); return;
    }
    scnLog(QString("[Scene3D→JS] tangentsForSelected id=%1 in=(%2,%3,%4) out=(%5,%6,%7)")
        .arg(kf->id)
        .arg(kf->tangentIn.x()).arg(kf->tangentIn.y()).arg(kf->tangentIn.z())
        .arg(kf->tangentOut.x()).arg(kf->tangentOut.y()).arg(kf->tangentOut.z()));
    emit tangentsForSelected(kf->id,
        double(kf->pos.x()),        double(kf->pos.y()),        double(kf->pos.z()),
        double(kf->tangentIn.x()),  double(kf->tangentIn.y()),  double(kf->tangentIn.z()),
        double(kf->tangentOut.x()), double(kf->tangentOut.y()), double(kf->tangentOut.z()));
}

void Scene3DController::clearTangentSelection()
{
    emit tangentsCleared();
}

// Helper: locate the (track, time, automation) tuple owning a kf with the
// given stable UUID. Returns true on success.
static bool resolveKfId(const QHash<int, ObjectPosAutomation*>& automations,
                        const QString& kfId,
                        int& track, qint64& time, ObjectPosAutomation*& oa)
{
    if (kfId.isEmpty()) return false;
    for (auto it = automations.constBegin(); it != automations.constEnd(); ++it) {
        if (!it.value()) continue;
        const qint64 t = it.value()->timeForId(kfId);
        if (t >= 0) { track = it.key(); time = t; oa = it.value(); return true; }
    }
    return false;
}

void Scene3DController::tangentChangedLiveInScene(const QString& kfId, const QString& side,
                                                  double x, double y, double z)
{
    int track = -1; qint64 time = -1; ObjectPosAutomation* oa = nullptr;
    if (!resolveKfId(m_automations, kfId, track, time, oa)) return;
    const ObjectKeyFrame* kf = oa->key(time);
    if (!kf) return;

    QVector3D newIn  = kf->tangentIn;
    QVector3D newOut = kf->tangentOut;
    const QVector3D dragVec{float(x), float(y), float(z)};
    if (side == QLatin1String("in"))       newIn  = dragVec;
    else if (side == QLatin1String("out")) newOut = dragVec;
    else return;

    oa->setTangents(time, newIn, newOut);
    setSelectedKeyframe(track, time);
}

void Scene3DController::tangentCommittedInScene(const QString& kfId, const QString& side,
                                                double x, double y, double z)
{
    // For the moment a commit is just a final live apply (no full undo wiring
    // yet — that comes after the live/commit split is verified). Logging the
    // commit gives us a clear signal in kf_debug.log for verification.
    int track = -1; qint64 time = -1; ObjectPosAutomation* oa = nullptr;
    if (!resolveKfId(m_automations, kfId, track, time, oa)) return;
    scnLog(QString("[Scene3D] tangentCommittedInScene id=%1 side=%2 vec=(%3,%4,%5)")
        .arg(kfId).arg(side).arg(x).arg(y).arg(z));
    tangentChangedLiveInScene(kfId, side, x, y, z);
}

void Scene3DController::tangentChangedInScene(const QString& kfId, const QString& side,
                                              double x, double y, double z)
{
    // Back-compat alias.
    tangentChangedLiveInScene(kfId, side, x, y, z);
}

void Scene3DController::pushPathFor(int trackIndex)
{
    auto* oa = m_automations.value(trackIndex, nullptr);
    if (!oa || oa->isEmpty()) {
        scnLog(QString("[Scene3D→JS] pathSampled track=%1 EMPTY").arg(trackIndex));
        emit pathSampled(trackIndex, 0.0, 0.0, QVariantList{});
        return;
    }
    const QList<qint64> times = oa->keyTimes();
    const qint64 t0 = times.first();
    const qint64 t1 = times.last();
    QVariantList flat;
    if (t1 <= t0) {
        QVector3D p = oa->evaluate(t0);
        flat << double(p.x()) << double(p.y()) << double(p.z());
        scnLog(QString("[Scene3D→JS] pathSampled track=%1 single t=%2 pos=(%3,%4,%5)")
            .arg(trackIndex).arg(t0).arg(p.x()).arg(p.y()).arg(p.z()));
        emit pathSampled(trackIndex, double(t0), double(t0), flat);
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
    QVector3D pStart = oa->evaluate(t0);
    QVector3D pEnd   = oa->evaluate(t1);
    scnLog(QString("[Scene3D→JS] pathSampled track=%1 N=%2 t0=%3..t1=%4 start=(%5,%6,%7) end=(%8,%9,%10)")
        .arg(trackIndex).arg(N).arg(t0).arg(t1)
        .arg(pStart.x()).arg(pStart.y()).arg(pStart.z())
        .arg(pEnd.x()).arg(pEnd.y()).arg(pEnd.z()));
    emit pathSampled(trackIndex, double(t0), double(t1), flat);
}
