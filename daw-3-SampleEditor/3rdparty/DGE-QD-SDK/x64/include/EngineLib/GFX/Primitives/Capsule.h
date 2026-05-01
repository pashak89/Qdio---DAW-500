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

#include "Geometry.h"
#include <Core/System/DProperty.h>

namespace DGE
{
	namespace GFX
	{
		namespace Primitives
		{
			/// \brief Render a Cylinder with specified radius and distance
			class DGE_GFXAPI Capsule : public DGE::GFX::Primitives::Geometry
			{
				D_OBJECT(Capsule)
			public:
				Capsule(float radius = 1.0f, float distance = 5.0f, float thetaSteps = 8,
						float distanceSteps = 8);
				D_Destructor(Capsule);

				virtual std::shared_ptr<IGObjectDrawable> clone(uint16 lod) const override;

			protected:
				virtual void regenerateVertices(bool mustBeSync) override;

			protected:
				/// \brief radius of Capsule
				D_PROPERTY(float, radius, Radius)
				/// \brief distance between two center point of Capsule
				D_PROPERTY(float, distance, Distance)
				/// \brief number of steps for generating circle of Capsule
				D_PROPERTY(float, thetaSteps, ThetaSteps)
				/// \brief number of steps for generating body of Capsule
				D_PROPERTY(float, distanceSteps, DistanceSteps)
			};
			using CapsuleRef = std::shared_ptr<DGE::GFX::Primitives::Capsule>;
		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
