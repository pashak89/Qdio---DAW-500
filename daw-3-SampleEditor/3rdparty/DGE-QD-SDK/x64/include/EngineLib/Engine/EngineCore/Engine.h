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

//-----------------------------------------------------------------
// Engine header
//-----------------------------------------------------------------
// #include"Physics\2D\ShapeProperties.h"
// #include"Physics\2D\Physics2D.h"
// #include"Physics\3D\BoundingVolume.h"
// #include"Physics\3D\Physics.h"
// #include"Sound\SoundSystem.h"
// #include"Sound\Sound.h"
// #include"Sound\AudioPath3D.h"
// #include"Sound\DirectMusic\SoundSystemDM.h"

#include "Managers/InputEventsManager.h"
#include <GFX/GFXModule.h>
#include <Scene/SceneModule.h>
#include <UI/UIModule.h>

#include <Core/Core.h>
#include <Core/OS/Timer.h>
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Interfaces/IExecutable.h>

#include <Helpers/GameState.h>
#include <Scene/ViewportManager.h>
#include <ScriptSet/Script.h>
#include <memory>

#include <Managers/AssetsManager.h>
#include <Managers/RenderManager.h>

#if defined(DGE_USE_DEPERCATED)
#include <Object2D/2DObject.h>
// #include"Object2D\PhysicBased\Physical2DObject.h"
// #include"Object2D\PhysicBased\StaticObject.h"
// #include"Object2D\PhysicBased\DynamicObject.h"
// #include"Object2D\PhysicBased\PlayerController.h"
#include <GameObject/AnimatedObject.h>
#include <GameObject/CameraObject.h>
#include <GameObject/GameObject.h>
#include <GameObject/SceneObject.h>
#include <GameObject/SpawnerObject.h>
#include <Object2D/2DObjectsManager.h>
#include <Object2D/GUI/Control.h>
#include <Object2D/GUI/Image.h>
#endif

#include <EngineModuleDef.h>

namespace DGE
{
	namespace QtWrapper
	{
		class QEngine;
		class QMLEngine;
	} // namespace QtWrapper
#if defined(DGE_Memory_Allocator) && DGE_Memory_Allocator == DGE_Memory_NedMalloc
	using namespace nedalloc;
#endif

	using namespace DGE::Core;
	using namespace DGE::Core::Array;
	using namespace DGE::Core::BaseType;
	using namespace DGE::Core::BaseType::CPU;
	using namespace DGE::Core::IO;
	using namespace DGE::Core::IO::FileSystem;
	using namespace DGE::Core::IO::LogSystem;
	using namespace DGE::Core::IO::Input;
	using namespace DGE::Core::OS;
	using namespace DGE::Core::Math;
	using namespace DGE::Core::System;
	using namespace DGE::Core::Tree;
	using namespace DGE::Core::Utility;
	using namespace DGE::Core::Utility::Interfaces;
	using namespace DGE::Core::Xml;
	using namespace DGE::Core::Windows;
	using namespace DGE::GFX;
	using namespace DGE::GFX::Resource;
	using namespace DGE::Memory;
	using namespace DGE::Platforms;
	using namespace DGE::ScriptSet;
	using namespace DGE::GameTools;
	using namespace DGE::Managers;
#if defined(DGE_USE_DEPERCATED)
	using namespace DGE::GameObjects;
	using namespace DGE::Objects2D;
	using namespace DGE::Objects2D::GUI;
#endif
	// using namespace DGE::Objects2D::PhysicBase;
	// using namespace DGE::SFX;
	//	using namespace DGE::Physic::_2D;
	//	using namespace DGE::Physic;

#if DGE_Graphic_API == DGE_Use_Directx
	using namespace GFX::DirectX9;
#endif
#ifdef DGE_UseEngine
	//-----------------------------------------------------------------
	// Engine Setup Structure
	//-----------------------------------------------------------------
	class DGE_EngineAPI EngineSetup
	{
	public:
		DString name; // Title windows render.

		uint16 Width		   = 800;	// Width of Render Screen
		uint16 Height		   = 600;	// Height of Render Screen
		bool   FullScreen	   = false; // Set Engine To Full screen mode
		uint16 TotalBackBuffer = 1;		// total back buffer
		float  scale3D		   = 1.0f;
		float  scale2D		   = 1.0f;

		Function<void>* RegisterState
			= nullptr; // a Function pointer for Register All state that use in engine!
		Function<void>* UnregisterState
			= nullptr; // a Function pointer for Unregister All state that use in engine!

		Function<void, Material**, const DChar*, const DChar*>* CreateMaterialFunction = nullptr;
#if DGE_Platform == DGE_Windows_Platform
		HWND	  m_WindowHandle	 = nullptr;
		HINSTANCE instance			 = nullptr; // instance of it.
		HWND	  m_DrawWindowHandle = nullptr;
		WNDPROC	  m_WindowFunc		 = nullptr;
#endif
		DString AssistObjectPathFolder;

		EngineSetup()
		{
#if DGE_Platform == DGE_Windows_Platform
			instance = nullptr;
#endif
			name				   = DL("Engine");
			Width				   = 800;
			Height				   = 600;
			FullScreen			   = false;
			RegisterState		   = nullptr;
			UnregisterState		   = nullptr;
			AssistObjectPathFolder = DL("./");
		}
		EngineSetup(const DChar* name, Function<void>* RegisterState,
					Function<void>* UnRegisterState, uint16 Width, uint16 Height, bool FullScreen
#if DGE_Platform == DGE_Windows_Platform
					,
					HINSTANCE hinstance
#endif
					) // default value for this structure.
		{
#if DGE_Platform == DGE_Windows_Platform
			instance = hinstance;
#endif
			this->name			  = (DChar*) name;
			this->Width			  = Width;
			this->Height		  = Height;
			this->FullScreen	  = FullScreen;
			this->RegisterState	  = RegisterState;
			this->UnregisterState = UnRegisterState;
		}

		EngineSetup(const DChar* name, uint16 Width, uint16 Height, bool FullScreen
#if DGE_Platform == DGE_Windows_Platform
					,
					HINSTANCE hinstance, HWND WindowHandle,
					HWND DrawWindowHandle
#endif
					) // default value for this structure.
		{
#if DGE_Platform == DGE_Windows_Platform
			instance		   = hinstance;
			m_WindowHandle	   = WindowHandle;
			m_DrawWindowHandle = DrawWindowHandle;
#endif
			this->name			  = (DChar*) name;
			this->Width			  = Width;
			this->Height		  = Height;
			this->FullScreen	  = FullScreen;
			this->RegisterState	  = nullptr;
			this->UnregisterState = nullptr;
		}

		EngineSetup(const DChar* name, uint16 Width,
					uint16 Height) // default value for this structure.
		{
#if DGE_Platform == DGE_Windows_Platform
			instance		   = nullptr;
			m_WindowHandle	   = nullptr;
			m_DrawWindowHandle = nullptr;
#endif
			this->name			  = (DChar*) name;
			this->Width			  = Width;
			this->Height		  = Height;
			this->FullScreen	  = false;
			this->RegisterState	  = nullptr;
			this->UnregisterState = nullptr;
		}

		EngineSetup(const DChar* name) // default value for this structure.
		{
#if DGE_Platform == DGE_Windows_Platform
			instance		   = nullptr;
			m_WindowHandle	   = nullptr;
			m_DrawWindowHandle = nullptr;
#endif
			this->name			  = (DChar*) name;
			this->Width			  = 800;
			this->Height		  = 600;
			this->FullScreen	  = false;
			this->RegisterState	  = nullptr;
			this->UnregisterState = nullptr;
		}
		~EngineSetup();
	};

	// flage for use in RegisterState() function
	enum class RegisterFlage
	{
		OnlyRegister,
		OnlyRegisterAndLoad,
		FullRegister
	};
	//-----------------------------------------------------------------
	// Engine Main Class
	//-----------------------------------------------------------------
	class DGE_EngineAPI Engine
		: public Core::System::DSubsystem
		, public GFX::GCore::Interfaces::IRender
		, public GFX::GCore::Interfaces::IUpdate
		, public Core::Utility::Interfaces::IExecutable
		, public Singleton<Engine>
	{
		D_OBJECT_SINGLETON(Engine)
		// static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

		Engine();

	public:
		/// IInitialize interface
		/// \brief Use default implementation of
		/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
		/// function from IInitialize interface.
		using Utility::Interfaces::IInitialize::initialize;

		/// \brief Initialize Engine with given parameters
		///
		/// \details Call this function with these parameters to initialize Engine that this process
		/// will register subsystems and prepare Engine for execution and rendering items
		///
		/// \param parameters Input parameters for initialize this class.
		/// must set by below example:
		///
		/// \return return true if initialization is successfully otherwise return false
		///
		///	\code
		/// #include <iostream>
		/// #include <Engine.h>
		///
		/// using namespace DGE;
		/// using namespace std;
		///
		/// int main(int argc, char* argv[])
		/// {
		///		// contain other codes
		///		Backend backendInitRequest;
		///		auto engine=Engine::CreateEngine();
		///		if (engine->initialize({{App::s_argVKey, argv},
		///								{
		///									Context::s_backendInitKey,backendInitRequest
		///								}}))
		///			std::cout<<"Engine is initialized."<<std::endl;
		///		else
		///			std::cout<<"Engine initialization is failed."<<std::endl;
		///
		///		return 0;
		/// }
		/// \endcode
		virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

		/// \brief Destructor of Engine that prepare subsystems for stopping and release all
		/// resources and stop main loop of Engine execution.
		D_Destructor(Engine);

		virtual bool release() override;

		//! Enable Engine & Show Render window.
		void enable();

		//! Disable Engine & Hide Render window & Stop Rendering!
		void disable();

		/// \brief Run one frame
		void runFrame(GCore::InputParameters& input);

		/// \brief Execute engine loop and exit from it when close main window or call exit event
		virtual int32 exec() override;

		/*!
		\brief	Add a state pointer to m_State.if \param change is true
		then engine switch to this state when add this state to
		engine.

		\param state is pointer to object of type GameState

		RegisterFlage change:
		0: state add to register list but not loaded.
		1: state add to register list and loaded but don't switched current state to it.
		2: state add to register list and loaded and switched current state to it.
		*/
		uint32 registerState(GameState* state, RegisterFlage change);

		/*!
		\brief Remove State pointer from m_State that matched.
		*/
		void unregisterState(GameState* state);

		/*!
		\brief Change current state engine to the state that have this ID.
		\return true when change state is successfully.
		*/
		bool changeState(BaseType::uint32 ID);

		/**
		\brief Get pointer to Current state engine.
		\return a pointer to \c State class.
		*/
		GameState* getCurrentState();

		void					setMainWindow(std::shared_ptr<Core::Windows::Window> window);
		std::shared_ptr<Window> getMainWindow();

		//! Get Renderer Object.
		GCore::RendererWRef getRenderer();

		Scene::ViewportManagerRef getViewportManager();

		//! retrieve a pointer to \c InputManager object
		std::shared_ptr<InputManager> getInputManager();

		//! retrieve a pointer to \c ResourceManager<ScriptResource> object
		Core::ResourceLoader<ScriptResource>* getScriptManager();

		Core::ResourceLoader<Material>* getMaterialManager();

		Core::ResourceLoader<Mesh>* getMeshManager();

		/// \brief set enable flag for creating debug item for ray tracing
		void setIsEnabledDebugRayTracing(bool isEnabled);
		bool isEnabledDebugRayTracing();

		/// \b Dump profiling data into string for logging or showing in UI
		std::string dumpProfilingData();

		// SoundSystem *GetSoundSystem();

		float getScale();

		bool isLoaded() const;
		/// state:
		/// 0-->hide
		/// 1-->show
		/// 2-->use software cursor.
		bool setCursorState(int state);

		void exit();

		Core::System::Event<bool, GFX::GCore::InputParameters>& updated() const;

	protected:
		virtual bool render(GFX::GCore::InputParameters& input) override;
		// update state & input & ... of engine.
		virtual bool update(GCore::InputParameters& input) override;

		// DSubsystem interface
	protected:
		virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

	public:
		/// \brief Key for getting elapsed time in input variable of Engine::render(...)
		static inline const DString s_elapsedVariableKey = "ElapsedTime";
		/// \brief Key for default store path in the initialize parameters.
		static inline const DString s_defaultAssetsPathKey = "DefaultAssetsPath";

	private:
		/// \brief Module member variables
		Core::CoreModuleRef	  m_coreModule;
		UI::UIModuleRef		  m_uiModule;
		GFX::GFXModuleRef	  m_gfxModule;
		Scene::SceneModuleRef m_sceneModule;

		std::atomic_bool m_loaded;			// Show when Engine loaded or not.
		std::atomic_bool m_AutoInputUpdate; // if true use a thread for input.
		std::atomic_bool m_IsEnabled;		// Show when Engine is Active or not.
		std::atomic_bool m_exited;			// true when Engine Exited.
		std::atomic_bool m_exiting;			// true when Engine Began to exit.
		std::atomic_bool m_hasOwnWindows;

		/// must cleared members
		Core::Windows::WindowRef	 m_window;			// Use for create and show window
		std::shared_ptr<EngineSetup> m_setup = nullptr; // a Copy of Engine Structure.
		void*		 m_MutexHandle = nullptr; // handle to mutex that run in this application.
		Vector2D	 m_ScreenDefault;		  // Default position of Window
		PreciseTimer m_timer;

		Core::Platforms::PlatformRef m_platform;

		// Graphic variable
		GFX::GCore::RendererWRef	   m_renderer;
		DrawablesManagerRef			   m_drawablesManager;
		UI::UIManagerWRef			   m_uiManager;
		Managers::AssetsManagerRef	   m_assetsManager;

		/// \brief Managers in the Engine module
		DGE::Managers::RenderManagerRef		  m_renderManager;
		DGE::Managers::InputEventsManagerWRef m_inputEventsManager;

		GFX::GraphicsManagerRef				 m_graphicManager;
		Scene::ViewportManagerRef			 m_viewportManager;
		DisplayMode							 m_DisplayMode;
		std::shared_ptr<InputManager>		 m_input;
		std::shared_ptr<Profiling::Profiler> m_profiler;

		GFX::GCore::ClearParam* m_ClearParam = nullptr;

		GFX::GCore::PresentParam m_PresentParam;

		ViewerSetup m_viewer; // have details needed to render current frame.

		DArray<GameState*> m_States;				 // list of array that register.
		GameState*		   m_CurrentState = nullptr; // pointer to current state.
		bool			   m_StateChanged = false;	 // true when State changed.

		Core::ResourceLoader<ScriptResource>* m_ScriptManager
			= nullptr; // Object for management Script file.
		Core::ResourceLoader<Material>* m_MaterialManager
			= nullptr; // Object for management Material file.
		Core::ResourceLoader<Mesh>*			   m_MeshManager	  = nullptr;
		Core::ResourceLoader<TextureResource>* m_2DTextureManager = nullptr;
		Core::ResourceLoader<TextureResource>* m_3DTextureManager = nullptr;

		// SFX::SoundSystem* m_SoundSystem = nullptr;
		//_2DObjectsManager* m_GuiManager = nullptr; // a Object for managed & render GUI objects!
		// Physics* m_Physic3D = nullptr;
		// Physics2D* m_Physic2D = nullptr;

		uint16 m_CurrentBackBuffer = 0;
		uint64 timeBetweenFrame	   = 0;

		/// \brief Events:
		///
		/// \brief Update Event. Call when update function is executed and finished
		mutable Core::System::Event<bool, GFX::GCore::InputParameters> m_updated;

		friend class WinWindow;
		friend class DGE::QtWrapper::QMLEngine;
		friend class DGE::QtWrapper::QEngine;
	};
	using EngineRef = std::shared_ptr<DGE::Engine>;
#endif
} // namespace DGE

#include <Core/GlobalObjects/GlobalObjects.h>

#include <Core/BaseTypes/CPU/CPU.h>
#if defined(_LIB) && !defined(EntryPoint)
#include "EntryPoint.h"
#endif
