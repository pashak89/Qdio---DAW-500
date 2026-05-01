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
#include "../../Core/Array/Array.h"
#include "../BaseTypes/BaseTypes.h"
#include "../BaseTypes/Color.h"
#include "Vector.h"
#include <Core/MemoryManager/MemoryManager.h>

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
			using namespace DGE::Core;
			//-----------------------------------------------------------------
			//Light Vertex Structure 
			//-----------------------------------------------------------------
			class DGE_CoreAPI LightVertex
			{
			public:
				Vector3D translation;//translation of vertex in world space.
				BaseType::Color diffuse;	// Color of vertex.

				LightVertex();

				/*
				*
				*	\param[in]	translation	:	translation of vertex in world space.
				*
				*	\param[in]	diffuse		:	Color of vertex.
				*
				*/
				LightVertex(Vector3D translation, BaseType::Color diffuse);
			};
#define LightVertex_FVF ( D3DFVF_XYZ |D3DFVF_DIFFUSE )
#define LightVertex_FVF_Size D3DXGetFVFVertexSize( LightVertex_FVF )

			//-----------------------------------------------------------------
			//Vertex Structure 
			//-----------------------------------------------------------------
			class DGE_CoreAPI Vertex
			{
			public:
				Vector3D translation;//translation of vertex in world space.
				Vector3D normal;//Vertex's normal vector.
				float tu, tv;//texture UV coordinate.

				Vertex();

				/*
				*
				*	\param[in]	translation	:	translation of vertex in world space.
				*
				*	\param[in]	normal		:	Vertex's normal vector.
				*
				*	\param[in]	U			:	texture U coordinate.(in UV System)
				*
				*	\param[in]	V			:	texture V coordinate.(in UV System)
				*
				*/
				Vertex(Vector3D translation, Vector3D normal, float U, float V);

			};
#define Vertex_FVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
#define Vertex_FVF_Size D3DXGetFVFVertexSize( Vertex_FVF )

			//-----------------------------------------------------------------
			//Lit Vertex Structure
			//-----------------------------------------------------------------
			class DGE_CoreAPI LVertex
			{
			public:
				Vector3D translation;//translation of vertex in world space.
				BaseType::Color diffuse;	// Color of vertex.
				float tu, tv;//texture UV coordinate.

				LVertex();

				/*
				*
				*	\param[in]	translation	:	translation of vertex in world space.
				*
				*	\param[in]	diffuse		:	Color of vertex.
				*
				*	\param[in]	U			:	texture U coordinate.(in UV System)
				*
				*	\param[in]	V			:	texture V coordinate.(in UV System)
				*
				*/
				LVertex(Vector3D translation, BaseType::Color diffuse, float U, float V);
			};
#define L_Vertex_FVF ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define L_Vertex_FVF_Size D3DXGetFVFVertexSize( L_Vertex_FVF )

			//-----------------------------------------------------------------
			// Transformed & Lit Vertex Structure.
			//-----------------------------------------------------------------
			class DGE_CoreAPI TLVertex
			{
			public:
				Vector4D  translation;//translation of vertex in screen space.
				BaseType::Color diffuse;	// Color of vertex.
				float tu, tv;//texture UV coordinate.

				TLVertex();

				/*
				*
				*	\param[in]	translation	:	translation of vertex in screen space.
				*
				*	\param[in]	diffuse		:	Color of vertex.
				*
				*	\param[in]	U			:	texture U coordinate.(in UV System)
				*
				*	\param[in]	V			:	texture V coordinate.(in UV System)
				*
				*/
				TLVertex(Vector4D translation, BaseType::Color diffuse, float U, float V);
			};
#define TL_Vertex_FVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define TL_Vertex_FVF_Size D3DXGetFVFVertexSize( TL_Vertex_FVF )

			//-----------------------------------------------------------------
			// Edge Structure
			//-----------------------------------------------------------------
			class DGE_CoreAPI Edge
			{
			public:
				Vertex* v0;//first vertex of edge.
				Vertex* v1;//second vertex of edge.

				Edge(Vertex* vertex0, Vertex* vertex1);
			};

			//-----------------------------------------------------------------
			// Indexed Edge Structure
			//-----------------------------------------------------------------
			struct IndexedEdge
			{
				uint8 vertex0;
				uint8 vertex1;
			};

			//-----------------------------------------------------------------
			// Face Structure
			//-----------------------------------------------------------------
			class DGE_CoreAPI Face
			{
			public:
				Vertex* v0;//first vertex of edge.
				Vertex* v1;//second vertex of edge.
				Vertex* v2;//third vertex of edge.

				Face(Vertex* vertex0, Vertex* vertex1, Vertex* vertex2);
			};

			//-----------------------------------------------------------------
			// Indexed Face Structure
			//-----------------------------------------------------------------
			struct IndexedFace
			{
				uint8 vertex0;
				uint8 vertex1;
				uint8 vertex2;
			};

			/// \brief structor for storing index buffer
			struct IndexBuffer
			{
			};

			/*
			\return true if Box1 is inside Box2
			*/
			inline bool IsBoxInBox(Vector3D Box1Min, Vector3D Box1Max, Vector3D Box2Min, Vector3D Box2Max);

			/*
			\return true if face is inside of box
			*/
			inline bool IsFaceInBox(Vertex* vertex0, Vertex* vertex1, Vertex* vertex2, Vector3D BoxMin, Vector3D BoxMax);

			/*
			\return true if Box is Completely enclosed by volume.
			*/
			inline bool IsBoxEnclosedByVolume(const Array::DArray<Plane3D*>& planes,
											  Vector3D BoxMin, Vector3D BoxMax);

			/*
			\return true if sphere is overlapping with volume
			*/
			inline bool IsSphereOverlappingVolume(const Array::DArray<Plane3D*>& planes,
												  Vector3D translation, float radius);

			/*
			\return true if two spheres collide.
			*/
			inline bool IsSphereCollidingWithSphere(Vector3D translation1, Vector3D translation2, Vector3D VelocitySum, float RadiiSum, float* CollisionDistance);
		}
	}

}
