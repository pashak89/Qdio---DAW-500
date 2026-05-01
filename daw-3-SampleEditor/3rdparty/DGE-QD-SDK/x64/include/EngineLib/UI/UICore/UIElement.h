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

#include "Core/Math/Vector2D.h"
#include <GFX/GCore/Drawable.h>
#include <UI/UIModuleDef.h>

namespace DGE
{
	namespace UI
	{
		namespace UICore
		{
			/// \brief Base class for UI classes that provides basic interface
			class DGE_UIAPI UIElement : public DGE::GFX::GCore::Drawable
			{
				D_OBJECT(UIElement)
			public:
				UIElement(DGE::Core::System::ContextRef context
						  = DGE::Core::System::Context::getInstance());
				D_Destructor(UIManager);

				// IInitialize interface
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

				// IUpdate interface
				virtual bool update(GFX::GCore::InputParameters &input) override;

				// IRender interface
				virtual bool render(GFX::GCore::InputParameters &input) override;

				// IPreProcessRender interface
				virtual bool preProcessRender(GFX::GCore::InputParameters &input) override;

				// IPostProcessRender interface
				virtual bool postProcessRender(GFX::GCore::InputParameters &input) override;

			protected:
				Core::Math::Vector2D m_position;
			};
			using UIElementRef	= std::shared_ptr<DGE::UI::UICore::UIElement>;
			using UIElementWRef = std::weak_ptr<DGE::UI::UICore::UIElement>;

		} // namespace UICore
	}	  // namespace UI
} // namespace DGE
