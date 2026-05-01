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
#include "Core/Utility/Singleton/SingletonMacrosDef.h"
#include <Core/System/DObject.h>
#include <Core/System/DObjectMacrosDef.h>
#include <Core/System/Event.h>

namespace DGE
{
	namespace GFX
	{
		class DGE_GFXAPI OrthoLens : public GFX::CameraLens
		{
			D_OBJECT(OrthoLens)
		public:
			OrthoLens();
			D_Destructor(OrthoLens);

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
			static inline const float s_defaultLeft	  = -1.0f;
			static inline const float s_defaultRight  = 1.0f;
			static inline const float s_defaultTop	  = -1.0f;
			static inline const float s_defaultBottom = 1.0f;

		protected:
			D_PROPERTY(float, left, Left, s_defaultLeft)
			D_PROPERTY(float, right, Right, s_defaultRight)
			D_PROPERTY(float, top, Top, s_defaultTop)
			D_PROPERTY(float, bottom, Bottom, s_defaultBottom)

			D_READONLY_PROPERTY(std::atomic_bool, isChanged, bool)
		};

		using OrthoLensRef = std::shared_ptr<GFX::OrthoLens>;
	} // namespace GFX
} // namespace DGE
