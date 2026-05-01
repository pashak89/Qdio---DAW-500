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

#include <Core/Utility/Interfaces/IRelease.h>
#include <Scene/Component.h>
#include <Scene/Node.h>
#include <mutex>

namespace DGE
{
	namespace Scene
	{
		class SceneModule;
		/// \brief Header node for scene rendering concept
		class DGE_SceneAPI SceneNode
			: public Node
			, public Core::Utility::Interfaces::IRelease
			, public Core::Utility::Singleton<Scene::SceneNode>
		{
			D_OBJECT_SINGLETON(SceneNode)
			friend class Scene::Node;
			friend class Scene::SceneModule;

		protected:
			/// TODO: Must remove default value for renderer parameter.
			SceneNode();
			SceneNode(std::shared_ptr<System::Context> context);

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

		public:
			D_Destructor(SceneNode);

			// IRelease interface
			virtual bool release() override;

			/// \brief Find the Node by ID
			///
			/// \param id It's id of the Node that want to find
			///
			/// \return Return shared pointer to Node object with specified id
			template<typename NodeType>
			std::shared_ptr<NodeType> getNode(uint64 id)
			{
				if (id == 0)
					return std::dynamic_pointer_cast<NodeType>(SceneNode::getInstance());
				if (id < Scene::SceneNode::s_firstSceneID && id > Scene::SceneNode::s_lastSceneID)
					return nullptr;
				auto it = m_localNodes.find(id);
				if (it != m_localNodes.end())
				{
					return std::dynamic_pointer_cast<NodeType>(it->second);
				}
				return nullptr;
			}

			/// \brief Find the Component by ID
			///
			/// \param id It's id of the Component that want to find
			///
			/// \return Return shared pointer to Component object with specified id
			template<typename ComponentType>
			std::shared_ptr<ComponentType> getComponent(uint64 id)
			{
				auto it = m_localComponents.find(id);
				if (it != m_localComponents.end())
					return std::dynamic_pointer_cast<ComponentType>(it->second);
				return nullptr;
			}

			Core::System::Event<bool, NodeRef> &nodeAddedToScene() const;

			Core::System::Event<bool, NodeRef> &nodeRemovedFromScene() const;

			Core::System::Event<bool, DGE::Scene::ComponentRef> &componentAddedToScene() const;

			Core::System::Event<bool, DGE::Scene::ComponentRef> &componentRemovedFromScene()
				const;

			bool isOptimizedUpdate() const;
			void setIsOptimizedUpdate(bool flag);

			/// \brief Retrieve total children in this scene
			uint64 totalChildren();

		protected:
			/// \brief Get last free id
			///
			/// \return Return last free id
			uint64 getFreeNodeID();

			/// \brief Get last free id
			///
			/// \return Return last free id
			uint64 getFreeComponentID();

			template<typename NodeType>
			void nodeAdded(std::shared_ptr<NodeType> node);

			template<typename NodeType>
			void nodeRemoved(std::shared_ptr<NodeType> node);

			template<typename ComponentType>
			void componentAdded(std::shared_ptr<ComponentType> component);

			template<typename ComponentType>
			void componentRemoved(std::shared_ptr<ComponentType> component);

		public:
			/// \brief beginning of ID number range
			static const uint64 s_firstSceneID;

			/// \brief end of ID number range
			static const uint64 s_lastSceneID;

		protected:
			/// \brief Next free node ID
			std::atomic_uint64_t m_localNodeID;

			/// \brief Next free component ID
			std::atomic_uint64_t m_localComponentID;

			/// \brief Map of Nodes of the Scene concept that use for accessing to the node by ID of the Node
			std::map<uint64, Scene::NodeRef> m_localNodes;

			/// \brief Store free ID of Node that is released by removing owner Node.
			std::set<uint64> m_freeLocalNodeID;

			/// \brief Map of Components of the Scene concept that use for accessing to the
			/// components by ID of the Component
			std::map<uint64, DGE::Scene::ComponentRef> m_localComponents;

			/// \brief Store free ID of Component that is released by removing owner Component.
			std::set<uint64> m_freeLocalComponentsID;

			/// \brief Indicate whether scene optimize updating nodes.
			std::atomic_bool m_isOptimizedUpdate;

			/// \brief A mutex for this scene that is locked modifying this scene when we need to
			/// change into the scene
			std::mutex m_lockSceneMutex;

			/// TODO: Move all of this events to Node class and Component class for every node and component

			/// \brief It's a event that emit when node added to the SceneNode
			mutable Core::System::Event<bool, DGE::Scene::NodeRef> m_nodeAddedToScene;

			/// \brief It's a event that emit when node removed from the SceneNode
			mutable Core::System::Event<bool, DGE::Scene::NodeRef> m_nodeRemovedFromScene;

			/// \brief It's a event that emit when component added to the SceneNode
			mutable Core::System::Event<bool, DGE::Scene::ComponentRef>
				m_componentAddedToScene;

			/// \brief It's a event that emit when component removed from the SceneNode
			mutable Core::System::Event<bool, DGE::Scene::ComponentRef>
				m_componentRemovedFromScene;
		};
		using SceneNodeRef = std::shared_ptr<DGE::Scene::SceneNode>;
		using SceneNodeWRef = std::weak_ptr<DGE::Scene::SceneNode>;

		template<typename NodeType>
		void SceneNode::nodeAdded(std::shared_ptr<NodeType> node)
		{
			if (!node || node->scene().lock().get() == this)
				return;
			auto oldScene = node->scene().lock();
			if (oldScene)
				oldScene->nodeRemoved(node);

			node->setScene(getInstance());
			auto id = node->id();
			if (id == 0)
			{
				id = getFreeNodeID();
				node->setId(id);
			}

			{
				std::scoped_lock<mutex> scopeLock(m_lockSceneMutex);
				if (m_localNodes.contains(id))
				{
					auto it		 = m_localNodes.find(id);
					auto oldNode = std::dynamic_pointer_cast<NodeType>(it->second);

					if (oldNode != node)
					{
						LogWarning("Duplicate id occurred.");
						nodeRemoved(oldNode);
					}
				}
				m_localNodes[id] = node;
			}

			auto components = node->components();
			for (auto [order, componentsList] : components)
			{
				for (auto [componentType, component] : componentsList)
				{
					componentAdded(component);
				}
			}
			auto children = node->children();
			for (auto [order, childsList] : children)
			{
				for (auto child : childsList)
				{
					nodeAdded(child);
				}
			}

			// m_nodeAddedToScene(node);
		}

		template<typename NodeType>
		void SceneNode::nodeRemoved(std::shared_ptr<NodeType> node)
		{
			if (!node || node->scene().lock().get() != this)
				return;
			auto id = node->id();
			{
				std::scoped_lock<mutex> scopeLock(m_lockSceneMutex);
				m_localNodes.erase(id);
			}
			node->resetScene();

			auto components = node->components();
			for (auto [order, componentsList] : components)
			{
				for (auto [componentType, component] : componentsList)
				{
					componentRemoved(component);
					node->m_uninitializedComponents.erase(component);
				}
			}
			auto children = node->children();
			for (auto [order, childsList] : children)
			{
				for (auto child : childsList)
				{
					nodeRemoved(child);
					node->m_uninitializedChildren.erase(child);
				}
			}
			m_uninitializedChildren.erase(node);
			// m_nodeRemovedFromScene(node);
		}

		template<typename ComponentType>
		void SceneNode::componentAdded(std::shared_ptr<ComponentType> component)
		{
			if (!component || component->scene().lock().get() == this)
				return;
			auto oldScene = component->scene().lock();
			if (oldScene)
				oldScene->componentRemoved(component);

			auto id = component->id();
			if (id == 0)
			{
				id = getFreeComponentID();
				component->setId(id);
			}

			{
				std::scoped_lock<mutex> scopeLock(m_lockSceneMutex);
				if (m_localComponents.contains(id))
				{
					auto it			  = m_localComponents.find(id);
					auto oldComponent = std::dynamic_pointer_cast<ComponentType>(it->second);

					if (oldComponent != component)
					{
						LogWarning("Duplicate id occurred.");
						componentRemoved(oldComponent);
					}
				}
				m_localComponents[id] = component;
			}
			// m_componentAddedToScene(component);
		}

		template<typename ComponentType>
		void SceneNode::componentRemoved(std::shared_ptr<ComponentType> component)
		{
			if (!component || component->scene().lock().get() != this)
				return;
			auto id = component->id();
			{
				std::scoped_lock<mutex> scopeLock(m_lockSceneMutex);
				m_localComponents.erase(id);
			}
			component->resetNode();
			m_uninitializedComponents.erase(component);
			// m_componentRemovedFromScene(component);
		}
	} // namespace Scene
} // namespace DGE
