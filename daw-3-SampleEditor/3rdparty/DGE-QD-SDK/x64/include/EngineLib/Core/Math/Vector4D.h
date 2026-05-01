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
			class Vector2D;
			class Vector3D;

			/// \brief Use this structure for storing 4d position
			class DGE_CoreAPI Vector4D
			{

			public:
				Vector4D();
				Vector4D(float X, float Y, float Z, float W);
				~Vector4D();

				Vector4D(const float* pf);

				Vector4D(const Vector4D& vec);

				Vector4D(const Vector3D& vec, float fw);

				// casting
				operator float*();

				Vector4D& operator=(const Vector4D& v);

				// assignment operators				
				Vector4D& operator+=(const Vector4D& v);

				Vector4D& operator-=(const Vector4D& v);

				Vector4D& operator*=(float f);

				Vector4D& operator/=(float f);

				// unary operators
				Vector4D operator+() const;

				Vector4D operator-() const;

				// binary operators
				Vector4D operator+(const Vector4D& v) const;

				Vector4D operator-(const Vector4D& v) const;

				Vector4D operator*(float f) const;

				Vector4D operator/(float f) const;

				bool operator==(const Vector4D& v) const;

				bool operator!=(const Vector4D& v) const;

				//				 Vector4D operator*(float f, const Vector4D& v)
				//				{
				//					return Vector4D(f * v.x, f * v.y, f * v.z, f * v.w);
				//				}

				/// \brief Get length of this Vector4D
				float length();

				/// \brief Get normalized Vector4D
				Vector4D normalize();

#if defined(DGE_Use_PhysXEngine) && DGE_Use_PhysXEngine == 1
				Vector4D(physx::PxVec4 px)
				{
					m_x = px.x;
					m_y = px.y;
					m_z = px.z;
					m_w = px.w;
				}
				Vector4D(physx::PxQuat px)
				{
					m_x = px.x;
					m_y = px.y;
					m_z = px.z;
					m_w = px.w;
				}
				operator physx::PxVec4() { return PxVec4(m_x, m_y, m_z, m_w); }
				operator physx::PxQuat() { return PxQuat(m_x, m_y, m_z, m_w); }
#endif

#ifdef DGE_USE_GLM
				operator glm::vec4();
#endif

				float x() const;
				void  setX(float newX);

				float y() const;
				void  setY(float newY);

				float z() const;
				void  setZ(float newZ);

				float w() const;
				void  setW(float newZ);

			protected:
				/// \brief x position in Cartesian
				float m_x;

				/// \brief y position in Cartesian
				float m_y;

				/// \brief z position in Cartesian
				float m_z;
				/// \brief w position in Cartesian
				float m_w;
			};
		} // namespace Math
	}
}
