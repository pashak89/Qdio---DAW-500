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

#include <Core/System/DObjectMacrosDef.h>
#include <Core/System/DProperty.h>
#include <Core/System/DSubsystem.h>
#include <Core/System/Event.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <GFX/GCore/Renderer.h>
#include <GFX/GFXModuleDef.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		/// \brief Manage the graphic objects and properties of general graphic objects
		class DGE_GFXAPI GraphicsManager
			: public DGE::Core::System::DSubsystem
			, public Core::Utility::Singleton<GraphicsManager>
		{
			D_OBJECT_SINGLETON(GraphicsManager)
			GraphicsManager(
				GFX::GCore::RendererRef		  renderer,
				DGE::Core::System::ContextRef context = DGE::Core::System::Context::getInstance());

		public:
			D_Destructor(GraphicsManager);

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual bool release() override;

			// DSubsystem interface
			virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

			DString defaultVertexShader();
			DString defaultFragmentShader();

			void setIsEnabledDebugRayTracing(bool isEnabled);
            bool isEnabledDebugRayTracing();

		protected:
			DString getInitializeShaderCommand();

			/// \brief Value of initialize shader command
			D_STATIC_READONLY_PROPERTY(DString, initializeShaderCommand)

			GFX::GCore::RendererRef m_renderer;
		};

		using GraphicsManagerRef = std::shared_ptr<GFX::GraphicsManager>;
		using GraphicsManagerWRef = std::weak_ptr<GFX::GraphicsManager>;
	} // namespace GFX
} // namespace DGE
