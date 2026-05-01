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
#include <GFX/Resource/TextureResource.h>

namespace DGE
{
	using namespace DGE::ResourceManager;
	using namespace DGE::Core::Math;
	namespace GFX
	{

		class BaseTexture;
		namespace DirectX9
		{
			// using namespace ScriptSet;
			// bass class for Store Material object that use for different API graphics
			class DGE_GFXAPI MaterialDX9 : public DGE::GFX::Material
			{
			public:
				MaterialDX9();
				MaterialDX9(const DChar* name, const DChar* path = DL("./"));
				~MaterialDX9();
			};
		} // namespace DirectX9
	} // namespace GFX
}
