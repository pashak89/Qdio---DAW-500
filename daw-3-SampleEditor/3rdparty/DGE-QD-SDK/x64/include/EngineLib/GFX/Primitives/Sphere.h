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

#include <Core/System/DProperty.h>
#include <GFX/Primitives/Geometry.h>

namespace DGE
{
	namespace GFX
	{
		namespace Primitives
		{
			/// \brief Render a Sphere with specified radius
			class DGE_GFXAPI Sphere : public DGE::GFX::Primitives::Geometry
			{
				D_OBJECT(Sphere)
			public:
				Sphere(float radius = 1.0f, float steps = 16);
				D_Destructor(Sphere);

				virtual std::shared_ptr<IGObjectDrawable> clone(uint16 lod) const override;
				// Geometry interface
			protected:
				virtual void regenerateVertices(bool mustBeSync) override;

			protected:
				/// \brief Radius of Sphere
				D_PROPERTY(float, radius, Radius)
				D_PROPERTY(float, steps, Steps)
			};

			using SphereRef = std::shared_ptr<DGE::GFX::Primitives::Sphere>;
		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
