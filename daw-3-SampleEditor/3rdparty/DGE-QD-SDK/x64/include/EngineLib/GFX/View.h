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
#include "Core/System/Event.h"
#include <Core/Math/Math.h>
#include <Core/System/Tags.h>
#include <GFX/GCore/DebugRenderHelper.h>
#include <GFX/GCore/Drawable.h>
#include <GFX/GCore/Renderer.h>
#include <GFX/GraphicsManager.h>
#include <GFX/RayTracer.h>
#include <Scene/Components/PredefinedComponents.h>
#include <Scene/DEntity.h>
#include <atomic>

namespace DGE
{
	namespace GFX
	{
		/// \brief Base abstract class for represent view object in every API
		class DGE_GFXAPI View : public GFX::GCore::Drawable
		{
			D_OBJECT(View)
			friend class GraphicsManager;

		public:
			View(DGE::Core::System::ContextRef context
				 = DGE::Core::System::Context::getInstance());

			// IInitialize interface
			/// \brief Call when object must be initialized.
			using Core::Utility::Interfaces::IInitialize::initialize;

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

			// IUpdate interface
			virtual bool update(GCore::InputParameters &input) override;

			// IPreProcessRender interface
			virtual bool preProcessRender(GCore::InputParameters &input) override;

			// IRender interface
			/// \brief Render current view-port and sub-views
			virtual bool render(GCore::InputParameters &input) override;

			// IPostProcessRender interface
			virtual bool postProcessRender(GCore::InputParameters &input) override;

			/// \brief Get geometry of current view
			///
			/// \return Return geometry of current view
			Core::Math::Bound geometry() const;
			/// \brief Set geometry of current view
			///
			/// \param newGeometry New geometry of current view will set to it
			void setGeometry(const Core::Math::Bound &newGeometry);

			/// \brief Assign a tag to current view
			///
			/// \param tagName Name of tag for assigning into current view
			///
			/// \return Return true if name of tag is assigned to current view successfully else
			/// return false.
			bool assignTag(DString tagName);

			/// \brief Determine is assign a tag to current view or not
			///
			/// \param tagName Name of tag for checking existence the tag name in current view
			///
			/// \return Return true if name of tag is found in current view else return false.
			bool isAssignedTag(DString tagName);

			/// \brief Remove a tag from current view
			///
			/// \param tagName Name of tag for removing into current view
			///
			/// \return Return true if removing the tag name from current view is successful else
			/// return false.
			bool removeTag(DString tagName);

			/// \brief Get set of all tags that is assigned to current view
			///
			/// \return Return set of all tags that is assigned to current view
			const Core::System::Tags &tags();

			/// \brief Set list of all tags that is assigned to current view
			///
			/// \param newTagList List of tag that is assigned to current view
			void setTags(const std::set<DString> &newTagList);

			/// \brief unique tag name for current view
			const DString specialViewTagName();

			DString id() const;
			void   setID(const DString &newID);

			/// \brief Emit the event when selected object is changed
			static Core::System::Event<void, Scene::DEntityRef> &selectedEntityChanged();

			static void setSelectedEntity(Scene::DEntityRef entity);
			static Scene::DEntityRef currentSelectedEntity();

		public:
			/// \brief Default tag name for untagged items that will be used for general object rendering
			static const DString s_defaultUntaggedItemName;
			/// \brief Special tag name for current view and mixed with view ID that will be used
			/// for rendering object in specified view
			static const DString s_specialViewTagName;

			/// \brief Key for getting geometry value of this view in View::initialize(...) function
			static const DString s_geometryKey;

			/// \brief Key for getting scene value of this view in View::initialize(...) or
			/// View::update(...) or View::preProcessRender(...) or View::render(...) or
			/// View::postProcessRender(...) function
			static const DString s_sceneKey;

			/// \brief Key for getting current view value in View::initialize(...) or
			/// View::update(...) or View::preProcessRender(...) or View::render(...) or
			/// View::postProcessRender(...) function
			static const DString s_currentViewKey;

			/// \brief Key for getting list of rendered DEntity objects
			static const DString s_renderedDEntityObjectsListKey;

			/// \brief Key for getting current clicked mouse position
			static const DString s_clickedMousePositionKey;

		protected:
			/// \brief The last selected entity that can be null
			static Scene::DEntityRef s_lastSelectedEntity;

			/// \brief The event object is using for emitting event when selected object is changed
			static Core::System::Event<void, Scene::DEntityRef> s_selectedEntityChanged;

			/// \brief Indicate whether debug ray tracing is activated
			static atomic_bool s_isEnabledDebugRayTracing;

		protected:
			/// \brief Unique id for this view
			DString m_id;

			/// \brief Geometry of view for rendering
			Core::Math::Bound m_geometry;

			/// \brief Set of tags for grouping items in the views and apply special policy on them
			Core::System::Tags m_tags;

			/// \brief Shared pointer to renderer object that is used for rendering with direct help
			/// of graphic API
			std::shared_ptr<GFX::GCore::Renderer> m_renderer;

			/// \brief a list of rendered entity object
			std::shared_ptr<std::vector<Scene::DEntityRef>> m_dEntityRenderedList;

			/// \brief Ray tracer object that is used for ray-tracing with boundary of entity
			GFX::RayTracer m_rayTracer;

			/// \brief The helper class object for create debug item that will be used in render pipeline
			GCore::DebugRenderHelperWRef m_debugRender;

			/// \brief last entity is hit by ray-tracing
			Scene::DEntityWRef m_lastHitEntity;

			/// \brief last entity is hovered by ray-tracing
			Scene::DEntityWRef m_lastHoveredEntity;

			/// \brief last position of the mouse that is ray-tracing for it.
			Core::Math::Vector2D m_lastMousePosition;

			/// FIXED Must use the native engine directly and fix this incorrect usage of upper
			/// layer

			/// \brief A shared pointer to the graphic manager of engine
			GFX::GraphicsManagerWRef m_graphicsManager;

			/// \brief Default vertex shader in the engine
			DString m_defaultVertexShader;
			/// \brief Default fragment shader in the engine
			DString m_defaultFragmentShader;
		};
	} // namespace GFX
} // namespace DGE
