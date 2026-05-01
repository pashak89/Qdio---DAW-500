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

#include "Core/System/DObjectMacrosDef.h"
#include <Scene/Components/DebugVolume.h>
#include <Scene/Components/MaterialComponent.h>
#include <Scene/Components/ModelComponent.h>
#include <Scene/Components/SystemComponent.h>

namespace DGE
{
	namespace Scene
	{
		class SolidRenderer : public DGE::Scene::SystemComponent
		{
			D_OBJECT(SolidRenderer)
		public:
			SolidRenderer(std::shared_ptr<System::Context> context);

			// IRenderPipeline interface
			/// \brief Call when object must be initialized.
			using Scene::Component::initialize;

			// IInitialize interface
			bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			bool update(GFX::GCore::InputParameters& input) override;

			// IPreProcessRender interface
			bool preProcessRender(GFX::GCore::InputParameters& input) override;

			// IRender interface
			bool render(GFX::GCore::InputParameters& input) override;

			// IPostProcessRender interface
			bool postProcessRender(GFX::GCore::InputParameters& input) override;

		public:
			/// \brief Default name for isSelected variable in main shader code to update it when
			/// it's necessary
			static const DString s_currentColorName;
			/// \brief Default name for isRandomColoring variable in main shader code to update it
			/// when it's necessary
			static const DString s_isRandomColoringName;

		protected:
			D_PROPERTY(Scene::MaterialComponentRef, materialComponent, MaterialComponent)
			D_PROPERTY(Scene::ModelComponentRef, modelComponent, ModelComponent)
			D_PROPERTY(Scene::DebugVolumeRef, debugVolume, DebugVolume)
			D_PROPERTY(bool, isFixedSize, IsFixedSize, false);
		};
		D_DefRefType(SolidRenderer);
	} // namespace Scene
} // namespace DGE
