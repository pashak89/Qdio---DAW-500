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

#include "DSubmodule.h"
#include <Core/System/DSmartInheritanceObject.h>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			enum class Category
			{
				Core,
				GFX,
			};

			/// \brief Base class for inheritance engine systems
			/// that it is managing tasks of systems and it contains a list of submodules
			/// that will be used for separating systems in engine structure and future usage.
			class DGE_CoreAPI DModule : public DSubmodule
			{
				D_OBJECT(DModule)
			public:
				DModule(std::shared_ptr<Context> context);

				/// IInitialize interface
				///
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
				/// function from IInitialize interface.
				using Utility::Interfaces::IInitialize::initialize;

				/// \brief Register submodule with specified Category and id
				///
				/// \param category Determine general variant of the submodule
				///
				/// \param id Determine unique id of the submodule
				///
				/// \param submodule Shared pointer of the submodule object
				///
				/// \return Return true if registering this submodule is successful
				bool registerSubmodule(Category category, const DString& id,
									   std::shared_ptr<DSubmodule> submodule);

			protected:
				/// Use for storing submodule of this system
				std::map<Category, std::map<DString, Utility::DSmartInheritanceObject<DSubmodule>>>
					m_submoduleTable;
			};

		} // namespace System
	}	  // namespace Core
} // namespace DGE
