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

#include <Core/ResourceManager/ResourceLoader.h>

#include <GFX/Resource/TextureResource.h>

namespace DGE
{
	using namespace DGE::ResourceManager;
	using namespace DGE::Core::Math;
	using namespace Math;
	namespace GFX
	{

		struct MaterialData
		{
			Color Diffuse;
			Color Ambient;
			Color Specular;
			Color Emissive;
			float Power;
		};

		/// \brief bass class for Storing Material object that use for different API graphics
		class DGE_GFXAPI Material : public DGE::Core::ResourceManager::ObjectResource
		{
		public:
			Material();
			Material(const DChar* name, const DChar* path = DL("./"));
			~Material();

			GFX::Resource::TextureResource* textureResource();
			DByteArray		 textureData();

			const MaterialData&		 GetMaterial();
			void					 SetMaterial(const MaterialData& data);
			BaseType::uint32		 GetWidth();
			BaseType::uint32		 GetHeight();
			bool					 GetIgnoreFace();
			bool					 GetIgnoreFog();
			bool					 GetIgnoreRay();
			void					 SetIgnoreFace(bool flag);
			void					 SetIgnoreFog(bool flag);
			void					 SetIgnoreRay(bool flag);

		protected:
			// TODO: Add some Special variable to material script for full control this class,exp:
			// Set MipMapFilter.

			DGE::GFX::Resource::TextureResource* m_Texture = nullptr; // Direct3D texture.
			MaterialData m_materialData;
			bool		 m_ignoreFace; // ignored faces when this material is rendering.
			bool		 m_ignoreFog;  // ignored fog when this material is rendering.
			bool		 m_ignoreRay;  // ignored ray intersections when this material is rendering.
		};
	} // namespace GFX
} // namespace DGE
