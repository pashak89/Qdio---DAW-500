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
#include <ScriptSet/ScriptModuleDef.h>
#include <ScriptSet/Variable.h>

//Done: Add checking state for script and get this from HasLoaded() method.
namespace DGE
{
	namespace ScriptSet
	{
		//Script Resource Class.
		class DGE_ScriptAPI Script : public DGE::Core::ResourceManager::ObjectResource
		{
			DArray<Variable*> m_Variables;
			char buffer[MaxPathLength];
			bool Loaded=false;
		public:

			Script(const DChar* name, const DChar* path = DL("./"));
			virtual ~Script();

			void addVariable(const char* name, VariableType type, void* value);
			//true when Set value is successfully.
			bool setVariable(const char* name, void* value);

			bool saveToFile(const DChar* filename = nullptr);
			//Get a Variable with \param VariableName
			std::any data(const char* variableName);

			template<typename type>
			type data(const char* variableName)
			{
				for (auto var : m_Variables)
				{
					if (var->name() == variableName)
					{
						return var->data<type>();
					}
				}
				return type();
			}

			// Test Value of All variable.
			void traceVariable();

			//Script is loaded successfully!
			bool isLoaded();
		};
		typedef Script ScriptResource;
		using ScriptRef = std::shared_ptr<Script>;
	}
}
