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
#include <Core/Math/Vector3D.h>
#include <Core/System/DEvent.h>

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
			/// \brief An Axis Aligned Bounding Box
			class DGE_CoreAPI AABB : public IBoundary
			{
			public:
				/// \brief Create an invalid AABB
				AABB();
				/// \brief Create an AABB that contains a sphere
				AABB(const Vector3D& center, float radius);
				/// \brief Create an AABB that contains a sphere
				AABB(const Vector3D& min, Vector3D max);
				/// \brief Create an AABB that is a copy of AABB object
				AABB(const AABB& object);

				/// \brief assignment operator for AABB class
				AABB& operator=(const AABB& object);

				/// \brief Indicate whether an object of this class has valid values
				bool isValid() const;

				/// \brief Extend the bounding box on all sides by \p val.
				void extend(float val);

				/// \brief Expand the AABB to include point \p p.
				void extend(const Vector3D& p);

				/// \brief Expand the AABB to include a sphere centered at \p center and of radius \p
				/// radius.
				/// \param[in]  center Center of sphere.
				/// \param[in]  radius Radius of sphere.
				void extend(const Vector3D& center, float radius);

				/// Expand the AABB to encompass the given \p aabb.
				void extend(const AABB& aabb);

				/// \briefExpand the AABB to include a disk centered at \p center, with normal \p
				/// normal, and radius \p radius.
				/// \xxx Untested -- This function is not represented in our unit tests.
				void extendDisk(const Vector3D& center, const Vector3D& normal, float radius);

				/// \brief Translates AABB by vector \p position and pivot \pivotOffset pivotOffset
				void translate(const Vector3D& position, const Vector3D& pivotOffset);

				/// \brief Scale the AABB by \p scale, centered around \p origin.
				/// \param[in]  scale  3D vector specifying scale along each axis.
				/// \param[in]  origin Origin of scaling operation. Most useful origin would
				///                    be the center of the AABB.
				void scale(const Vector3D& scale, const Vector3D& origin);

				/// \briefRetrieves the center of the AABB.
				Vector3D getCenter() const;

				/// \brief Retrieves the diagonal vector (computed as mMax - mMin).
				/// If the AABB is NULL, then a vector of all zeros is returned.
				Vector3D getDiagonal() const;

				/// \brief Retrieves the longest edge.
				/// If the AABB is NULL, then 0 is returned.
				float getLongestEdge() const;

				/// \brief Retrieves the shortest edge.
				/// If the AABB is NULL, then 0 is returned.
				float getShortestEdge() const;

				Vector3D minPoint() const;

				Vector3D maxPoint() const;

				/// \brief the width of cube in X-axis
				float width() const;
				/// \brief the height of cube in Y-axis
				float height() const;
				/// \brief the depth of cube in X-axis
				float depth() const;

				template<typename Matrix>
				AABB operator*(Matrix m) const
				{
					AABB result(*this);
					result *= m;
					return result;
				}

				template<typename Matrix>
				AABB& operator*=(Matrix m)
				{
					m_position *= m;
					return *this;
				}

				// IBoundary interface
				virtual BoundaryVolumeType type() override;

				virtual void update(const Transformation& transformation) override;

				/// \brief Returns one of the intersection types. If either of the aabbs are
				/// invalid, then OUTSIDE is returned.
				virtual bool intersect(
					const AABB&									bb,
					const std::optional<IntersectDetailResult>& detailResult = std::nullopt) const;

				/// \brief Return one of the intersection types. If Ray intersect with AABB , return
				/// IntersectionType::Intersect
				virtual bool intersect(const Ray&								   ray,
									   const std::optional<IntersectDetailResult>& detailResult
									   = std::nullopt) const override;

				Vector3D position() const;

				Vector3D size() const;

				DString toString();

				D_Event(positionChanged, void, Vector3D);
				D_Event(sizeChanged, void, Vector3D);

			protected:
				void updateByMinMax(Vector3D min, Vector3D max);

			protected:
				/// \brief Position of this boundary.
				Vector3D m_position;
				/// \brief Size of this boundary.
				Vector3D m_size;
			};
		} // namespace Math
	}	  // namespace Core
} // namespace DGE
