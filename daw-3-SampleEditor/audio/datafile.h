#ifndef DATAFILE_H
#define DATAFILE_H

#include <QJsonDocument>
#include <QObject>

class DataFile : public QObject {
    Q_OBJECT
public:
    enum Types {
        SampleClip,
        SampleTrack,
        JournalData
    };
    using Type = Types;
    explicit DataFile(Type type, QObject* parent = Q_NULLPTR);
    explicit DataFile(Type type, QJsonDocument jsonDoc, QObject* parent = Q_NULLPTR);

    QJsonDocument jsonDoc();

private:
    Type m_type;
    QJsonDocument _jsonDoc;
};

#endif // DATAFILE_H
