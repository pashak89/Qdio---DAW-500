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
 * API Configurator header file is using for detecting available features and libraries
 */
#pragma once

#include <Core/Math/IBoundary.h>
#include <GFX/GCore/DebugRenderHelper.h>
#include <GFX/GCore/Interfaces/IRenderer.h>
#include <GFX/GCore/Renderer.h>
#include <Scene/Component.h>
#include <Scene/Components/SystemComponent.h>

namespace DGE
{
	namespace Scene
	{
		/// \brief Store list of IBasicGObjects or other stuff for draw debug items
		class DGE_SceneAPI DebugVolume
			: public DGE::Scene::Component
			, public DGE::GFX::GCore::Interfaces::IRender
		{
			D_OBJECT(DebugVolume)
		public:
			DebugVolume(System::ContextRef context);
			D_Destructor(DebugVolume);

			// IRenderPipeline interface
			using Scene::Component::initialize;

			// IInitialize interface
			bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			bool update(GFX::GCore::InputParameters& input) override;

			// IRender interface
			bool render(GFX::GCore::InputParameters& input) override;

			bool add(Core::Math::BoundaryRef boundary);

			bool isActivated() const;
			void setIsActivated(bool newIsActivated);

			Core::BaseType::Color debugColor() const;
			void				  setDebugColor(const Core::BaseType::Color& newDebugColor);

		protected:
			/// \brief Helper object for creating debug item that will be used in render pipeline
			GFX::GCore::DebugRenderHelperWRef m_debugRender;
			/// \brief Indicate whether this debug volume is activated in render pipeline.
			std::atomic_bool m_isActivated;
			/// \brief Map of boundary object and debug items
			std::map<Core::Math::BoundaryRef, GFX::GCore::DrawableRef> m_debugItemsMap;
			/// \brief List of uninitialized items
			std::vector<GFX::GCore::DrawableRef> m_uninitializedDebugItem;
			/// \brief Color of debug items that will be used for all debug items of this Debug volume.
			Core::BaseType::Color m_debugColor;
		};
		using DebugVolumeRef  = std::shared_ptr<DGE::Scene::DebugVolume>;
		using DebugVolumeWRef = std::weak_ptr<DGE::Scene::DebugVolume>;
	} // namespace Scene
} // namespace DGE
