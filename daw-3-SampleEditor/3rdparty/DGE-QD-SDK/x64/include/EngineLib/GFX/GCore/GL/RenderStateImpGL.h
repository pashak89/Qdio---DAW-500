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

#include "GFX/GCore/RenderPipelineDef.h"
#include <GFX/GCore/GL/RendererGL.h>
#include <GFX/GCore/RenderState.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace GL
			{
				using PairValues = std::pair<float, float>;
				/// \brief Render state data that is using in render state implementation class in OpenGL
				struct DGE_GFXAPI RenderStateDataGL : public RenderStateData
				{
					/// \brief Render state data
					static float	  s_minDepthValue;
					static float	  s_maxDepthValue;
					static float	  s_minNormalDepthValue;
					static float	  s_maxNormalDepthValue;
					static float	  s_minPinToFrontDepthValue;
					static float	  s_maxPinToFrontDepthValue;
					static float	  s_minPinToBackDepthValue;
					static float	  s_maxPinToBackDepthValue;
					static uint32	  s_srcBlends[(uint8) BlendMode::MaxCount];
					static uint32	  s_destBlends[(uint8) BlendMode::MaxCount];
					static uint32	  s_blendOperation[(uint8) BlendMode::MaxCount];
					static uint32	  s_compareFunction[(uint8) CompareMode::MaxCount];
					static PairValues s_depthValues[(uint32) DepthValueType::MaxCount];

					/// \brief Indicate whether must be enabled blend mode
					bool isBlendEnabled = false;
					/// \brief Store current blend mode
					BlendMode blendMode = BlendMode::Replace;
					/// \brief Indicate whether must be used alpha to coverage
					bool isAlphaToCoverageEnabled = false;

					/// \brief Indicate whether must be enabled color write functionality and
					/// related buffers
					bool isColorWriteEnabled = false;

					/// \brief Store current cull mode
					CullMode cullMode = CullMode::None;

					/// \brief Indicate whether must be enabled depth test
					bool isDepthTestEnabled = false;
					/// \brief Store current depth test mode
					CompareMode depthTestMode = CompareMode::Less;
					/// \brief  Indicate whether must be enabled depth write functionality and
					/// related buffers
					bool isDepthWriteEnabled = false;

					bool		   isDepthRangeChanged = false;
					float		   nearDepthRange = 0.0f, farDepthRange = 1.0f;
					DepthValueType depthValueType = DepthValueType::CustomValues;

					/// \brief  Indicate whether must be enabled stencil test functionality and
					/// related buffers
					bool isStencilTestEnabled = false;
				};

				/// \brief Custom class for implementing render state of every drawable object in OpenGL
				class DGE_GFXAPI RenderStateImpGL : public DGE::GFX::GCore::RenderStateImp
				{
				public:
					RenderStateImpGL(std::shared_ptr<GFX::GCore::GL::RendererGL> renderer);

					// RenderState interface
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
					virtual bool setStencilTest(bool enable, CompareMode mode,
												StencilOperation pass, StencilOperation fail,
												StencilOperation zFail, uint32 stencilRef,
												uint32 compareMask, uint32 writeMask) override;

					// virtual bool update() override;
					virtual bool activate(
						std::shared_ptr<GFX::GCore::RenderStateImp> newState) override;
					virtual bool activate(
						std::shared_ptr<GFX::GCore::RenderStateData> newState) override;
					virtual bool deactivate();

					virtual std::shared_ptr<RenderStateData> cloneData() override;
					virtual void setData(std::shared_ptr<RenderStateData> data) override;

					// virtual void* stateData() override;
				protected:
					void applyNewState(RenderStateDataGL);

				protected:
					virtual void activeBlendMode(bool enable, BlendMode mode, bool alphaToCoverage);
					virtual void activeColorWrite(bool enable);
					virtual void activeCullMode(CullMode mode);
					virtual void activeDepthTest(bool enable, CompareMode mode);
					virtual void activeDepthWrite(bool enable);
					virtual void activeDepthRange(float near, float far);
					virtual void activeDepthValueType(DepthValueType depthValueType);

					virtual void activeFillMode(FillMode fillMode);
					virtual void activeLineAntialias(bool enable);
					virtual void activeScissorTest(bool enable, const Core::Math::Bound);
					virtual void activeStencilTest(bool enable, CompareMode mode,
												   StencilOperation pass, StencilOperation fail,
												   StencilOperation zFail, uint32 stencilRef,
												   uint32 compareMask, uint32 writeMask);

				protected:
					RenderStateDataGL m_data;
				};

				using RenderStateImpGLRef  = std::shared_ptr<GFX::GCore::GL::RenderStateImpGL>;
				using RenderStateImpGLWRef = std::weak_ptr<GFX::GCore::GL::RenderStateImpGL>;
			} // namespace GL
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
