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
#ifdef DGE_USE_GLM
#include <glm/glm.hpp>
#endif

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
			class Vector3D;
#if DGE_Platform == DGE_Windows_Platform
			// typedef DWORD Color;
#endif
			/// \brief Use this structure for storing 2d position
			class DGE_CoreAPI Vector2D
			{

			public:

				Vector2D();

				Vector2D(float fx, float fy);

				Vector2D(const float* pf);

				Vector2D(const Vector2D& v);

				operator float*();

				Vector2D& operator=(const Vector2D& v);

				Vector2D& operator+=(const Vector2D& v);

				Vector2D& operator-=(const Vector2D& v);

				Vector2D& operator*=(float f);

				Vector2D& operator/=(float f);

				Vector2D operator+() const;

				Vector2D operator+(const Vector2D& v) const;

				Vector2D operator-() const;

				Vector2D operator-(const Vector2D& v) const;

				Vector2D operator*(float f) const;

				Vector2D operator/(float f) const;

				Vector2D& operator()(float fx, float fy);

				bool operator==(const Vector2D& v) const;

				bool operator!=(const Vector2D& v) const;

				bool operator>=(const Vector2D& v) const;
				bool operator>(const Vector2D& v) const;
				bool operator<=(const Vector2D& v) const;
				bool operator<(const Vector2D& v) const;

				//				 Vector2D operator*(float f, const struct Vector2D& v)
				//				{
				//					return Vector2D(f * v.x, f * v.y);
				//				}

				/// \brief Get length of this Vector2D
				float length();

				/// \brief Get normalized Vector2D
				Vector2D normalize();

				/// \brief Return represented of this Vector2D in a String
				DString toString();

				/// \brief Convert this Vector2D to Vector3D
				Vector3D toVector3D(float valueAxis, uint8 insertAxis);

				float x() const;
				void  setX(float newX);

				float y() const;
				void  setY(float newY);

			protected:
				/// \brief x position in Cartesian
				float m_x;

				/// \brief y position in Cartesian
				float m_y;
			};
		} // namespace Math
	}
}
