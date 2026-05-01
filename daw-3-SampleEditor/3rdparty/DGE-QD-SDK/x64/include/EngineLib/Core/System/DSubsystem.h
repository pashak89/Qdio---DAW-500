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

#include <Core/System/Context.h>
#include <Core/System/DObject.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <Core/Utility/Interfaces/IRelease.h>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			/// \brief The base class for subsystem of the engine
			class DGE_CoreAPI DSubsystem
				: public Core::System::DObject
				, public Core::Utility::Interfaces::IInitialize
				, public Core::Utility::Interfaces::IRelease
			{
				D_OBJECT(DSubsystem)
			public:
				/// \brief Constructor for DSubsystem class.
				///
				/// \param context Reference to context object
				/// \param isAutoRegisterInContext Indicate 'is registering this subsystem in
				/// context automatically or not'
				DSubsystem(DGE::Core::System::ContextRef context,
						   bool									isAutoRegisterInContext = true);

				D_Destructor(DSubsystem);

				/// IInitialize interface
				///
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
				/// function from IInitialize interface.
				using Utility::Interfaces::IInitialize::initialize;
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				/// \brief Retrieve the flag auto registering in context
				bool isAutoRegisterInContext() const;
				/// \brief Set the flag auto registering in context to specified value 'newValue'.
				///
				/// \note Must set this flag before of initializing this class.
				void setIsAutoRegisterInContext(bool newValue);

			protected:
				/// \brief an abstract method for registering all data-types and events.
				virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) = 0;

				bool m_isAutoRegisterInContext = true;
			};
			using DSubsystemRef = std::shared_ptr<Core::System::DSubsystem>;
			using DSubsystemWRef = std::weak_ptr<Core::System::DSubsystem>;
		} // namespace System
	}	  // namespace Core
} // namespace DGE
