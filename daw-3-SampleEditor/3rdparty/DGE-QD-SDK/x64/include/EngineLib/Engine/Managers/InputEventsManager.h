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

#include "EngineModuleDef.h"
#include <Core/IO/Input/InputEvents.h>
#include <Core/System/DObjectMacrosDef.h>
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Singleton/Singleton.h>

namespace DGE
{
	namespace Managers
	{
		/// \brief Manages all input events connections to manage all of them in the one place.
		class DGE_EngineAPI InputEventsManager
			: public DGE::Core::System::DSubsystem
			, public DGE::Core::Utility::Singleton<InputEventsManager>
			, public Core::IO::Input::InputEvents
		{
			D_OBJECT_SINGLETON(InputEventsManager)
		public:
			InputEventsManager(DGE::Core::System::ContextRef context);
			D_Destructor(InputEventsManager);

			/// \brief Register input events to be accessed by the user class.
			bool registerInputEvents(Core::IO::Input::InputEventsRef inputEvents);

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IRelease interface
			virtual bool release() override;

			// DSubsystem interface
			virtual bool registerAll(
				Core::Utility::Interfaces::InitializeParams& parameters) override;

		protected:
			void prepareInputEvents(Core::IO::Input::InputEventsRef inputEvents);

		protected:
			std::unordered_set<Core::IO::Input::InputEventsRef> m_inputEventsSet;
		};
		D_DefRefType(InputEventsManager)
	} // namespace Managers
} // namespace DGE
