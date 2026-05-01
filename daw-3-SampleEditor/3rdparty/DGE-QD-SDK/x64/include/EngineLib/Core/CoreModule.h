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

#include <Core/IO/IO.h>
#include <Core/Platforms/Platform.h>
#include <Core/System/DApplicationInfo.h>
#include <Core/System/DModule.h>
#include <Core/System/DObject.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <memory>

using namespace DGE::Core;

namespace DGE
{
	namespace Core
	{
		/// \module CoreModule
		/// \brief Core module of the engine that contain essential sub-modules for working other
		/// modules and sub-modules properly.
		/// Also this class handle loading and unloading owner sub-modules
		class DGE_CoreAPI CoreModule
			: public Core::System::DModule
			, public Core::Utility::Singleton<CoreModule>
		{
			D_OBJECT_SINGLETON(CoreModule)

			CoreModule(std::shared_ptr<Core::System::Context> context
					   = DGE::Core::System::Context::getInstance());

		public:
			D_Destructor(CoreModule);
			/// IInitialize interface
			///
			/// \brief Use default implementation of
			/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
			/// function from IInitialize interface.
			using Core::Utility::Interfaces::IInitialize::initialize;

			/// \brief Initialize CoreModule and if it is initialized successfully return true
			///
			/// \return Return true if initializing is successfully else return false
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual bool release() override;

			Core::System::DApplicationInfoRef applicationInfo() const;

			Core::IO::FileSystem::FileManagerRef fileManager() const;

			Core::IO::LogSystem::LoggerRef logger() const;

			Platforms::PlatformWRef platform() const;

		protected:
			virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

		protected:
			Core::System::DApplicationInfoRef				 m_app;
			Core::IO::FileSystem::FileManagerRef					 m_fileManager;
			Core::IO::LogSystem::LoggerRef					 m_logger;
			Core::Platforms::PlatformRef					 m_platform;
		};

		using CoreModuleRef = std::shared_ptr<CoreModule>;
	} // namespace Core
} // namespace DGE
