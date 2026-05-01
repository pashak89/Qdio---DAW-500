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
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Interfaces/IProcess.h>
#include <compare>
#include <memory>

namespace DGE
{
	class Engine;
	namespace Core
	{
		namespace Windows
		{
			using namespace DGE::Core::Math;
			struct DisplayProperty
			{
				uint16_t width;
				uint16_t height;
				DString	 title;
				bool	 isFullScreen;

				DisplayProperty()
					: DisplayProperty(0, 0)
				{
				}
				DisplayProperty(uint16_t width, uint16_t height, bool isFullScreen = false)
				{
					this->width		   = width;
					this->height	   = height;
					this->isFullScreen = isFullScreen;
					title		 = "";
				}
			};
			enum CursorType
			{
				CT_Show,
				CT_Hide,
				CT_Pointer
			};

			//! base class for Window that engine use this in every OS!
			class DGE_CoreAPI Window
				: public Core::System::DSubsystem
				, public Utility::Interfaces::IProcess
			{
				D_OBJECT(Window)
				friend class DGE::Engine;

			public:
				Window(std::shared_ptr<Core::System::Context> context);
				/*
				Initialize Window & Create it
				If AutoSetPosition Is true then Position of window  Automatically set to Center of
				screen
				*/
				using System::DSubsystem::initialize;

				// IProcessEvents interface
				using Core::Utility::Interfaces::IProcess::process;

				// Update window contents
				virtual uint16 update() = 0;

				//Set Title of window
				virtual void setCaption(const DChar* name) = 0;

				//Set mouse cursor
				virtual void setCursor(CursorType cursortype) = 0;

				//Show Window
				virtual void show() = 0;

				//Hide window
				virtual void hide() = 0;

				// Get Handle to Window
				virtual void setHandler(void* handler) = 0;

				//Get Handle to Window
				virtual void* getHandler() = 0;

				//Get Handle to Draw Rectangle Window
				virtual void* getDrawHandler() = 0;

				//Set position of window in screen
				virtual void setPosition(Vector2D pos) = 0;

				virtual Bound getBoundOfWindow() = 0;

			public:
				static const DString s_displayPropertyKey;
				static const DString s_autoSetPositionKey;
				static const DString s_transparentBorderKey;
				static const DString s_nativeWindowKey;

			protected:
				DChar			  m_pWinName[MaxLengthName];
				Core::Math::Bound m_RectWindow; // Position of Top Left Cornel of Window
			};
			using WindowRef = std::shared_ptr<DGE::Core::Windows::Window>;
			using WindowWRef = std::weak_ptr<DGE::Core::Windows::Window>;
		}
	}
}
