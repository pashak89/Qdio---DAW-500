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
#include <Core/System/DObject.h>
#include <Core/System/Tags.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <GFX/GCore/Drawable.h>
#include <GFX/GCore/Renderer.h>
#include <Scene/SceneModuleDef.h>

namespace DGE
{
	namespace Scene
	{
		class SceneNode;
		class Node;
		/// \brief Base class for components that add it to the nodes to extend functionality
		class DGE_SceneAPI Component
			: public Core::System::DObject
			, public Core::Utility::Interfaces::IInitialize
			, public GFX::GCore::Interfaces::IUpdate
		{
			D_OBJECT(Component)
			friend class SceneNode;
			friend class Node;

		public:
			/// \brief Default constructor that get context and for real initializing must call
			/// Component::initialize(std::map<String, std::any>& parameters)
			Component(DGE::Core::System::ContextRef context);

			/// IInitialize interface
			/// \brief Use default implementation of
			/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
			/// function from IInitialize interface.
			using Core::Utility::Interfaces::IInitialize::initialize;

			/// \brief Initialize Component and if it is initialized successfully return true
			///
			/// \return Return true if initializing is successfully else return false
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			/// \brief before of starting render pipeline call this function
			virtual bool update(GFX::GCore::InputParameters& input) override;

			// /// \brief Before main rendering, this function is called
			// virtual bool preProcessRender(GFX::GCore::InputParameters& input) override;

			// // IRender interface
			// /// \brief Main rendering function do at this function
			// virtual bool render(GFX::GCore::InputParameters& input) override;

			// /// \brief After main rendering, this function is called
			// virtual bool postProcessRender(GFX::GCore::InputParameters& input) override;

			/// \brief Get shared pointer to SceneNode object that this component exists in it.
			///
			/// \return Return shared pointer to SceneNode object that this component exists in it.
			std::weak_ptr<SceneNode> scene();

			/// \brief Get shared pointer to Node object that this component exists in it.
			///
			/// \return Return shared pointer to Node object that this component exists in it.
			const std::weak_ptr<Node>& node() const;

			/// \brief Set parent Node of this Component that this component exists in it
			///
			/// \param newNode shared pointer to parent node
			void setNode(const std::weak_ptr<Node>& newNode);

			/// \brief Get ID of this component
			uint32 id() const;

			/// \brief Set ID of this component
			void setId(uint32 newId);

			/// \brief Indicate that this component and node of this component is enable at
			/// same-time or not.
			///
			/// \return Return true if this component and node of this component is enable at
			/// same-time else return false
			bool isRealEnable() const;

			/// \brief Get name of this Component
			const DString name() const;
			/// \brief Set name of this Component
			void setName(const DString& newName);

			/// \brief reset reference to parent node
			void resetNode();

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

			/// \brief retrieve enable property of this object
			bool enable() const;

			/// \brief set enable property of this object
			void setEnable(bool isEnable);

			/// \brief Retrieve the total number of rendered vertices in latest frame for this component
			virtual uint64 totalNumberVertices();

			/// \brief Schedules a Component::update event to be delivered to this component object.
			/// Must be called to update this component.
			void requestToUpdate();

			/// Indicates whether this object needs to be updated
			bool isUpdateNeeded();

			bool checkTag(const System::Tags& tags);

			Core::System::Event<bool, Core::Utility::Interfaces::InitializeParams>& initialized()
				const;

			Core::System::Event<bool, GFX::GCore::InputParameters>& updated() const;

		protected:

		protected:
			/// \brief The name of this component
			DString m_name;

			std::atomic_bool											   m_isEnable;
			/// \brief This boolean flag indicates that this component object requires updating
			std::atomic_bool											   m_needsToUpdate;
			mutable Core::System::Event<bool, Core::Utility::Interfaces::InitializeParams>
																		   m_initialized;
			mutable Core::System::Event<bool, GFX::GCore::InputParameters> m_updated;

			/// \brief Shared pointer to node object that this component is assigned to it
			std::weak_ptr<Node> m_node;

			/// \brief The unique id for this component
			uint32 m_id;

			/// \brief Total rendered vertex in the latest frame
			uint64 m_totalNumberVertices;

			/// \brief Set of tags for grouping items in the views and apply special policy on them
			Core::System::Tags m_tags;
		};
		D_DefRefType(Component);

	} // namespace Scene
} // namespace DGE
