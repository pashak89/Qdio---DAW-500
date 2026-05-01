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

#include <Core/IO/LogSystem/Log.h>

#include <GFX/GCore/VertexArray.h>
#include <GFX/GCore/VertexArrayObject.h>
#include <GFX/GCore/VertexBuffer.h>
#include <GFX/DrawablesManager.h>
#include <GFX/GCore/GL/RendererGL.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace GL
			{
				/// \brief A class for vertex array that implements in OpenGL
				///
				class DGE_GFXAPI VertexArrayGL : public DGE::GFX::GCore::VertexArray
				{
					D_OBJECT(VertexArrayGL)
				public:
					/// \brief Default constructor
					///
					/// \details Creates an empty vertex array
					VertexArrayGL(GFX::GCore::GL::RendererGLRef renderer);

					/// \brief Construct the vertex array with the given type
					///
					/// \param type Type of primitives
					VertexArrayGL(GFX::GCore::GL::RendererGLRef	   renderer,
								  GFX::GCore::RenderPrimitivesType type);

					// IInitialize interface
					using Core::Utility::Interfaces::IInitialize::initialize;
					virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

					// IUpdate interface
					virtual bool update(InputParameters &) override;

					// IPreProcessRender interface
					virtual bool preProcessRender(InputParameters &input) override;

					// IRender interface
					virtual bool render(InputParameters &input) override;

					// IPostProcessRender interface
					virtual bool postProcessRender(InputParameters &input) override;

				protected:
					virtual bool syncGBuffer() override;

				public:
					static uint64 s_defaultNumberReservedVertex;

				protected:
					/// \brief The renderer subsystem object that is used for doing basic render
					/// functionality
					GFX::GCore::GL::RendererGLRef m_renderer;

					/// \brief vertex array object
					VertexArrayObjectRef m_vertexArrayObject;

					/// \brief vertex buffer use for store vertices in GPU
					GFX::GCore::VertexBufferRef m_vertextBuffer;

					/// \brief Store number of vertex that reserved for this vertex array
					uint64 m_numberReservedVertex;
				};
			} // namespace GL
		}	  // namespace GCore
	}	  // namespace GFX
} // namespace DGE
