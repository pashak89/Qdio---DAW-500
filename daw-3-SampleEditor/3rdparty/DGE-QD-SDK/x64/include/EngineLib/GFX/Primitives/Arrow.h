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
			/// \brief Render a arraw with specified length,headLength,doubleHead
			class Arrow : public DGE::GFX::Primitives::Geometry
			{
				D_OBJECT(Arrow)
			public:
				/// \brief Constructort of Arrow object
				///
				/// \param length size of arrow body
				/// \param headLength size of Arrow head
				/// \param doubleHead indicate whether head of Arrow contains double lines
				/// \param direction set direction of arrow that is used below table:
				/// 0 direction of axis x
				/// 1 direction of axis y
				/// 2 direction of axis z
				Arrow(float length = 1, float headLength = 0.5, bool doubleHead = false,
					  uint8 direction = 0);
				D_Destructor(Arrow);

				virtual std::shared_ptr<IGObjectDrawable> clone(uint16 lod) const override;

			protected:
				/// \brief add verteces of an arrow in specified direction
				///
				/// \param arrowDirection set direction of arrow that is used below table:
				/// 0 direction of axis x
				/// 1 direction of axis y
				/// 2 direction of axis z
				void		 addArrowVerteces(uint8 arrowDirection = 0, bool clearBuffers = true);
				virtual void regenerateVertices(bool mustBeSync) override;
				void		 onPropertiesChanged();

			protected:
				/// \brief Length of Arrow
				D_PROPERTY(float, length, Length)
				/// \brief Size of Arrow head
				D_PROPERTY(float, headLength, HeadLength)
				/// \brief Indicate whether head of Arrow contains double lines
				D_PROPERTY(float, doubleHead, DoubleHead)
				/// \brief Default direction of arrow
				D_PROPERTY(uint8, direction, Direction)
			};
			using ArrowRef = std::shared_ptr<DGE::GFX::Primitives::Arrow>;
		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
