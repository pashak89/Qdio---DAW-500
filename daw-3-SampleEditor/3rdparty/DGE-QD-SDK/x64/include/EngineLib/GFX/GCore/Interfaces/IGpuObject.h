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

#include <Core/BaseTypes/BaseTypes.h>
#include <GFX/GCore/AttributeGObjectDescriptor.h>
#include <GFX/GCore/Interfaces/IActivable.h>
#include <GFX/GCore/RenderPipelineDef.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace Interfaces
			{
				/// \brief An interface for GPU object
				class DGE_GFXAPI IGpuObject : public IActivable
				{
				public:
					/// \brief Create the GPU object
					///
					/// \return Result of creation of the GPU object
					virtual bool create() = 0;

					/// \brief destroy the created GPU object
					///
					/// \return Result of destroying of this GPU object
					virtual bool destroy() = 0;

					/// \brief Get handler of native object in Graphic API.
					///
					/// \return Handler of native object
					virtual void* nativeObjectHandler() = 0;

					/// \brief Get type of this GPU object
					///
					/// \return Return type of this GPU object
					virtual GPUObjectType type() = 0;
				};

				using IGpuObjectRef = std::shared_ptr<GFX::GCore::Interfaces::IGpuObject>;
			} // namespace Interfaces
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
