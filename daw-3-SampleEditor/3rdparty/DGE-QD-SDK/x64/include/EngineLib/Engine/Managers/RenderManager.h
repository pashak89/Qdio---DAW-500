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

#include <Core/Profiling/Fps.h>
#include <Core/System/Context.h>
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Interfaces/IProcess.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <Core/Windows/Window.h>
#include <EngineModuleDef.h>
#include <GFX/DrawablesManager.h>
#include <GFX/GCore/Renderer.h>
#include <Scene/Components/CameraController.h>
#include <Scene/Components/PredefinedComponents.h>
#include <Scene/ViewportManager.h>
#include <memory>

namespace DGE
{
	class Engine;
	using EngineRef = std::shared_ptr<DGE::Engine>;
	namespace Managers
	{
		/// \brief Rendering subsystem manager implements whole rendering of the engine with using
		/// all subsystems of the engine without depending on special Graphic API
		class DGE_EngineAPI RenderManager
			: public DGE::Core::System::DSubsystem
			, public DGE::Core::Utility::Interfaces::IProcess
			, public Core::Utility::Singleton<RenderManager>
		{
			D_OBJECT_SINGLETON(RenderManager)
			RenderManager(DGE::Core::System::ContextRef context);

		public:
			D_Destructor(RenderManager);

			// IInitialize interface
			using Core::System::DSubsystem::initialize;
			/// \brief Initialize itself with getting current Renderer subsystem
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual bool release() override;

			// IRender interface
			/// \brief render all Drawable Lists
			virtual bool process() override;
			void		 setViewport(GFX::ViewportRef viewport);
			void		 setSceneNodeID(uint32 sceneNodeID);

			Bound viewPortGeometry() const;
			void  setViewPortGeometry(const Bound& newViewPortGeometry);
			void  setViewPortSize(int32 width, int32 height);
			float currentFPS() const;

		protected:
			// DSubsystem interface
			virtual bool registerAll(
				Core::Utility::Interfaces::InitializeParams& parameters) override;

			void updateCamera();

			void updateViewport();

		public:
			static uint16					 s_updateInterval;
			static uint16					 s_minimumUpdateInterval;
			static uint16					 s_decreaseIntervalStep;
			static uint16					 s_increaseIntervalStep;
			static std::chrono::milliseconds s_updateProfilingInterval;

			Scene::CameraComponentRef activeCameraComponent() const;
			void setActiveCameraComponent(const Scene::CameraComponentRef& newActiveCameraComponent);

			Scene::CameraControllerRef activeCameraController() const;
			void					   setActiveCameraController(
									  const Scene::CameraControllerRef& newActiveCameraController);

		protected:
			Core::Utility::Interfaces::InitializeParams m_initializedParameters;
			GFX::GCore::InputParameters					m_inputRenderParameters;

			EngineRef				   m_engine;
			Core::Windows::WindowWRef  m_window;
			GFX::GCore::RendererRef	   m_renderer;
			std::vector<GFX::ViewInfo> m_viewsInfo;
			Scene::ViewportManagerWRef m_viewportManager;
			GFX::DrawablesManagerWRef  m_renderItemsManager;
			GFX::ViewportRef		   m_viewport;
			Scene::CameraComponentRef  m_activeCameraComponent;
			Scene::CameraControllerRef m_activeCameraController;
			uint8					   m_viewportID;
			Core::Math::Bound		   m_geometry;

			Core::OS::PreciseTimer			m_profilerTimer;
			Core::OS::PreciseTimer			m_frameTimer;
			std::shared_ptr<Profiling::FPS> m_fps;
			/// \brief No. number of current frame
			uint64 m_frameNumber = 0;

			/// \brief the properties of Render view-port that must be updated before executing
			/// render function.
			Bound m_viewPortGeometry;
			/// \brief ID of node in scene graph that is assign to this view-port
			uint32 m_sceneNodeID;
			/// \brief Store current fps of last rendered frame
			float m_currentFPS;
			/// \brief Indicate whether must be retrieved geometry from Core::Windows::Window
			/// directly. When set both width & height to zero, this flag enabled automatically.
			bool m_isAutoUpdateGeometry;
		};
		using RenderManagerRef	= std::shared_ptr<DGE::Managers::RenderManager>;
		using RenderManagerWRef = std::weak_ptr<DGE::Managers::RenderManager>;
	} // namespace Managers
} // namespace DGE
