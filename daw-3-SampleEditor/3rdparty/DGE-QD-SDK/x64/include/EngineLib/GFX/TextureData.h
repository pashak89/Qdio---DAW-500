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
#include <Core/System/DCoreObject.h>
#include <GFX/GFXModuleDef.h>

namespace DGE
{
	namespace GFX
	{
		// Texture data that is loaded from texture file
		class DGE_GFXAPI TextureData : public Core::System::DCoreObject
		{
		public:
			TextureData();

			DByteArray data() const;
			void	   setData(const DByteArray& newData);
			void	   setData(const char* newData, uint64 size);

			uint32 height() const;
			void   setHeight(uint32 newHeight);

			uint32 width() const;
			void   setWidth(uint32 newWidth);

			uint16 bpp() const;
			void   setBpp(uint16 newBpp);

		protected:
			DByteArray m_data;
			uint32	   m_width, m_height;
			uint16	   m_bpp; // byte per pixel
		};

	} // namespace GFX
} // namespace DGE
