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
#include <Core/MemoryManager/UndefMemoryMacros.h>

#include <QtCore/QObject>
#include <Qt/QMLScene/QNode3D.h>

#include <Core/MemoryManager/RedefMemoryMacros.h>

#include <GFX/GCore/Drawable.h>
#include <Scene/SceneNode.h>

Q_DECLARE_METATYPE(DGE::Scene::NodeRef);

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			/// \brief Scene class is used for create scene root node for the ViewPort3D class
			class DGE_QtAPI QScene3D
				: public QMLScene::QNode3D
				, public Singleton<QScene3D, std::shared_ptr<QScene3D>,
								   Deleter<QScene3D>::emptyDeleter>
			{
				D_OBJECT_SINGLETON(QScene3D, std::shared_ptr<QScene3D>,
								   Deleter<QScene3D>::emptyDeleter)
				Q_OBJECT
				QML_ELEMENT

				// Q_PROPERTY(type name READ name WRITE setName NOTIFY nameChanged)
				explicit QScene3D(QINode3D* parent = nullptr);

				bool registerSceneObject(uint64 id, QINode3D* qScene3DObject,
										 std::shared_ptr<Scene::Node> sceneObject = nullptr);

				bool registerSceneComponent(QObject* qScene3DObject,
											std::shared_ptr<Scene::Component> sceneObject = nullptr);

				template<class Node3DType, class... Args>
				std::shared_ptr<Scene::Node> node3DBinder(std::shared_ptr<Scene::Node> nodeRef,
														  Args... args);

				template<class NodeType, class... Args>
				QINode3D* nodeBinder(Args... args);

				template<class Component3DType, class... Args>
				std::shared_ptr<Scene::Component> component3DBinder(
					std::shared_ptr<Scene::Node> nodeRef, Args... args);

				template<class ComponentType, class... Args>
				QObject* componentBinder(Args... args);

				friend class QMLScene::QNode3D;

			public:
				template<class Node3DType, class NodeType, class... Args>
				QINode3D* createNode3D(QINode3D* parent, Args... args);

				template<class Node3DType, class NodeType, class... Args>
				std::shared_ptr<NodeType> attachNode(QINode3D* parent, QINode3D* self, Args... args);

				template<class QComponent3DType, class ComponentType, class... Args>
				QComponent3DType* createComponent3D(QINode3D* parent, Args... args);

				template<class QComponent3DType, class ComponentType, class... Args>
				std::shared_ptr<ComponentType> attachComponent(QINode3D*		 parent,
															   QComponent3DType* self,
															   Args... args);

				QINode3D*					 toQNode3D(uint64 id);
				std::shared_ptr<Scene::Node> toNode(uint64 id);

				QObject* toQComponent(std::shared_ptr<Scene::Component> object);
				std::shared_ptr<Scene::Component> toComponent(QComponent3D* qObject);

			public slots:
				bool onNodeAdded(DGE::Scene::NodeRef node);
				bool onNodeRemoved(DGE::Scene::NodeRef node);

			signals:
				void nodeAdded(DGE::Scene::NodeRef);
				void nodeRemoved(DGE::Scene::NodeRef);
				void nodesListUpdated();

			protected:
				// Scene::SceneNodeRef m_sceneObject;
				/// \brief Map Scene into QScene and vice versa.
				std::map<uint64, std::pair<QINode3D*, std::shared_ptr<Scene::Node>>>
																		  m_sceneNodeMapping;
				std::map<std::shared_ptr<Scene::Component>, QObject*>	  m_sceneComponentMapping;
			};

			using QScene3DRef = std::shared_ptr<QScene3D>;

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
