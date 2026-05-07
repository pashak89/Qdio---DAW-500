#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QString>
#include <QVector3D>

enum class KeyInterp : uint8_t { Hold = 0, Linear = 1, Bezier = 2 };

struct ObjectKeyFrame {
    QString   id;                 // stable identity — survives time changes
    qint64    time       = 0;
    QVector3D pos        = {};
    KeyInterp interp     = KeyInterp::Linear;
    // control-point offsets in world-space relative to each key's pos
    QVector3D tangentOut = {};   // outgoing handle from this key
    QVector3D tangentIn  = {};   // incoming handle into this key (points toward the key)
};

// Generate a short uuid suitable for an ObjectKeyFrame::id.
QString makeKeyFrameId();

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

    // ── Identity-preserving API (id is stable; time can change) ───────────
    // Find a key by its stable id. Returns nullptr if not found.
    const ObjectKeyFrame* keyById(const QString& id) const;
    // Time of the key with this id, or -1 if not found.
    qint64 timeForId(const QString& id) const;
    // Add a key with a caller-chosen id. If a key with this id exists at a
    // different time, it is moved to the new time first. Existing time-slot is
    // overwritten as in addKey().
    void addKeyWithId(const QString& id, qint64 t, QVector3D pos,
                      KeyInterp k = KeyInterp::Linear);
    // Move the key with this id to a new time, preserving pos/interp/tangents.
    // No-op if the id doesn't exist or newTime already holds a different key
    // (callers should remove or merge first).
    void moveTimeById(const QString& id, qint64 newTime);
    // Remove the key with this id.
    void removeById(const QString& id);

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
