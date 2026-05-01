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
#include <GFX/GCore/AttributeGObjectDescriptor.h>
#include <GFX/GCore/Interfaces/IGpuObject.h>
#include <GFX/GCore/RenderPipelineDef.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace Interfaces
			{
				/// \brief An interface for base graphic object
				class DGE_GFXAPI IGpuBuffer : public IGpuObject
				{
				public:
					/// \brief Create the base graphic object
					///
					/// \return Result of creation of the base graphic object
					using IGpuObject::create;

					/// \brief Active this base graphic object for using in rendering pipeline
					///
					/// \return Result of activation of the base graphic object
					using IGpuObject::activate;

					/// \brief Active this base graphic object for using in rendering pipeline
					///
					/// \return Result of activation of the base graphic object
					using IGpuObject::deactivate;

					/// \brief destroy the created base graphic object
					///
					/// \return Result of destroying of this base graphic object
					using IGpuObject::destroy;

					/// \brief Get handler of native object in Graphic API.
					///
					/// \return Handler of native object
					using IGpuObject::nativeObjectHandler;

					/// \brief Get type of this base graphic object
					///
					/// \return Return type of this base graphic object
					using IGpuObject::type;

					/// \brief Set data of buffer of this base graphic object for using in rendering
					/// pipeline
					///
					/// \return Result of setting data of buffer of the base graphic object
					virtual bool setBufferData(
						DByteArray& data, uint64 elementSize,
						BufferUsageType bufferUsageType = BufferUsageType::Dynamic_Draw)
						= 0;
					virtual bool setBufferData(
						DByteArray&		data,
						BufferUsageType bufferUsageType = BufferUsageType::Dynamic_Draw)
						= 0;

					/// \brief Specify structure of data in the buffer of the base graphic object
					virtual bool setDataLayout(uint32 index, uint32 size,
													AttributeValueType type, bool normalized,
													int32 stride, const void* data)
						= 0;

					virtual AttributeGObjectDescriptor descriptor()							 = 0;
					virtual void setDescriptor(const AttributeGObjectDescriptor& descriptor) = 0;
				};

				using IGpuBufferRef = std::shared_ptr<GFX::GCore::Interfaces::IGpuBuffer>;
			} // namespace Interfaces
		}	  // namespace GCore
	} // namespace GFX
} // namespace DGE
