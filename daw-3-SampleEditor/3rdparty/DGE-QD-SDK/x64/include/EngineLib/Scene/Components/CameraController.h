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

#include <Core/Math/Vector2D.h>
#include <Scene/Components/PredefinedComponents.h>
#include <Scene/Components/SystemComponent.h>
#include <memory>

namespace DGE
{
	using namespace Core::Math;
	namespace Scene
	{
		enum class CameraMovementType
		{
			Forward,
			Backward,
			Left,
			Right,
			Up,
			Down,
		};

		class DGE_SceneAPI CameraController : public Scene::Component
		{
			D_OBJECT(CameraController)
		public:
			CameraController(Core::System::ContextRef context);
			CameraController(Scene::CameraComponentRef cameraObj,
							 Core::System::ContextRef  context);
			D_Destructor(CameraController);

			// IRenderPipeline interface
			/// \brief Call when object must be initialized.
			using Scene::Component::initialize;

			// IInitialize interface
			bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			bool update(GFX::GCore::InputParameters& input) override;

			virtual bool lookingAround(float mouseX, float mouseY)							= 0;
			virtual bool move(CameraMovementType cameraMovementType) = 0;
			virtual bool zoom(float				   zoomLevel,
							  Core::Math::Vector2D zoomCenterPoint = Core::Math::Vector2D(-1, -1))
				= 0;

		protected:
			virtual bool updateLookingAround(float elapsed) = 0;
			virtual bool updatePosition(float elapsed)		= 0;
			virtual bool updateZoom(float elapsed)			= 0;

			float getSpeed(float speed);

		protected:
			// Scene::CameraComponentRef m_cameraObject;
			// bool  m_mustInitializeMouse = false;
			// bool  m_limitPitch			= false;
			// bool  m_isMoveFaster		= false;
			// float m_lastX				= 0;
			// float m_lastY				= 0;
			// float m_speedX				= 0.01;
			// float m_speedY				= 0.01;
			// float m_zoomSpeed			= 1;
			// float m_movementSpeed		= 0.1;
			// float m_shiftMultiply		= 10;
			// float m_maxElapsedTime		= 5;

			static float s_rotateViewSpeedX;
			static float s_rotateViewSpeedY;
			static float s_zoomSpeed;
			static float s_moveSpeed;
			static float s_multiplySpeed;

			D_PROPERTY(Scene::CameraComponentRef, cameraObject, CameraObject)
			D_PROPERTY(bool, mustInitializeMouse, MustInitializeMouse, false)
			D_PROPERTY(bool, limitPitch, LimitPitch, false)
			D_PROPERTY(bool, isMoveFaster, IsMoveFaster, false)
			D_PROPERTY(float, lastX, LastX, 0.0f)
			D_PROPERTY(float, lastY, LastY, 0.0f)
			D_PROPERTY(float, speedX, SpeedX, s_rotateViewSpeedX)
			D_PROPERTY(float, speedY, SpeedY, s_rotateViewSpeedY)
			D_PROPERTY(float, zoomSpeed, ZoomSpeed, s_zoomSpeed)
			D_PROPERTY(float, movementSpeed, MovementSpeed, s_moveSpeed)
			D_PROPERTY(float, shiftMultiply, ShiftMultiply, s_multiplySpeed)
			D_PROPERTY(float, maxElapsedTime, MaxElapsedTime, 5.0f)
			D_PROPERTY(Core::Math::Vector2D,
					   zoomCenterPoint,
					   ZoomCenterPoint,
					   Core::Math::Vector2D(-1, -1))
			std::atomic_bool m_isChanged;
		};
		using CameraControllerRef = std::shared_ptr<Scene::CameraController>;
		using CameraControllerWRef = std::weak_ptr<Scene::CameraController>;
	} // namespace Scene
} // namespace DGE
