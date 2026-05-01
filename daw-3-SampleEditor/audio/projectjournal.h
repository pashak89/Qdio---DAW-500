#ifndef PROJECTJOURNAL_H
#define PROJECTJOURNAL_H

#include "audio_global.h"
#include "datafile.h"
#include <QObject>
#include <QStack>
class JournallingObject;

class ProjectJournal : public QObject {
    Q_OBJECT
public:
    static const int MAX_UNDO_STATES;
    explicit ProjectJournal();
    virtual ~ProjectJournal() = default;

    //    bool isJournalling() const
    //    {
    //        return m_journalling;
    //    }

    //    void setJournalling(const bool _on)
    //    {
    //        m_journalling = _on;
    //    }

    void addJournalCheckPoint(JournallingObject* jo);

private:
};

#endif // PROJECTJOURNAL_H
