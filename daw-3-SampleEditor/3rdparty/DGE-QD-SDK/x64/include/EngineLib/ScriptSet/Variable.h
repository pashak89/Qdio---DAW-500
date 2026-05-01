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

#include <Core/Array/Array.h>
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <ScriptSet/ScriptModuleDef.h>
#include <any>

namespace DGE
{
	namespace ScriptSet
	{
		// script variable type.
		enum class DGE_ScriptAPI VariableType
		{
			Bool,
			Color,
			Float,
			Number,
			String,
			Vector3D,
			Vector2D,
			Unknown,
		};

		// Variable Class.
		class DGE_ScriptAPI Variable
		{
		public:
			Variable(const char* name, FILE* file);
			Variable(const char* name, VariableType type, void* value);

			virtual ~Variable();
			VariableType type();
			string		 name();
			//? Sure Get Copy of this Data because address of data may be changed & occur problem.
			any data();
			template<typename type>
			type data()
			{
				return std::any_cast<type>(m_data);
			}
			void setValue(any value);

			void printValue();

		protected:
			VariableType							   m_type;			 // type of variable
			std::string								   m_name = nullptr; // name of variable
			std::any								   m_data = nullptr; // Data stored in variable.
			uint64									   m_size;
			static std::map<VariableType, std::string> s_variableFormatTable;
		};
	} // namespace ScriptSet
} // namespace DGE
