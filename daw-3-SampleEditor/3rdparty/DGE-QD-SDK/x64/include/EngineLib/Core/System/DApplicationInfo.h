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
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/System/DCoreObject.h>

#include <Core/Utility/Interfaces/IInitialize.h>

#include <Core/Utility/Singleton/Singleton.h>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			/// \brief Contain information about the current running Application.
			///
			/// This class is first class that is initialized in core of engine
			/// and must call initialize(std::map<std::string, std::any>& parameters) function.
			struct DGE_CoreAPI DApplicationInfo
				: public DGE::Core::System::DCoreObject
				, public DGE::Core::Utility::Interfaces::IInitialize
				, public Utility::Singleton<DApplicationInfo>
			{
				D_SINGLETON(DApplicationInfo)
			protected:
				DApplicationInfo();

			public:
				D_Destructor(DApplication);
				/// IInitialize interface overload
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&) function
				/// from IInitialize interface.
				using Utility::Interfaces::IInitialize::initialize;

				/// \brief Initialize this class with given parameters
				/// \details Call this function with these parameters
				///
				/// \param parameters Input parameters for initialize this class.
				/// must set by below example:
				///
				/// \return return true if initialization is successfully otherwise return false
				///
				///	\code
				/// #include <iostream>
				/// #include <Core/Application/Application.h>
				/// int main(int argc, char* argv[])
				/// {
				///		// contain other codes
				///		DGE::Core::App app;
				///		if(app.initialize({{"Argv",argv}}))
				///			std::cout<<"App is initialized."<<std::endl;
				///		else
				///			std::cout<<"App initialization is failed."<<std::endl;
				///
				///		return 0;
				/// }
				/// \endcode
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				/// \brief get current path that application use for root of setting path of
				/// resources or files
				///
				/// \return current path of application
				const DString currentAppPath() const;

			public:
				/// \brief Use for setting key of argV parameter that pass into initialize(...) function
				static const DString s_argVKey;

				DString appName() const;
				void   setAppName(const DString& newAppName);

			protected:
				/// \brief Default path for access to disk in Application.
				DString m_appPath;

				/// \brief Default name of app
				DString m_appName;
			};

			using DApplicationInfoRef = std::shared_ptr<DApplicationInfo>;
		} // namespace System
	}	  // namespace Core
} // namespace DGE
