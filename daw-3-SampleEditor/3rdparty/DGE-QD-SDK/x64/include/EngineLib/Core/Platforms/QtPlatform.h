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

#include "Core/Utility/Singleton/SingletonMacrosDef.h"
#include "Platform.h"

#ifdef DGE_USE_QT
class QQuickWindow;
#endif

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace QtWrapper
			{
				class QtRenderer;
			}
		} // namespace GCore
	}	  // namespace GFX

	namespace Core
	{
		namespace Windows
		{
			class QtWindow;
		}

		namespace Platforms
		{
			/// \brief Qt Implementation of Platform class that prepare related subsystem
			class DGE_CoreAPI QtPlatform : public DGE::Platforms::Platform
			{
				D_OBJECT(QtPlatform)
			public:
				QtPlatform(Backend backend, DGE::Core::System::ContextRef context);

				D_Destructor(QtPlatform);

				/// IInitialize interface
				///
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
				/// function from IInitialize interface.
				using Core::Utility::Interfaces::IInitialize::initialize;
				///
				/// \details It's created by Context automatically based on initialize parameters.
				/// Call this function with these parameters
				///
				/// \param parameters Input parameters for initialize this class.
				/// must set by below example:
				///
				/// \return return true if initialization is successfully otherwise return false
				///
				///	\code
				/// #include <iostream>
				/// #include <Core/Application/Application.h>
				/// int main(int argc, char* argv[])
				/// {
				///		// contain other codes
				///
				///  	auto context= Context::getInstance();
				///		if(context.initialize({
				///						{Window::s_nativeWindowKey,quickWindow},
				/// 					{DGE::Platforms::Platform::s_backendInitKey,
				/// backendInitRequest},
				///						}))
				///			std::cout<<"Context is initialized."<<std::endl;
				///		else
				///			std::cout<<"Context initialization is failed."<<std::endl;
				///
				///		return 0;
				/// }
				/// \endcode
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				// IProcessEvents interface
				virtual bool process() override;

				// Platform interface
				virtual bool  release() override;
				virtual void* createWindow(const DGE::Platforms::WindowSpec& params) override;
				virtual bool destroyWindow(void* handle) override;
				//	virtual void  swapBuffers(void* handle) override;
				//	virtual void  process() override;
				//	virtual void  getSize(void* handle, uint32* width, uint32* height) override;
				//	virtual void  setWindowTitle(void* handle, const DChar* title) override;

			protected:
				virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

			protected:
				std::shared_ptr<Core::Windows::QtWindow>	m_window;
				std::shared_ptr<GFX::GCore::QtWrapper::QtRenderer> m_renderer;
			};
		} // namespace Platforms
	}	  // namespace Core
} // namespace DGE
