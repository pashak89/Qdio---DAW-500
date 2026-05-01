#include "serializingobject.h"

SerializingObject::SerializingObject()

{
}
SerializingObject::~SerializingObject()
{
}

QJsonDocument SerializingObject::saveState(QJsonDocument doc)
{
    QJsonObject element = doc.object();
    element[nodeName()] = doc.object();

    //    saveSettings(doc, element);

    //    if (hook()) {
    //        hook()->saveSettings(doc, element);
    //    }

    return QJsonDocument(element);
}

void SerializingObject::restoreState(const QJsonObject& element)
{
    // loadSettings(element);

    //    if (hook()) {
    //        hook()->loadSettings(element);
    //    }
}
