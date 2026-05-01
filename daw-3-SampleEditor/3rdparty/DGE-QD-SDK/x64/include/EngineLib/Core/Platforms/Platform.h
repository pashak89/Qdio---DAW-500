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
#include <Core/Profiling/Profiler.h>
#include <Core/System/Context.h>
#include <Core/System/DObject.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <Core/Utility/Interfaces/IProcess.h>
#include <GFX/GCore/Renderer.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			class Renderer;
		}
	}

	namespace Core
	{
		namespace System
		{
			class DObject;
		}
		namespace Platforms
		{
			/// \brief Show Type of platform that engine is running on it currently
			enum class PlatformType
			{
				Unknown,
				Windows,
				Linux,
				MacOS,
				WindowsPhone,
				Android,
				IOS,
				Emscription
			};

			// TODO: Break down Native platform for every main supported platform.
			/// \brief Show Type of cross-platform backend that is going to use it
			enum class Backend
			{
				Native,
				Qt,
				QML,
				SDL
			};

			/// \brief Use for storing Backend initialize information
			struct BackendInit
			{
				/* Surface parameters */
				uint32 redBits	   = 8;
				uint32 greenBits   = 8;
				uint32 blueBits	   = 8;
				uint32 depthBits   = 24;
				uint32 alphaBits   = 8;
				uint32 sampleCount = 0;

				/* Requested render API parameters */
				GFX::GCore::RenderAPIInfo requestedAPI;

				Backend requestedBackend = Backend::QML;

				/* Misc. parameters */
				bool forceGLCompatibilityProfile = false;
				bool debugContext				 = false;
			};

			/// \brief Storing window properties
			struct WindowSpec
			{
				std::string windowTitle;

				uint32_t width	= 640;
				uint32_t height = 480;

				uint32_t swapInterval = 0;

				bool fullScreen = false;
			};

			class Platform;

			class DGE_CoreAPI PlatformProvider : public Core::System::DCoreObject
			{
			public:
				/// \brief use for getting suitable platform instance base on real current platform
				/// and requested backend and Renderer API
				Platform *provide(Core::Utility::Interfaces::InitializeParams &parameters ,
								  std::shared_ptr<Core::System::Context>
									  context=DGE::Core::System::Context::getInstance());

				Platform *provide(
					std::initializer_list<std::pair<const DString, std::any>> &&parameters = {},
					std::shared_ptr<Core::System::Context>					   context
					= DGE::Core::System::Context::getInstance());
			};

			/// \brief An abstract class that use for loading different type of supported
			/// backend and Renderer in runtime
			class DGE_CoreAPI Platform
				: public Core::System::DSubsystem
				, public Core::Utility::Interfaces::IProcess
				, public Core::Utility::Singleton<Platform,
												  std::shared_ptr<Platform>,
												  Deleter<Platform>::deleter,
												  PlatformProvider>
			{
				D_OBJECT_SINGLETON(Platform,
								   std::shared_ptr<Platform>,
								   Deleter<Platform>::deleter,
								   PlatformProvider)
				friend class PlatformProvider;

			protected:
				Platform(Backend backend, DGE::Core::System::ContextRef context);

			public:
				D_Destructor(Platform);

				using Core::System::DSubsystem::initialize;

				using Core::System::DSubsystem::release;

				virtual void *createWindow(const WindowSpec &params) = 0;

				virtual bool destroyWindow(void *handle) = 0;

				PlatformType getPlatform() { return m_currentPlatform; }

				GFX::GCore::RenderAPIInfo getRenderApiInfo() { return m_renderApiInfo; }

				std::shared_ptr<GFX::GCore::Renderer> getRenderer() { return m_renderer; }

				BackendInit backendInfo() const;

			public:
				/// \brief Key name of input parameters in
				/// initialize(std::map<String, any>& parameters) function. Type
				///  of input parameter is BackendInit.
				static const DString s_backendInitKey;
				static const DString s_windowKey;

				Backend backendType() const;

			protected:
				/// \brief Platform we are running on
				PlatformType m_currentPlatform;

				/// \brief Used render interface
				GFX::GCore::RenderAPIInfo m_renderApiInfo;

				/// \brief Show type of running environment
				Backend m_backendType;

				/// \brief information about backend
				BackendInit m_backendInfo;

				/// \brief Renderer subsystem
				std::shared_ptr<GFX::GCore::Renderer> m_renderer;
			};
			using PlatformRef = std::shared_ptr<DGE::Core::Platforms::Platform>;
			using PlatformWRef = std::weak_ptr<DGE::Core::Platforms::Platform>;
		} // namespace Platforms
	}	  // namespace Core
} // namespace DGE
