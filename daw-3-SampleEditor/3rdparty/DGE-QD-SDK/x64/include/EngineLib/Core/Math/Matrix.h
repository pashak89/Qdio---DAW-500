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

#include "Vector.h"

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
#if defined(DGE_UseGraphicAPI)

			//===========================================================================
			//
			// Matrices
			//
			//===========================================================================
			class DGE_CoreAPI Matrix
			{
			public:
				union
				{
					struct
					{
						float _11, _12, _13, _14;
						float _21, _22, _23, _24;
						float _31, _32, _33, _34;
						float _41, _42, _43, _44;
					};
					float m[4][4];
				};

				static const Matrix Identity;
				static const Matrix Zero;

				//--------------------------
				// Matrix
				//--------------------------
				Matrix()
					: Matrix(0.0f)
				{
				}

#pragma region Matrix
				Matrix(const float* pf)
				{
#ifdef D3DX_DEBUG
					if (!pf)
						return;
#endif

					memcpy(m, pf, 16 * sizeof(float));
				}

				Matrix(const float value)
				{
					for (int i = 0; i < 4; i++)
						for (int j = 0; j < 4; j++)
							m[i][j] = value;
				}

				Matrix(float f11, float f12, float f13, float f14, float f21, float f22, float f23,
					   float f24, float f31, float f32, float f33, float f34, float f41, float f42,
					   float f43, float f44)
				{
					_11 = f11;
					_12 = f12;
					_13 = f13;
					_14 = f14;
					_21 = f21;
					_22 = f22;
					_23 = f23;
					_24 = f24;
					_31 = f31;
					_32 = f32;
					_33 = f33;
					_34 = f34;
					_41 = f41;
					_42 = f42;
					_43 = f43;
					_44 = f44;
				}

				// access grants
				float& operator()(uint8 iRow, uint8 iCol) { return m[iRow][iCol]; }

				float operator()(uint8 iRow, uint8 iCol) const { return m[iRow][iCol]; }

				// casting operators
				operator float*() { return (float*) &_11; }

				// assignment operators
				Matrix& operator*=(const Matrix&)
				{
					// D3DXMatrixMultiply(this, this, &mat);
					return *this;
				}

				Matrix& operator+=(const Matrix& mat)
				{
					_11 += mat._11;
					_12 += mat._12;
					_13 += mat._13;
					_14 += mat._14;
					_21 += mat._21;
					_22 += mat._22;
					_23 += mat._23;
					_24 += mat._24;
					_31 += mat._31;
					_32 += mat._32;
					_33 += mat._33;
					_34 += mat._34;
					_41 += mat._41;
					_42 += mat._42;
					_43 += mat._43;
					_44 += mat._44;
					return *this;
				}

				Matrix& operator-=(const Matrix& mat)
				{
					_11 -= mat._11;
					_12 -= mat._12;
					_13 -= mat._13;
					_14 -= mat._14;
					_21 -= mat._21;
					_22 -= mat._22;
					_23 -= mat._23;
					_24 -= mat._24;
					_31 -= mat._31;
					_32 -= mat._32;
					_33 -= mat._33;
					_34 -= mat._34;
					_41 -= mat._41;
					_42 -= mat._42;
					_43 -= mat._43;
					_44 -= mat._44;
					return *this;
				}

				Matrix& operator*=(float f)
				{
					_11 *= f;
					_12 *= f;
					_13 *= f;
					_14 *= f;
					_21 *= f;
					_22 *= f;
					_23 *= f;
					_24 *= f;
					_31 *= f;
					_32 *= f;
					_33 *= f;
					_34 *= f;
					_41 *= f;
					_42 *= f;
					_43 *= f;
					_44 *= f;
					return *this;
				}

				Matrix& operator/=(float f)
				{
					float fInv = 1.0f / f;
					_11 *= fInv;
					_12 *= fInv;
					_13 *= fInv;
					_14 *= fInv;
					_21 *= fInv;
					_22 *= fInv;
					_23 *= fInv;
					_24 *= fInv;
					_31 *= fInv;
					_32 *= fInv;
					_33 *= fInv;
					_34 *= fInv;
					_41 *= fInv;
					_42 *= fInv;
					_43 *= fInv;
					_44 *= fInv;
					return *this;
				}

				// unary operators
				Matrix operator+() const { return *this; }

				Matrix operator-() const
				{
					return Matrix(-_11, -_12, -_13, -_14, -_21, -_22, -_23, -_24, -_31, -_32, -_33,
								  -_34, -_41, -_42, -_43, -_44);
				}

				// binary operators
				Matrix operator*(const Matrix&) const
				{
					Matrix matT;
					// D3DXMatrixMultiply(&matT, this, &mat);
					return matT;
				}

				Matrix operator+(const Matrix& mat) const
				{
					return Matrix(_11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
								  _21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
								  _31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
								  _41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44);
				}

				Matrix operator-(const Matrix& mat) const
				{
					return Matrix(_11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
								  _21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
								  _31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
								  _41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44);
				}

				Matrix operator*(float f) const
				{
					return Matrix(_11 * f, _12 * f, _13 * f, _14 * f, _21 * f, _22 * f, _23 * f,
								  _24 * f, _31 * f, _32 * f, _33 * f, _34 * f, _41 * f, _42 * f,
								  _43 * f, _44 * f);
				}

				Matrix operator/(float f) const
				{
					float fInv = 1.0f / f;
					return Matrix(_11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv, _21 * fInv,
								  _22 * fInv, _23 * fInv, _24 * fInv, _31 * fInv, _32 * fInv,
								  _33 * fInv, _34 * fInv, _41 * fInv, _42 * fInv, _43 * fInv,
								  _44 * fInv);
				}

				//				 Matrix operator*(float f, const Matrix& mat)
				//				{
				//					return Matrix(f * mat._11, f * mat._12, f * mat._13, f * mat._14, f
				//* mat._21, 								  f * mat._22, f * mat._23, f * mat._24,
				// f * mat._31, f * mat._32, 								  f * mat._33, f *
				// mat._34, f * mat._41, f * mat._42, f * mat._43, 								  f *
				// mat._44);
				//				}

				bool operator==(const Matrix& mat) const
				{
					return 0 == memcmp(this, &mat, sizeof(Matrix));
				}

				bool operator!=(const Matrix& mat) const
				{
					return 0 != memcmp(this, &mat, sizeof(Matrix));
				}

				void Transform2D(Vector2D&)
				{
					// D3DXMatrixTranslation(this, position.x, position.y, 0);
				}
				void Transform2D(Vector3D&)
				{
					// D3DXMatrixTranslation(this, position.x, position.y, position.z);
				}
				void Transform2D(Vector2D&, Vector2D&, float)
				{
					/*D3DXMATRIX m6, m7;
					D3DXQUATERNION rot;
					rot.w = cos(Angle / 2.0f);
					rot.x = 0.0f;
					rot.y = 0.0f;
					rot.z = sin(Angle / 2.0f);
					D3DXMatrixRotationQuaternion(&m6, &rot);
					D3DXMatrixTranslation(&m7, CenterRotation.x + position.x, CenterRotation.y +
					position.y, 0); D3DXMatrixMultiply(this, &m6, &m7);*/
					//					D3DXMatrixTransformation2D(this, nullptr, 0, nullptr,
					//&CenterRotation, Angle, 											   &position);
				}
				void Rotation2D(Vector2D&, float)
				{
					Matrix ret;
					//					D3DXMatrixTranslation(this, CenterPosition.x, CenterPosition.y,
					//0); 					D3DXMatrixRotationZ(&ret, Angle);
					*this *= ret;
				}
			};

			//---------------------------------------------------------------------------
			// Aligned Matrices
			//
			// This class helps keep matrices 16-byte aligned as preferred by P4 cpus.
			// It aligns matrices on the stack and on the heap or in global scope.
			// It does this using __declspec(align(16)) which works on VC7 and on VC 6
			// with the processor pack. Unfortunately there is no way to detect the 
			// latter so this is turned on only on VC7. On other compilers this is the
			// the same as Matrix.
			//
			// Using this class on a compiler that does not actually do the alignment
			// can be dangerous since it will not expose bugs that ignore alignment.
			// E.g if an object of this class in inside a struct or class, and some code
			// memcopys data in it assuming tight packing. This could break on a compiler
			// that eventually start aligning the matrix.
			//---------------------------------------------------------------------------
			class DGE_CoreAPI Matrix16 : public Matrix
			{
			public:
				Matrix16()
					: Matrix()
				{
				}
				Matrix16(const float *);
				Matrix16(float _11, float _12, float _13, float _14, float _21, float _22,
						 float _23, float _24, float _31, float _32, float _33, float _34,
						 float _41, float _42, float _43, float _44);

				Matrix16(const Matrix&);
#if 0
#ifdef DGE_Track_Memory
#undef new
#undef delete
#endif
				// new operators
				void* operator new   (size_t);
				void* operator new [](size_t);

				// delete operators
				void operator delete(void*);   // These are NOT virtual; Do not 
				void operator delete [](void*);   // cast to Matrix and delete.

#ifdef DGE_Track_Memory
#define new(param, ...) DBG_NEW(param, DGE_FileCodeInfo, DGE_LineCodeInfo)
#define delete(param, ...) DBG_Delete(param, DGE_FunctionInfo, DGE_FileCodeInfo, DGE_LineCodeInfo)
#endif
#endif

				// assignment operators
				Matrix16& operator=(const Matrix&);
			};

			typedef /*DGE_Align*/ Matrix16 MatrixA16;
			using Mat4x4 = MatrixA16;
		}
	}
}
#else
		}
	}
}
#endif


