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
#include <Core/BaseTypes/Color.h>
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <GFX/Font.h>

namespace DGE
{
	using namespace Core::ResourceManager;
	using namespace DGE::Core::Math;
	namespace GFX
	{
		class Text;

		namespace GCore
		{
			class Renderer;

			namespace DirectX9
			{
				class RendererDX9;
				class DGE_GFXAPI FontDX9 : public Font
				{
					// ID3DXFont* m_Font = nullptr;
#if DGE_Graphic_API == DGE_Use_Directx && defined(DGE_UseGraphicAPI)
					IDirect3DDevice9* m_Device = nullptr;
#endif
					RendererDX9* m_Renderer = nullptr;
					// ID3DXSprite* m_Sprite;
					bool m_ReadyToUse = false;

				public:
					FontDX9(GFX::GCore::Renderer* Device, const DChar* FontName = DL("Arial"),
							uint16 Size = 18, uint32 Weight = 12, bool Italic = false,
							uint32 MipLevels = 0);
					FontDX9(uint32 idFontRegister, GFX::GCore::Renderer* m_Device);
					~FontDX9();
					bool  WriteText(Text*, void* param = nullptr);
					bool  WriteText(DChar* text, void* param = nullptr);
					bool  WriteText(DChar* text, int x, int y, void* param = nullptr);
					bool  WriteText(DChar* text, int x, int y, Core::BaseType::Color color,
									void* param = nullptr);
					bool  SetDefaultFont(uint32 idFontRegister);
					void* GetFont();
					Math::RectangleDX GetBoundOfCharacter();
					void			  SetDefaultFormat(TextFormat Format);
					bool			  GetReady();
				};
			} // namespace DirectX9
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
