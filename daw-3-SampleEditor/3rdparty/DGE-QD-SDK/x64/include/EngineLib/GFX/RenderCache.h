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

#include "GFX/Material.h"

namespace DGE
{
	using namespace DGE::ResourceManager;
	using namespace DGE::Core::Math;
	using namespace Math;
	namespace GFX
	{
		/// \brief bass class for Storing cache data and object that use for different API graphics
		class DGE_GFXAPI RenderCache
		{
		public:
			RenderCache(Material* material);
			virtual ~RenderCache();

			virtual void AddFace();
			virtual void Prepare(BaseType::uint32 totalVertices);

			virtual void Begin();
			virtual void RenderFace(uint16 vertex0, uint16 vertex1, uint16 vertex2);
			virtual void End();

			Material* GetMaterial();

		protected:
			Material* m_material; // Pointer to a the material used by this render cache.

			IndexBuffer*	 m_indexBuffer;	 // Index buffer pointing to vertices to render.
			uint16*			 m_indexPointer; // Pointer for accessing the index buffer.
			BaseType::uint32 m_totalIndices; // Total number of indices this render cache can handle.
			BaseType::uint32 m_faces;		 // Total number of faces to be rendered.

			BaseType::uint32 m_totalVertices; // Total number of vertices.
		};
	}
}
