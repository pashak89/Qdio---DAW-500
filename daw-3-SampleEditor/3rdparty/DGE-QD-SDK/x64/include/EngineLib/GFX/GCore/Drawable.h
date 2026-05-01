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

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/System/Context.hpp>
#include <Core/System/DObject.h>
#include <Core/System/Event.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <GFX/GCore/Interfaces/IPostProcessRender.h>
#include <GFX/GCore/Interfaces/IPreProcessRender.h>
#include <GFX/GCore/Interfaces/IRenderPipeline.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Basic interfaces for drawable items RenderManager
			class DGE_GFXAPI Drawable
				: public Core::System::DObject
				, public Core::Utility::Interfaces::IInitialize
				, public GFX::GCore::Interfaces::IRenderPipeline
			{
				D_OBJECT(Drawable)
			public:
				Drawable(DGE::Core::System::ContextRef context
						 = Core::System::Context::getInstance());

				// IRenderPipeline interface
				/// \brief Call when object must be initialized.
				using Core::Utility::Interfaces::IInitialize::initialize;

				virtual bool initialize(
					Core::Utility::Interfaces::InitializeParams &parameters) override;

				// IInitialize interface
				virtual bool isInitialized() override;

				// IUpdate interface
				/// \brief Call when engine is updated
				using GFX::GCore::Interfaces::IRenderPipeline::update;

				// IPreRender interface
				/// \brief call this function before main function of rendering render(const
				/// std::any &input)
				using GFX::GCore::Interfaces::IPreProcessRender::preProcessRender;

				// IRender interface
				/// \brief call when process of rendering started and must this object
				/// complete own rendering process
				using GFX::GCore::Interfaces::IRenderPipeline::render;

				// IPostRender interface
				/// \brief call this function after main function of rendering render(const std::any &input)
				using GFX::GCore::Interfaces::IPostProcessRender::postProcessRender;

				/// retrieve enable property of this object
				bool enable() const;

				/// set enable property of this object
				void setEnable(bool isEnable);

				Core::System::Event<bool, Core::Utility::Interfaces::InitializeParams> &initialized()
					const;

				Core::System::Event<bool, GCore::InputParameters> &updated() const;

				Core::System::Event<bool, GCore::InputParameters> &preProcessRendered() const;

				Core::System::Event<bool, GCore::InputParameters> &rendered() const;

				Core::System::Event<bool, GCore::InputParameters> &postProcessRendered() const;

				Core::System::Event<void, bool> &isEnableChanged() const;

				/// \brief Retrieve total number of vertices that was rendered
				virtual uint64 totalNumberVertices();

			protected:
				virtual void setIsInitialized(bool isInitialized) override;

			protected:
				std::atomic_bool							m_isEnable;
				mutable Core::System::Event<bool, Core::Utility::Interfaces::InitializeParams>
																		  m_initialized;
				mutable Core::System::Event<bool, GCore::InputParameters> m_updated;
				mutable Core::System::Event<bool, GCore::InputParameters> m_preProcessRendered;
				mutable Core::System::Event<bool, GCore::InputParameters> m_rendered;
				mutable Core::System::Event<bool, GCore::InputParameters> m_postProcessRendered;
				mutable Core::System::Event<void, bool>					  m_isEnableChanged;
			};

			using DrawableRef = std::shared_ptr<Drawable>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
