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

#include "glm/ext/matrix_float4x4.hpp"
#include <Core/Math/Bound.h>
#include <Core/Math/Vector2D.h>
#include <Core/Math/Vector3D.h>
#include <Core/System/DObject.h>
#include <Core/System/DProperty.h>
#include <GFX/GFXModuleDef.h>

namespace DGE
{
	namespace GFX
	{
		enum class CameraLensType
		{
			None,
			Perspective,
			Ortho
		};

		/// \brief Base abstract class for
		class DGE_GFXAPI CameraLens : public Core::System::DObject
		{
			D_OBJECT(CameraLens)
		public:
			CameraLens(
				DGE::Core::System::ContextRef context = Core::System::Context::getInstance());

			virtual glm::mat4 projectionMatrix() = 0;

			virtual bool isProjectMatrixChanged();

			virtual Core::Math::Vector3D mouseToSpace(Core::Math::Vector2D	mousePos,
													  Core::Math::Vector3D	lookatVector,
													  Core::Math::Vector3D	cameraPosition,
													  Core::Math::Vector3D	cameraUp,
													  glm::mat4				viewMatrix,
													  Core::Math::Vector3D& position)
				= 0;

		public:
			static inline const float s_defaultZNearPlane = 0.1f;
			static inline const float s_defaultZFarPlane  = 1000.0f;
			static inline const float s_defaultWidthScreen	= 4.0f;
			static inline const float s_defaultHeightScreen = 3.0f;

		protected:
			D_PROPERTY(float, zNearPlane, ZNearPlane, s_defaultZNearPlane)
			D_PROPERTY(float, zFarPlane, ZFarPlane, s_defaultZFarPlane)
			D_PROPERTY(float, widthScreen, WidthScreen, s_defaultWidthScreen)
			D_PROPERTY(float, heightScreen, HeightScreen, s_defaultHeightScreen)
			D_PROPERTY(CameraLensType, type, Type, CameraLensType::None)
		};

		using CameraLensRef = std::shared_ptr<GFX::CameraLens>;
	} // namespace GFX
} // namespace DGE
