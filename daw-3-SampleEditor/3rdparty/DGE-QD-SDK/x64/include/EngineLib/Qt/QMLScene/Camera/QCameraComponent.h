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

#include "Qt/QEngineDef.h"
#include <Core/Utility/Singleton/SingletonMacrosDef.h>

#include <Core/MemoryManager/UndefMemoryMacros.h>

#include <QVector3D>
#include <QtCore/QObject>

#include <Core/MemoryManager/RedefMemoryMacros.h>

#include <Qt/QMLScene/Camera/QCameraLens.h>
#include <Qt/QMLScene/QComponent3D.h>

#include <Scene/Components/PredefinedComponents.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			/// \brief Camera Component is used for adding Camera in QML codes
			class DGE_QtAPI QCameraComponent : public QMLScene::QComponent3D
			{
				D_OBJECT(QCameraComponent)

				Q_OBJECT
				QML_ELEMENT
				/// \brief Define Qt and Engine properties and bind them
				D_QT_PROPERTY(DGE::Scene::CameraComponentRef, cameraObject, CameraObject)
				D_QT_PROPERTY_BINDING(float, roll, Roll, cameraObject)
				D_QT_PROPERTY_BINDING(float, pitch, Pitch, cameraObject)
				D_QT_PROPERTY_BINDING(float, yaw, Yaw, cameraObject)
				D_QT_PROPERTY_BINDING(QVector3D, glm::vec3, position, Position, cameraObject,
									  convertVector3D)
				D_QT_PROPERTY_BINDING(QVector3D, glm::vec3, lookatPoint, LookatPoint, cameraObject,
									  convertVector3D)
				D_QT_READONLY_PROPERTY_BINDING(QVector3D, glm::vec3, lookatVector, LookatVector,
											   cameraObject, convertVector3D)
				D_QT_READONLY_PROPERTY_BINDING(QVector3D, glm::vec3, upVector, UpVector,
											   cameraObject, convertVector3D)
				D_QT_READONLY_PROPERTY_BINDING(QVector3D, glm::vec3, rightVector, RightVector,
											   cameraObject, convertVector3D)
				D_QT_PROPERTY_BINDING(bool, isUsingPivot, IsUsingPivot, cameraObject)
				D_QT_PROPERTY_BINDING(bool, isLockedRotationX, IsLockedRotationX, cameraObject)
				D_QT_PROPERTY_BINDING(bool, isLockedRotationY, IsLockedRotationY, cameraObject)
				D_QT_PROPERTY_BINDING(bool, isLockedRotationZ, IsLockedRotationZ, cameraObject)
				D_QT_PROPERTY_BINDING(DGE::QtWrapper::QMLScene::QCameraLens*, GFX::CameraLensRef,
									  cameraLens, CameraLens, cameraObject, convertCameraLens)

			public:
				explicit QCameraComponent(QNode3D* parent				= nullptr,
										  bool	   createCameraInstance = true);

				D_Destructor(QCameraComponent);

				Q_INVOKABLE void setRotation(QVector3D direction, float distance);

				QVector3D mouseToSpace(QVector2D mousePos, QVector3D& position);

			protected:
				glm::vec3 convertVector3D(QVector3D inputValue) const;
				QVector3D convertVector3D(glm::vec3 inputValue) const;

				GFX::CameraLensRef convertCameraLens(
					DGE::QtWrapper::QMLScene::QCameraLens* qCameraLens) const;
				DGE::QtWrapper::QMLScene::QCameraLens* convertCameraLens(
					GFX::CameraLensRef cameraLensRef) const;
			};
		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
