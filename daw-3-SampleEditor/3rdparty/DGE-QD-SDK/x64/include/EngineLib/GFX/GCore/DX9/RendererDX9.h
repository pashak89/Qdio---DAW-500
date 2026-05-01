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
#include <GFX/GCore/Renderer.h>

namespace DGE
{
	using namespace DGE::ResourceManager;
	using namespace DGE::Core::Math;
	using namespace Math;
	namespace GFX
	{
		namespace GCore
		{
			namespace DirectX9
			{
				class DeviceExploreDX9;

				// bass class for renderer that use for different API graphics
				class DGE_GFXAPI RendererDX9 : public Renderer
				{
					D_OBJECT(RendererDX9)
				protected:
					friend class DeviceExploreDX9;
#if DGE_Graphic_API == DGE_Use_Directx && defined(DGE_UseGraphicAPI)
					IDirect3DDevice9* m_Device = nullptr;
#endif
					// ID3DXSprite* m_Sprite=nullptr;
					uint32		  StageTexture = 0;
					static uint32 m_fontCounter;
					// D3DCAPS9* m_Caps = nullptr;
					RendererDX9(
						RenderAPIInfo requestedRenderAPI, std::shared_ptr<System::Context> context);

				public:
					D_Destructor(RendererDX9);

					/// IInitialize interface
					///
					/// \brief Use default implementation of
					/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
					/// function from IInitialize interface.
					using GFX::GCore::Renderer::initialize;

					/// \brief Initialize RendererDX9 and if it is initialized successfully return true
					///
					/// \return Return true if initializing is successfully else return false
					virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

					// Renderer interface
					virtual bool clear(ClearParam& param) override;
					virtual bool beginScene() override;
					virtual bool render(
						const std::shared_ptr<Interfaces::IRender>& renderItem) override;
					/// \brief draw an vertex array with specified info
					virtual bool drawVertexArray(const VertexArrayDrawInfo& vertexArrayInfo) override;
					/// \brief draw an element with specified info
					virtual bool drawElement(const ElementDrawInfo& elementInfo) override;
					virtual bool endScene() override;
					virtual bool present(PresentParam& param) override;
					virtual shared_ptr<GFX::GCore::Shader> createShader() override;
					virtual ShaderCompilerRef			   createShaderCompiler() override;
					virtual void*						   getNativeObject() override;

				protected:
					virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

					// IRelease interface
				public:
					virtual bool release() override;
				};
			} // namespace DirectX9
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
