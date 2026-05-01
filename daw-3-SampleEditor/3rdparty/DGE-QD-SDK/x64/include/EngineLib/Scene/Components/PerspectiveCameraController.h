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

#include "CameraController.h"

namespace DGE
{
	namespace Scene
	{
		///
		class DGE_SceneAPI PerspectiveCameraController : public DGE::Scene::CameraController
		{
		public:
			PerspectiveCameraController(Core::System::ContextRef context);
			PerspectiveCameraController(Scene::CameraComponentRef cameraObj,
										Core::System::ContextRef  context);

			using Scene::CameraController::initialize;

			// AbstractCameraController interface
			virtual bool lookingAround(float mouseX, float mouseY) override;
			virtual bool move(CameraMovementType cameraMovementType) override;
			virtual bool zoom(float zoomLevel, Vector2D zoomCenterPoint = Vector2D(-1, -1)) override;

		protected:
			virtual bool updateLookingAround(float elapsed) override;
			virtual bool updatePosition(float elapsed) override;
			virtual bool updateZoom(float elapsed) override;

			float lerpValue(float begin, float& totalStep, float elapsedInSec, float newStep);

			/// \brief filter unusable zoom value
			float normalizeZoomValue(float zoomValue, float currentDistance, float minimumDistance);
			/// \brief set position with using pivot (look at point) and distance to it
			void setPositionWithPivot(float distance);

		protected:
			/// \brief Distance to zero value in comparison with zero
			static const float s_nearZeroDistance;
			/// \brief Distance to center point that will be used for zoom in/out
			static const float s_distanceToCenterPoint;
			/// \brief Minimum value for distance to center point that will be used for zoom in/out
			static const float s_minimumDistanceToCenterPoint;
			/// \brief FPS step for update property
			static const float s_fpsStep;

			/// \brief remain values for update roll property of camera
			float m_remainRollValue;
			/// \brief remain values for update pitch property of camera
			float m_remainPitchValue;
			/// \brief remain values for update yaw property of camera
			float m_remainYawValue;

			/// \brief remain values for update forward-backward movement of camera
			float m_remainFBValue;
			/// \brief remain values for update left-right movement of camera
			float m_remainLRValue;
			/// \brief remain values for update up-down movement of camera
			float m_remainUDValue;
			/// \brief remain values for update up-down movement of camera
			float m_remainZoomValue;

			D_PROPERTY(float, mouseMovementX, MouseMovementX, 0);
			D_PROPERTY(float, mouseMovementY, MouseMovementY, 0);
			D_PROPERTY(float, zoomLevel, ZoomLevel, 0);
			D_PROPERTY(float, forwardBackwardMovement, ForwardBackwardMovement, 0);
			D_PROPERTY(float, leftRightMovement, LeftRightMovement, 0);
			D_PROPERTY(float, upDownMovement, UpDownMovement, 0);

			D_PROPERTY(bool, isLockedMouseMovementX, IsLockedMouseMovementX, false);
			D_PROPERTY(bool, isLockedMouseMovementY, IsLockedMouseMovementY, false);

			D_PROPERTY(bool,
					   isLockedForwardBackwardMovement,
					   IsLockedForwardBackwardMovement,
					   false);
			D_PROPERTY(bool, isLockedLeftRightMovement, IsLockedLeftRightMovement, false);
			D_PROPERTY(bool, isLockedUpDownMovement, IsLockedUpDownMovement, false);

			D_PROPERTY(bool, isLockedZoom, IsLockedZoom, false);

			D_PROPERTY(bool, isUsingRoll, IsUsingRoll, false);

			D_PROPERTY(float, distanceToCenterPoint, DistanceToCenterPoint, s_distanceToCenterPoint);
			D_PROPERTY(float,
					   minimumDistanceToCenterPoint,
					   MinimumDistanceToCenterPoint,
					   s_minimumDistanceToCenterPoint);
		};

		using PerspectiveCameraControllerRef = std::shared_ptr<Scene::PerspectiveCameraController>;
	} // namespace Scene
} // namespace DGE
