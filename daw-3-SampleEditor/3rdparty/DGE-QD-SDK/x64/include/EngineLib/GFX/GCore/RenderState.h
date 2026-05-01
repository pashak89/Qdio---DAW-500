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

#include <Core/Math/Bound.h>
#include <Core/System/DSubsystem.h>
#include <GFX/GCore/Interfaces/IActivable.h>
#include <GFX/GCore/Interfaces/IRenderState.h>
#include <GFX/GCore/Interfaces/IUpdate.h>
#include <GFX/GCore/RenderPipelineDef.h>
#include <GFX/GCore/RenderStateImp.h>
#include <GFX/GCore/RenderStateManager.h>
#include <GFX/GCore/Renderer.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Base class for storing render state of every drawable object to use in the
			/// rendering pipeline
			class DGE_GFXAPI RenderState : public Interfaces::IRenderState
			{
				friend class RenderStateManager;
				friend class Renderer;

			public:
				RenderState(std::shared_ptr<GFX::GCore::Renderer> renderer);

				// IRenderState interface
				virtual bool setBlendMode(bool enable, BlendMode mode,
										  bool alphaToCoverage) override;
				virtual bool setColorWrite(bool enable) override;
				virtual bool setCullMode(CullMode mode) override;
				virtual bool setDepthTest(bool enable, CompareMode mode) override;
				virtual bool setDepthWrite(bool enable) override;
				virtual bool setDepthRange(float nearRange, float farRange) override;
				virtual bool setDepthValueType(DepthValueType depthValueType) override;

				virtual bool setFillMode(FillMode fillMode) override;
				virtual bool setLineAntialias(bool enable) override;
				virtual bool setScissorTest(bool enable, const Core::Math::Bound) override;
				virtual bool setStencilTest(bool enable, CompareMode mode, StencilOperation pass,
											StencilOperation fail, StencilOperation zFail,
											uint32 stencilRef, uint32 compareMask,
											uint32 writeMask) override;

				/// \brief Update render state and render pipeline
				virtual bool update();

				// IActivable interface
				/// \brief Active render state object for using in rendering pipeline
				///
				/// \return Result of activation render state object
				virtual bool activate() override;

				/// \brief Deactive render state object for next render operations in rendering pipeline
				virtual bool deactivate() override;

				/// \brief Return render state data of this render state object
				virtual RenderStateImpRef stateData();

			protected:
				std::shared_ptr<GFX::GCore::Renderer> m_renderer;
				RenderStateImpRef					  m_renderStateImp;
				/// \brief Indicate whether current render state is activated
				bool m_isActivated;
			};
			using RenderStateRef  = std::shared_ptr<GFX::GCore::RenderState>;
			using RenderStateWRef = std::weak_ptr<GFX::GCore::RenderState>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
