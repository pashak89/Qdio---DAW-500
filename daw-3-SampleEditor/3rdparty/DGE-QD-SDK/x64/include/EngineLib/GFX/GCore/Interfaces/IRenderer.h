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
#include <Core/Windows/Window.h>
#include <GFX/GCore/BaseTexture.h>
#include <GFX/GCore/IndexBuffer.h>
#include <GFX/GCore/Interfaces/IGpuObject.h>
#include <GFX/GCore/Interfaces/IRender.h>
#include <GFX/GCore/Interfaces/IRenderState.h>
#include <GFX/GCore/RenderPipelineDef.h>
#include <GFX/GCore/RenderStateImp.h>
#include <GFX/GCore/Shader.h>
#include <GFX/GCore/ShaderCompiler.h>
#include <GFX/GCore/TextureCoord.h>
#include <GFX/GCore/VertexArray.h>
#include <GFX/GCore/VertexArrayObject.h>
#include <GFX/GCore/VertexBuffer.h>
#include <GFX/GFXModuleDef.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace Interfaces
			{
				class DGE_GFXAPI IRenderer
				{
				public:
					/// \brief set boundary of view-port
					virtual bool setViewport(Core::Math::Bound bound) = 0;

					/// \brief set rendering window with \param window
					virtual bool setWindow(std::shared_ptr<Core::Windows::Window> window) = 0;

					/// \brief clean rendering buffer with specific \param param
					virtual inline bool clear(ClearParam& param) = 0;

					/// \brief start rendering process
					virtual inline bool beginScene() = 0;

					///  \brief do render process
					virtual inline bool render(const std::shared_ptr<Interfaces::IRender>& renderItem)
						= 0;

					/// \brief draw an vertex array with specified info
					virtual bool drawVertexArray(const VertexArrayDrawInfo& vertexArrayInfo) = 0;

					/// \brief draw an element with specified info
					virtual bool drawElement(const ElementDrawInfo& elementInfo) = 0;

					/// \brief finish rendering process
					virtual inline bool endScene() = 0;

					/// \brief show result of rendering in output
					virtual inline bool present(PresentParam& param) = 0;

					/// \brief create shader that related to current Renderer class and backend
					///
					/// \return shared pointer to Shader object
					virtual GFX::GCore::ShaderRef createShader() = 0;

					/// \brief create shader compiler that related to current Renderer class and backend
					///
					/// \return shared pointer to ShaderCompiler object
					virtual GFX::GCore::ShaderCompilerRef createShaderCompiler() = 0;

					/// \brief create a VertexBuffer object and return a shared pointer to it
					///
					/// \return shared pointer to VertexBuffer object
					virtual GFX::GCore::VertexBufferRef createVertexBuffer() = 0;

					/// \brief create a IndexBuffer object and return a shared pointer to it
					///
					/// \return shared pointer to IndexBuffer object
					virtual GFX::GCore::IndexBufferRef createIndexBuffer() = 0;

					/// \brief create a TextureCoord buffer object and return a shared pointer to it
					///
					/// \return shared pointer to TextureCoord buffer object
					virtual GFX::GCore::TextureCoordRef createTextureCoordBuffer() = 0;

					/// \brief create a IndexBuffer object and return a shared pointer to it
					///
					/// \return shared pointer to IndexBuffer object
					virtual GFX::GCore::BaseTextureRef createTexture() = 0;

					/// \brief create a VertexArray object and return a shared pointer to it
					///
					/// \return shared pointer to VertexArray object
					virtual GFX::GCore::VertexArrayRef createVertexArray() = 0;

					/// \brief create a VertexArrayObject object and return a shared pointer to it
					///
					/// \return shared pointer to VertexArrayObject object
					virtual GFX::GCore::VertexArrayObjectRef createVertexArrayObject() = 0;

					/// \brief create a RenderState object and return a shared pointer to it
					///
					/// \return shared pointer to RenderState object
					virtual GFX::GCore::Interfaces::IRenderStateRef createRenderState() = 0;

					/// \brief store states of all Graphical API that can be restored by calling
					/// restoreState()
					///
					/// \return If saving state is successful return true else return false
					virtual bool saveStates() = 0;

					/// \brief restore states of all Graphical API that was saved by calling
					/// saveStates()
					///
					/// \return If restoring state is successful return true else return false
					virtual bool restoreStates() = 0;

					/// \brief Return a raw pointer to native renderer object
					virtual inline void* getNativeObject() = 0;

					/// \brief Check for occurred error to now and log it and if error isn't existed
					/// return false else return true.
					virtual bool checkForError() = 0;

					/// \brief Retrieve shading language version
					virtual const DString shadingVersion() = 0;

					/// \brief Retrieve shading language version
					virtual const DString shadingVersion(const DString& renderAPIVersion) = 0;

					/// \brief activate render state for current rendering task
					virtual bool activeRenderState(Interfaces::IRenderStateRef renderState) = 0;

					/// \brief deactivate render state for current rendering task
					virtual bool deactiveRenderState(Interfaces::IRenderStateRef renderState) = 0;

					/// \brief Return information about current render API
					virtual RenderAPIInfo renderApiInfo() const = 0;

				protected:
					/// \brief create a RenderState object and return a shared pointer to it
					///
					/// \return shared pointer to RenderState object
					virtual std::shared_ptr<GFX::GCore::RenderStateImp> createRenderStateImp() = 0;

					/// \brief activate render state for current rendering task
					virtual bool activeRenderStateImp(std::shared_ptr<RenderStateImp> renderState)
						= 0;

					/// \brief deactivate render state for current rendering task
					virtual bool deactiveRenderStateImp(std::shared_ptr<RenderStateImp> renderState)
						= 0;
				};
			} // namespace Interfaces
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
