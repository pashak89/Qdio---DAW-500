#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QVector3D>

enum class KeyInterp : uint8_t { Hold = 0, Linear = 1, Bezier = 2 };

struct ObjectKeyFrame {
    qint64    time       = 0;
    QVector3D pos        = {};
    KeyInterp interp     = KeyInterp::Linear;
    // control-point offsets in world-space relative to each key's pos
    QVector3D tangentOut = {};   // outgoing handle from this key
    QVector3D tangentIn  = {};   // incoming handle into this key (points toward the key)
};

// Per-track keyframe store for 3D object position.
// Provides Hold / Linear / cubic-Bezier evaluate() across stored keyframes.
class ObjectPosAutomation : public QObject {
    Q_OBJECT
public:
    explicit ObjectPosAutomation(QObject* parent = nullptr);

    void addKey(qint64 t, QVector3D pos, KeyInterp k = KeyInterp::Linear);
    void removeKey(qint64 t);
    bool hasKey(qint64 t) const;
    void setInterp(qint64 t, KeyInterp k);
    void setTangents(qint64 t, QVector3D tangentIn, QVector3D tangentOut);

    const ObjectKeyFrame* key(qint64 t) const;

    QVector3D evaluate(qint64 t) const;

    const QMap<qint64, ObjectKeyFrame>& keys() const { return m_keys; }
    QList<qint64> keyTimes() const { return m_keys.keys(); }
    bool isEmpty() const { return m_keys.isEmpty(); }
    int  count()   const { return m_keys.count(); }

    QJsonObject save() const;
    void        load(const QJsonObject& obj);

signals:
    void changed();

private:
    static QVector3D lerp(QVector3D a, QVector3D b, float t);
    static QVector3D cubicBezier(float t, QVector3D p0, QVector3D p1,
                                 QVector3D p2, QVector3D p3);

    QMap<qint64, ObjectKeyFrame> m_keys;
};
