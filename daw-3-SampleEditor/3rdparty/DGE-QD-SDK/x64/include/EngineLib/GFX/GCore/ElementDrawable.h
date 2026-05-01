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

#include "GFX/GFXModuleDef.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/System/Context.h>
#include <Core/System/DObjectMacrosDef.h>
#include <GFX/GCore/Element.h>
#include <GFX/GCore/Interfaces/IGObjectDrawable.h>
#include <GFX/GCore/Interfaces/IGpuBuffer.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			class DGE_GFXAPI ElementDrawable : public GFX::GCore::Interfaces::IGObjectDrawable
			{
				D_OBJECT(ElementDrawable)
			public:
				ElementDrawable(
					GFX::GCore::RenderPrimitivesType renderPrimitivesType
					= GFX::GCore::RenderPrimitivesType::Triangles,
					Core::System::ContextRef context = Core::System::Context::getInstance())
					: GFX::GCore::Interfaces::IGObjectDrawable()
				{
				}
				// IInitialize interface
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

				// IUpdate interface
				virtual bool update(GFX::GCore::InputParameters &input) override;

				// IPreProcessRender interface
				virtual bool preProcessRender(GFX::GCore::InputParameters &input) override;

				// IRender interface
				virtual bool render(GFX::GCore::InputParameters &input) override;

				// IPostProcessRender interface
				virtual bool postProcessRender(GFX::GCore::InputParameters &input) override;

				// IGObjectDrawable interface
				virtual GFX::GCore::Interfaces::IGObjectDrawableRef clone(uint16 lod) const override;
				virtual std::vector<Core::Math::Vector3D>			vertices() const override;

				uint64_t totalNumberVertices() override;

				GFX::GCore::ElementRef element() const;
				void				   setElement(const GFX::GCore::ElementRef &newElement);

				GFX::GCore::RenderPrimitivesType renderPrimitivesType() const;
				void setRenderPrimitivesType(GFX::GCore::RenderPrimitivesType newRenderType);

			protected:
				GFX::GCore::ElementRef			 m_element;
				GFX::GCore::RenderPrimitivesType m_renderType;
			};
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
