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
#include <GFX/GCore/BaseTexture.h>
#include <GFX/GCore/Interfaces/IGpuBuffer.h>
#include <GFX/GCore/RenderPipelineDef.h>
#include <GFX/GCore/Renderer.h>
#include <GFX/GCore/Shader.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace GL
			{
				class BaseTextureGL;
				class IndexBufferGL;
				class VertexBufferGL;
				class TextureCoordGL;
				class VertexArrayGL;
				class VertexArrayObjectGL;
				class RenderStateImpGL;
				struct RenderStateDataGL;

				// TODO: must merge RendererGL and QtRenderer

				/// \brief Implementation of Renderer with OpenGL Backend
				class DGE_GFXAPI RendererGL : public GFX::GCore::Renderer
				{
					D_OBJECT(RendererGL)

					friend class GFX::GCore::GL::BaseTextureGL;
					friend class GFX::GCore::GL::IndexBufferGL;
					friend class GFX::GCore::GL::VertexArrayGL;
					friend class GFX::GCore::GL::VertexBufferGL;
					friend class GFX::GCore::GL::VertexArrayObjectGL;
					friend class GFX::GCore::GL::RenderStateImpGL;

				public:
					RendererGL(GFX::GCore::RenderAPIInfo	 requestedRenderAPI,
							   DGE::Core::System::ContextRef context);

					/// IInitialize interface
					///
					/// \brief Use default implementation of
					/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
					/// function from IInitialize interface.
					using Utility::Interfaces::IInitialize::initialize;

					///
					/// \details It's created by QtPlatform automatically based on initialize
					/// parameters. Call this function with these parameters
					///
					/// \param parameters Input parameters for initialize this class.
					/// must set by below example:
					///
					/// \return return true if initialization is successfully otherwise return false
					///
					///	\code
					/// #include <iostream>
					/// #include <Core/Application/Application.h>
					/// int main(int argc, char* argv[])
					/// {
					///		// contain other codes
					///
					///  	auto context= Context::getInstance();
					///		if(context.initialize({
					///						{Window::s_nativeWindowKey,quickWindow},
					/// 					{DGE::Platforms::Platform::s_backendInitKey,
					/// backendInitRequest},
					///						}))
					///			std::cout<<"Context is initialized."<<std::endl;
					///		else
					///			std::cout<<"Context initialization is failed."<<std::endl;
					///
					///		return 0;
					/// }
					/// \endcode
					virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

					virtual bool release() override;

					virtual bool isInitialized() override;

					// Renderer interface
					/// \brief Clear target buffers of rendering with specified input parameter
					///
					/// \param param Set how clearing target buffers and also which buffers must clear
					///
					/// \return if this process is successful return true else return false
					virtual bool clear(GFX::GCore::ClearParam &param) override;
					virtual bool beginScene() override;
					virtual bool render(
						const std::shared_ptr<GFX::GCore::Interfaces::IRender> &renderItem) override;
					/// \brief draw an vertex array with specified info
					virtual bool drawVertexArray(const VertexArrayDrawInfo &vertexArrayInfo) override;
					/// \brief draw an element with specified info
					virtual bool drawElement(const ElementDrawInfo &elementInfo) override;
					virtual bool endScene() override;

					/// \brief Send result of rendering for showing in destination
					///
					/// \return if this process is successful return true else return false
					virtual bool present(GFX::GCore::PresentParam &param) override;

					/// \brief Get native object of Render API
					virtual void *getNativeObject() override;

					virtual GFX::GCore::ShaderRef			 createShader() override;
					virtual ShaderCompilerRef				 createShaderCompiler() override;
					virtual GFX::GCore::VertexBufferRef		 createVertexBuffer() override;
					virtual GFX::GCore::IndexBufferRef		 createIndexBuffer() override;
					virtual GFX::GCore::TextureCoordRef		 createTextureCoordBuffer() override;
					virtual GFX::GCore::BaseTextureRef		 createTexture() override;
					virtual GFX::GCore::VertexArrayRef		 createVertexArray() override;
					virtual GFX::GCore::VertexArrayObjectRef createVertexArrayObject() override;
					virtual GFX::GCore::Interfaces::IRenderStateRef createRenderState() override;

					virtual bool setViewport(Core::Math::Bound bound) override;
					virtual bool setWindow(std::shared_ptr<Core::Windows::Window> window) override;
					virtual bool checkForError() override;

					virtual bool saveStates() override;
					virtual bool restoreStates() override;

					virtual const DString shadingVersion() override;
					virtual const DString shadingVersion(const DString &renderAPIVersion) override;

					/// \brief Activate a shader program with specified ID and if there is an
					/// activated shader, store previous activated shader program id and suspend it.
					///
					/// \return Return true if there is an activated shader
					bool activateShaderProgram(uint32 id);
					/// \brief Deactivate a shader program with specified ID and if there is a
					/// suspend shader in shader stack, reactive it.
					///
					/// \return Return true if there is an activated shader
					bool deactivateShaderProgram(uint32 id);

				protected:
					/// \brief Convert RenderPrimitivesType into equal OpenGL value
					uint32 getRenderPrimitivesModeGL(
						GFX::GCore::RenderPrimitivesType renderPrimitivesType);
					/// \brief Convert BufferUsageType into equal OpenGL value
					uint32 getVertexBufferUsageTypeGL(GFX::GCore::BufferUsageType bufferUsageType);
					/// \brief Convert AttributeTypeValue into equal OpenGL value
					uint32 getAttributeTypeValueGL(GFX::GCore::AttributeValueType attributeTypeValue);
					/// \brief Convert TextureType into	equal OpenGL value
					uint32 getTextureType(GFX::GCore::TextureType textureType);

					virtual bool registerAll(Core::Utility::Interfaces::InitializeParams &parameters) override;
					/// \brief Generate buffers
					virtual void generateBuffer(uint32 size, uint32 *buffers);
					/// \brief Active a buffer
					virtual void activateBuffer(uint32 target, uint32 buffer);

					/// \brief set data of buffer to use in rendering
					virtual void setBufferData(uint32					   target,
											   uint64					   size,
											   const void				  *data,
											   GFX::GCore::BufferUsageType usageType);
					/// \brief update data of buffer to use in rendering
					virtual void updateBufferData(uint32	  target,
												  int64		  offset,
												  uint64	  size,
												  const void *data);

					/// \brief Generate vertex array object
					virtual void generateVertexArrayObject(uint32 size, uint32 *vaoIDs);
					/// \brief Active a vertex array object
					virtual void activateVertexArrayObject(uint32 vaoID);

					/// \brief Activate a attribute location
					virtual void activateAttribute(uint32 id);
					/// \brief Deactivate a attribute location
					virtual void deactivateAttribute(uint32 id);

					/// \brief Generate texture objects and set ids of them to input list \p textureIDs
					virtual void generateTexture(uint32 size, uint32 *textureIDs);
					/// \brief Active a texture that is specified it id
					virtual void activateTexture(uint32 target, uint32 textureID);
					/// \brief set unit(location) of texture when using multiple textures
					virtual void setTextureUnit(uint32 unit);
					/// \brief set data of texture in 2-dimensional to use in rendering
					virtual void setTexture2DData(uint32	  target,
												  int32		  lod,
												  int32		  internalFormat,
												  int32		  width,
												  int32		  height,
												  int32		  border,
												  uint32	  format,
												  uint32	  type,
												  const void *data);
					/// \brief set texture parameters for current active texture
					virtual void setTextureParameterInt(uint32 target, uint32 paramName,
														int32 paramValue);

					/// \brief generate mipmap automatically for current active texture
					virtual void generateMipMap(uint32 target);

					/// \brief Add render state data for current rendering frame
					virtual bool addRenderState(RenderStateDataGL &renderState);

					/// We added some specialize function for removing dependency on Qt library
					/// and using the engine with any similar and lightweight library

					/// \brief set format of array of vertex attributes
					virtual void setVertexAttribute(uint32 index, uint32 size,
													AttributeValueType type, bool normalized,
													int32 stride, const void *data);
					/// \brief render primitives from array data
					virtual void drawVertexArray(const RenderPrimitivesType &type, int32 first,
												 uint32 size);

					virtual std::shared_ptr<GFX::GCore::RenderStateImp> createRenderStateImp()
						override;

				public:
					static inline DString s_defaultMinimumShaderVersion = "110";

				protected:
					/// \brief Cache parameter used for optimizing render pipeline.
					std::shared_ptr<RendererGL> m_renderer;
					/// \brief Stack of activated shader program in render pipeline
					std::stack<uint32> m_activatedShaders;
					/// \brief ID of current activated shader program in render pipeline
					uint32 m_currentdShaderID;
				};

				using RendererGLRef = std::shared_ptr<GFX::GCore::GL::RendererGL>;
				using RendererGLWRef = std::weak_ptr<GFX::GCore::GL::RendererGL>;
			} // namespace GL
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
