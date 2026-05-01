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
			/// \brief Render a Cylinder with specified radius1 and radius2
			class DGE_GFXAPI Cylinder : public DGE::GFX::Primitives::Geometry
			{
				D_OBJECT(Cylinder)
			public:
				Cylinder(float radius1 = 1.0f, float radius2 = 0.1f, float radiusSteps = 2,
						 float thetaSteps = 16);
				D_Destructor(Cylinder);

				// Geometry interface

				virtual std::shared_ptr<IGObjectDrawable> clone(uint16 lod) const override;

			protected:
				virtual void regenerateVertices(bool mustBeSync) override;

			protected:
				/// \brief greater radius of cylinder
				D_PROPERTY(float, radius1, Radius1)
				/// \brief smaller radius of cylinder
				D_PROPERTY(float, radius2, Radius2)
				/// \brief number of steps for generating circle of cylinder
				D_PROPERTY(float, thetaSteps, ThetaSteps)
				/// \brief number of steps for generating body of cylinder
				D_PROPERTY(float, radiusSteps, RadiusSteps)
			};
			using CylinderRef = std::shared_ptr<DGE::GFX::Primitives::Cylinder>;
		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
