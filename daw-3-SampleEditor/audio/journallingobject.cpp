#include "journallingobject.h"
#include "Engine.h"
JournallingObject::JournallingObject()
    : SerializingObject()
    , m_journalling(true)

{
}

QJsonDocument JournallingObject::saveState(QJsonDocument _doc)
{
    if (isJournalling()) {
        QJsonObject _this = SerializingObject::saveState(_doc).object();

        //        QDomElement journalNode = _doc.createElement("journallingObject");
        //        journalNode.setAttribute("id", id());
        //        journalNode.setAttribute("metadata", true);
        //        _this.appendChild(journalNode);

        _this["clip_index"] = (int)id();
        _this["metadata"] = (qint64)this;

        qDebug() << "SAVE";
        return QJsonDocument(_this);
    } else {
        return QJsonDocument();
    }
}

void JournallingObject::addJournalCheckPoint()
{
    if (isJournalling()) {
        AudioManager::projectJournal()->addJournalCheckPoint(this);
    }
}
