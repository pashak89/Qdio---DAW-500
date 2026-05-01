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
#include <GFX/GCore/Interfaces/IRenderState.h>
#include <GFX/GCore/RenderPipelineDef.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			struct DGE_GFXAPI RenderStateData
			{
			};

			/// \brief Render state implementation class that is using for render state object
			class DGE_GFXAPI RenderStateImp : public Interfaces::IRenderState
			{
				friend class RenderState;

			public:
				RenderStateImp(std::shared_ptr<GFX::GCore::Renderer> renderer);

				virtual bool activate(std::shared_ptr<GFX::GCore::RenderStateImp> newState) = 0;
				virtual bool activate(std::shared_ptr<GFX::GCore::RenderStateData> newState) = 0;

				virtual std::shared_ptr<RenderStateData> cloneData()		= 0;
				virtual void setData(std::shared_ptr<RenderStateData> data) = 0;

			private:
				/// \brief Active render state object for using in rendering pipeline
				///
				/// \return Result of activation render state object
				virtual bool activate() override;

				/// \brief Deactive render state object for next render operations in rendering pipeline
				virtual bool deactivate() override;

			protected:
				std::shared_ptr<GFX::GCore::Renderer> m_renderer;
			};

			using RenderStateImpRef	 = std::shared_ptr<GFX::GCore::RenderStateImp>;
			using RenderStateImpWRef = std::weak_ptr<GFX::GCore::RenderStateImp>;
			using RenderStateImpURef = std::unique_ptr<GFX::GCore::RenderStateImp>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
