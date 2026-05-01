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
 * API Configurator header file is using for detecting available features and libraries
 */
#pragma once

#include <QVector3D>

#include "QINode3D.h"
#include "QNodeMacros.h"
#include <Qt/QEngineDef.h>

namespace DGE
{
    namespace QtWrapper
    {
        namespace QMLScene
        {
            class QComponent3D;
            class QScene3D;
            class DGE_QtAPI QNode3D
                : public QINode3D
                , public QQmlParserStatus
            {
                D_OBJECT(QNode3D)
                D_MapNode(QNode3D, Scene::Node);

                Q_OBJECT
                Q_INTERFACES(QQmlParserStatus)
                Q_CLASSINFO("DefaultProperty", "childrenNode")
                QML_ELEMENT

                D_QT_PROPERTY(Scene::NodeRef, nodeRef)
                D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF(QVector3D, localPosition, LocalPosition, nodeRef, convertVector3D)
                D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF(
                    QVector3D, localRotation, LocalRotation, nodeRef, convertVector3D)
                D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF(QVector3D, localScale, LocalScale, nodeRef, convertVector3D)

                Q_PROPERTY(QQmlListProperty<DGE::QtWrapper::QMLScene::QNode3D> childrenNode READ
                               childrenNode)

                Q_PROPERTY(QQmlListProperty<DGE::QtWrapper::QMLScene::QComponent3D> componentList
                               READ componentList)

                Q_PROPERTY(DGE::QtWrapper::QMLScene::QScene3D *sceneRoot READ sceneRoot WRITE
                               setSceneRoot NOTIFY sceneRootChanged FINAL)
                friend class DGE::QtWrapper::QMLScene::QScene3D;

            public:
                explicit QNode3D(QINode3D *parent = nullptr);

                QQmlListProperty<QNode3D> childrenNode();
                void					  appendChildNode(QNode3D *p);
                int						  childrenNodeCount() const;
                QNode3D					 *childNode(int index) const;
                void					  clearChildrenNode();

                QQmlListProperty<QComponent3D> componentList();
                void						   appendComponent(QComponent3D *p);
                int							   componentListCount() const;
                QComponent3D				  *component(int index) const;
                void						   clearComponentList();

                virtual void setNodeRef(Scene::NodeRef nodeRef) override;

                virtual DGE::QtWrapper::QMLScene::QScene3D *sceneRoot() const override;

                virtual void classBegin() override;
                virtual void componentComplete() override;

            public slots:
                virtual void onChildAdded(QINode3D *) override{};
                virtual void onComponentAdded(QINode3D *) override{};

                uint64 totalNumberVertices();

                void setSceneRoot(DGE::QtWrapper::QMLScene::QScene3D *newSceneRoot) override;

            signals:
                void childrenNodeChanged();

                void componentListChanged();

                void sceneRootChanged(DGE::QtWrapper::QMLScene::QScene3D *sceneRoot);

            protected:
                /// \brief Register of specified type of Node in the scene system automatically
                template<typename QNodeType, typename NodeType>
                void registerNode(QObject *parentNode);

                virtual void init();
                glm::vec3	 convertVector3D(QVector3D inputValue) const;
                QVector3D	 convertVector3D(glm::vec3 inputValue) const;

            private:
                static void appendChildNode(QQmlListProperty<QNode3D> *list, QNode3D *p);
                static void clearChildrenNode(QQmlListProperty<QNode3D> *list);
                // static void replaceChildNode(QQmlListProperty<QNode3D> *list, int i, QNode3D *p);
                // static void removeLastChildNode(QQmlListProperty<QNode3D> *list);
                static QNode3D *childNode(QQmlListProperty<QNode3D> *list, int i);
                static int		childrenNodeCount(QQmlListProperty<QNode3D> *list);

                static void appendComponent(QQmlListProperty<QComponent3D> *list, QComponent3D *p);
                static void clearComponentList(QQmlListProperty<QComponent3D> *list);
                // static void replaceComponent(QQmlListProperty<QComponent3D> *list, int i, QNode3D
                // *p); static void removeLastComponent(QQmlListProperty<QComponent3D> *list);
                static QComponent3D *component(QQmlListProperty<QComponent3D> *list, int i);
                static int			 componentListCount(QQmlListProperty<QComponent3D> *list);

            protected:
                DGE::QtWrapper::QMLScene::QScene3D *m_sceneRoot;
                QList<QNode3D *>					m_childrenNodes;
                QList<QComponent3D *>				m_componentsList;
				/// \brief Indicate whether this class is initialized internally  .
				std::atomic_bool m_isInternallyInititialized;
			};
        } // namespace QMLScene
    }	  // namespace QtWrapper
} // namespace DGE
