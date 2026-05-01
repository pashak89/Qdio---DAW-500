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
 *
 */
#pragma once

#include <Core/CoreModuleDef.h>
#include <Core/IO/Input/Mouse.h>
#include <Core/Math/Vector2D.h>
#include <Core/System/DEvent.h>
#include <Core/System/DObjectMacrosDef.h>

namespace DGE
{
	namespace Core
	{
		namespace IO
		{
			namespace Input
			{
				/// \brief Base Class for managing input events from mouse, keyboard, touch, joystick, etc.
				class DGE_CoreAPI InputEvents
				{
				public:

					/// \brief Get clicked event of mouse
					virtual bool onMouseClicked(Core::Math::Vector2D pos, void* eventData);
					/// \brief Get key pressed event of mouse
					virtual bool onMouseHoverMoved(Core::Math::Vector2D pos, void* eventData);
					/// \brief Get moved event of mouse
					virtual bool onMousePressed(Core::Math::Vector2D   pos,
												Core::IO::MouseButtons mouseButton,
												void*				   eventData);
					/// \brief Get moved event of mouse with pressing any key
					virtual bool onMouseMoved(Core::Math::Vector2D	 pos,
											  Core::IO::MouseButtons mouseButton, void* eventData);
					/// \brief Get key released event of mouse
					virtual bool onMouseReleased(Core::Math::Vector2D	pos,
												 Core::IO::MouseButtons mouseButton,
												 void*					eventData);
					/// \brief Get wheeled event of mouse
					virtual bool onMouseWheeled(Core::Math::Vector2D   pos,
												Core::IO::MouseButtons mouseButton,
												Core::Math::Vector2D angleDelta, void* eventData);
					/// \brief Get key pressed event of key of keyboard
					virtual bool onKeyPressed(int key, IO::KeyboardModifiers modifires,
											  void* eventData);
					/// \brief Get key released event of key of keyboard
					virtual bool onKeyReleased(int key, IO::KeyboardModifiers modifires,
											   void* eventData);
					/// \brief Get touch event
					virtual bool onTouchEvent(Core::Math::Vector2D pos, void* eventData);

					/// Event emit when mouse clicked
					///
					/// \param position|Vector2D position of mouse in 2d space
					/// \param eventData|void* native event data
					///
					/// \return Return true if default event processing must be called
					D_Event(mouseClicked, bool, Core::Math::Vector2D, void*);
					D_Event(mouseHoverMoved, bool, Core::Math::Vector2D, void*);
					D_Event(mousePressed, bool, Core::Math::Vector2D, Core::IO::MouseButtons, void*);
					D_Event(mouseMoved, bool, Core::Math::Vector2D, Core::IO::MouseButtons, void*);
					D_Event(mouseReleased, bool, Core::Math::Vector2D, Core::IO::MouseButtons,
							void*);
					D_Event(mouseWheeled, bool, Core::Math::Vector2D, Core::IO::MouseButtons,
							Core::Math::Vector2D, void*);

					D_Event(keyPressed, bool, int, IO::KeyboardModifiers, void*);
					D_Event(keyReleased, bool, int, IO::KeyboardModifiers, void*);
					D_Event(touchEvent, bool, Core::Math::Vector2D, void*);
				};

				D_DefRefType(InputEvents)

			} // namespace Input
		}	  // namespace IO
	}		  // namespace Core
} // namespace DGE
