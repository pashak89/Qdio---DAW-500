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

#include <GFX/GCore/GL/RendererGL.h>
#include <GFX/GCore/VertexBuffer.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace GL
			{
				class DGE_GFXAPI VertexBufferGL : public GFX::GCore::VertexBuffer
				{
				public:
					/// \brief Constructor for Vertex buffer that need renderer as input parameter
					///
					/// \param renderer shared pointer to GFX::GL::RendererGL class.
					VertexBufferGL(std::shared_ptr<GFX::GCore::GL::RendererGL> renderer);

					virtual bool  create() override;
					virtual bool  activate() override;
					virtual bool  deactivate() override;
					virtual bool  destroy() override;
					virtual void *nativeObjectHandler() override;
					virtual bool  setBufferData(DByteArray& data, uint64 elementSize,
												BufferUsageType bufferUsageType) override;
					virtual bool  setBufferData(DByteArray&		data,
												BufferUsageType bufferUsageType) override;
					virtual bool  setDataLayout(uint32 index, uint32 size, AttributeValueType type,
												bool normalized, int32 stride,
												const void* data) override;

				protected:
					/// \brief Shared pointer to GFX::GCore::Renderer object.
					std::shared_ptr<GFX::GCore::GL::RendererGL> m_renderer;
					/// \brief Store vertex buffer ID from glGenBuffers
					uint32 m_vbo;
					/// \brief Store number of bytes that reserved for this vertex buffer
					uint64 m_numberReservedBytes;
				};
			} // namespace GL
		}	  // namespace GCore
	}	  // namespace GFX
} // namespace DGE
