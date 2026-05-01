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

#if DGE_Compiler_Mode == DGE_DEBUG

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			/// \brief Retrieves human readable names from C++ run-time type information.
			class DbgTypeDemangler
			{
			public:
				DGE_CoreAPI DbgTypeDemangler(const std::type_info& typeInfo);
				DGE_CoreAPI ~DbgTypeDemangler();

				/// \brief Retrieve human readable names from C++ run-time type information.
				/// @return The human readable type name.
				DGE_CoreAPI const char* get();

			protected:
				/// \brief The translated name name.
				const char* name;
			};

		} // namespace Utility
	}	  // namespace Core
} // namespace DGE

#endif
