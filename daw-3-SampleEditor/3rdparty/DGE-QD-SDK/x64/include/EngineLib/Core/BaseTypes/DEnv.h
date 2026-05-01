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

#include <Core/BaseTypes/BaseTypes.h>
#include <regex>

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{
			/// TODO: Implement applying EnvironmentVariables to changing Graphic-API and \
			/// any platform dependent requirements
			/// DONE: Move it to DPath
			/// TODO: Using Context for applying global EV.
			struct EnvironmentVariable
			{
				/// \brief Name of Template parameter that will be located in the string of path
				/// between '<>' signs
				DString name;
				/// \brief Value equal to the name of template that will be replaced with name variable
				DString value;
			};
			using DEnV = EnvironmentVariable;
		} // namespace BaseType
	}	  // namespace Core
} // namespace DGE
