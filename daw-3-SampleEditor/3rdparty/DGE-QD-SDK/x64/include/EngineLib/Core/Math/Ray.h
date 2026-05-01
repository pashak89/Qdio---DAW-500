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

#include <Core/Math/Vector3D.h>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
			/// \brief The ray class is used to storing property of a ray
			class DGE_CoreAPI Ray
			{
			public:
				Ray();
				Ray(const Vector3D& origin, const Vector3D& direction);

				Vector3D origin() const;
				Vector3D direction() const;

				Vector3D at(double t) const;

				void setOrigin(const Vector3D& newOrigin);

				void setDirection(const Vector3D& newDirection);

				Vector3D invertDirection() const;

			protected:
				Vector3D m_origin;
				Vector3D m_direction;
				Vector3D m_invertDirection;
			};

			using RayRef = std::shared_ptr<Math::Ray>;
		} // namespace Math
	}	  // namespace Core
} // namespace DGE
