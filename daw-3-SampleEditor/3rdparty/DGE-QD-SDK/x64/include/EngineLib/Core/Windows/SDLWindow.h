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

#include "Window.h"

struct SDL_Window;

namespace DGE
{
	namespace Core
	{
		namespace Windows
		{
			/// \brief A wrapper for SDL-Window object that can control and manage it.
			class SDLWindow : public DGE::Core::Windows::Window
			{
				D_OBJECT(SDLWindow)
			public:
				explicit SDLWindow(std::shared_ptr<System::Context> context);
				D_Destructor(SdlWindow);

				/// IInitialize interface
				///
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
				/// function from IInitialize interface.
				using Utility::Interfaces::IInitialize::initialize;
				///
				/// \details Call this function with these parameters
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
				///		std::shared_ptr<QQuickWindow> quickWindow;//*** note: must get from qml
				///		// object!
				///  	auto window=
				///		std::make_shared<Core::Windows::QtWindow>(Context::getInstance());
				///		if(app.initialize({{Window::s_nativeWindowKey,quickWindow}}))
				///			std::cout<<"QtWindow is initialized."<<std::endl;
				///		else
				///			std::cout<<"QtWindow initialization is failed."<<std::endl;
				///
				///		return 0;
				/// }
				/// \endcode
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				virtual bool release() override;

				// IProcessEvents interface
				virtual bool process() override;

				// Window interface
				virtual uint16 update() override;
				virtual void   setCaption(const DChar* name) override;
				virtual void   setCursor(CursorType cursortype) override;
				virtual void   show() override;
				virtual void   hide() override;
				virtual void*  getHandler() override;
				virtual void   setHandler(void* handler) override;
				virtual void*  getDrawHandler() override;
				virtual void   setPosition(Vector2D pos) override;
				virtual Bound  getBoundOfWindow() override;

			protected:
				/// \brief Register all data-type and events in this method
				virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

			public:
				// static const String s_widthKey;
				// static const String s_heightKey;
			private:
				/// \brief Shared pointer to SDL_Window object that is created by SDL library
				std::shared_ptr<SDL_Window> m_nativeWindow;
				/// \brief properties of window that will be created
				DisplayProperty m_displayProperty;
			};

		} // namespace Windows
	}	  // namespace Core
} // namespace DGE
