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

#include <Core/System/DObject.h>
#include <Core/System/DProperty.h>
#include <GFX/Primitives/Geometry.h>

namespace DGE
{
	namespace GFX
	{
		namespace Primitives
		{
			/// \brief Render a Triangle with specified p1,p2,p3 (three points)
			class DGE_GFXAPI Triangle : public Primitives::Geometry
			{
				D_OBJECT(Triangle)
			public:
				Triangle();
				Triangle(Core::Math::Vector3D p1, Core::Math::Vector3D p2, Core::Math::Vector3D p3);
				Triangle(Core::Math::Vector3D p1, Core::Math::Vector3D p2);

				virtual std::shared_ptr<IGObjectDrawable> clone(uint16 lod) const override;

				// Geometry interface
			protected:
				virtual void regenerateVertices(bool mustBeSync) override;

			protected:
				D_PROPERTY(Core::Math::Vector3D, point1, Point1)
				D_PROPERTY(Core::Math::Vector3D, point2, Point2)
				D_PROPERTY(Core::Math::Vector3D, point3, Point3)
			};

			using TriangleRef = std::shared_ptr<DGE::GFX::Primitives::Triangle>;
		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
