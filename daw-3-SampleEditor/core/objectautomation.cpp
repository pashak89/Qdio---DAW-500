#include "objectautomation.h"

ObjectPosAutomation::ObjectPosAutomation(QObject* parent)
    : QObject(parent)
{
}

void ObjectPosAutomation::addKey(qint64 t, QVector3D pos, KeyInterp k)
{
    ObjectKeyFrame kf;
    if (m_keys.contains(t))
        kf = m_keys[t];   // preserve existing tangents when overwriting
    kf.time   = t;
    kf.pos    = pos;
    kf.interp = k;
    m_keys[t] = kf;
    emit changed();
}

void ObjectPosAutomation::removeKey(qint64 t)
{
    if (m_keys.remove(t))
        emit changed();
}

bool ObjectPosAutomation::hasKey(qint64 t) const
{
    return m_keys.contains(t);
}

void ObjectPosAutomation::setInterp(qint64 t, KeyInterp k)
{
    auto it = m_keys.find(t);
    if (it == m_keys.end())
        return;
    it->interp = k;
    emit changed();
}

void ObjectPosAutomation::setTangents(qint64 t, QVector3D tangentIn, QVector3D tangentOut)
{
    auto it = m_keys.find(t);
    if (it == m_keys.end())
        return;
    it->tangentIn  = tangentIn;
    it->tangentOut = tangentOut;
    emit changed();
}

const ObjectKeyFrame* ObjectPosAutomation::key(qint64 t) const
{
    auto it = m_keys.constFind(t);
    return (it == m_keys.constEnd()) ? nullptr : &(*it);
}

QVector3D ObjectPosAutomation::evaluate(qint64 t) const
{
    if (m_keys.isEmpty())
        return {};
    if (t <= m_keys.firstKey())
        return m_keys.first().pos;
    if (t >= m_keys.lastKey())
        return m_keys.last().pos;

    auto right = m_keys.lowerBound(t);   // first key with time >= t
    auto left  = right;
    --left;                               // last key with time < t

    const ObjectKeyFrame& kL = left.value();
    const ObjectKeyFrame& kR = right.value();

    const float span = float(kR.time - kL.time);
    if (span <= 0.f)
        return kR.pos;
    const float u = float(t - kL.time) / span;   // normalised 0..1

    switch (kL.interp) {
    case KeyInterp::Hold:
        return kL.pos;
    case KeyInterp::Linear:
        return lerp(kL.pos, kR.pos, u);
    case KeyInterp::Bezier: {
        QVector3D p1 = kL.pos + kL.tangentOut;
        QVector3D p2 = kR.pos + kR.tangentIn;
        return cubicBezier(u, kL.pos, p1, p2, kR.pos);
    }
    }
    return lerp(kL.pos, kR.pos, u);
}

QVector3D ObjectPosAutomation::lerp(QVector3D a, QVector3D b, float t)
{
    return a + (b - a) * t;
}

QVector3D ObjectPosAutomation::cubicBezier(float t, QVector3D p0, QVector3D p1,
                                         QVector3D p2, QVector3D p3)
{
    const float s = 1.f - t;
    return s*s*s*p0 + 3.f*s*s*t*p1 + 3.f*s*t*t*p2 + t*t*t*p3;
}

QJsonObject ObjectPosAutomation::save() const
{
    QJsonObject root;
    QJsonArray arr;
    for (auto it = m_keys.constBegin(); it != m_keys.constEnd(); ++it) {
        const ObjectKeyFrame& kf = it.value();
        QJsonObject o;
        o["time"]   = kf.time;
        o["x"]      = double(kf.pos.x());
        o["y"]      = double(kf.pos.y());
        o["z"]      = double(kf.pos.z());
        o["interp"] = int(kf.interp);
        o["tout_x"] = double(kf.tangentOut.x());
        o["tout_y"] = double(kf.tangentOut.y());
        o["tout_z"] = double(kf.tangentOut.z());
        o["tin_x"]  = double(kf.tangentIn.x());
        o["tin_y"]  = double(kf.tangentIn.y());
        o["tin_z"]  = double(kf.tangentIn.z());
        arr.append(o);
    }
    root["keys"] = arr;
    return root;
}

void ObjectPosAutomation::load(const QJsonObject& obj)
{
    m_keys.clear();
    const QJsonArray arr = obj["keys"].toArray();
    for (const auto& v : arr) {
        const QJsonObject o = v.toObject();
        ObjectKeyFrame kf;
        kf.time   = qint64(o["time"].toDouble());
        kf.pos    = { float(o["x"].toDouble()),
                      float(o["y"].toDouble()),
                      float(o["z"].toDouble()) };
        kf.interp = KeyInterp(o["interp"].toInt(int(KeyInterp::Linear)));
        kf.tangentOut = { float(o["tout_x"].toDouble()),
                          float(o["tout_y"].toDouble()),
                          float(o["tout_z"].toDouble()) };
        kf.tangentIn  = { float(o["tin_x"].toDouble()),
                          float(o["tin_y"].toDouble()),
                          float(o["tin_z"].toDouble()) };
        m_keys[kf.time] = kf;
    }
    emit changed();
}
