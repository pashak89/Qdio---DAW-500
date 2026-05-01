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
#if defined(DGE_Track_Memory) || defined(DGE_UseCRTDebugMemory)
#undef new
#undef delete
#endif

#if defined(DGE_USE_QT)
#include <QtOpenGL>
#endif

#include <Core/Platforms/QtPlatform.h>
#ifdef DGE_Track_Memory
#define new(param, ...) DBG_NEW(param, DGE_FileCodeInfo, DGE_LineCodeInfo)
#define delete(param, ...) DBG_Delete(param, DGE_FunctionInfo, DGE_FileCodeInfo, DGE_LineCodeInfo)
#elif defined(DGE_UseCRTDebugMemory)
#define _New DBG_NEW
#define new DBG_NEW
#endif
#include <Core/Windows/QtWindow.h>

#include <Core/System/Context.hpp>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <GFX/GCore/GL/RendererGL.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace QtWrapper
			{
#if defined(DGE_USE_QT)
				/// \brief Implementation of Renderer with Qt Backend
				class DGE_GFXAPI QtRenderer
					: public QObject
					, public GL::RendererGL
					, public QOpenGLExtraFunctions
				{
					Q_OBJECT
					D_OBJECT(QtRenderer)

				public:
					QtRenderer(RenderAPIInfo						requestedRenderAPI,
							   DGE::Core::System::ContextRef context,
							   QObject*								parent = nullptr);
					D_Destructor(QtRenderer);

					/// \brief Initialize Render API for begin render
					virtual bool beginScene() override;

					/// \brief start render process
					///
					/// \return if this process is successful return true else return false
					virtual bool render(
						const std::shared_ptr<Interfaces::IRender>& renderItem) override;

					/// \brief draw an vertex array with specified info
					virtual bool drawVertexArray(const VertexArrayDrawInfo& vertexArrayInfo) override;

					/// \brief draw an element with specified info
					virtual bool drawElement(const ElementDrawInfo& elementInfo) override;

					/// \brief Finish render process and uninitialized temporary resources
					///
					/// \return if this process is successful return true else return false
					virtual bool endScene() override;

					/// \brief Create a shader for using in codes
					///
					/// \return return a shared pointer to GFX::GCore::Shader object.
					virtual std::shared_ptr<GFX::GCore::Shader> createShader() override;

					/// \brief Set boundary of view port and if setting it successful return true
					/// else return false
					///
					/// \return if setting it successful return true else return false
					virtual bool setViewport(Core::Math::Bound bound) override;

					/// \brief Set target window for render on it
					///
					/// \param window A shared pointer to Window object
					///
					/// \return if this process is successful return true else return false
					virtual bool setWindow(std::shared_ptr<Core::Windows::Window> window) override;

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
					virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

					// RendererGL interface
				protected:
					virtual void generateBuffer(uint32 size, uint32* buffers) override;
					virtual void activateBuffer(uint32 target, uint32 buffer) override;
					virtual void setBufferData(uint32		   target,
											   uint64		   size,
											   const void*	   data,
											   BufferUsageType usageType) override;
					virtual void updateBufferData(uint32	  target,
												  int64		  offset,
												  uint64	  size,
												  const void* data) override;
					virtual void generateVertexArrayObject(uint32 size, uint32* vaoIDs) override;
					virtual void activateVertexArrayObject(uint32 vaoID) override;
					virtual void activateAttribute(uint32 id) override;
					virtual void deactivateAttribute(uint32 id) override;

					/// \brief Generate texture objects and set ids of them to input list \p textureIDs
					virtual void generateTexture(uint32 size, uint32* textureIDs);
					/// \brief Active a texture that is specified it id
					virtual void activateTexture(uint32 target, uint32 textureID);
					/// \brief set data of texture in 2-dimensional to use in rendering
					virtual void setTexture2DData(uint32	  target,
												  int32		  lod,
												  int32		  internalFormat,
												  int32		  width,
												  int32		  height,
												  int32		  border,
												  uint32	  format,
												  uint32	  type,
												  const void* data);

					/// We added some specialize function for removing dependency on Qt library
					/// and using the engine with any similar and lightweight library

					/// \brief set format of array of vertex attributes
					virtual void setVertexAttribute(uint32 index, uint32 size,
													AttributeValueType type, bool normalized,
													int32 stride, const void* data);
					/// \brief render primitives from array data
					virtual void drawVertexArray(const RenderPrimitivesType& type, int32 first,
												 uint32 size);

					// IRelease interface
					virtual bool release() override;

				signals:
					void paint();

				public slots:
					void init();

				protected:
					std::shared_ptr<QQuickWindow>		m_window;
					std::shared_ptr<GFX::GCore::Shader> m_shader;

					friend class DGE::Platforms::QtPlatform;
				};

				using QtRendererRef = std::shared_ptr<GFX::GCore::QtWrapper::QtRenderer>;
#endif
			} // namespace QtWrapper
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
