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

#include <Qt/QMLScene/Camera/QCameraController.h>

#include <Scene/Components/FpsCameraController.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{

			class QPerspectiveCameraController : public DGE::QtWrapper::QMLScene::QCameraController
			{
				D_OBJECT(QPerspectiveCameraController)
				Q_OBJECT
				QML_ELEMENT

			protected:
				D_QT_PROPERTY(Scene::PerspectiveCameraControllerRef,
							  cameraController,
							  CameraController)
				D_QT_PROPERTY_BINDING(DGE::QtWrapper::QMLScene::QCameraComponent *,
									  DGE::Scene::CameraComponentRef, cameraObject, CameraObject,
									  cameraController, convertCameraObject)

				D_QT_PROPERTY_BINDING(bool,
									  isLockedMouseMovementX,
									  IsLockedMouseMovementX,
									  cameraController)
				D_QT_PROPERTY_BINDING(bool,
									  isLockedMouseMovementY,
									  IsLockedMouseMovementY,
									  cameraController)
				D_QT_PROPERTY_BINDING(bool,
									  isLockedForwardBackwardMovement,
									  IsLockedForwardBackwardMovement,
									  cameraController)
				D_QT_PROPERTY_BINDING(bool,
									  isLockedLeftRightMovement,
									  IsLockedLeftRightMovement,
									  cameraController)
				D_QT_PROPERTY_BINDING(bool,
									  isLockedUpDownMovement,
									  IsLockedUpDownMovement,
									  cameraController)
				D_QT_PROPERTY_BINDING(bool, isLockedZoom, IsLockedZoom, cameraController)
				D_QT_PROPERTY_BINDING(bool, isUsingRoll, IsUsingRoll, cameraController)
			public:
			public:
				QPerspectiveCameraController(QNode3D *parent = nullptr);
				explicit QPerspectiveCameraController(
					Scene::PerspectiveCameraControllerRef cameraController,
					QNode3D								 *parent = nullptr);

				// QCameraController interface
				virtual QCameraComponent *getCameraComponnent() override;

				Q_INVOKABLE virtual void rotate(float x, float y) override;
				Q_INVOKABLE
				virtual void			 zoom(float zoomValue) override;
				Q_INVOKABLE virtual void moveLeftRight(float value) override;
				Q_INVOKABLE virtual void moveUpDown(float value) override;

			protected:
				void resetKeyMap();
				/// \brief Do common initialize tasks of this class constructor
				void commonInitConstructor();
				/// \brief Call when the camera controller changed
				void onCameraControllerChanged();

			protected:
				DGE::Scene::CameraComponentRef convertCameraObject(
					DGE::QtWrapper::QMLScene::QCameraComponent *qCameraComponent) const;
				DGE::QtWrapper::QMLScene::QCameraComponent *convertCameraObject(
					DGE::Scene::CameraComponentRef cameraComponentRef) const;

			public slots:
				virtual void onMousePressed(QMouseEvent *event) override;
				virtual void onMouseMoved(QMouseEvent *event) override;
				virtual void onMouseReleased(QMouseEvent *event) override;
				virtual void onMouseWheeled(QWheelEvent *event) override;
				virtual void onKeyPressed(QKeyEvent *event) override;
				virtual void onKeyReleased(QKeyEvent *event) override;

			signals:
			protected:
				std::map<int, Scene::CameraMovementType> m_keysMap;
			};

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
