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
#include <Core/BaseTypes/DPath.h>
#include <Core/IO/FileSystem/FileManager.h>
#include <Core/IO/LogSystem/LogMacrosDef.h>
#include <Core/Sync/BaseThread.h>
#include <Core/Sync/Mutex.h>
#include <Core/System/DApplicationInfo.h>
#include <Core/System/DObject.h>
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <Core/Utility/Singleton/Singleton.h>

// TODO: Improve type of lock for thread in log system.(change with atomic operations if possible)!

namespace DGE
{
	class Engine;
	namespace Core
	{
		namespace IO
		{
			namespace LogSystem
			{

				/*extern DGE_Extern void LogUnexceptionError();

				extern DGE_Extern void AddDebugConsole();

				extern DGE_Extern int GenerateDump(EXCEPTION_POINTERS* pExceptionPointers);

				extern DGE_Extern fstream ferr;*/

				/// \brief Logger subsystem that use for logging in engine
				class DGE_CoreAPI Logger
					: public Core::System::DSubsystem
					, public Sync::BaseThread
					, public Utility::Singleton<Logger>
				{
					D_OBJECT_SINGLETON(Logger)

					friend class DGE::Engine;

					/// \brief
					Logger();
					Logger(DGE::Core::System::ContextRef context);

					// IInitialize interface
				public:
					D_Destructor(Logger);

					/// \brief Release Logger subsystem
					virtual bool release() override;
					virtual bool release(bool isExecutedDestructor);
					/// \brief Use default implementation of
					/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
					/// function from IInitialize interface.
					using Utility::Interfaces::IInitialize::initialize;

					/// \brief Initialize Logger with given parameters
					///
					/// \details Call this function with these parameters to initialize Logger that
					/// this process will check existing of Logs folder and rename previous Log file
					/// into other name (The new name has date and time of renaming) and open log
					/// files and start logger thread and show starting Logger subsystem with
					/// logging message "Log: Log file open in follow path:".
					///
					/// \param parameters Input parameters for initialize this class.
					/// must set by below example:
					///
					/// \return return true if initialization is successfully otherwise return false
					///
					///	\code
					/// #include <iostream>
					/// #include <Engine.h>
					///
					/// using namespace DGE;
					/// using namespace std;
					///
					/// int main(int argc, char* argv[])
					/// {
					///		// contain other codes
					///		auto logger = Logger::getInstance(m_context);
					///		if (logger->initialize({{Logger::s_contextKey, Context::getInstance()},
					///								{
					///									DGE::IO::LogSystem::Logger::s_logModeKey,
					///			  DGE::IO::LogSystem::LogMode::Both
					///								}}))
					///			std::cout<<"Logger is initialized."<<std::endl;
					///		else
					///			std::cout<<"Logger initialization is failed."<<std::endl;
					///
					///		return 0;
					/// }
					/// \endcode
					virtual bool initialize(
						Core::Utility::Interfaces::InitializeParams& parameters) override;

					/// \brief Generate a text in output log with given message with Information label.
					///
					/// \details Generate a customize text to output of log with following format:
					/// Information: [clock] {Function Name: functionName} :
					///		MainMessage
					///
					/// \param msg It's main message of log
					/// \param functionName It's name function that call logger function
					/// \param file It's name of file that call logger function
					/// \param line It's line of file that call logger function
					void Info(const DString& msg, const char* functionName,
							  const char* file = DGE_FileCodeInfo, uint64 line = DGE_LineCodeInfo,
							  ...);

					/// \brief Generate a text in output log with given message with Warning label.
					///
					/// \details Generate a customize text to output of log with following format:
					/// Warning: [clock] {Function Name: functionName} :
					///		MainMessage
					///
					/// \param msg It's main message of log
					/// \param functionName It's name function that call logger function
					/// \param file It's name of file that call logger function
					/// \param line It's line of file that call logger function
					void Warning(const DString& msg, const char* functionName,
								 const char* file = DGE_FileCodeInfo,
								 uint64		 line = DGE_LineCodeInfo, ...);

					/// \brief Generate a text in output log with given message with Error label.
					///
					/// \details Generate a customize text to output of log with following format:
					/// Error: [clock] {File: file, Line: line, Function Name: functionName} :
					///		MainMessage
					///
					/// \param msg It's main message of log
					/// \param functionName It's name function that call logger function
					/// \param file It's name of file that call logger function
					/// \param line It's line of file that call logger function
					void Error(const DString& msg, const char* functionName,
							   const char* file = DGE_FileCodeInfo, uint64 line = DGE_LineCodeInfo,
							   ...);

					/// \brief Generate a text in output log with given message with FatalError label.
					///
					/// \details Generate a customize text to output of log with following format:
					/// FatalError: [clock] {File: file, Line: line, Function Name: functionName} :
					///		MainMessage
					///
					/// \param msg It's main message of log
					/// \param functionName It's name function that call logger function
					/// \param file It's name of file that call logger function
					/// \param line It's line of file that call logger function
					void FatalError(const DString& msg, const char* functionName,
									const char* file = DGE_FileCodeInfo,
									uint64		line = DGE_LineCodeInfo, ...);

					/// \brief Generate a text in output log with given message with Debug label.
					///
					/// \details Generate a customize text to output of log with following format:
					/// Debug: [clock] {Function Name: functionName} :\n\tMainMessage\n
					/// \param msg It's main message of log
					/// \param functionName It's name function that call logger function
					/// \param file It's name of file that call logger function
					/// \param line It's line of file that call logger function
					void Debug(const DString& msg, const char* functionName,
							   const char* file = DGE_FileCodeInfo, uint64 line = DGE_LineCodeInfo,
							   ...);

					/// \brief Generate a text in output log with given message with Success label.
					///
					/// \details Generate a customize text to output of log with following format:
					/// Success: [clock] {Function Name: functionName} :\n\tMainMessage\n
					/// \param msg It's main message of log
					/// \param functionName It's name function that call logger function
					/// \param file It's name of file that call logger function
					/// \param line It's line of file that call logger function
					void Success(const DString& msg, const char* functionName,
								 const char* file = DGE_FileCodeInfo,
								 uint64		 line = DGE_LineCodeInfo, ...);

					/// \brief Generate a text in output log with given message with SimpleDebug label.
					///
					/// \details Generate a customize text to output of log with following format:
					/// SimpleDebug: [clock] :\n\tMainMessage\n
					/// \param msg It's main message of log
					/// \param functionName It's name function that call logger function
					/// \param file It's name of file that call logger function
					/// \param line It's line of file that call logger function
					void SimpleDebug(const DString& msg, const char* functionName, const char* file,
									 uint64 line, ...);

					/// \brief get maximum bytes that file flush to disk when total bytes of log
					/// that written to file reach to it.
					uint16 maxTotalLoggedData() const;
					void   setMaxTotalLoggedData(uint16 newMaxTotalLoggedData);

					static DString logToString(LogLevel logLevel, LogType type, const char* msg,
											   const char* functionName, const char* file,
											   uint64 line, va_list args);

					LogLevel logLevel() const;
					void	 setLogLevel(LogLevel newLogLevel);

				protected:
					/// \brief Check queue log for log if exist.
					void checkLog();

					/// \brief Show Log in Console System.
					void logToConsole(std::shared_ptr<LogStruct>& ls);

					/// \brief Show Log in File log System.
					void logToFile(std::shared_ptr<LogStruct>& ls);

					/// \brief It's thread function that use for logging storing logs in file or in
					/// output without blocking
					virtual void* threadProcFunc() override;

					/// \brief a function that is useful if Log System Don't run Correctly
					void forceLogInternal(const char* msg, const char* file = DGE_FileCodeInfo,
										  uint64 line = DGE_LineCodeInfo, ...);
					bool sendLog(LogType type, const char* msg, const char* functionName,
								 const char* file = DGE_FileCodeInfo,
								 uint64 line = DGE_LineCodeInfo, va_list args = {});
					/// \brief override method of DSubsystem for registering everything.
					virtual bool registerAll(
						Core::Utility::Interfaces::InitializeParams& parameters) override;

				public:
					static const DString s_contextKey;
					static const DString s_logModeKey;
					static const DString s_logPathKey;
					static const DString s_rootSrcPath;

				protected:
					DString							  m_fullPathLog			= "";
					DString							  m_fullPathLogFileName = "";
					queue<std::shared_ptr<LogStruct>> m_qLogQueue;

#ifndef _M_CEE
					Sync::StdMutex		   m_mutex;
					condition_variable_any mCondVar;
					atomic<int>			   m_lock;
#endif
					static const DPath	 s_pathLogs;
					static const DString s_defaultLogFileName;

					FILE* m_loger;

					LogMode	 m_logMode;
					LogLevel m_logLevel;

					Core::System::DApplicationInfoRef	 m_app;
					Core::IO::FileSystem::FileManagerRef m_fileManager;

					std::atomic_bool m_bRun; // Run Log System.
					std::atomic_bool m_bExited;
					std::atomic_bool m_isInSleep;
					uint16			 m_totalLoggedData = 0;
					static uint16	 s_maxTotalLoggedData;
					static uint8	 s_maxLoggedMessageInOneCall;
				};

				D_DefRefType(Logger);
			} // namespace LogSystem
		}	  // namespace IO
	}		  // namespace Core
} // namespace DGE
