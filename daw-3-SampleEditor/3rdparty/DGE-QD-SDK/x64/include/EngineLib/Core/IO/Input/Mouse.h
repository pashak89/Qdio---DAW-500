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
#include <Core/BaseTypes/Flags.h>
#include <Core/IO/Input/InputEventDef.h>
#include <Core/Math/Vector.h>
#include <Core/System/Functor.h>

namespace DGE
{
	namespace Core
	{
		namespace IO
		{
#if defined(DGE_UseGraphicAPI)
			/// \brief Use for showing type button of mouse
			enum class MouseButton
			{
				None   = 0x00000000, /// \brief None button of mouse
				Left   = 0x00000001, /// \brief Left button of mouse
				Right  = 0x00000002, /// \brief Right button of mouse
				Middle = 0x00000004, /// \brief Middle button of mouse
				Count
			};

			D_EnumFlags(MouseButton);

			/// \brief Internal structure for storing input data related to mouse
			struct InputData
			{
				/// \brief Type of mouse button
				MouseButton m_MB;

				/// \brief Type of event that occurred
				EventType m_EtMBStatus;

				/// \brief Last position of mouse
				Math::Vector2D m_PosMouse;

				/// \brief Set of keys that is pressed
				set<char> keyinput;

				/// \brief It's Reserved variable that used for storing extra data.
				///
				/// \details Any data that a member need more that normal member can get from it.
				void* m_ExtraData;
			};

			class DGE_CoreAPI MouseEventArgs : public EventArgs
			{
			public:
				/// \brief x Position Mouse
				int x;
				/// \brief y Position Mouse
				int y;
				/// \brief x delta Position
				int				deltaX;
				/// \brief y delta Position
				int				deltaY;
				/// \brief Type of mouse button
				IO::MouseButton Mousebutton;
			};

			/// \brief Manage input event that is related to mouse device.
			class DGE_CoreAPI Mouse
			{
			public:
				/// \brief Default constructor that initialize this class
				Mouse(void);

				/// \brief Virtual Destructor that release all resources that this class get them
				virtual ~Mouse(void);

				/*virtual void Connect(EventType et, Utility::Function<void, EventArgs*>*fn);
				void OnMouseMotion(int x, int y);
				void OnKeyDown(unsigned char button, int x, int y);
				void OnMouse(int button, int status, int x, int y);*/

				static InputData m_IInput;
			};
#endif
		}
	}
}

