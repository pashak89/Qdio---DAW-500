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

#include <QEvent>
#include <QMouseEvent>
#include <QtCore/QObject>
#include <Qt/QMLScene/Camera/QCameraController.h>

#include <Core/Utility/Interfaces/IInitialize.h>
#include <Engine/Managers/Environment3DManager.h>
#include <Engine/Managers/InputEventsManager.h>
#include <Qt/QtModuleDef.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QtDManagers
		{
			/// \brief Wrapper Qt class for Environment3DManager in the engine
			class DGE_QtAPI QEnvironment3DManager
				: public QObject
				, public Interfaces::IInitialize
			{
				Q_OBJECT
			public:
				explicit QEnvironment3DManager(QObject* parent = nullptr);

				// IInitialize interface
				virtual bool initialize(
					Core::Utility::Interfaces::InitializeParams& parameters) override;

				DGE::QtWrapper::QMLScene::QCameraController* cameraController() const;
				void										 setCameraController(
															DGE::QtWrapper::QMLScene::QCameraController* newCameraController);

				bool isActivated() const;
				void setIsActivated(bool newIsActivated);

			public slots:
				/// \brief Get clicked event of mouse
				void onMouseClicked(QMouseEvent* event);
				/// \brief Get key pressed event of mouse
				void onMousePressed(QMouseEvent* event);
				/// \brief Get moved event of mouse
				void onMouseHoverMoved(QHoverEvent* event);
				/// \brief Get moved event of mouse with pressing any key
				void onMouseMoved(QMouseEvent* event);
				/// \brief Get key released event of mouse
				void onMouseReleased(QMouseEvent* event);
				/// \brief Get wheeled event of mouse
				void onMouseWheeled(QWheelEvent* event);
				/// \brief Get key pressed event of key of keyboard
				void onKeyPressed(QKeyEvent* event);
				/// \brief Get key released event of key of keyboard
				void onKeyReleased(QKeyEvent* event);
				/// \brief Get touch event
				void onTouchEvent(QTouchEvent* event);
				/// \brief Get selected entity changed
				void onSelectedEntityChanged(Scene::DEntityRef);

			signals:
				/// \brief Emit when is pressed any buttons of mouse
				void mousePressed(QMouseEvent* event);
				/// \brief Emit when is moved the mouse
				void mouseHoverMoved(QHoverEvent* event);
				/// \brief Emit when is moved the mouse with pressing any key
				void mouseMoved(QMouseEvent* event);
				/// \brief Emit when is released any buttons of mouse
				void mouseReleased(QMouseEvent* event);
				/// \brief Emit when is released any buttons of mouse
				void mouseClicked(QMouseEvent* event);
				/// \brief Emit when is wheeled the mouse
				void mouseWheeled(QWheelEvent* event);
				/// \brief Emit when is pressed any keys of keyboard
				void keyPressed(QKeyEvent* event);
				/// \brief Emit when is released any keys of keyboard
				void keyReleased(QKeyEvent* event);
				/// \brief Emit when touch event occurred
				void touchEvent(QTouchEvent* event);
				/// \brief Emit when selected entity changed
				void selectedEntityChanged(Scene::DEntityRef);

			protected:
				/// \brief The environment 3d manager object of the engine
				DGE::Managers::Environment3DManagerRef m_environment3dManager;
				/// \brief The input events manager object of the engine
				DGE::Managers::InputEventsManagerWRef m_inputEventsManager;
				/// \brief The camera controller object
				DGE::QtWrapper::QMLScene::QCameraController* m_cameraController = nullptr;
				/// \brief Indicate whether this object is activated
				atomic_bool m_isActivated;
				Managers::Environment3DInfo m_envInfo;
			};
		} // namespace QtDManagers
	}	  // namespace QtWrapper
} // namespace DGE
