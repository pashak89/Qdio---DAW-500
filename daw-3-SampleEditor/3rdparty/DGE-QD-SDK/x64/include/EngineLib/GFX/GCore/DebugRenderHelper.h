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

#include <Core/Math/AABB.h>
#include <Core/Math/Ray.h>
#include <Core/System/DSubsystem.h>
#include <GFX/GCore/Interfaces/IRenderPipeline.h>
#include <GFX/Primitives/Cube.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Helper class for creating debug items
			class DGE_GFXAPI DebugRenderHelper
				: public DGE::Core::System::DSubsystem
				, public Core::Utility::Singleton<GFX::GCore::DebugRenderHelper>
			{
				D_OBJECT_SINGLETON(DebugRenderHelper)
				DebugRenderHelper(DGE::Core::System::ContextRef context
								  = Core::System::Context::getInstance());

			public:
				// DSubsystem interface
				/// \brief Call when object must be initialized.
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				virtual bool release() override;

				bool createDebugLine();

				/// \brief Create debug item for the specified boundary
				GFX::GCore::DrawableRef createDebugBoundary(Core::Math::BoundaryRef boundary);

				/// /brief Create debug item for the specified ray
				GFX::GCore::DrawableRef createDebugRay(Core::Math::RayRef ray, double length = 1000);

				// DSubsystem interface
				Core::BaseType::Color debugColor() const;
				void				  setDebugColor(const Core::BaseType::Color& newDebugColor);

			protected:
				virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

			protected:
				/// \brief Color of debug items that will be used for all debug items.
				Core::BaseType::Color m_debugColor;
			};
			using DebugRenderHelperRef = std::shared_ptr<DGE::GFX::GCore::DebugRenderHelper>;
			using DebugRenderHelperWRef = std::weak_ptr<DGE::GFX::GCore::DebugRenderHelper>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
