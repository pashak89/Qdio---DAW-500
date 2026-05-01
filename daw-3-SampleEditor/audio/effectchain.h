#ifndef EFFECTCHAIN_H
#define EFFECTCHAIN_H

#include "audio_global.h"
#include "effect.h"

#include <QObject>
#include <QQmlEngine>

class EffectChain : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum EffectChaintRole {
        ROLE_Index = Qt::UserRole + 1,
        ROLE_Name,
        ROLE_Type,
        ROLE_Enabled,
        ROLE_TrackIndex,
        ROLE_ControlVisible
    };
    Q_ENUM(EffectChaintRole)

    EffectChain(int trackIndex = -1, QObject* _parent = Q_NULLPTR);
    ~EffectChain() override;

    int appendEffect(QSharedPointer<Effect> _effect);
    void removeEffect(int index);

    bool processAudioBuffer(sampleFrame* _buf, const fpp_t _frames, bool hasInputNoise);
    void startRunning();
    void clear();
    int size();

    QSharedPointer<Effect> effect(int index);
    Q_INVOKABLE Effect* getEffect(QString name);
    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }

    Q_INVOKABLE void up(int index);
    Q_INVOKABLE void down(int index);

    bool isAutomated(QString menu, QString submenu);
    bool isAutomationEnabled(QString menu, QString submenu);
    bool isEnabled(QString menu);
    void controlVisibleChanged(QString menu);

    QMap<QString, int> getEffectList();

    int count()
    {
        return size();
    }
    Q_INVOKABLE bool moveRowQml(int from, int to)
    {
        const int n = size();
        if (from < 0 || from >= n)
            return false;

        // allow "insert at end"
        if (to < 0)
            to = 0;
        if (to > n)
            to = n;

        if (to == from || to == from + 1)
            return true;

        // Convert QML insertion index -> final index after removal
        int dest = to;
        if (dest > from)
            dest -= 1;

        if (dest < 0)
            dest = 0;
        if (dest >= n)
            dest = n - 1;

        // beginMoveRows destinationChild is the index BEFORE which rows are inserted
        int destinationChild = dest;
        if (destinationChild > from)
            destinationChild += 1;

        beginMoveRows(QModelIndex(), from, from, QModelIndex(), destinationChild);

        // Reorder your storage:
        // Example if you store items in QVector<Item> m_items;
        m_effects.move(from, dest);

        endMoveRows();
        return true;
    }

    Q_INVOKABLE QVariantMap get(int row)
    {
        QVariantMap out;
        if (row < 0 || row >= size())
            return out;

        const QModelIndex idx = index(row, 0);

        const auto roles = roleNames(); // QHash<int,QByteArray>
        for (auto it = roles.begin(); it != roles.end(); ++it) {
            const int roleId = it.key();
            const QString roleName = QString::fromUtf8(it.value());
            out.insert(roleName, data(idx, roleId)); // fetch each role's value
        }
        return out;
    }

private:
    QHash<int, QByteArray> m_roleNames;

    QVector<QSharedPointer<Effect>> m_effects;
    QMap<QString, int> effect_names;

    int m_trackIndex;
    bool m_enabledModel;

signals:
    void aboutToClear();
    void countChanged();
    // void dataChanged();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);

    void enabledChanged(QString menu);
    QVector<QSharedPointer<Effect>> effects() const;
};

#endif // EFFECTCHAIN_H
