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

#include <Core/System/DSubsystem.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <GFX/GCore/Drawable.h>
#include <GFX/GCore/Interfaces/IRender.h>
#include <GFX/GCore/Interfaces/IUpdate.h>
#include <GFX/GCore/RenderState.h>
#include <GFX/GCore/Renderer.h>

namespace DGE
{
	namespace GFX
	{
		/// \brief Manage rendering pipeline of register object in itself
		class DGE_GFXAPI DrawablesManager
			: public Core::System::DSubsystem
			, public GFX::GCore::Interfaces::IRender
			, public GFX::GCore::Interfaces::IUpdate
			, public Singleton<DrawablesManager>
		{
			D_OBJECT_SINGLETON(DrawablesManager)

			DrawablesManager();
			DrawablesManager(DGE::Core::System::ContextRef context);

		public:
			D_Destructor(RenderManager);

			// IInitialize interface
			using Core::System::DSubsystem::initialize;
			/// \brief Initialize itself with getting current Renderer subsystem
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual bool release() override;

			/// \brief Add an item to rendering queue.
			///
			/// \details Add the drawable item to rendering queue and uninitialized items queue and
			/// after initializing it remove from queue.
			///
			/// \param drawableItem A object that inherit from Drawable class.
			bool addItem(GFX::GCore::DrawableRef drawableItem);

			/// \brief Remove an item from rendering queue.
			///
			/// \details Remove the drawable item from rendering queue and uninitialized items queue
			///
			/// \param drawableItem A object that inherit from Drawable class.
			bool removeItem(GFX::GCore::DrawableRef drawableItem);

			// IRender interface
			/// \brief render all Drawable Lists
			virtual bool render(GCore::InputParameters& inputParam) override;

			// IUpdate interface
			/// \brief render all Drawable Lists
			virtual bool update(GCore::InputParameters& inputParam) override;

			std::shared_ptr<GFX::GCore::Renderer> renderer() const;

		protected:
			virtual bool registerAll(
				Core::Utility::Interfaces::InitializeParams& parameters) override;

		public:
			static const DString s_rendererKey;
			static const DString s_clearParamKey;
			static const DString s_currentFrameNumber;

		protected:
			/// \brief priority Lists of Drawable items
			std::map<int, std::list<GFX::GCore::DrawableRef>> m_drawablesList;
			/// \brief helper variable that store map between the drawable and the queue number
			std::map<GFX::GCore::DrawableRef, int> m_drawablesMap;
			/// \brief uninitialized items queue that uses for initializing items
			std::list<GFX::GCore::DrawableRef> m_uninitalizedItem;
			/// \brief Renderer object that uses for rendering
			std::shared_ptr<GFX::GCore::Renderer> m_renderer;
			/// \brief Current priority number that is used for rendering pipeline
			uint32 m_currentPriority;
			/// \brief Current render state
			GCore::RenderStateRef m_renderState;

			// TODO: Must moved into DGE::System::Context class. After doing this work, we
			// can call Interfaces::IInitialize::initialize without any parameters and must
			// Interfaces::IInitialize::initialize pass initialized parameters from context by
			// retrieving from it.

			/// \brief Reference to initialized parameters in
			/// Interfaces::IInitialize::initialize
			Core::Utility::Interfaces::InitializeParams							m_tempParemeters;
			std::reference_wrapper<Core::Utility::Interfaces::InitializeParams> m_parameters
				= m_tempParemeters;
		};
		using DrawablesManagerRef  = std::shared_ptr<DGE::GFX::DrawablesManager>;
		using DrawablesManagerWRef = std::weak_ptr<DGE::GFX::DrawablesManager>;
	} // namespace GFX
} // namespace DGE
