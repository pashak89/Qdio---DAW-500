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

#include <any>

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			namespace Interfaces
			{
/// \brief Helper macro for simplify and unifying logging and parsing parameters in initializing
/// process
///
/// \param parameters The input initialize parameters of IInitialize::initialize() function
/// \param key The key that want it from param
/// \param typeCast The type of destination variable
/// \param variable The destination variable for storing data
/// \param className The name of class that is initialized and use it for log message
/// \param isOptional Indicate that getting this parameter is necessary for initialization or not
/// \return result_##className If find specific key in parameters list and converted it
/// successfully, set result_##className into true else set to false that log it if couldn't find it
#define ParameterAnyCast(parameters, key, variable, className, isOptional)                   \
	bool result_##variable##className = false;                                               \
	if (parameters.find(key) != parameters.end())                                            \
	{                                                                                        \
		auto anyParam = parameters[key];                                                     \
		try                                                                                  \
		{                                                                                    \
			variable					 = std::any_cast<decltype(variable)>(anyParam);      \
			result_##variable##className = true;                                             \
		}                                                                                    \
		catch (const std::bad_any_cast& e)                                                   \
		{                                                                                    \
			UNUSED(e);                                                                       \
			LogFatalError(                                                                   \
				DST("Can't \"" #className "\" initialize with input parameters. Expected ")  \
				+ typeid(variable).name() + DST(" type but got ") + anyParam.type().name()); \
		}                                                                                    \
	}                                                                                        \
	else                                                                                     \
	{                                                                                        \
		if (!isOptional)                                                                     \
		{                                                                                    \
			LogFatalError(DST("Can't initialize \"" #className "\" with input parameters."   \
							  " Couldn't find parameter with name ")                         \
						  + key);                                                            \
		}                                                                                    \
	}
/// \brief Helper macro for simplify and unifying initialize process and log
///
/// \param object It's the object that want to initialize it.
/// \param initParam It's input initialize parameter
/// \param objectName It's name of object that is showing in log message
#define D_InitializeObject(object, initParam, objectName)          \
	bool result_##object = false;                                  \
	if (object && object->initialize(initParam))                   \
	{                                                              \
		result_##object = true;                                    \
	}                                                              \
	else                                                           \
	{                                                              \
		LogError("\"" #objectName "\" initialization is failed."); \
	}

				using InitializeParams = std::map<DString, std::any>;

				/// Initialize Interface
				/// \brief Use this interface for showing 'must call Initialize(
				class DGE_CoreAPI IInitialize
				{
				public:
					/// Initialize this object with input parameters
					/// \brief Abstract method for initialize inherited class
					virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters)
						= 0;
					virtual bool initialize(
						std::initializer_list<std::pair<const DString, std::any>>&& parameters = {})
					{
						InitializeParams param = parameters;
						return initialize(param);
					}
				};
			} // namespace Interfaces
		}	  // namespace Utility
	}
}
