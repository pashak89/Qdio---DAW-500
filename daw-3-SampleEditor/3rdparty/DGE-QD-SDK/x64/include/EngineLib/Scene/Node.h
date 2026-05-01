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
#include <Core/Math/Transformation.h>
#include <Core/Math/Vector2D.h>
#include <Core/Math/Vector3D.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <Core/System/DProperty.h>
#include <Core/System/Tags.h>
#include <GFX/GCore/RenderState.h>

#include <GFX/GCore/Drawable.h>
#include <Scene/SceneModuleDef.h>

namespace DGE
{
	namespace Scene
	{
		class Component;
		class SceneNode;

		/// TODO: add isInitialized flag into every node that we can add a node in middle of running
		/// app.
		/// TODO: Simplify Node with change parent from Drawable class into DObject

		/// \brief Base class for scene rendering concept and It has basic functionality for every
		/// node that inherit from it
		class DGE_SceneAPI Node : public GFX::GCore::Drawable
		{
			D_OBJECT(Node)
			friend class Scene::SceneNode;

		public:
			/// \brief Default constructor that get context and for real initializing must call
			/// Component::initialize(std::map<String, std::any>& parameters)
			Node(DGE::Core::System::ContextRef context);

			D_Destructor(Node);

			/// IInitialize interface
			/// \brief Use default implementation of
			/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
			/// function from IInitialize interface.
			using Core::Utility::Interfaces::IInitialize::initialize;

			/// \brief Initialize Node and if it is initialized successfully return true
			///
			/// \return Return true if initializing is successfully else return false
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			/// \brief before of starting render pipeline call this function
			virtual bool update(GFX::GCore::InputParameters& input) override;

			/// \brief Before main rendering, this function is called
			virtual bool preProcessRender(GFX::GCore::InputParameters& input) override;

			// IRender interface
			/// \brief Main rendering function do at this function
			virtual bool render(GFX::GCore::InputParameters& input) override;

			/// \brief After main rendering, this function is called
			virtual bool postProcessRender(GFX::GCore::InputParameters& input) override;

			/// \brief Create Node child with specified name and id and return shared pointer of it
			///
			/// \param name Name of created node
			/// \param id The ID of created node
			/// \param args The argument for creating node type
			///
			/// \return Return shared pointer of created child
			template<typename NodeType = DGE::Scene::Node, typename... Args>
			std::shared_ptr<NodeType> createChild(const DString name, uint64 id, Args... args);

			/// \brief Create Node child with specified name and id and return shared pointer of it
			///
			/// \param name Name of created node
			/// \param args The argument for creating node type
			///
			/// \return Return shared pointer of created child
			template<typename NodeType = DGE::Scene::Node, typename... Args>
			std::shared_ptr<NodeType> createChild(const DString name, Args... args);

			/// \brief Create Node child with specified id and return shared pointer of it
			///
			/// \param id The ID of created node
			/// \param args The argument for creating node type
			///
			/// \return Return shared pointer of created child
			template<typename NodeType = DGE::Scene::Node, typename... Args>
			std::shared_ptr<NodeType> createChild(uint64 id = 0, Args... args);

			/// \brief Add Node to list of children of this node
			///
			/// \param node
			template<typename NodeType = DGE::Scene::Node>
			void addChild(std::shared_ptr<NodeType> node);

			template<typename NodeType = DGE::Scene::Node>
			std::shared_ptr<NodeType> getChild(uint64 id);

			template<typename NodeType = DGE::Scene::Node>
			std::shared_ptr<NodeType> getChild(const DString name);

			/// \brief Return list of children with type of NodeType that is specified
			template<typename NodeType = DGE::Scene::Node>
			std::vector<std::shared_ptr<NodeType>> getChildren() const;

			/// \brief Return list of children with type of NodeType that is specified and they are visible
			template<typename NodeType = DGE::Scene::Node>
			std::vector<std::shared_ptr<NodeType>> getVisibleChildren() const;

			template<typename NodeType = DGE::Scene::Node>
			void removeChild(uint8 order, std::shared_ptr<NodeType> node);

			template<typename NodeType = DGE::Scene::Node>
			void removeChild(std::shared_ptr<NodeType> node);

			void removeChildren();

			/// \brief Check whether this node is direct child or indirect child of specified node.
			///
			/// \return Return true if this node is direct child or indirect child of specified node
			/// else return false
			template<typename NodeType = DGE::Scene::Node>
			bool isChildOf(std::shared_ptr<NodeType> node);

			/// \brief Create component child with specified name and id and return shared pointer of it
			///
			/// \param name Name of created component
			/// \param id The ID of created component
			/// \param args The arguments of constructor component
			///
			/// \return Return shared pointer of created component
			template<typename ComponentType = DGE::Scene::Component, typename... Args>
			std::shared_ptr<ComponentType> createComponent(const DString name,
														   uint64		id = 0,
														   Args... args);

			/// \brief Create component child with specified name and id and return shared pointer of it
			///
			/// \param name Name of created component
			/// \param id The ID of created component
			/// \param args The arguments of constructor component
			///
			/// \return Return shared pointer of created component
			template<typename ComponentType = DGE::Scene::Component, typename... Args>
			std::shared_ptr<ComponentType> createComponent(const DString name, Args... args);

			/// \brief Create a component for this node and assign it to this node
			///
			/// \param id The ID of created component
			/// \param args The arguments of constructor component
			///
			/// \return Return shared pointer of created component
			template<typename ComponentType = DGE::Scene::Component, typename... Args>
			std::shared_ptr<ComponentType> createComponent(uint64 id, Args... args);

			/// \brief Create a component for this node and assign it to this node
			///
			/// \param args The arguments of constructor component
			///
			/// \return Return shared pointer of created component
			template<typename ComponentType = DGE::Scene::Component, typename... Args>
			std::shared_ptr<ComponentType> createComponent(Args... args);

			/// \brief Add Component to list of component of this node
			///
			/// \param component
			///
			/// \return Return shared pointer of created component
			template<typename ComponentType = DGE::Scene::Component>
			void addComponent(std::shared_ptr<ComponentType> component);

			/// \brief Remove Component from list of component of this node
			template<typename ComponentType = DGE::Scene::Component>
			void removeComponent(uint8 order, std::shared_ptr<ComponentType> component);

			/// \brief Remove Component from list of component of this node
			template<typename ComponentType = DGE::Scene::Component>
			void removeComponent(std::shared_ptr<ComponentType> component);

			/// \brief Determinate whether this node has this component in list of components
			template<typename ComponentType = DGE::Scene::Component>
			bool hasComponent(std::shared_ptr<ComponentType> component);

			/// \brief Determinate whether this node has this type of component in list of components.
			template<typename ComponentType = DGE::Scene::Component>
			bool hasComponentType();

			/// \brief Return a component with type of ComponentType that is specified
			template<typename ComponentType = DGE::Scene::Component>
			std::shared_ptr<ComponentType> getComponent();

			/// \brief Add order of specific type to use in adding Component and Node
			///
			/// \param typeInfo Reference to type information of this Node or Component
			/// \param orderNumber number of order for this typeInfo
			void addOrderType(System::DTypeInfo typeInfo, uint8 orderNumber);

			/// \brief Add specified order number into set of order numbers to process it at end of
			/// processing this node
			void addFinazlizeOrderNumber(uint8 orderNumber);

			/// \brief Get parent of this node
			const std::weak_ptr<Scene::Node>& parent() const;

			/// \brief Get scene node of the scene
			const std::weak_ptr<Scene::SceneNode>& scene() const;

			/// \brief Get children of this node
			const std::map<uint8, std::unordered_set<std::shared_ptr<Scene::Node>>>& children() const;

			/// \brief Get components of this node
			const std::map<uint8, std::unordered_multimap<TypeInfoRef, std::shared_ptr<Component>>>&
				components() const;

			/// \brief Get scene node of the scene
			void setScene(const std::shared_ptr<Scene::SceneNode>& newScene);

			void resetScene();

			/// \brief Get parent of this node
			void setParent(const std::shared_ptr<Scene::Node>& newParent);

			/// \brief Get ID of this node
			uint64 id() const;

			/// \brief Set ID of this node
			void setId(uint64 newId);

			/// \brief Get name of this Node
			const DString name() const;
			/// \brief Set name of this Node
			void setName(const DString& newName);

			/// \brief Assign a tag to this node
			///
			/// \param tagName Name of tag for assigning into this node
			///
			/// \return Return true if name of tag is assigned to this node successfully else
			/// return false.
			bool assignTag(DString tagName);

			/// \brief Determine is assign a tag to this node or not
			///
			/// \param tagName Name of tag for checking existence the tag name in this node
			///
			/// \return Return true if name of tag is found in this node else return false.
			bool isAssignedTag(DString tagName);

			/// \brief Remove a tag from this node
			///
			/// \param tagName Name of tag for removing into this node
			///
			/// \return Return true if removing the tag name from this node is successful else
			/// return false.
			bool removeTag(DString tagName);

			/// \brief Get set of all tags that is assigned to this node
			///
			/// \return Return set of all tags that is assigned to this node
			const Core::System::Tags& tags();

			/// \brief Set list of all tags that is assigned to this node
			///
			/// \param newTagList List of tag that is assigned to this node
			void setTags(const std::set<DString>& newTagList);

			/// \brief Retrieve the total number of rendered vertices in latest frame for this node
			virtual uint64 totalNumberVertices() override;

			/// \brief Retrieve transformation object
			const Core::Math::Transformation& transformation() const;

			/// \brief Get localPosition
			const glm::vec3& localPosition() const;
			/// \brief Set position
			void setLocalPosition(const glm::vec3& newPosition);
			/// \brief Set position
			void setLocalPosition(float x, float y, float z);

			/// \brief Get localRotation
			const glm::vec3& localRotation() const;
			/// \brief Set rotation
			void setLocalRotation(const glm::vec3& newRotation);
			/// \brief Set rotation
			void setLocalRotation(float x, float y, float z);

			/// \brief Get localScale
			const glm::vec3& localScale() const;
			/// \brief Set scale
			void setLocalScale(const glm::vec3& newScale);
			/// \brief Set scale
			void setLocalScale(float x, float y, float z);

			/// \brief Get position of this node in the whole world.
			const glm::vec3 worldPosition() const;

			/// \brief Get pivot of this entity
			Core::Math::Vector3D pivot();
			/// \brief Set pivot of this entity
			void setPivot(Core::Math::Vector3D pivot);

			/// \brief Attach node object to this node and apply new transformation of this node.
			void attach(std::shared_ptr<Scene::Node> node);

			/// \brief Deattach node object from this node.
			void deattach(std::shared_ptr<Scene::Node> node);

			/// \brief Schedules a Node::update event to be delivered to this node object. Must be
			/// called to update this node.
			void requestToUpdate();

			/// Indicates whether this object needs to be updated
			bool isUpdateNeeded();

			/// \brief Apply move vector in next update of this entity
			void move(const Math::Vector3D& movement);

			/// \brief Emitted when world position has been changed
			Core::System::Event<void, glm::vec3>& positionChanged() const;

			/// \brief Emitted when world rotation has been changed
			Core::System::Event<void, glm::vec3>& rotationChanged() const;

			/// \brief Emitted when world scale has been changed
			Core::System::Event<void, glm::vec3>& scaleChanged() const;

			/// \brief Emitted when world position has been changed
			Core::System::Event<void, glm::vec3>& moved() const;

			/// \brief Emitted when local position has been changed
			Core::System::Event<void, glm::vec3>& localPositionChanged() const;

			/// \brief Emitted when local rotation has been changed
			Core::System::Event<void, glm::vec3>& localRotationChanged() const;

			/// \brief Emitted when local scale has been changed
			Core::System::Event<void, glm::vec3>& localScaleChanged() const;

			/// \brief Emitted when local position has been changed
			Core::System::Event<void, glm::vec3>& locallyMoved() const;

			Core::System::Event<void>& transformationChanged() const;

		protected:
			/// \brief Check whether specified tags contains tags of this node or tags of this node
			/// is empty
			///
			/// \return Return true if specified tags contains tags of this node or tags of this
			/// node is empty else return false
			bool checkTag(const Core::System::Tags& tags);
			/// \brief Set children of this Node
			void setChildren(
				const std::map<uint8, std::unordered_set<std::shared_ptr<DGE::Scene::Node>>>&
					newChildren);

			/// \brief Set components of this Node
			void setComponents(
				const std::map<
					uint8,
					std::unordered_multimap<TypeInfoRef, std::shared_ptr<DGE::Scene::Component>>>&
					newComponents);

			/// \brief Initialize new child and new components
			bool initializeNewNodes(Core::Utility::Interfaces::InitializeParams& input);

			/// \brief Update all matrices of this node
			void updateMatrices();

		public:
			static const DString s_currentDrawableNode;
			static const DString s_currentDrawableComponent;

			bool hasRenderState() const;
			void setHasRenderState(bool newHasRenderState);

			GFX::GCore::RenderStateRef renderState() const;
			void setRenderState(const GFX::GCore::RenderStateRef& newRenderState);

			bool attachParentLocationOnly() const;
			void setAttachParentLocationOnly(const bool& newAttachParentLocationOnly);

		protected:
			/// \brief The name of this Node
			DString m_name;

			/// \brief The unique id for this Node
			uint64 m_id;

			/// \brief Total rendered vertex in the latest frame
			uint64 m_totalNumberVertices;

			/// \brief Set of tags for grouping items in the views and apply special policy on them
			Core::System::Tags m_tags;

			/// \brief Parent of this node
			std::weak_ptr<Scene::Node> m_parent;

			/// \brief Scene node of the scene
			std::weak_ptr<Scene::SceneNode> m_scene;

			/// \brief Children of the scene node
			std::map<uint8, std::unordered_set<std::shared_ptr<Scene::Node>>> m_children;

			/// \brief Uninitialized children of this scene node
			std::set<std::shared_ptr<Scene::Node>> m_uninitializedChildren;

			/// \brief Components of this Node
			std::map<uint8, std::unordered_multimap<TypeInfoRef, std::shared_ptr<Scene::Component>>>
				m_components;

			/// \brief Uninitialized components of this scene node
			std::set<std::shared_ptr<Scene::Component>> m_uninitializedComponents;

			/// \brief Store Order object type to add in children list or components list base on
			/// this list;
			std::map<TypeInfoRef, uint8> m_orderList;

			/// \brief Process this order numbers of components list at end of processing this node
			/// to finalize the process function
			std::set<uint8> m_finalizeOrderList;

			/// \brief A mutex for this node that is locked modifying this node when process the node
			std::mutex m_lockNodeMutex;

			/// \brief Shared pointer of render state object;
			GFX::GCore::RenderStateRef m_renderState;
			/// \brief Indicate whether this node has separated render state
			bool m_hasRenderState;

			/// \brief The transform information for this entity
			Core::Math::Transformation m_transformation;

			/// \brief The transform information for parent of this entity
			Core::Math::Transformation m_parentTransformation;

			/// \brief Attached node to this scene node
			std::set<std::shared_ptr<Scene::Node>> m_attachedNodes;

			/// \brief This boolean flag indicates that this node object requires updating
			std::atomic_bool m_needsToUpdate;

			/// \brief store movement of this entity for next frame
			std::atomic<float> m_movementX;
			std::atomic<float> m_movementY;
			std::atomic<float> m_movementZ;
			std::atomic_bool   m_isMovementChanged;

			/// \brief This flag indicate whether this node using location of parent only. if this
			/// flag set to true, this node only use of location of parent in updating it matrix.
			std::atomic<bool> m_attachParentLocationOnly;

			D_PROPERTY(Core::Math::Vector3D, position3D, Position3D, Core::Math::Vector3D(0, 0, 0))
		};
		D_DefRefType(Node);
	} // namespace Scene
} // namespace DGE
