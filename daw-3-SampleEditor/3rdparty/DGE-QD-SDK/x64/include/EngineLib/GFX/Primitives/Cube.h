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

#include <GFX/Primitives/Geometry.h>

#include <Core/Math/Vector.h>
#include <Core/System/DProperty.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		namespace Primitives
		{
			/// Render a Cube with specified width,height,depth
			class DGE_GFXAPI Cube : public Primitives::Geometry
			{
				D_OBJECT(Cube)
			public:
				Cube(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
				D_Destructor(Cube);

				virtual std::shared_ptr<IGObjectDrawable> clone(uint16 lod) const override;

			protected:
				/// \brief regenerate vertices for this geometry
				void regenerateVertices(bool mustBeSync = false) override;

			protected:
				/// \brief Width of Cube;
				D_PROPERTY(float, width, Width)
				/// \brief Height of Cube;
				D_PROPERTY(float, height, Height)
				/// \brief Depth of Cube;
				D_PROPERTY(float, depth, Depth)
				/// \brief Indicate whether this cube is drawn compactly
				D_PROPERTY(bool, isCompact, IsCompact)
				/// \brief Indicate whether this cube is drawn for showing boundary of a simple
				/// wire-frame cube
				D_PROPERTY(bool, isSimpleBoundOnly, IsSimpleBoundOnly)
			};
			using CubeRef = std::shared_ptr<DGE::GFX::Primitives::Cube>;
		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
