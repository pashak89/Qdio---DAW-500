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
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/Config/EngineConfigurator.h>
#include <Core/Math/Vector.h>

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
			/// \brief Storing Boundary of an object
			struct DGE_CoreAPI Bound
			{
			public:
				Bound(int32 X, int32 Y, int32 Width, int32 Height)
				{
					x	   = X;
					y	   = Y;
					width  = Width;
					height = Height;
				}
				Bound() { x = y = width = height = 0; }

				Bound(const Bound& bound)
				{
					x	   = bound.x;
					y	   = bound.y;
					width  = bound.width;
					height = bound.height;
				}

				bool operator==(const Bound& bound) const
				{
					return this->x == bound.x && this->y == bound.y && this->width == bound.width
						   && this->height == bound.height;
				}

				Bound& operator=(const Bound& bound)
				{
					x	   = bound.x;
					y	   = bound.y;
					width  = bound.width;
					height = bound.height;
					return *this;
				}

				/// \brief Check if position is in this bound return true else return false
				///
				/// \param position The point that want to check for exist in this boundary
				///
				/// \return Return true if this point exist in this boundary else return false
				inline bool isInBound(Vector2D position)
				{
					if (position.x() > x && position.x() < x + width)
						if (position.y() > y && position.y() < y + height)
							return true;
					return false;
				}

				/// \brief Check if position is in this bound return true else return false
				///
				/// \param X The x position of point that want to check for exist in this boundary
				/// \param Y The y position of point that want to check for exist in this boundary
				///
				/// \return Return true if this point exist in this boundary else return false
				inline bool isInBound(long X, long Y)
				{
					if (X > x && X < x + width)
						if (Y > y && Y < y + height)
							return true;
					return false;
				}

				/// \brief Set properties of this boundary
				///
				/// \param X The x position of this boundary
				/// \param Y The Y position of this boundary
				/// \param Width The width of this boundary
				/// \param Height The height of this boundary
				inline void setValue(long X, long Y, long Width, long Height)
				{
					x	   = X;
					y	   = Y;
					width  = Width;
					height = Height;
				}

				/// \brief Set size of this boundary
				///
				/// \param Width The width of this boundary
				/// \param Height The height of this boundary
				inline void setSize(long Width, long Height)
				{
					width  = Width;
					height = Height;
				}

				/// \brief Set position of this boundary
				///
				/// \param position The position of this boundary
				inline void setPosition(Vector2D position)
				{
					x = position.x();
					y = position.y();
				}

				DGE_Force_Inline long Width() { return width; }
				DGE_Force_Inline long Height() { return height; }

				const DString toString()
				{
					return DST("{x: ") + DString(x) + ", y: " + DString(y)
						   + ", width: " + DString(width) + ", height: " + DString(height) + "}";
				}

			public:
				static const Core::Math::Bound s_zero;

			public:
				int32 y = 0;
				int32 x = 0;
				int32 width;
				int32 height;
			};
		} // namespace Math
	}	  // namespace Core
} // namespace DGE
