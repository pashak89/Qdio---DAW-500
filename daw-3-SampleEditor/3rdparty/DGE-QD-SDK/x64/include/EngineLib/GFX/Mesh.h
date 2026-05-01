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
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <GFX/Material.h>

namespace DGE
{
	using namespace DGE::ResourceManager;
	using namespace DGE::Core::Math;
	using namespace Math;
	namespace GFX
	{

		//bass class for Store Material object that use for different API graphics
		struct Frame //: public D3DXFRAME
		{
			Matrix FinalTransformation; // Frame's final transformation after
										// combined with its parent frame.

			// Returns the frame's translation.
			Vector3D GetTranslation()
			{
				return Vector3D(FinalTransformation._41, FinalTransformation._42,
								FinalTransformation._43);
			}
			~Frame() {}
		};

		// Mesh Container Structure
		struct MeshContainer // : public D3DXMESHCONTAINER
		{
			char		 **MaterialNames; // Temporary array of material (texture) names.
			Material	 **Materials;	   // Array of materials used by the mesh container.
			ObjectMesh *OriginalMesh;  // Original mesh.
			// D3DXATTRIBUTERANGE *AttributeTable; // Mesh's attribute table.
			BaseType::uint32 SizeGroups; // Total number of attribute groups.
			Matrix **BoneMatrixPointers; // Array of pointers to the bone transformation matrices.
		};

		// Allocate Hierarchy Class
		class AllocateHierarchy //: public ID3DXAllocateHierarchy
		{
			//			STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
			//			STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, CONST D3DXMESHDATA
			//*pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE
			// *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO
			// pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer);
			//STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree); 			STDMETHOD(DestroyMeshContainer)(THIS_
			//LPD3DXMESHCONTAINER pMeshContainerToFree);
		};

		// using namespace ScriptSet;

		/// \brief bass class for Storing Mesh object that use for different API graphics
		class DGE_GFXAPI Mesh
			: public DGE::Core::ResourceManager::ObjectResource //, public BoundingVolume
		{
			Frame *m_FirstFrame = nullptr; // First frame in the mesh's frame hierarchy.
			// ID3DXAnimationController *m_AnimationController = nullptr; // Animation controller.

			Matrix		   *m_boneMatrices = nullptr; // Array of bone transformation matrices.
			BaseType::uint32 m_uSizeBoneMatrices = 0;  // Number of bones in the array.

			MeshContainer *m_StaticMesh = nullptr; // A static (non-animated) version of the mesh.
			Vertex		   *m_Vertices	= nullptr; // Array of vertices from the static mesh.
			uint16		   *m_Indices	= nullptr; // Array of indices into the vertex array.
			uint16		   m_failedMaterial = 0;   // number of material that can not load it.
#if DGE_Graphic_API == DGE_Use_Directx && defined(DGE_UseGraphicAPI)
			IDirect3DDevice9 *m_Device = nullptr;
#endif
			DArray<Frame *> m_Frames; // Linked list of pointers to all the frames in the mesh.
			DArray<Frame *>
				m_RefPoints; // Linked list of pointers to all the reference points in the mesh.

			// Prepares the given frame.
			void PrepareFrame(Frame *frame);

			// Updates the given frame's transformation matrices.
			void UpdateFrame(Frame *frame, Matrix *parentTransformationMatrix = nullptr);

			// Render the given frame's mesh containers, if it has any.
			void RenderFrame(Frame *frame);

		public:
			Mesh(const DChar *name, const DChar *path = DL("./"));
			virtual ~Mesh();

			void Update();
			void Render();

			// Create a clone of the mesh's animation controller.
			// void CloneAnimationController(ID3DXAnimationController **animationController);

			// Returns the static (non-animated) version of the mesh.
			MeshContainer *GetStaticMesh();

			// Returns the mesh's static (non-animated) vertices.
			Vertex *GetVertices();

			// Returns the mesh's face indices for the vertices.
			uint16 *GetIndices();

			// Returns the list of frames in the mesh.
			const DArray<Frame *> GetFrameList();

			// Returns the frame with the given name.
			Frame *GetFrame(char *name);

			// Returns the reference point with the given name.
			Frame *GetReferencePoint(char *name);
		};
	}
}
