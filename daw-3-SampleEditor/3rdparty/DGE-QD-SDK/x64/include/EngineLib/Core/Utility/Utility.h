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

#include <Core/CoreModuleDef.h>
#include <algorithm>
#include <atomic>
#include <iterator>
#include <sstream>
#include <string>

#include <Core/BaseTypes/Color.h>
#include <Core/IO/LogSystem/LogMacrosDef.h>

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
#define UNUSED(x) ((void) x)
#define LogConsole(msg)                                                  \
	DGE::Core::Utility::WriteDebugConsoleToFile(msg,              \
													   DGE_FileCodeInfo, \
													   DGE_FunctionInfo, \
													   DGE_LineCodeInfo)
#define DoNothing
#ifdef DGE_Debug
			/// \brief In Debug Mode,When expression is true,log msg in Log system & do
			/// funexp.(funexp=function expression!)
#define DGE_AssertLog(expression, msg, funexp) \
	if (expression)                            \
	{                                          \
		LogError(msg);                         \
		funexp;                                \
	}
			/// \brief In Debug Mode,When expression is true, do funexp.(funexp=function expression!)
#define DGE_Assert(expression, funexp) \
	if (expression)                    \
	{                                  \
		funexp;                        \
	}
#else
			//? Do nothing because,DGE is in Release Mode!
#define DGE_AssertLog(expersion, msg, funexp)
#define DGE_Assert(expression, funexp)
#endif
			extern DGE_CoreExtern std::atomic_bool g_isExitingApp;
			extern DGE_CoreExtern std::atomic<fstream *> fConsole;

			extern DGE_CoreExtern std::atomic<Core::IO::LogSystem::LogMode> g_logType;

			/// \brief Log exception that can't send into log subsystem.
			extern DGE_CoreExtern bool InitUnexceptionErrorLogger();

			/// \brief Create a debug console and attach to it
			extern DGE_CoreExtern void AddDebugConsole();

			/// \brief Direct writing log into file.
			extern DGE_CoreExtern void WriteDebugConsoleToFile(const char *msg,
															   const char *filename,
															   const char *function, int lineNumber,
															   ...);

			/// \brief Direct writing log into file.
			extern DGE_CoreExtern void WriteDebugConsoleToFile(const DString &msg,
															   const char	*filename,
															   const char *function, int lineNumber,
															   ...);
#if DGE_Platform == DGE_Windows_Platform
			/// \brief Generate Memory Dump when an exception occurred.
			extern DGE_CoreExtern int GenerateDump(EXCEPTION_POINTERS *pExceptionPointers);
#endif

			/// \brief if more than one copy of this application is run,
			/// then return false;
			/// else return true;
			///
			extern DGE_CoreExtern void *hasRunOnce();

			/// \brief get size of file in byte
			extern DGE_CoreExtern size_t fgetlength(FILE *file, char EndChar = '\n');

			/// \brief Split given string with default white-spaces
			template<class Container>
			void split1(const std::string &str, Container &cont)
			{
				std::istringstream iss(str);
				std::copy(std::istream_iterator<std::string>(iss),
						  std::istream_iterator<std::string>(),
						  std::back_inserter(cont));
			}
			template<class Container>
			void split1(const DString &str, Container &cont)
			{
				std::istringstream iss(str);
				std::copy(std::istream_iterator<std::string>(iss),
						  std::istream_iterator<std::string>(),
						  std::back_inserter(cont));
			}

			/// \ref https://stackoverflow.com/a/28410911/4739722
			template<typename F, size_t... Is>
			auto gen_tuple_impl(F func, std::index_sequence<Is...>)
			{
				return std::make_tuple(func(Is)...);
			}

			/// \brief Convert a vector into tuple in compile time
			template<size_t N, typename F>
			auto gen_tuple(F func)
			{
				return gen_tuple_impl(func, std::make_index_sequence<N>{});
			}

			extern DGE_CoreExtern int lerp(int startValue,
										   int endValue,
										   int stepNumber,
										   int lastStepNumber);

			extern DGE_CoreExtern Color lerp(Color startValue,
											 Color endValue,
											 int   stepNumber,
											 int   lastStepNumber);

			extern DGE_CoreExtern float randomNum(float a, float b);

			extern DGE_CoreExtern Color randomColor(Color a, Color b);
			extern DGE_CoreExtern Color randomColor(Color end);
			extern DGE_CoreExtern Color randomColor();

			extern DGE_CoreExtern double convertRange(double value,
													  double minValue,
													  double maxValue,
													  double minConvert,
													  double maxConvert);
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
