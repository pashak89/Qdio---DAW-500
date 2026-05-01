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
#include <optional>
#include <thread>

namespace DGE
{
	namespace Core
	{
		namespace Sync
		{

			/// \brief Show current status of thread
			enum class ThreadStatus
			{
				None,	 /// \brief Don't anything occur
				Created, /// \brief Thread created and ready for running
				Paused,	 /// \brief Thread is paused
				Running, /// \brief Thread is running
				Stopped, /// \brief Thread is stopped
			};

			/// \brief Base class for using thread in other classes
			class DGE_CoreAPI BaseThread
			{
				friend void* innerThreadProcFunc(std::shared_ptr<BaseThread> p);
			public:
				/// \brief Abstract Definition of Function Thread.
				virtual void* threadProcFunc();

				BaseThread();
				virtual ~BaseThread();

				/// \brief Call this function to create thread and start its functionality and if
				/// waitToStartdThread is true then wait for start thread
				///
				/// \param waitToStartedThread if it's true then this function is block until all
				/// previous operation is finished.
				bool create(bool waitToStartedThread = true);

				/// \brief Get Status Of Current Thread
				ThreadStatus status();

				/// \brief Set Status Of Current Thread
				void setStatus(ThreadStatus ts);

				bool isSetPriorityThread() const;
				void setIsSetPriorityThread(bool newIsSetPriorityThread);

				/// \brief Separates the thread of execution from the thread object, allowing
				/// execution to continue independently. Any allocated resources will be freed once
				/// the thread exits.
				bool detach();

				/// \brief waits for the thread to finish its execution
				bool join();

			protected:
				/// \brief Status of Current Thread
				mutable ThreadStatus m_eThreadStatus;

				/// \brief It's show that must set priority of thread or not.
				bool m_isSetPriorityThread;

#ifdef WIN32
				// uintptr_t m_hThreadHandle = 0;
				mutable std::thread m_hThreadHandle;
				uint32		m_dThreadID = -1;
#else
				/// \brief handle of thread
				mutable std::thread m_hThreadHandle;
#endif
			};
		} // namespace OS

	} // namespace Core
} // namespace DGE
