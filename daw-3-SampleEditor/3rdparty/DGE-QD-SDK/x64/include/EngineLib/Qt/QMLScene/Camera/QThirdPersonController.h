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

#include <Qt/QMLScene/Camera/QPerspectiveCameraController.h>
#include <Qt/QMLScene/QComponent3D.h>

#include <Scene/Components/ThirdPersonCameraController.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			class QThirdPersonController
				: public DGE::QtWrapper::QMLScene::QPerspectiveCameraController
			{
				D_OBJECT(QThirdPersonController)
				D_RegisterComponent(QThirdPersonController, Scene::ThirdPersonCameraController);

				Q_OBJECT
				QML_ELEMENT

				D_QT_PROPERTY_BINDING(float,
									  distanceToCenterPoint,
									  DistanceToCenterPoint,
									  cameraController)
			public:
				QThirdPersonController(QNode3D* parent = nullptr);

				explicit QThirdPersonController(
					Scene::ThirdPersonCameraControllerRef cameraController,
					QNode3D*							  parent = nullptr);

			protected:
				void updateComponent(Scene::ComponentRef componentRef);

			private:
				/// \brief Do common initialize tasks of this class constructor
				void commonInitConstructor();
			};

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
