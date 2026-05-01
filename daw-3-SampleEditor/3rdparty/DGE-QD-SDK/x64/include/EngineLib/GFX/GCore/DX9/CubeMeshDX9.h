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

#include <GFX/BaseMesh.h>
#include <GFX/GCore/DX9/BaseMeshDX9.h>

namespace DGE
{
	using namespace DGE::ResourceManager;
	using namespace DGE::Core::Math;
	// using namespace Physic;
	namespace GFX
	{
		// using namespace ScriptSet;
		namespace DirectX9
		{

			// bass class for Store Mesh object that use for different API graphics
			class DGE_GFXAPI CubeMeshDX9 : /* public BoundingVolume,*/ public BaseMesh
			{
			public:
				CubeMeshDX9(float Width, float Height, float Depth, bool IsDynamic = false,
							const DChar* MaterialPath = nullptr,
							const DChar* MaterialName = nullptr);
				CubeMeshDX9(float Width, float Height, float Depth, Vector3D v, float mass = 100,
							bool IsDynamic = false, const DChar* MaterialPath = nullptr,
							const DChar* MaterialName = nullptr);
				virtual ~CubeMeshDX9();
			};
		} // namespace DirectX9
	}
}
