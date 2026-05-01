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

#include <Core/Math/Ray.h>
#include <Core/Math/Transformation.h>
#include <optional>

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
			/// \brief Type of intersect boundary with ray or another boundary that returned from
			/// call to intersect.
			enum class IntersectionType
			{
				InSide,
				Intersect,
				OutSide
			};

			/// \brief Type of boundary volume
			enum class BoundaryVolumeType
			{
				AABB,
				OBB,
				Sphere,
				Capsule
			};

			struct IntersectDetailResult
			{
				Core::Math::Vector3D point;
				Core::Math::Vector3D normal;
				float				 distance;
				IntersectionType	 type;
				bool				 isHit;
			};

			/// \brief An interface for Boundary volume that can detect intersecting with a ray or
			/// other IBoundary volume
			class DGE_CoreAPI IBoundary
			{
			public:
				virtual BoundaryVolumeType type() = 0;
				//				/// \brief Returns one of the intersection types. If IBoundray
				// intersect with
				//				/// IBoundray , return IntersectionType::Intersect
				//				virtual IntersectionType intersect(const IBoundary& ib) const = 0;

				/// \brief update boundary with specified transformation
				virtual void update(const Transformation& transformation) = 0;

				/// \brief Return one of the intersection types. If Ray intersect with IBoundray ,
				/// return IntersectionType::Intersect
				virtual bool intersect(
					const Ray&									ray,
					const std::optional<IntersectDetailResult>& detailResult = std::nullopt) const
					= 0;
			};
			using BoundaryRef = std::shared_ptr<DGE::Core::Math::IBoundary>;
		} // namespace Math
	}	  // namespace Core
} // namespace DGE
