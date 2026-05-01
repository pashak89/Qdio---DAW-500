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

#include "Core/System/DObjectMacrosDef.h"
#include "Core/Utility/Singleton/SingletonMacrosDef.h"
#include "GFX/TextureData.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/BaseTypes/Color.h>
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <GFX/GCore/BaseTexture.h>

namespace DGE
{
	namespace GFX
	{
		namespace Resource
		{
#if defined(DGE_UseGraphicAPI)
			/// \brief Texture resource that load from specified source file and is used by objects in Engine
			class DGE_GFXAPI TextureResource : public DGE::Core::ResourceManager::ObjectResource
			{
				D_OBJECT(TextureResource);

			public:
				TextureResource(const DString name, const DString path = DL("./"));
				D_Destructor(TextureResource);
				virtual void unloadResource() override;
				virtual bool reloadResource(const DString& Name, const DString& Path) override;

				uint32 width() const;

				uint32 height() const;

				uint16 bpp() const;

				DByteArray data() const;

				GFX::TextureData& rawTextureData();

			protected:
				GFX::TextureData m_textureData;
			};
			D_DefRefType(TextureResource)
#endif
		} // namespace Resource
	}	  // namespace GFX
} // namespace DGE
