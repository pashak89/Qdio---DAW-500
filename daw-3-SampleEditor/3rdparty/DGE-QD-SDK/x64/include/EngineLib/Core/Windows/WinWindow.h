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
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/Windows/Window.h>

namespace DGE
{
	extern DGE_CoreExtern vector<std::string> dropped;
	extern DGE_CoreExtern bool				  DroppedFile;
	namespace Core
	{
		namespace Windows
		{
			enum class WindowMessage
			{

			};
#if DGE_Platform == DGE_Windows_Platform
			class DGE_CoreAPI WinWindow : public Window
			{
				int m_posX, m_posY;
				DWORD m_extraStyle;
				DWORD m_style;
				WinWindow();
				HWND m_window;//a handle to window.
				HWND m_DrawWindow;
				HINSTANCE m_hInstance;//Instance of an application.
				MSG m_msg;
				WNDPROC m_WinFunc;

				//-----------------------------------------------------------------
				// Manage Windows Message
				//-----------------------------------------------------------------
				static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
			public:
				WinWindow(std::shared_ptr<System::Context> context, const DChar* WinName,
						  HINSTANCE hInstance, WNDPROC WinFunc = nullptr,
						  HWND WindowHandle = nullptr, HWND DrawWindowHandle = nullptr);

				virtual ~WinWindow();

				/// IInitialize interface
				///
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
				/// function from IInitialize interface.
				using Utility::Interfaces::IInitialize::initialize;
				/*
				Initialize Window & Create it
				If AutoSetPosition Is true then Position of window  Automatically set to Center of
				screen
				*/
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				virtual bool release() override;

				// IProcessEvents interface
				virtual bool process() override;

				//Manage Message of Window that sent form OS
				//0-->normal State
				//1-->exit application message receive.
				virtual uint16 update();

				//Set Title of window
				void setCaption(const DChar* name);

				//Set mouse cursor
				void setCursor(CursorType cursortype);

				//Show Window
				void show();

				//Hide window
				void hide();

				//Get Handle to Window
				void* getHandler();

				//Get Handle to Draw Rectangle Window
				void* getDrawHandler();

				//Set position of window in screen
				void setPosition(Vector2D pos);

				Bound getBoundOfWindow();

			protected:
				virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;
			};
#endif
		}
	}
}
