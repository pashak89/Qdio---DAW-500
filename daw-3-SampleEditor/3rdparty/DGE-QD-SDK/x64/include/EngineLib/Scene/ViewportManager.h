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

#include <Core/Math/Bound.h>
#include <Core/Utility/Interfaces/IProcess.h>
#include <Core/Utility/Interfaces/IRelease.h>
#include <GFX/Viewport.h>

namespace DGE
{
	namespace Scene
	{
		/// \brief Manage multi-view-ports in the engine and help to render one view-port in every
		/// render call
		class DGE_SceneAPI ViewportManager
			: public GFX::GCore::Drawable
			, public Core::Utility::Interfaces::IRelease
			, public Core::Utility::Singleton<ViewportManager>
		{
			D_OBJECT_SINGLETON(ViewportManager)
			ViewportManager(Scene::SceneNodeRef					 sceneRef = nullptr,
							DGE::Core::System::ContextRef context
							= DGE::Core::System::Context::getInstance());

		public:
			D_Destructor(ViewportManager);

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

			virtual bool release() override;

			// IUpdate interface
			virtual bool update(GFX::GCore::InputParameters &input) override;

			// IRender interface
			virtual bool render(GFX::GCore::InputParameters &input) override;

			// IPreProcessRender interface
			virtual bool preProcessRender(GFX::GCore::InputParameters &input) override;

			// IPostProcessRender interface
			virtual bool postProcessRender(GFX::GCore::InputParameters &input) override;

			/// \brief Register a view-port with specific geometry and node-ID and view ids in the
			/// registered view-port
			///
			/// \param nodeID
			/// \param viewsInfo
			///
			/// \return Return unique id of registered view-port
			std::shared_ptr<GFX::Viewport> registerViewport(
				std::vector<GFX::ViewInfo> &&viewsInfo = {
					GFX::ViewInfo{.viewID = GFX::Viewport::s_defaultViewID}});

			/// \brief Register a view-port with specific geometry and node-ID and view ids in the
			/// registered view-port
			///
			/// \param nodeID
			/// \param viewsInfo
			///
			/// \return Return unique id of registered view-port
			std::shared_ptr<GFX::Viewport> registerViewport(std::vector<GFX::ViewInfo> &viewsInfo);

			/// \brief Register a view-port with specific geometry and node-ID and view ids in the
			/// registered view-port
			///
			/// \param nodeID
			/// \param viewsInfo
			///
			/// \return Return unique id of registered view-port
			std::shared_ptr<GFX::Viewport> registerViewport(
				uint32						 nodeID,
				std::vector<GFX::ViewInfo> &&viewsInfo = {
					GFX::ViewInfo{.viewID = GFX::Viewport::s_defaultViewID}});

			/// \brief Register a view-port with specific geometry and node-ID and view ids in the
			/// registered view-port
			///
			/// \param nodeID
			/// \param viewsInfo
			///
			/// \return Return unique id of registered view-port
			std::shared_ptr<GFX::Viewport> registerViewport(uint32						nodeID,
															std::vector<GFX::ViewInfo> &viewsInfo);

			/// \brief Register a view-port with specific geometry and node reference and view ids
			/// in the registered view-port
			///
			/// \param nodeReference
			/// \param viewsInfo
			///
			/// \return Return unique id of registered view-port
			std::shared_ptr<GFX::Viewport> registerViewport(Scene::NodeRef nodeReference,
															std::vector<GFX::ViewInfo> &viewsInfo);

			/// \brief Set Node-ID of the view-port with specific viewportID
			///
			/// \param viewportID id of view-port object
			/// \param nodeID
			///
			/// \return Return true if node-ID of the view-port is set successfully else return false
			bool setViewportNodeID(uint8 viewportID, uint32 nodeID);

			/// \brief Set current focused view-port by viewportID
			///
			/// \param viewportID id of view-port object
			///
			/// \return Return true if changing current focused view-port is successfully else return false
			bool setCurrentFocusedViewport(uint8 viewportID);

			/// \brief Set current focused view-port by shared pointer to view-port object
			///
			/// \param viewport shared pointer to view-port object
			///
			/// \return Return true if changing current focused view-port is successfully else return false
			bool setCurrentFocusedViewport(std::shared_ptr<GFX::Viewport> viewport);

			bool setEnabledViewport(std::shared_ptr<GFX::Viewport> viewport, bool enabled = true);

			uint64 totalEnabledViewport();

			bool isRenderedAllViewport();
			void clearRenderedViewport();

		protected:
			/// \brief Get last free view-port id
			///
			/// \return Return last free view-port id
			uint8 getFreeViewportID();

		public:
			/// \brief beginning of ID number range
			static const uint8 s_firstViewportID;

			/// \brief end of ID number range
			static const uint8 s_lastViewportID;

			/// \brief Key for getting variable of view-port ID in input parameters of
			/// ViewportManager::render()
			static const DString s_viewportIDVariableKey;

		protected:
			/// \brief Next free view-port ID
			uint8 m_viewportID;

			/// \brief Store free ID of view-ports that is released by removing the view-port.
			std::set<uint32> m_freeViewportsID;

			/// \brief Table of view-port IDs and shared pointer to registered view-ports object
			std::map<uint8, std::shared_ptr<GFX::Viewport>> m_viewports;

			/// \brief Set of shared pointer to view-port object is not initialized
			std::set<std::shared_ptr<GFX::Viewport>> m_uninitializedViewports;

			/// \brief Set of shared pointer to view-port object is enabled
			std::set<std::shared_ptr<GFX::Viewport>> m_enabledViewports;

			/// \brief Set of shared pointer to rendered view-port object for current render pipeline
			std::set<std::shared_ptr<GFX::Viewport>> m_renderedViewports;

			/// \brief Store shared pointer to last focused view-port
			std::shared_ptr<GFX::Viewport> m_lastFocusedViewport;

			/// \brief Shared pointer to scene node of scene graph that is used for detecting node
			/// reference from node id
			Scene::SceneNodeRef m_sceneReference;
		};
		using ViewportManagerRef  = std::shared_ptr<ViewportManager>;
		using ViewportManagerWRef = std::weak_ptr<ViewportManager>;
	} // namespace Scene
} // namespace DGE
