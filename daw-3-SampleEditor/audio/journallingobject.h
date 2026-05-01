#ifndef JOURNALLINGOBJECT_H
#define JOURNALLINGOBJECT_H

#include "audio_global.h"
#include "serializingobject.h"
#include <QObject>
#include <QDebug>

class JournallingObject : public SerializingObject {

public:
    explicit JournallingObject();

    inline bool isJournalling() const
    {
        return m_journalling;
    }

    inline void setJournalling(const bool _sr)
    {
        m_journalling = _sr;
    }

    QJsonDocument saveState(QJsonDocument _doc) override;

    inline jo_id_t id() const
    {
        return m_id;
    }

    void addJournalCheckPoint();

private:
    bool m_journalling;
    jo_id_t m_id;
};

#endif // JOURNALLINGOBJECT_H
