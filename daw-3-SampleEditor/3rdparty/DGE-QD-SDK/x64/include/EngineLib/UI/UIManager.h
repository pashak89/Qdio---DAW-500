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

#include "UI/UICore/UIElement.h"
#include <GFX/GCore/Drawable.h>
#include <UI/UIModuleDef.h>

namespace DGE
{
	namespace UI
	{
		/// \brief The UIManager class
		class DGE_UIAPI UIManager : public DGE::GFX::GCore::Drawable
		{
			D_OBJECT(UIManager)
		public:
			UIManager(
				DGE::Core::System::ContextRef context = DGE::Core::System::Context::getInstance());

			D_Destructor(UIManager);

			/// \brief Add an UI element.
			///
			/// \details Add the UI element to current UI Elements list of UI Manager and
			/// uninitialized UI elements queue and after initializing it remove from queue.
			///
			/// \param uiElement A object that inherit from UIElement class.
			bool addUIElement(UI::UICore::UIElementRef uiElement);

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
			/// \brief priority Lists of UIElement items
			std::map<int, std::list<UI::UICore::UIElementRef>> m_uiElementsList;
			/// \brief uninitialized items queue that uses for initializing items
			std::list<UI::UICore::UIElementRef> m_uninitalizedItem;
			/// \brief Current priority number that is used for rendering pipeline
			uint32 m_currentPriority;
		};
		using UIManagerRef	= std::shared_ptr<DGE::UI::UIManager>;
		using UIManagerWRef = std::weak_ptr<DGE::UI::UIManager>;
	} // namespace UI
} // namespace DGE
