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
#include <Core/Array/Array.h>
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/IO/Input/InputEvents.h>
#include <Core/IO/Input/Mouse.h>
#include <Core/Math/Vector.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/Sync/Mutex.h>
#include <Core/System/Functor.h>

#include <Core/System/DSubsystem.h>

namespace DGE
{
	namespace Core
	{
		namespace IO
		{
			namespace Input
			{
				/// \brief Manage the input event from mouse, keyboard, touch, joystick, etc.
				/// \deprecated Please use InputEventsManager class
				class DGE_CoreAPI InputManager : public Core::System::DObject
				{
					D_OBJECT(InputManager)

				protected:
					/// \brief An internal function that send events to those objects that register
					/// for them
					///
					/// \details An internal function that call in updating process (update() and
					/// autoUpdate() function) and send events to those objects that register for them
					void		updateEvent();
					inline bool HasKeyPress(char* keys, int size);

					/// \brief An internal function that call in a thread for manage the input event faster.
					void autoUpdate();

					/// \brief An internal function that reset states of all devices.
					bool reset();

					/// \brief Execute an event of input class
					bool sendEvent(EventType eventType, EventArgs* eventArg);

				public:
					/// \brief Constructor of InputManager subsystem
					///
					/// \param context The contect object that is necessary for all engine classes
					/// \param autoUpdate indicate whether must be used of auto update this class
					/// with a separated thread
					///
					/// \details The Engine register an object of this class automatically.
					///
					/// \param context shared pointer of engine context
					InputManager(DGE::Core::System::ContextRef context,
								 bool								  autoUpdate = false);

					/// \brief Destructor of InputManager that terminate the thread of InputManager
					/// and release all resources
					D_Destructor(InputManager);

					/// \brief Call from engine every frame before call any other object that has
					/// needed to input data
					void update();

					/// \brief return true for key that in parameter when pressed.
					bool getKeyPress(char key, bool IgnorePressStamp = false);

					/// \brief return true for that button get in parameter when pressed.
					bool getMouseButtonPressed(MouseButton button, bool IgnorePressStamp = false);

					/// \brief get position of mouse in application window.
					Math::Vector2D getPosMouse();

					/// \brief get mouse position in this format:(dx,dy)
					Math::Vector2D getDeltaPosMouse();

					/// \brief get rotate Wheel mouse from last frame!
					int getDeltaWheel();

					/// \brief Check for existing pressed key if any pressed key is existed return
					/// true else return false
					///
					/// \return Return true if any pressed key is existed else return false
					bool hasKeyPressed();

					/// \brief Connect to an event!
					void connectToEvent(EventType eventType,
										std::shared_ptr<Core::System::Function<void, IO::EventArgs*>>
											pFunction);

					bool autoUpdateEnabled() const;
					void setAutoUpdateEnabled(bool newAutoUpdateEnabled);

					bool isRunning();

				protected:
#if defined(DGE_UseInputAPI)
					/// \brief pointer to native window object.
					void* m_window = nullptr;

					/// \brief List of register object function for getting motion events
					Array::DArray<std::shared_ptr<Core::System::Function<void, EventArgs*>>>
						m_motionFunctions;

					/// \brief List of register object function for getting mouse buttons events
					Array::DArray<std::shared_ptr<Core::System::Function<void, EventArgs*>>>
						m_mouseButtonFunctions;

					/// \brief List of register object function for getting mouse wheel events
					Array::DArray<std::shared_ptr<Core::System::Function<void, EventArgs*>>>
						m_mouseWheelFunctions;

					/// \brief List of register object function for getting mouse in/out events
					Array::DArray<std::shared_ptr<Core::System::Function<void, EventArgs*>>>
						m_mouseInOutFunctions;

					/// \brief List of register object function for getting key pressed events
					Array::DArray<std::shared_ptr<Core::System::Function<void, EventArgs*>>>
						m_keyPressFunction;

					/// \brief List of register object function for getting drop file events
					Array::DArray<std::shared_ptr<Core::System::Function<void, EventArgs*>>>
						m_dropFileFunction;

					char m_KeyState[256],		  // store state of keyboard keys.
						m_PreviousKeyState[256];  // store previous state of keyboard keys.
					uint32 m_KeyPressStamp[256];  // Stamps the last frame each key was pressed.
					uint32 m_ButtonPressStamp[(uint32) MouseButton::Count]; // Stamps last frame each
																			// button was pressed.
					Math::Vector2D m_Position,	  // Store position of mouse cursor on screen
						m_PreviousPosition; // Store previous position of mouse cursor on screen
					uint32 m_PressStamp = 0; // Current press stamp (increment every frame).
#if DGE_Graphic_API == DGE_Use_Directx
					HRESULT		   m_hr;
					IDirectInput8* m_DI = nullptr; // DirectInput object.

					IDirectInputDevice8* m_Keyboard = nullptr; // DirectInput keyboard device.

					IDirectInputDevice8* m_Mouse = nullptr; // DirectInput mouse device.
					DIMOUSESTATE2		 m_MouseState,		// Store state of mouse buttons.
						m_PreviousMouseState; // Store previous state of mouse buttons.

					HANDLE m_hInputEvent = nullptr; // handle to event object for input.

#elif DGE_Graphic_API == DGE_Use_OpenGL
#endif
#ifndef _M_CEE
					thread*		   m_thread = nullptr;
					Sync::StdMutex m_lock;
#endif
					atomic_bool m_isExited			= false;
					atomic_bool m_autoUpdateEnabled = false;
#endif
				};
			} // namespace Input
		}	  // namespace IO
	}		  // namespace Core
} // namespace DGE
