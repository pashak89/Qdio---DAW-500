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

#include <Scene/Components/FpsCameraController.h>

namespace DGE
{
	namespace Scene
	{
		class DGE_SceneAPI ThirdPersonCameraController
			: public DGE::Scene::PerspectiveCameraController
		{
		public:
			ThirdPersonCameraController(Core::System::ContextRef context);
			ThirdPersonCameraController(CameraComponentRef cameraObj, System::ContextRef context);

			virtual ~ThirdPersonCameraController();

		protected:
			/// \brief Do common initialize tasks of this class constructor
			void commonInitConstructor();

		protected:
			virtual bool updatePosition(float elapsed) override;
			virtual bool updateZoom(float elapsed) override;
		};

		using ThirdPersonCameraControllerRef = std::shared_ptr<Scene::ThirdPersonCameraController>;
	} // namespace Scene
} // namespace DGE
