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

#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <Core/System/DSubsystem.h>
#include <GFX/GCore/Interfaces/IRenderer.h>
#include <GFX/GCore/RenderStateManager.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief bass class for renderer that use for different API graphics
			class DGE_GFXAPI Renderer
				: public Core::System::DSubsystem
				, public GFX::GCore::Interfaces::IRenderer
			{
				D_OBJECT(Renderer)
			protected:
				DArray<StyleFont*> m_styleFonts;
				friend class Font;
				friend class RenderStateManager;
				friend class RenderState;

			public:
				Renderer(RenderAPIInfo						  requestedRenderAPI,
						 DGE::Core::System::ContextRef context);

				/// IInitialize interface
				///
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
				/// function from IInitialize interface.
				using Core::System::DSubsystem::initialize;

				/// \brief Create base graphic object based on specified type
				///
				/// \return Return shared pointer to created object
				Interfaces::IGpuObjectRef createGPUObject(GPUObjectType type);

				virtual bool activeRenderState(Interfaces::IRenderStateRef renderState) override;

				virtual bool deactiveRenderState(Interfaces::IRenderStateRef renderState) override;

				virtual RenderAPIInfo renderApiInfo() const override;

			protected:
				/// \brief activate render state for current rendering task
				virtual bool activeRenderStateImp(
					std::shared_ptr<RenderStateImp> renderState) override;

				/// \brief deactivate render state for current rendering task
				virtual bool deactiveRenderStateImp(
					std::shared_ptr<RenderStateImp> renderState) override;

			public:
				static const DString s_nativeWindowKey;

			protected:
				RenderAPIInfo m_renderApiInfo;
				/// \brief Current render state data that is activated
				std::shared_ptr<RenderStateManager> m_stateManager;
			};

			using RendererRef = std::shared_ptr<GFX::GCore::Renderer>;
			using RendererWRef = std::weak_ptr<GFX::GCore::Renderer>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
