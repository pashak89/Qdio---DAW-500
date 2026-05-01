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

#include <GFX/Primitives/Arrow.h>

namespace DGE
{
	namespace GFX
	{
		namespace Primitives
		{
			/// \brief Render 3 axes with specified length,headLength,doubleHead
			class ThreeAxes : public DGE::GFX::Primitives::Arrow
			{
				D_OBJECT(ThreeAxes)
			public:
				/// \brief Constructort of ThreeAxes object
				///
				/// \param length size of ThreeAxes body
				/// \param headLength size of ThreeAxes head
				/// \param doubleHead indicate whether head of ThreeAxes contains double lines
				ThreeAxes(float length = 1, float headLength = 0.5, bool doubleHead = false);

				D_Destructor(ThreeAxes);

				virtual std::shared_ptr<IGObjectDrawable> clone(uint16 lod) const override;

			protected:
				virtual void regenerateVertices(bool mustBeSync) override;

			protected:
			};
			using ThreeAxesRef = std::shared_ptr<DGE::GFX::Primitives::ThreeAxes>;
		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
