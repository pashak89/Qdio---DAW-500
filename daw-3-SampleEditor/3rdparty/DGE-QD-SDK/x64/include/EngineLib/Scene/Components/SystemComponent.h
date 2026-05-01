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

#include <Scene/Component.h>

namespace DGE
{
	namespace Scene
	{
		/// \brief Base class for modifying other components of the parent node
		class SystemComponent
			: public DGE::Scene::Component
			, public GFX::GCore::Interfaces::IPreProcessRender
			, public GFX::GCore::Interfaces::IRender
			, public GFX::GCore::Interfaces::IPostProcessRender
		{
			D_OBJECT(SystemComponent)
		public:
			SystemComponent(std::shared_ptr<System::Context> context);
			D_Destructor(SystemComponent);

			/// \brief Initialize SystemComponent and if it is initialized successfully return true
			///
			/// \return Return true if initializing is successfully else return false
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IPostProcessRender interface
			virtual bool postProcessRender(GFX::GCore::InputParameters& input) override;

			// IRender interface
			virtual bool render(GFX::GCore::InputParameters& input) override;

			// IPreProcessRender interface
			virtual bool preProcessRender(GFX::GCore::InputParameters& input) override;

			Core::System::Event<bool, GFX::GCore::InputParameters>& preProcessRendered() const;

			Core::System::Event<bool, GFX::GCore::InputParameters>& rendered() const;

			Core::System::Event<bool, GFX::GCore::InputParameters>& postProcessRendered() const;

		protected:
			mutable Core::System::Event<bool, GFX::GCore::InputParameters> m_preProcessRendered;
			mutable Core::System::Event<bool, GFX::GCore::InputParameters> m_rendered;
			mutable Core::System::Event<bool, GFX::GCore::InputParameters> m_postProcessRendered;
		};
		D_DefRefType(SystemComponent);

	} // namespace Scene
} // namespace DGE
