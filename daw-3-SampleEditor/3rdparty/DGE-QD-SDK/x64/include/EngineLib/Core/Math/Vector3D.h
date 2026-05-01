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
#include <glm/detail/qualifier.hpp>
#include <glm/glm.hpp>
#endif

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
			class Vector2D;

			/// \brief Use this structure for storing 3d position
			class DGE_CoreAPI Vector3D
			{
			public:
				Vector3D() = default;

				Vector3D(float value);

				Vector3D(int value);

				Vector3D(float fx, float fy, float fz);

				// Vector3D(const Vector3D& v);
#ifdef DGE_USE_GLM
				Vector3D(const glm::vec3& v);
#endif

				Vector3D(const float* pf);

				// casting
				operator float*();
#ifdef DGE_USE_GLM
				operator glm::vec3() const;
#endif

				// Vector3D& operator=(const Vector3D& v);

				Vector3D& operator+=(float f);

				Vector3D& operator+=(const Vector3D& v);

				Vector3D& operator-=(float f);

				Vector3D& operator-=(const Vector3D& v);

				Vector3D& operator*=(float f);

				Vector3D& operator*=(const Vector3D& v);
#ifdef DGE_USE_GLM
				Vector3D& operator*=(const glm::mat4& v);
#endif

				Vector3D& operator/=(float f);

				Vector3D& operator/=(const Vector3D& v);

				Vector3D operator+() const;

				Vector3D operator+(const Vector3D& v) const;

				Vector3D operator-() const;

				Vector3D operator-(const Vector3D& v) const;

				Vector3D operator*(float f) const;

				Vector3D operator*(const Vector3D& v) const;

				Vector3D operator/(float f) const;

				Vector3D operator/(int devider) const;

				Vector3D operator/(const Vector3D& v) const;

				bool operator==(const Vector3D& v) const;

				bool operator!=(const Vector3D& v) const;

				bool operator>(const Vector3D& v) const;

				bool operator<(const Vector3D& v) const;

				bool operator>=(const Vector3D& v) const;

				bool operator<=(const Vector3D& v) const;
#ifdef min
#undef min
#endif
				Vector3D min(const Vector3D& v) const;
#ifdef max
#undef max
#endif
				Vector3D max(const Vector3D& v) const;

				float dot(const Vector3D& v) const;

				Vector3D cross(const Vector3D& v) const;

				/// \brief Get length of this Vector3D
				float length() const;

				/// \brief Get normalized Vector3D
				Vector3D normalize() const;

				/// \brief Convert this Vector3D to Vector4D
				Vector2D toVector2D(uint16 deleteAxis = 2);

				static Vector3D lerp(const Vector3D& start, const Vector3D& end, float delta);

#ifdef DGE_Use_PhysXEngine
//				 Vector3D(physx::PxVec3 px)
//				{
//					x = px.x;
//					y = px.y;
//					z = px.z;
//				}
//				 operator PxVec3()
//				{
//					return PxVec3(x, y, z);
//				}
#endif

				float x() const;
				void  setX(float newX);

				float y() const;
				void  setY(float newY);

				float z() const;
				void  setZ(float newZ);

				/// \brief Return min value between values for x,y,z
				float minValue() const;

				/// \brief Return max value between values for x,y,z
				float maxValue() const;

				/// \brief Return inverted vector3d of this vector3d
				Vector3D invert() const;

				/// \brief Return logical NOT of elements of this vector3d
				Vector3D logicNot() const;

				/// \brief swap value of element of vector3d at specified index
				Vector3D swapElement(uint8 sourceIndex, uint8 destinationIndex);

				/// \brief Convert this vector3d into string
				DString toString() const;

			public:
				static const Vector3D s_zero;

			protected:
				/// \brief x position in Cartesian
				float m_x = 0.0f;

				/// \brief y position in Cartesian
				float m_y = 0.0f;

				/// \brief z position in Cartesian
				float			m_z = 0.0f;
				friend ostream& operator<<(ostream& os, const Vector3D& vec3Value);
			};

			inline ostream& operator<<(ostream& os, const Vector3D& vec3Value)
			{
				os << "(" << vec3Value.m_x << "," << vec3Value.m_y << "," << vec3Value.m_z << ")";
				return os;
			}

			inline Vector3D operator*(double t, const Vector3D& v)
			{
				return Vector3D(t * v.x(), t * v.y(), t * v.z());
			}

			inline Vector3D operator*(const Vector3D& v, double t)
			{
				return t * v;
			}

			inline Vector3D operator/(double t, const Vector3D& v)
			{
				return Vector3D(t / v.x(), t / v.y(), t / v.z());
			}

			inline Vector3D operator/(const Vector3D& v, double t)
			{
				return v / t;
			}

			extern DGE_CoreExtern Vector3D abs(Vector3D value);

			extern DGE_CoreExtern bool fuzzyCompare(Vector3D p1, Vector3D p2, Vector3D resolution);
		} // namespace Math
	}
}
