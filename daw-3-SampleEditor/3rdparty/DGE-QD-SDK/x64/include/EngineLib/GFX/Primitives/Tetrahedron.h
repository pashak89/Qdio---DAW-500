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
 *
 */
#pragma once

#include "Geometry.h"

namespace DGE
{
	namespace GFX
	{
		namespace Primitives
		{

			class DGE_GFXAPI Tetrahedron : public DGE::GFX::Primitives::Geometry
			{
				D_OBJECT(Tetrahedron)
			public:
				Tetrahedron(float width = 1.0f, float height = 1.0f, float depth = 1.0f);

				virtual std::shared_ptr<IGObjectDrawable> clone(uint16 lod) const override;

			protected:
				/// \brief regenerate vertices for this geometry
				void regenerateVertices(bool mustBeSync = false) override;

			protected:
				/// \brief width of Tetrahedron
				D_PROPERTY(float, width, Width)
				D_PROPERTY(float, height, Height)
				D_PROPERTY(float, depth, Depth)
			};

		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
