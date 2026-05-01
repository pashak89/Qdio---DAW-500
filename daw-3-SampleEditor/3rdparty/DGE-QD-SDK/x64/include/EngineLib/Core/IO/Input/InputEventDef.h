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
#include "Core/BaseTypes/Flags.h"
#include <Core/BaseTypes/BaseTypes.h>

namespace DGE
{
	namespace Core
	{
		namespace IO
		{
			/// \brief Type of Event That Occurred
			enum class EventType
			{
				None  = 0xff, /// \brief Default value for the EventType enum.
				Click = 0,	  /// \brief when the mouse clicked
				MouseUp,	  /// \brief when the button of mouse released
				MouseDown,	  /// \brief when the button of mouse pressed
				MouseMotion,  /// \brief when position of mouse changed
				MouseWheel,	  /// \brief when wheel of mouse moved
				MouseIn,	  /// \brief when mouse enter into boundary of an object
				MouseOut,	  /// \brief when mouse exit from boundary of an object
				KeyDown,	  /// \brief when the button of keyboard pressed
				KeyUp,		  /// \brief when a file drop in area of keyboard released
				DropFiles,	  /// \brief when a file drop in area of object
			};

			/// \brief An interface for storing data of event that has described this event
			class DGE_CoreAPI EventArgs
			{
			public:
				/// \brief return Type of Event
				EventType getEventType() { return m_eventType; }

				/// \brief set type of event
				/// \param newEventType type of event that is occurred
				void setEventType(EventType newEventType);

			protected:
				/// \brief specified type of event
				EventType m_eventType = EventType::None;
			};

			/// \brief modifired of Keyboard
			enum class KeyboardModifier
			{
				None		= 0x00000000,
				Shift		= 0x02000000,
				Control		= 0x04000000,
				Alt			= 0x08000000,
				Meta		= 0x10000000,
				Keypad		= 0x20000000,
				GroupSwitch = 0x40000000,
			};
			D_EnumFlags(KeyboardModifier);

			/// \brief Arguments of Keyboard event that contain some details of occurred event
			class DGE_CoreAPI KeyEventArgs : public EventArgs
			{
			public:
				/// \brief get code of current key pressed
				int getCurrentKeyDown() const;

				/// \brief set code of current key pressed
				///
				/// \param newCurrentKeyDown code of key pressed
				void setCurrentKeyDown(int newCurrentKeyDown);

				/// \brief get status of current key pressed
				///
				/// \return return code of key pressed
				uint8 getKeyState() const;

				/// \brief set status of current key pressed
				void setKeyState(uint8 newKeyState);

			protected:
				/// \brief code of current Key Pressed
				int currentKeyDown;

				/// \brief state of current Key pressed
				uint8 keyState;
			};
		} // namespace IO
	}	  // namespace Core
} // namespace DGE
