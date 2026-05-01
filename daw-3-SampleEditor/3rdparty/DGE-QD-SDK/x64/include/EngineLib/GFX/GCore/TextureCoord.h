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
 *
 */
#pragma once

#include "Core/System/DObjectMacrosDef.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <GFX/GCore/Interfaces/IGpuBuffer.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Store texture coordinates for the verteces
			class DGE_GFXAPI TextureCoord : public GCore::Interfaces::IGpuBuffer
			{
			public:
				virtual GPUObjectType type() override;

				virtual AttributeGObjectDescriptor descriptor() override;
				virtual void setDescriptor(const AttributeGObjectDescriptor& descriptor) override;

			protected:
				AttributeGObjectDescriptor m_descriptor;
			};
			D_DefRefType(TextureCoord)
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
