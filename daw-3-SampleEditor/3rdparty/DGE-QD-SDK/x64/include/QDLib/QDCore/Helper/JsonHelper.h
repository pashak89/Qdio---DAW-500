/* Copyright (C) 2025, Abbas Aliakbari - All Rights Reserved
 * SPDX-License-Identifier: MIT
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: a.aliakbari91@gmail.com
 *
 * Written by Abbas Aliakbari
 *
 * @section DESCRIPTION
 *
 *
 */
#pragma once

#include <QJsonObject>
#include <QMetaProperty>
#include "QJsonDocument"
#include "qdebug.h"
#include "QJsonArray"

class JsonHelper
{
public:


    template<class T>
    static void my_deserialize(T* target_obj,QString json_object_str){


        QJsonDocument json_doc = QJsonDocument::fromJson(QByteArray::fromStdString(json_object_str.toStdString()));


        QStringList keys = json_doc.object().keys();

        foreach(QString key , keys){

            QJsonValue value = json_doc.object().value(key);
            if(value.isObject()){
                QJsonObject obj = value.toObject();


            }
            else if(value.isArray()){
                auto oo = value.toArray();
                //                foreach(QVariant var , oo.toVariantList()){
                //                    var.toString()
                //                }
                qDebug() << "ARRAY oo : " << oo.toVariantList() ;
            }
            else
                target_obj->setProperty(key.toStdString().c_str(),value);
        }
    }

    template<class T>
    static QList<T*> my_list_deserialize(T* model ,QString json_list_str){


        Q_UNUSED(model);

        QList<T*> result_list;
        QJsonDocument json_doc = QJsonDocument::fromJson(QByteArray::fromStdString(json_list_str.toStdString()));

        if(json_doc.isArray()){
            QJsonArray array = json_doc.array();
            foreach(QJsonValue value , array){
                QStringList keys = value.toObject().keys();
                T* tt = new T();
                foreach(QString key ,keys){
                    QJsonValue value2 = value.toObject().value(key);
                    tt->setProperty(key.toStdString().c_str(),value2);
                }
                result_list.append(tt);
            }
        }

        return result_list;
    }


    template<class T>
    static void Deserialize(T* arg, const QJsonObject &jobject)
    {

        const QMetaObject* metaObject =  arg->metaObject();
        foreach (QString k, jobject.keys()) {
			auto		name	  = k.toLower().toStdString();
			const char* kc		  = name.c_str();
			int propIndex = metaObject->indexOfProperty(kc);
            if ( propIndex < 0 )
                continue;

            QVariant variant = arg->property(kc);
            if ( jobject[k].isObject() ) {

                if (variant.canConvert<QObject *>()) {
                    // Both jobject and the property are objects
                    QObject* obj = variant.value<QObject *>();
                    if ( !obj ){
                        // TODO: create object
                        continue;
                    }

                    // Iterate
                    Deserialize<QObject>(obj, jobject[k].toObject());
                }

            } else {
                QVariant debug_variant = jobject[k].toVariant();
                qDebug() << debug_variant << " KC :" << kc;
                arg->setProperty(kc, debug_variant);
            }

        }
    }


    template<class T>
    static const QString Serialize(const T *arg)
    {
        QJsonObject jobj;
        const QMetaObject* metaObject = arg->metaObject();

        for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {

            QMetaProperty prop = metaObject->property(i);

            QVariant variant = arg->property(prop.name());

            if ( variant.canConvert<QObject *>() ) {

                QObject* sub_obj = variant.value<QObject*>();
                jobj.insert( prop.name(), Serialize<QObject>(sub_obj) );

            } else
                jobj.insert(prop.name(), QJsonValue::fromVariant(arg->property(prop.name())));
        }

        QJsonDocument doc(jobj);

        return doc.toJson(QJsonDocument::Compact);
    }

    static const QJsonObject ObjectFromString(const QString& in)
    {
        QJsonObject obj;

        QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

        // check validity of the document
        if(!doc.isNull())
        {
            if(doc.isObject())
            {
                obj = doc.object();
            }
            else
            {
				qDebug() << "Document is not an object" << Qt::endl;
			}
        }
        else
        {
			qDebug() << "Invalid JSON...\n" << in << Qt::endl;
		}

        return obj;
    }

private:
	JsonHelper();
};
