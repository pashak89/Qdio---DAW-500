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

#include <memory>
#include <QBasicTimer>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QtCore/QObject>

#include <Core/IO/Input/InputEvents.h>
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/IO/Input/Mouse.h>
#include <Qt/QtModuleDef.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QtDCore
		{
			namespace IO
			{
				/// \brief A proxy object for redirect input signal into target object
				class DGE_QtAPI InputHandler : public QObject
				{
					Q_OBJECT

				public:
					explicit InputHandler(QObject* parent = nullptr);

					/// \brief get key pressed event of mouse
					void onMousePressed(QMouseEvent* event);
					/// \brief get moved event of mouse
					void onMouseHoverMoved(QHoverEvent* event);
					/// \brief get moved event of mouse with pressing any key
					void onMouseMoved(QMouseEvent* event);
					/// \brief get key released event of mouse
					void onMouseReleased(QMouseEvent* event);
					/// \brief get wheeled event of mouse
					void onMouseWheeled(QWheelEvent* event);
					/// \brief get key pressed event of key of keyboard
					void onKeyPressed(QKeyEvent* event);
					/// \brief get key released event of key of keyboard
					void onKeyReleased(QKeyEvent* event);
					/// \brief get touch event
					void onTouchEvent(QTouchEvent* event);

					uint16 mousePressAndHoldInterval() const;
					void   setMousePressAndHoldInterval(uint16 newMousePressAndHoldInterval);

					/// \brief Convert Qt MouseButtons flags into MouseButtons flags of the engine
					static Core::IO::MouseButtons toMouseButton(::Qt::MouseButtons mouseButtons);
					/// \brief Convert MouseButtons flags of the engine flags into Qt MouseButtons
					static ::Qt::MouseButtons toQMouseButton(Core::IO::MouseButtons mouseButtons);

					/// \brief Convert Qt KeyboardModifiers flags into KeyboardModifiers flags of the engine
					static Core::IO::KeyboardModifiers toKeyboardModifier(
						::Qt::KeyboardModifiers keyboardModifiers);
					/// \brief Convert KeyboardModifiers flags of the engine flags into Qt KeyboardModifiers
					static ::Qt::KeyboardModifiers toQKeyboardModifier(
						Core::IO::KeyboardModifiers keyboardModifiers);

				protected:
					void timerEvent(QTimerEvent* event) override;

				signals:
					/// \brief emit when is pressed any buttons of mouse
					void mousePressed(QMouseEvent* event);
					/// \brief emit when is moved the mouse
					void mouseHoverMoved(QHoverEvent* event);
					/// \brief emit when is moved the mouse with pressing any key
					void mouseMoved(QMouseEvent* event);
					/// \brief emit when is released any buttons of mouse
					void mouseReleased(QMouseEvent* event);
					/// \brief emit when is released any buttons of mouse
					void mouseClicked(QMouseEvent* event);
					/// \brief emit when is wheeled the mouse
					void mouseWheeled(QWheelEvent* event);
					/// \brief emit when is pressed any keys of keyboard
					void keyPressed(QKeyEvent* event);
					/// \brief emit when is released any keys of keyboard
					void keyReleased(QKeyEvent* event);
					/// \brief emit when mouse or touch pressed more than
					void longPressed();

				protected:
					static const uint16 s_mousePressAndHoldInterval;
					uint16				m_mousePressAndHoldInterval;
					QEvent::Type		m_lastEventType;
					QPointF				m_lastMousePoint;
					QBasicTimer			m_pressTimer;
				};

				using InputHandlerRef = std::shared_ptr<DGE::QtWrapper::QtDCore::IO::InputHandler>;
			} // namespace IO
		}	  // namespace QtDCore
	}		  // namespace QtWrapper
} // namespace DGE
