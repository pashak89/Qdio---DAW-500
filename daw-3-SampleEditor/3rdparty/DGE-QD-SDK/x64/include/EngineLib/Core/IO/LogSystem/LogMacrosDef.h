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

namespace DGE
{
	namespace Core
	{
		namespace IO
		{
			namespace LogSystem
			{
				/// \enum DGE::Core::IO::LogSystem::LogMode
				/// \brief Use this enumeration to set that show log in which output.
				enum class LogMode
				{
					Console, // Show Log in Console Only
					File,	 // Show Log in File Only
					Both,	 // Show Log in Console & File
				};

				/// \enum DGE::Core::IO::LogSystem::LogLevel
				/// \brief Use this enumeration to set that show which type of log.
				enum class LogLevel
				{
					Verbose,	 /// Log everything with more detais for developing and debugging
					EveryThing,	 /// Log every things
					Information, /// Log Information, warnings and errors
					Warning,	 /// Log Warnings and errors
					Error,		 /// Log Errors
					Nothing		 /// Log nothing
				};

				/// \enum DGE::Core::IO::LogSystem::LogType
				/// \brief Use this enumeration to show type of log.
				enum class LogType
				{
					Info, // Information
					Warning,
					Error,
					FatalError,
					Debug,
					Success,
				};

				/// \brief Use this structor for storing log data
				///
				/// \details Internal structor for storing log data such as log message, type of log
				/// that is created automatically
				struct LogStruct
				{
					DString	logMsg;
					LogType logType;
				};
				/*inline void Warning(const Char* msg)
				{
				fprintf_s(g_loger, "Message: [%f] %s\n", clock() / 1000.0f, msg);
				}
				inline void Error(const Char* msg, const Char* file = DGE_FileCodeInfo, uint line =
				DGE_LineCodeInfo)
				{
				fprintf_s(g_loger, "Error: [%f] %s {File: %s, Line: %d}\n", clock() / 1000.0f, msg,
				file, line);
				}*/
#pragma region Macro Logger
#define LogInfo(msg)                                                                    \
	if (DGE::Core::IO::LogSystem::Logger::isValid())                             \
	DGE::Core::IO::LogSystem::Logger::getInstance()->Info(msg, DGE_FunctionInfo, \
																 DGE_FileCodeInfo,      \
																 DGE_LineCodeInfo)
#define LogWarning(msg)                                                                    \
	if (DGE::Core::IO::LogSystem::Logger::isValid())                                \
	DGE::Core::IO::LogSystem::Logger::getInstance()->Warning(msg, DGE_FunctionInfo, \
																	DGE_FileCodeInfo,      \
																	DGE_LineCodeInfo)
#define LogFatalError(msg)                                                                    \
	if (DGE::Core::IO::LogSystem::Logger::isValid())                                   \
	DGE::Core::IO::LogSystem::Logger::getInstance()->FatalError(msg, DGE_FunctionInfo, \
																	   DGE_FileCodeInfo,      \
																	   DGE_LineCodeInfo)
#define LogError(msg)                                                                    \
	if (DGE::Core::IO::LogSystem::Logger::isValid())                              \
	DGE::Core::IO::LogSystem::Logger::getInstance()->Error(msg, DGE_FunctionInfo, \
																  DGE_FileCodeInfo,      \
																  DGE_LineCodeInfo)
#define LogDebug(msg)                                                                    \
	if (DGE::Core::IO::LogSystem::Logger::isValid())                              \
	DGE::Core::IO::LogSystem::Logger::getInstance()->Debug(msg, DGE_FunctionInfo, \
																  DGE_FileCodeInfo,      \
																  DGE_LineCodeInfo)
#define LogSuccess(msg)                                                                    \
	if (DGE::Core::IO::LogSystem::Logger::isValid())                                \
	DGE::Core::IO::LogSystem::Logger::getInstance()->Success(msg, DGE_FunctionInfo, \
																	DGE_FileCodeInfo,      \
																	DGE_LineCodeInfo)

#define LogSD(msg, ...)                                                                        \
	if (DGE::Core::IO::LogSystem::Logger::isValid())                                    \
	DGE::Core::IO::LogSystem::Logger::getInstance()->SimpleDebug(msg, DGE_FunctionInfo, \
																		DGE_FileCodeInfo,      \
																		DGE_LineCodeInfo,      \
																		##__VA_ARGS__)
#pragma endregion
			} // namespace LogSystem
		}	  // namespace IO
	}		  // namespace Core
} // namespace DGE
