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

#include "Bound.h"
#include "Matrix.h"
#include "Vector.h"
#include "Vertex.h"

namespace DGE
{
	namespace Core
	{
		namespace Math
		{

#ifndef FLT_EPSILON
#define FLT_EPSILON __FLT_EPSILON__
#endif
#if DGE_Platform == DGE_Windows_Platform && defined(DGE_UseDirectxMath)
#define ToRECT(P)                                           \
	RECT                                                    \
	{                                                       \
		(P).x, (P).y, (P).x + (P).width, (P).y + (P).height \
	}
#endif
			/*template<typename T>
			class Rectangle
			{
			public:
				T x1 = 0, y1 = 0, x2 = 0, y2 = 0, width = 0, heigth = 0;
				Rectangle() {}
				Rectangle(T x1, T y1, T x2, T y2)
				{

		}
		Rectangle(Bound rc)
		{
			x1	   = rc.x;
			x2	   = rc.x + rc.width;
			y1	   = rc.y;
			y2	   = rc.y + rc.height;
			width = abs<T>(x1, x2);
			heigth = abs<T>(y1, y2);
		}
#ifdef DGE_UseGraphicAPI
#if DGE_Platform==DGE_Windows_Platform && defined(DGE_UseDirectxMath)
		inline operator const D3DRECT()
		{
			return D3DRECT{ x1, y1, x2, y2 };
		}
#else

#endif
#endif
	};

	using RectangleF = Rectangle<float>;*/
			template<typename T, T zero = (T) 0>
			T abs(T value)
			{
				return value > zero ? value : -value;
			}
			template<typename T>
			T abs(T a, T b)
			{
				return a > b ? a - b : b - a;
			}
#ifndef min
			template<typename T>
			T min(const T& a, const T& b)
			{
				return a > b ? b : a;
			}
#endif

#ifndef max
			template<typename T>
			T max(const T& a, const T& b)
			{
				return a < b ? b : a;
			}
			template<typename T>
			T lerp(const T& start, const T& end, float delta)
			{
				T v;
				v = start * (1.0f - delta) + end * (delta);
				return v;
			}
#endif

			template<typename T>
			bool isInBound(T lower, T upper, T value)
			{
				return Math::abs(2 * value - (upper + lower)) < (upper - lower);
			}

			template<typename T>
			bool isInBound(T distance, T value)
			{
				return Math::abs(value) <= distance;
			}

			template<typename T>
			bool fuzzyCompare(T p1, T p2, T resolution)
			{
				return (Math::abs(p1 - p2) * resolution <= Math::min(Math::abs(p1), Math::abs(p2)));
			}

			extern DGE_CoreExtern bool fuzzyCompare(double p1, double p2,
													float resolution = 1000000000000.0f);

			extern DGE_CoreExtern bool fuzzyCompare(float p1, float p2,
													float resolution = 100000.0f);
#define stringToInt(s) strtol(s, nullptr, 10)
#define stringToUInt(s) strtoul(s, nullptr, 10)
#define stringToFloat(s) strtof(s, nullptr)
#define stringToDouble(s) strtod(s, nullptr)
		} // namespace Math
	}	  // namespace Core
} // namespace DGE
