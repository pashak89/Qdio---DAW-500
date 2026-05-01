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

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/IO/LogSystem/Log.h>
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <Core/System/DObject.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <Scene/Component.h>
#include <Scene/Node.h>
#include <Scene/SceneNode.h>
#include <memory>

namespace DGE
{
	namespace Scene
	{
		template<typename NodeType>
		void Node::addChild(std::shared_ptr<NodeType> node)
		{
			if (!node || node.get() == this || node->m_parent.lock().get() == this)
				return;

			if (isChildOf(node))
				return;
			Scene::SceneNodeRef scene;
			if (!m_scene.expired())
				scene = m_scene.lock();
			auto oldParent = node->m_parent.lock();
			if (oldParent)
			{
				if (oldParent->scene().lock() != scene)
					oldParent->removeChild(node);
				else
				{
					if (scene)
					{
						scene->nodeRemovedFromScene()(dynamic_pointer_cast<Scene::Node>(node));
					}

					uint8 order	   = -1;
					auto  typeInfo = node->typeInfo();
					if (oldParent->m_orderList.contains(typeInfo.typeInfoRef))
						order = oldParent->m_orderList[typeInfo.typeInfoRef];
					{
						std::scoped_lock<std::mutex> lock(oldParent->m_lockNodeMutex);
						oldParent->m_children[order].erase(node);
					}
				}
			}

			uint8 order	   = -1;
			auto  typeInfo = node->typeInfo();
			{
				std::scoped_lock<std::mutex> lock(m_lockNodeMutex);
				if (m_orderList.contains(typeInfo.typeInfoRef))
					order = m_orderList[typeInfo.typeInfoRef];
				m_children[order].insert(node);

				if (scene && node->scene().lock() != scene)
					scene->nodeAdded(node);

				if (scene)
					node->setParent(scene->getNode<Scene::Node>(m_id));
				if (!node->isInitialized())
					m_uninitializedChildren.insert(node);
			}
			if (scene)
			{
				scene->nodeAddedToScene()(dynamic_pointer_cast<Scene::Node>(node));
			}
		}

		template<typename NodeType>
		std::shared_ptr<NodeType> Node::getChild(uint64 id)
		{
			if (m_scene.expired())
				return nullptr;
			auto it = m_scene.lock()->m_localNodes.find(id);
			if (it != m_scene.lock()->m_localNodes.end())
				return std::dynamic_pointer_cast<NodeType>(it->second);
			return nullptr;
		}

		template<typename NodeType>
		std::shared_ptr<NodeType> Node::getChild(const DString nameNode)
		{
			for (const auto& [order, childsList] : m_children)
			{
				for (const auto& child : childsList)
				{
					if (nameNode.toStdString() == child->name().toStdString())
						return child;
				}
			}
			return nullptr;
		}

		template<typename NodeType>
		std::vector<std::shared_ptr<NodeType>> Node::getChildren() const
		{
			std::vector<std::shared_ptr<NodeType>> children;
			for (const auto& [order, childsList] : m_children)
			{
				for (const auto& child : childsList)
				{
					if (typeid(NodeType) == (*child->typeInfo().typeInfoRef))
						children.push_back(std::dynamic_pointer_cast<NodeType>(child));
				}
			}

			return children;
		}

		template<typename NodeType>
		std::vector<std::shared_ptr<NodeType>> Node::getVisibleChildren() const
		{
			std::vector<std::shared_ptr<NodeType>> children;
			for (const auto& [order, childsList] : m_children)
			{
				for (const auto& child : childsList)
				{
					if (typeid(NodeType) == (*child->typeInfo().typeInfoRef) && child->enable())
						children.push_back(std::dynamic_pointer_cast<NodeType>(child));
				}
			}

			return children;
		}

		template<typename NodeType>
		void Node::removeChild(uint8 order, std::shared_ptr<NodeType> node)
		{
			if (!node)
				return;
			if (!m_scene.expired())
			{
				m_scene.lock()->nodeRemovedFromScene()(dynamic_pointer_cast<Scene::Node>(node));
			}

			node->m_parent.reset();
			{
				std::scoped_lock<std::mutex> lock(m_lockNodeMutex);
				if (!m_scene.expired())
					m_scene.lock()->nodeRemoved(node);
				m_children[order].erase(node);
				if (m_children[order].empty())
					m_children.erase(order);
			}
		}

		template<typename NodeType>
		void Node::removeChild(std::shared_ptr<NodeType> node)
		{
			if (!node)
				return;
			uint8 order	   = -1;
			auto  typeInfo = node->typeInfo();
			if (m_orderList.contains(typeInfo.typeInfoRef))
				order = m_orderList[typeInfo.typeInfoRef];
			removeChild(order, node);
		}

		template<typename NodeType, typename... Args>
		std::shared_ptr<NodeType> Node::createChild(uint64 id, Args... args)
		{
			auto child = std::make_shared<NodeType>(m_context, args...);
			if (!m_scene.expired())
			{
				if (id == 0 || m_scene.lock()->getNode<Node>(id))
					id = m_scene.lock()->getFreeNodeID();
			}
			child->setId(id);

			addChild(child);
			return child;
		}

		template<typename NodeType, typename... Args>
		std::shared_ptr<NodeType> Node::createChild(const DString name, uint64 id, Args... args)
		{
			auto child = createChild<NodeType>(id, args...);
			child->setName(name);

			return child;
		}

		template<typename NodeType, typename... Args>
		std::shared_ptr<NodeType> Node::createChild(const DString name, Args... args)
		{
			auto child = createChild<NodeType>(0, args...);
			child->setName(name);

			return child;
		}

		template<typename NodeType>
		bool Node::isChildOf(std::shared_ptr<NodeType> node)
		{
			auto parent = m_parent.lock();
			while (parent)
			{
				if (parent == node)
					return true;
				parent = parent->m_parent.lock();
			}
			return false;
		}

		template<typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> Node::createComponent(const DString name, uint64 id,
															 Args... args)
		{
			auto component = createComponent<ComponentType>(id, args...);
			component->setName(name);

			return component;
		}

		template<typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> Node::createComponent(const DString name, Args... args)
		{
			auto component = createComponent<ComponentType>(0, args...);
			component->setName(name);

			return component;
		}

		template<typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> Node::createComponent(uint64 id, Args... args)
		{
			auto component = std::make_shared<ComponentType>(m_context, args...);
			component->setId(id);
			addComponent(component);
			return component;
		}

		template<typename ComponentType, typename... Args>
		std::shared_ptr<ComponentType> Node::createComponent(Args... args)
		{
			auto component = std::make_shared<ComponentType>(m_context, args...);
			addComponent(component);
			return component;
		}

		template<typename ComponentType>
		void Node::addComponent(std::shared_ptr<ComponentType> component)
		{
			if (!component || component->m_node.lock().get() == this)
				return;

			Scene::SceneNodeRef scene;
			if (!m_scene.expired())
				scene = m_scene.lock();

			auto oldParent = component->m_node.lock();
			if (oldParent)
			{
				if (oldParent->scene().lock() != scene)
					oldParent->removeComponent(component);
				else
				{
					if (scene)
					{
						scene->componentRemovedFromScene()(
							dynamic_pointer_cast<Scene::Component>(component));
					}
					uint8 order	   = -1;
					auto  typeInfo = component->typeInfo();
					if (oldParent->m_orderList.contains(typeInfo.typeInfoRef))
						order = oldParent->m_orderList[typeInfo.typeInfoRef];
					{
						std::scoped_lock<std::mutex> lock(oldParent->m_lockNodeMutex);
						oldParent->m_components[order].erase(typeInfo.typeInfoRef);
					}
				}
			}

			uint8 order	   = -1;
			auto  typeInfo = component->typeInfo();
			{
				std::scoped_lock<std::mutex> lock(m_lockNodeMutex);
				if (m_orderList.contains(typeInfo.typeInfoRef))
					order = m_orderList[typeInfo.typeInfoRef];
				m_components[order].insert({typeInfo.typeInfoRef, component});

				if (!component->isInitialized())
					m_uninitializedComponents.insert(component);
			}

			if (scene && component->scene().lock() != scene)
				scene->componentAdded(component);

			if (scene)
				component->m_node = scene->getNode<Scene::Node>(m_id);

			if (scene)
			{
				scene->componentAddedToScene()(dynamic_pointer_cast<Scene::Component>(component));
			}
		}

		template<typename ComponentType>
		void Node::removeComponent(uint8 order, std::shared_ptr<ComponentType> component)
		{
			if (!component)
				return;
			if (!m_scene.expired())
			{
				m_scene.lock()->componentRemovedFromScene()(
					dynamic_pointer_cast<Scene::Component>(component));
			}

			{
				std::scoped_lock<std::mutex> lock(m_lockNodeMutex);
				if (!m_scene.expired())
					m_scene.lock()->componentRemoved(component);
				component->m_node.reset();
				auto typeInfo = component->typeInfo();
				m_components[order].erase(typeInfo.typeInfoRef);
			}
		}

		template<typename ComponentType>
		void Node::removeComponent(std::shared_ptr<ComponentType> component)
		{
			if (!component)
				return;
			uint8 order	   = -1;
			auto  typeInfo = component->typeInfo();
			if (m_orderList.contains(typeInfo.typeInfoRef))
				order = m_orderList[typeInfo.typeInfoRef];
			removeComponent(order, component);
		}

		template<typename ComponentType>
		bool Node::hasComponent(std::shared_ptr<ComponentType> component)
		{
			if (!component)
				return false;
			uint8 order	   = -1;
			auto  typeInfo = component->typeInfo();
			if (m_orderList.contains(typeInfo.typeInfoRef))
				order = m_orderList[typeInfo.typeInfoRef];
			return m_components[order].contains(typeInfo.typeInfoRef);
		}

		template<typename ComponentType>
		bool Node::hasComponentType()
		{
			uint8 order	   = -1;
			auto  typeInfo = ComponentType::staticTypeInfo();
			if (m_orderList.contains(typeInfo.typeInfoRef))
				order = m_orderList[typeInfo.typeInfoRef];
			return m_components[order].contains(typeInfo.typeInfoRef);
		}

		template<typename ComponentType>
		std::shared_ptr<ComponentType> Node::getComponent()
		{
			uint8 order	   = -1;
			auto  typeInfo = ComponentType::staticTypeInfo();
			if (m_orderList.contains(typeInfo.typeInfoRef))
				order = m_orderList[typeInfo.typeInfoRef];
			auto& componentList = m_components[order];
			auto  component		= componentList.find(typeInfo.typeInfoRef);
			if (component != componentList.end())
				return std::dynamic_pointer_cast<ComponentType>(component->second);
			return nullptr;
		}

	} // namespace Scene
} // namespace DGE
