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

#include <Core/Math/IBoundary.h>

namespace DGE
{
	namespace Core
	{
		namespace Math
		{

			/// \brief An sphere Boundary volume
			class SphereBoundary : public IBoundary
			{
			public:
				SphereBoundary();
				SphereBoundary(float radius);
				SphereBoundary(Vector3D position, float radius = 1.0f);

				Vector3D position() const;
				void	 setPosition(const Vector3D& newPosition);

				float radius() const;
				void  setRadius(float newRadius);

				virtual BoundaryVolumeType type() override;

				// IBoundary interface
				virtual bool intersect(const Ray&								   ray,
									   const std::optional<IntersectDetailResult>& detailResult
									   = std::nullopt) const override;

			protected:
				/// \brief position of sphere
				Vector3D m_position;
				/// \brief radius of sphere
				float m_radius;
			};

		} // namespace Math
	}	  // namespace Core
} // namespace DGE
