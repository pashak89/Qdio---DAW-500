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

#include "CameraLens.h"
#include <atomic>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		class DGE_GFXAPI PerspectiveLens : public GFX::CameraLens
		{
		public:
			PerspectiveLens();

			// CameraLens interface
			virtual glm::mat4 projectionMatrix() override;
			virtual bool	  isProjectMatrixChanged() override;

			virtual Core::Math::Vector3D mouseToSpace(Core::Math::Vector2D	mousePos,
													  Core::Math::Vector3D	lookatVector,
													  Core::Math::Vector3D	cameraPosition,
													  Core::Math::Vector3D	cameraUp,
													  glm::mat4				viewMatrix,
													  Core::Math::Vector3D& position) override;

		public:
			static inline const float s_defaultFOV			= 45.0f;

		protected:
			glm::mat4 m_lastProjectionMatrix;

			D_PROPERTY(float, fov, Fov, s_defaultFOV)

			D_READONLY_PROPERTY(std::atomic_bool, isChanged, bool)
		};
		using PerspectiveLensRef = std::shared_ptr<GFX::PerspectiveLens>;
	} // namespace GFX
} // namespace DGE
