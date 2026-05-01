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

namespace DGE
{
	using namespace DGE::Resource;
	using namespace DGE::Core::Math;
	namespace Physic
	{
		struct BoundingBox
		{
			Vector3D Min = Vector3D(0, 0, 0);//Minimum extent of the bounding box.
			Vector3D Max = Vector3D(0, 0, 0);//Maximum extent of the bounding box.
			float HalfSize = 0;//Distance from the center of the volume to the furthest point on any axis.
		};

		struct BoundingSphere
		{
			Vector3D Center = Vector3D(0, 0, 0);//Center point of the bounding sphere.
			float Radius = 0;//Radius of bounding sphere.
		};

		//Base Abstract class for Bounding Volume.
		class DGE_API BoundingVolume
		{
		protected:
			BoundingBox m_Box;//Box representation of the bounding volume.
			BoundingSphere m_Sphere;//Sphere representation of the bounding volume.

			Vector3D m_OriginalMin = Vector3D(0, 0, 0);//Original minimum extents of the bounding box.
			Vector3D m_OriginalMax = Vector3D(0, 0, 0);//Original maximum extents of the bounding box.
			Vector3D m_OriginalCenter = Vector3D(0, 0, 0);//Original center point of the bounding sphere.

			Vector3D m_EllipsoidRadius = Vector3D(0,0,0);//Ellipsoid Radius (content the radius along each axis
		public:
			BoundingVolume();

			//Create a Bounding Volume from a Mesh structure.
			void CreateFromMesh(ObjectMesh* mesh, Vector3D EllipsoidRadius = Vector3D(1.0f, 1.0f, 1.0f));

			//Create a Bounding Volume from an array of vertex.
			void CreateFromVertices(Vector3D* vertices, BaseType::uint32 TotalVertices, BaseType::uint32 VertexStride, Vector3D EllipsoidRadius = Vector3D(1.0f, 1.0f, 1.0f));

			//Create a Copy of an other Bounding Volume.
			void CloneBoundingVolume(BoundingBox* Box, BoundingSphere* Sphere, Vector3D EllipsoidRadius = Vector3D(1.0f, 1.0f, 1.0f));

			// Set Position of Bounding Volume by Get Matrix 
			void RepositionBoundingVolume(Matrix* Location);

			//Set Bounding Box Volume.
			void SetBoundingBox(Vector3D min, Vector3D max);

			//Get Bounding Box Volume.
			BoundingBox* GetBoundingBox();

			//Set Bounding Sphere Volume.
			void SetBoundingSphere(Vector3D Center, float radius, Vector3D EllipsoidRadius = Vector3D(1.0f, 1.0f, 1.0f));

			//Get Bounding Sphere Volume.
			BoundingSphere* GetBoundingSphere();

			//Set Ellipsoid Radius of Bounding Ellipsoid Volume.
			void SetEllipsoidRadius(Vector3D EllipsoidRadius);

			//Get Ellipsoid Radius of Bounding Ellipsoid Volume.
			Vector3D GetEllipsoidRadius();

			//Return Size of Box Bounding Volume in 3d space!!
			Vector3D GetSizeOfBox();
		};
	}
}