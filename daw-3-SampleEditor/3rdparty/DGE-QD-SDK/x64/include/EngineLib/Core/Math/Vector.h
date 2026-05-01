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
#include <Core/Math/Vector2D.h>
#include <Core/Math/Vector3D.h>
#include <Core/Math/Vector4D.h>

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
#if DGE_Platform == DGE_Windows_Platform
			// typedef DWORD Color;
#endif

			class DGE_CoreAPI Plane3D
			{
			public:
				float a, b, c, d;
			};

			class ObjectMesh
			{

			};

			//			typedef struct _Matrix {
			//				union {
			//					struct {
			//						float        _11, _12, _13, _14;
			//						float        _21, _22, _23, _24;
			//						float        _31, _32, _33, _34;
			//						float        _41, _42, _43, _44;

			//					};
			//					float m[4][4];
			//				};
			//			}Matrix;

			class RectangleDX
			{
			public:
				RectangleDX() {}
			};
		} // namespace Math
	}
}
