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

#include <Core/System/DModule.h>
#include <GFX/GCore/Drawable.h>
#include <UI/UIManager.h>
#include <UI/UIModuleDef.h>

namespace DGE
{
	namespace UI
	{
		class DGE_UIAPI UIModule
			: public DGE::Core::System::DModule
			, public Core::Utility::Singleton<UIModule>
		{
			D_OBJECT_SINGLETON(UIModule)
			UIModule(std::shared_ptr<Core::System::Context> context
					 = DGE::Core::System::Context::getInstance());

		public:
			D_Destructor(UIModule);

			/// IInitialize interface
			///
			/// \brief Use default implementation of
			/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
			/// function from IInitialize interface.
			using Core::Utility::Interfaces::IInitialize::initialize;

			/// \brief Initialize CoreModule and if it is initialized successfully return true
			///
			/// \return Return true if initializing is successfully else return false
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual bool release() override;

			UIManagerWRef uiManager() const;

		protected:
			virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

		protected:
			UIManagerRef m_uiManager;
		};

		using UIModuleRef  = std::shared_ptr<UI::UIModule>;
		using UIModuleWRef = std::weak_ptr<UI::UIModule>;
	} // namespace UI
} // namespace DGE
