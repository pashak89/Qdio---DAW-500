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
#include <GFX/GCore/Interfaces/IGpuBuffer.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Type of texture
			enum class TextureType : uint8
			{
				Texture1D,
				Texture2D,
				Texture3D,
			};

			/// \brief Format of texture
			enum class TextureFormat : uint8
			{
				None = 0,

				R8,
				RGB8,
				RGBA8,
				RGBA32F,

				Unknown,
			};

			/// \brief Base class for working with the basic texture object
			class DGE_GFXAPI BaseTexture : public GCore::Interfaces::IGpuBuffer
			{
			public:
				// IBaseGObject interface
				virtual GPUObjectType type() override;

				TextureType textureType() const;
				void		setTextureType(TextureType newTextureType);

				virtual AttributeGObjectDescriptor descriptor() override;
				virtual void setDescriptor(const AttributeGObjectDescriptor& descriptor) override;

			protected:
				/// \brief Type of the texture
				TextureType m_textureType;

				/// \brief Format of this texture object
				TextureFormat m_format;

				AttributeGObjectDescriptor m_descriptor;
			};

			using BaseTextureRef = std::shared_ptr<GFX::GCore::BaseTexture>;
			using BaseTextureWRef = std::weak_ptr<GFX::GCore::BaseTexture>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
