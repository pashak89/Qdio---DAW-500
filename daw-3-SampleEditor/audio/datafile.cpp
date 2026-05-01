#include "datafile.h"

DataFile::DataFile(Type type, QObject* parent)
    : QObject(parent)
    , _jsonDoc(QJsonDocument())
    , m_type(type)
{
}

DataFile::DataFile(DataFile::Type type, QJsonDocument jsonDoc, QObject* parent)
    : QObject(parent)
    , _jsonDoc(jsonDoc)
    , m_type(type)
{
}

QJsonDocument DataFile::jsonDoc()
{
    return _jsonDoc;
}
