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
#include <Core/Utility/Interfaces/IExecutable.h>

namespace DGE
{
	class Engine;
	using EngineRef = std::shared_ptr<DGE::Engine>;
	namespace Core
	{
		namespace System
		{
			/// \brief An base class for implement main logic of the app that will be
			/// communicated with the engine
			class DGE_CoreAPI DBaseApplication
				: public DGE::Core::System::DSubsystem
				, public Core::Utility::Interfaces::IExecutable
			{
				D_OBJECT(DBaseApplication)

			public:
				DBaseApplication();
				D_Destructor(DBaseApplication);

				/// IInitialize interface
				///
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
				/// function from IInitialize interface.
				using Core::System::DSubsystem::initialize;

				///\brief Execute this app and engine until close the engine
				virtual int32 exec() override;

				/*/// \brief Initialize Profiler and if it is initialized successfully return true
				///
				/// \return Return true if initializing is successfully else return false
				virtual bool initialize(std::map<String, std::any>& parameters) override;
				protected:
					virtual bool registerAll(std::map<String, any>& parameters) override;*/
			protected:
				Core::Utility::Interfaces::InitializeParams m_initializedParameters;
				DGE::EngineRef								m_engine;
			};

		} // namespace System
	}	  // namespace Core
} // namespace DGE
