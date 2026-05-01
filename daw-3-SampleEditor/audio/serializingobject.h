#ifndef SERIALIZINGOBJECT_H
#define SERIALIZINGOBJECT_H

#include <QJsonDocument>
#include <QJsonObject>

class SerializingObject {
public:
    SerializingObject();
    virtual ~SerializingObject();

    virtual QJsonDocument saveState(QJsonDocument _doc);

    virtual void restoreState(const QJsonObject& _this);

    // to be implemented by actual object
    virtual QString nodeName() const = 0;
};

#endif // SERIALIZINGOBJECT_H
