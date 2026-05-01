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
#include <GFX/GFXModuleDef.h>

namespace DGE
{
	using namespace Core::ResourceManager;
	using namespace DGE::Core::Math;
	namespace GFX
	{
		namespace GCore
		{
			enum DirectionFont
			{
				Top					 = 0x00000000,
				Left				 = 0x00000000,
				Center				 = 0x00000001,
				Right				 = 0x00000002,
				VerticalCenter		 = 0x00000004,
				Bottom				 = 0x00000008,
				WordBreak			 = 0x00000010,
				SingleLine			 = 0x00000020,
				ExpandTabs			 = 0x00000040,
				TabStop				 = 0x00000080,
				NoClip				 = 0x00000100,
				ExternalLeading		 = 0x00000200,
				CalcRect			 = 0x00000400,
				NoPrefix			 = 0x00000800,
				Internal			 = 0x00001000,
				EditControl			 = 0x00002000,
				Path_Ellipsis		 = 0x00004000,
				End_Ellipsis		 = 0x00008000,
				ModifyString		 = 0x00010000,
				RTLReading			 = 0x00020000,
				Word_Ellipsis		 = 0x00040000,
				NoFullWidthCharBreak = 0x00080000,
				HidePrefix			 = 0x00100000,
				PrefixOnly			 = 0x00200000,
			};
			struct StyleFont
			{
				uint32 id;
				DChar  FontName[MaxLengthName];
				void*  data;
			};
			struct TextFormat
			{
				// DString FontName;
				DirectionFont		  direction;
				RectangleDX			  Position;
				Core::BaseType::Color Color;
			};

			/// \brief Base class for storing text information
			class Text
			{
			public:
				DChar	   TextStr[MaxTextLength];
				TextFormat textFormat; // format of text in screen!
				Text(DChar* str, DirectionFont FlagOfDirection, RectangleDX RectangleArea,
					 DGE::Core::BaseType::Color color)
				{
					DStrcpy(TextStr, str);
					textFormat.direction = FlagOfDirection;
					textFormat.Position	 = RectangleArea;
					textFormat.Color	 = color;
				}
				~Text()
				{
					/*TextStr->clear();
					SafeDelete(TextStr);*/
				}
				void SetText(DChar* str) { DStrcpy(TextStr, str); }
			};

			/// \brief Base Abstract class for working with Font and text.
			class DGE_GFXAPI Font
			{
			protected:
				TextFormat m_DefaultFormat; // Default format of text in screen,use when call
											// WriteText(WChar*,void*) without format
			public:
				virtual ~Font(){};
				virtual bool WriteText(Text*, void* param = nullptr)					 = 0;
				virtual bool WriteText(DChar* text, void* param = nullptr)				 = 0;
				virtual bool WriteText(DChar* text, int x, int y, void* param = nullptr) = 0;
				virtual bool WriteText(DChar* text, int x, int y, Color color, void* param = nullptr)
					= 0;
				virtual bool			  SetDefaultFont(uint32 idFontRegister) = 0;
				virtual void*			  GetFont()								= 0;
				virtual Math::RectangleDX GetBoundOfCharacter()					= 0;
				virtual void SetDefaultFormat(TextFormat Format) = 0; // Set Default format.
				virtual bool GetReady()							 = 0;
			};
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
