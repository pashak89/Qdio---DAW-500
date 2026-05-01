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

#include "Core/Math/Vector3D.h"
#include "Geometry.h"
#include <Core/System/DProperty.h>

namespace DGE
{
	namespace GFX
	{
		namespace Primitives
		{
			/// \brief Draw a line between point1 and point2
			class DGE_GFXAPI Line : public DGE::GFX::Primitives::Geometry
			{
				D_OBJECT(Line)
			public:
				Line();
				Line(Core::Math::Vector3D point1, Core::Math::Vector3D point2);

				virtual std::shared_ptr<IGObjectDrawable> clone(uint16 lod) const override;

			protected:
				/// \brief regenerate vertices for this geometry
				void regenerateVertices(bool mustBeSync = false) override;

			protected:
				D_PROPERTY(Core::Math::Vector3D, point1, Point1, Core::Math::Vector3D{0});
				D_PROPERTY(Core::Math::Vector3D, point2, Point2, Core::Math::Vector3D{0});
			};
			using LineRef  = std::shared_ptr<DGE::GFX::Primitives::Line>;
			using LineWRef = std::weak_ptr<DGE::GFX::Primitives::Line>;
		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
