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

#include <Core/Math/Math.h>
#include <GFX/GCore/Drawable.h>
#include <GFX/View.h>
#include <Scene/SceneNode.h>

namespace DGE
{
	namespace Scene
	{
		class SceneNode;
	} // namespace Scene

	namespace GFX
	{
		/// \brief summarized information about the view
		struct DGE_GFXAPI ViewInfo
		{
			/// \brief Geometry of view for rendering
			Core::Math::Bound geometry = Core::Math::Bound::s_zero;

			/// \brief Set of tags for grouping items in the views and apply special policy on them
			std::set<DString> tags = {};

			/// \brief ID of the view
			DString viewID;
		};

		enum class PriorityView : uint8
		{
			None	   = 0,
			Background = 1,
			Low		   = 2,
			Normal	   = 3,
			High	   = 4
		};

		/// \brief Base class for represent view-port
		class DGE_GFXAPI Viewport : public GFX::GCore::Drawable
		{
			D_OBJECT(Viewport)

			friend class View;

		public:
			Viewport(DGE::Core::System::ContextRef context
					 = DGE::Core::System::Context::getInstance());

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

			// IUpdate interface
			virtual bool update(GFX::GCore::InputParameters &input) override;

			// IPreProcessRender interface
			virtual bool preProcessRender(GFX::GCore::InputParameters &input) override;

			// IRender interface
			/// \brief Render current view-port and sub-views
			virtual bool render(GFX::GCore::InputParameters &input) override;

			// IPostProcessRender interface
			virtual bool postProcessRender(GFX::GCore::InputParameters &input) override;

			/// \brief Add view to current view-port
			///
			/// \param id ID of view for future usage
			///
			/// \param viewPtr Shared pointer to object of view will added to current view-port
			///
			/// \return Return true if adding view with given ID is successful else return false.
			bool addView(DString id, std::shared_ptr<GFX::View> viewPtr);

			/// \brief Remove view from current view-port
			///
			/// \param id ID of view for removing from current view-port
			///
			/// \return Return true if removing view with given ID is successful else return false.
			bool removeView(DString id);

			/// \brief Get scene of this view-port that is used for showing in it
			///
			/// \return Shared pointer to scene object
			Scene::NodeRef scene() const;

			/// \brief Set scene of this view-port for showing in it
			///
			/// \param newScene Shared pointer to scene object
			void setScene(const Scene::NodeRef &newScene);

			/// \brief Determined whether viewID is unique or not
			///
			/// \param viewID
			///
			/// \return Return true if viewID is unique else return false
			bool isUniqueViewID(DString viewID);

			/// \brief Get id of current view-port
			///
			/// \return Return id of current view-port
			uint8 id() const;

			/// \brief Set id of current view-port
			///
			/// \param newId id of current view-port
			void setId(uint8 newId);

			/// \brief Set geometry of a view with specific viewID
			///
			/// \param viewID
			/// \param geometry
			///
			/// \return Return true if geometry of the view with specific viewID is updated
			/// successfully else return false
			bool setGeometryView(DString viewID, Core::Math::Bound geometry);

			bool focused() const;
			void setFocused(bool newFocused);

			/// \brief Retrieve special tag of default view
			const DString defaultSpecialTag();

			std::shared_ptr<Scene::CameraComponent> cameraComponent() const;
			void									setCameraComponent(
												   const std::shared_ptr<Scene::CameraComponent> &newCameraComponent);

			/// \brief Emit the event when selected object is changed
			static Core::System::Event<void, Scene::DEntityRef> &selectedEntityChanged();

			static void setSelectedEntity(Scene::DEntityRef entity);
			static Scene::DEntityRef currentSelectedEntity();

		protected:
			static void onSelectedEntityChanged(Scene::DEntityRef entity);

		public:
			/// \brief default name for view-port
			static inline const DString s_defaultViewportName = "Viewport";
			/// \brief ID for default view
			static inline const DString s_defaultViewID = "DefaultView";

			/// \brief Current view key name
			static inline const DString s_currentiView = "CurrentView";

			/// \brief Key for getting camera component in the scene of this view-port in
			/// Viewport::initialize(...) or Viewport::update(...) or Viewport::preProcessRender(...)
			/// or Viewport::render(...) or Viewport::postProcessRender(...) function
			static inline const DString s_cameraComponentKey = "CameraComponent";

			/// \brief Key for getting view matrix of camera component in the scene of this
			/// view-port in Viewport::initialize(...) or Viewport::update(...) or
			/// Viewport::preProcessRender(...) or Viewport::render(...) or
			/// Viewport::postProcessRender(...) function
			static inline const DString s_viewMatrixKey = "ViewMatrix";

			/// \brief Key for getting projection matrix of camera component in the scene of this
			/// view-port in Viewport::initialize(...) or Viewport::update(...) or
			/// Viewport::preProcessRender(...) or Viewport::render(...) or
			/// Viewport::postProcessRender(...) function
			static inline const DString s_projectionMatrixKey = "ProjectionMatrix";

			/// \brief The event object is using for emitting event when selected object is changed
			static Core::System::Event<void, Scene::DEntityRef> s_selectedEntityChanged;

		protected:
			/// \brief The ID of current view-port
			uint8 m_id;

			/// \brief list of views in current view-port
			std::map<DString, std::shared_ptr<GFX::View>> m_views;

			/// \brief shared pointer to Scene object that is used for showing in this view-port.
			Scene::NodeRef m_scene;

			/// \brief Indicate this Viewport is focused now.
			bool m_focused;

			/// \brief Shared pointer to a camera component in the scene
			std::shared_ptr<Scene::CameraComponent> m_cameraComponent;
		};

		using ViewportRef = std::shared_ptr<GFX::Viewport>;
		using ViewportWRef = std::weak_ptr<GFX::Viewport>;
	} // namespace GFX
} // namespace DGE
