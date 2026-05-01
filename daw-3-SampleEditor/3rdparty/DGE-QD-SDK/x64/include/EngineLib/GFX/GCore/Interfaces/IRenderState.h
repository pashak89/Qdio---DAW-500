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
#include <GFX/GCore/Interfaces/IUpdate.h>
#include <GFX/GCore/RenderPipelineDef.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace Interfaces
			{
				/// \brief Base class for storing render state of every drawable object to use in
				/// the rendering pipeline
				class DGE_GFXAPI IRenderState : public IActivable
				{
				public:
					virtual bool setBlendMode(bool enable, BlendMode mode, bool alphaToCoverage) = 0;
					virtual bool setColorWrite(bool enable)						  = 0;
					virtual bool setCullMode(CullMode mode)						  = 0;
					virtual bool setDepthTest(bool enable, CompareMode mode)	  = 0;
					virtual bool setDepthWrite(bool enable)						  = 0;
					virtual bool setDepthRange(float near, float far)			  = 0;
					virtual bool setDepthValueType(DepthValueType depthValueType) = 0;

					virtual bool setFillMode(FillMode fillMode)						  = 0;
					virtual bool setLineAntialias(bool enable)						  = 0;
					virtual bool setScissorTest(bool enable, const Core::Math::Bound) = 0;
					virtual bool setStencilTest(bool enable, CompareMode mode,
												StencilOperation pass, StencilOperation fail,
												StencilOperation zFail, uint32 stencilRef,
												uint32 compareMask, uint32 writeMask)
						= 0;

				protected:
				};
				using IRenderStateRef  = std::shared_ptr<GFX::GCore::Interfaces::IRenderState>;
				using IRenderStateWRef = std::weak_ptr<GFX::GCore::Interfaces::IRenderState>;
			} // namespace Interfaces
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
