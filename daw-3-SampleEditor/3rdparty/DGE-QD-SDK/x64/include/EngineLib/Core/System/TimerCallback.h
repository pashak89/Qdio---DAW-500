/* CCopyright (C) 2025, Abbas Aliakbari- All Rights Reserved
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
#include <Core/OS/Timer.h>
#include <Core/Sync/Mutex.h>
#include <Core/System/Functor.h>
#include <condition_variable>
#include <cstring>
#include <thread>
using namespace std;

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			void uSleep(int elapse, int max_fps = -1);

			enum class TimerStatus
			{
				Initialized,
				Started,
				Paused,
				Stoped,
			};
			template<typename UnitTime>
			class TimerCallback
			{
#ifndef _M_CEE
				thread							   m_Thread;
				Sync::StdMutex					   m_MutexLock;
				condition_variable_any			   m_Notifier;
				TimerStatus						   m_Status = TimerStatus::Stoped;
				UnitTime						   m_Interval;
				DGE::Core::OS::PreciseTimer m_Timer;

			public:
				TimerCallback() {}

				template<typename Treturn, typename... Args>
				void CallFunction(std::shared_ptr<Function<Treturn, Args...>> function, Args... args)
				{
					m_Timer.start();
					do
					{
						if (m_Status == TimerStatus::Paused)
						{
							unique_lock<Sync::StdMutex> lock(m_MutexLock);
							m_Notifier.wait(lock);
						}
						while (m_Status != TimerStatus::Stoped && m_Status != TimerStatus::Paused)
						{
							// unique_lock<StdMutex> lock(m_MutexLock);
							// m_Notifier.wait_for(lock, m_Interval);
							// cout <<endl<< m_Timer.GetTimeUpdate<UnitTime>() << endl;
							uSleep(std::chrono::duration_cast<microseconds>(m_Interval).count());
							(*function)(args...);
						}
					} while (m_Status == TimerStatus::Paused);
				}

				template<typename Treturn = void>
				void CallFunction(std::shared_ptr<Function<Treturn>> function)
				{
					m_Timer.start();
					do
					{
						if (m_Status == TimerStatus::Paused)
						{
							unique_lock<Sync::StdMutex> lock(m_MutexLock);
							m_Notifier.wait(lock);
						}
						while (m_Status != TimerStatus::Stoped && m_Status != TimerStatus::Paused)
						{
							// unique_lock<StdMutex> lock(m_MutexLock);
							// m_Notifier.wait_for(lock, m_Interval);
							// cout << endl << m_Timer.GetTimeUpdate<UnitTime>() << endl;
							uSleep(std::chrono::duration_cast<microseconds>(m_Interval).count());
							(*function)();
						}

					} while (m_Status == TimerStatus::Paused);
				}

				template<typename Duration, typename Treturn, typename... Args>
				TimerCallback(Duration const &d,
							  std::shared_ptr<Function<Treturn, Args...>>
								  function,
							  Args... args)
				{
					SetTimerCallback(d, function, args...);
				}

				template<typename Duration, typename Treturn = void>
				TimerCallback(Duration const &d, std::shared_ptr<Function<Treturn>> function)
				{
					SetTimerCallback(d, function);
				}

				template<typename Duration, typename Treturn, typename... Args>
				void SetTimerCallback(Duration const &d,
									  std::shared_ptr<Function<Treturn, Args...>>
										  function,
									  Args... args)
				{
					m_Interval = std::chrono::duration_cast<UnitTime>(d);
					m_Status   = TimerStatus::Initialized;
					m_Thread   = thread(
						  [this, d, function, args...]()
						  {
#ifdef __unix__
							sched_param sch_params;
							sch_params.sched_priority = 2;
							if (pthread_setschedparam(m_Thread.native_handle(),
													  SCHED_RR,
													  &sch_params))
							{
								std::cerr
									<< "Failed to set Thread scheduling : " << std::strerror(errno)
									<< std::endl;
							}
#endif
							// SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
							m_Status = TimerStatus::Started;
							this->CallFunction(function, args...);
							m_Status = TimerStatus::Stoped;
						});
				}

				template<typename Duration, typename Treturn = void>
				void SetTimerCallback(Duration const &d, std::shared_ptr<Function<Treturn>> function)
				{
					m_Interval = std::chrono::duration_cast<UnitTime>(d);
					m_Status   = TimerStatus::Initialized;
					m_Thread   = thread(
						  [this, d, function]()
						  {
#ifdef __unix__
							sched_param sch_params;
							sch_params.sched_priority = 2;
							if (pthread_setschedparam(m_Thread.native_handle(),
													  SCHED_RR,
													  &sch_params))
							{
								std::cerr
									<< "Failed to set Thread scheduling : " << std::strerror(errno)
									<< std::endl;
							}
#endif
							m_Status = TimerStatus::Started;
							this->CallFunction(function);
							m_Status = TimerStatus::Stoped;
						});
				}

				void Join() { m_Thread.join(); }

				bool Joinable() { return m_Thread.joinable(); }
				bool Joining()
				{
					auto temp = Joinable();
					if (temp)
						Join();
					return temp;
				}

				void Pause()
				{
					if (m_Status == TimerStatus::Started)
					{
						{
							m_Status = TimerStatus::Paused;
							// unique_lock<StdMutex> lock(m_MutexLock);
						}
						m_Notifier.notify_one();
					}
				}

				void Resume()
				{
					if (m_Status == TimerStatus::Paused)
					{
						{
							// unique_lock<StdMutex> lock(m_MutexLock);
							m_Status = TimerStatus::Started;
						}
						m_Notifier.notify_one();
					}
				}

				void Stop()
				{
					if (m_Status == TimerStatus::Started || m_Status == TimerStatus::Paused)
					{
						{
							// unique_lock<StdMutex> lock(m_MutexLock);
							m_Status = TimerStatus::Stoped;
						}
						m_Notifier.notify_all();
					}
				}

				TimerStatus GetStatus() const { return m_Status; }

				UnitTime GetInterval() const { return m_Interval; }

				template<typename Duration>
				void SetInterval(Duration val)
				{ /*unique_lock<StdMutex> lock(m_MutexLock);*/
					m_Interval = std::chrono::duration_cast<UnitTime>(val);
				}
#endif
			};

			using HighPrecisionTimerCallback = TimerCallback<std::chrono::nanoseconds>;

			template<typename UnitTime>
			class StandardTimerCallback
			{
#ifndef _M_CEE
				thread				   m_thread;
				Sync::StdMutex		   m_mutexLock;
				condition_variable_any m_notifier;
				TimerStatus			   m_status = TimerStatus::Stoped;
				UnitTime			   m_interval;
				OS::PreciseTimer	   m_timer;

			public:
				StandardTimerCallback() {}

				template<typename Treturn, typename... Args>
				void timeout(std::shared_ptr<Function<Treturn, Args...>> function, Args... args)
				{
					m_timer.start();
					do
					{
						if (m_status == TimerStatus::Paused)
						{
							unique_lock<Sync::StdMutex> lock(m_mutexLock);
							m_notifier.wait(lock);
						}
						while (m_status != TimerStatus::Stoped && m_status != TimerStatus::Paused)
						{
							unique_lock<Sync::StdMutex> lock(m_mutexLock);
							m_notifier.wait_for(lock, m_interval);
							// cout <<endl<< m_Timer.GetTimeUpdate<UnitTime>() << endl;
							(*function)(args...);
						}
					} while (m_status == TimerStatus::Paused);
				}

				template<typename Treturn = void>
				void timeout(std::shared_ptr<Function<Treturn>> function)
				{
					m_timer.start();
					do
					{
						if (m_status == TimerStatus::Paused)
						{
							unique_lock<Sync::StdMutex> lock(m_mutexLock);
							m_notifier.wait(lock);
						}
						while (m_status != TimerStatus::Stoped && m_status != TimerStatus::Paused)
						{
							unique_lock<Sync::StdMutex> lock(m_mutexLock);
							m_notifier.wait_for(lock, m_interval);
							// cout << endl << m_Timer.GetTimeUpdate<UnitTime>() << endl;
							(*function)();
						}

					} while (m_status == TimerStatus::Paused);
				}

				template<typename Duration, typename Treturn, typename... Args>
				StandardTimerCallback(Duration const &d,
									  std::shared_ptr<Function<Treturn, Args...>>
										  function,
									  Args... args)
				{
					setTimerCallback(d, function, args...);
				}

				template<typename Duration, typename Treturn = void>
				StandardTimerCallback(Duration const &d, std::shared_ptr<Function<Treturn>> function)
				{
					setTimerCallback(d, function);
				}

				template<typename Duration, typename Treturn, typename... Args>
				bool setTimerCallback(Duration const &d,
									  std::shared_ptr<Function<Treturn, Args...>>
										  function,
									  Args... args)
				{
					if (m_status == TimerStatus::Stoped)
					{
						m_interval = std::chrono::duration_cast<UnitTime>(d);
						m_status   = TimerStatus::Initialized;
						m_thread   = thread(
							  [this, d, function, args...]()
							  {
								  // SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
								  m_status = TimerStatus::Started;
								  this->timeout(function, args...);
								  m_status = TimerStatus::Stoped;
							  });
						return true;
					}
					return false;
				}

				template<typename Duration, typename Treturn = void>
				bool setTimerCallback(Duration const &d, std::shared_ptr<Function<Treturn>> function)
				{
					if (m_status == TimerStatus::Stoped)
					{
						m_interval = std::chrono::duration_cast<UnitTime>(d);
						m_status   = TimerStatus::Initialized;
						m_thread   = thread(
							  [this, d, function]()
							  {
								  // SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
								  m_status = TimerStatus::Started;
								  this->timeout(function);
								  m_status = TimerStatus::Stoped;
							  });
						return true;
					}
					else if (m_status == TimerStatus::Paused)
					{
						resume();
					}
					return false;
				}

				void join() { m_thread.join(); }

				bool joinable() { return m_thread.joinable(); }
				bool joining()
				{
					auto temp = joinable();
					if (temp)
						join();
					return temp;
				}

				void pause()
				{
					if (m_status == TimerStatus::Started)
					{
						{
							unique_lock<Sync::StdMutex> lock(m_mutexLock);
							m_status = TimerStatus::Paused;
						}
						m_notifier.notify_one();
					}
				}

				void resume()
				{
					if (m_status == TimerStatus::Paused)
					{
						{
							unique_lock<Sync::StdMutex> lock(m_mutexLock);
							m_status = TimerStatus::Started;
						}
						m_notifier.notify_one();
					}
				}

				void stop()
				{
					if (m_status == TimerStatus::Started || m_status == TimerStatus::Paused)
					{
						{
							unique_lock<Sync::StdMutex> lock(m_mutexLock);
							m_status = TimerStatus::Stoped;
						}
						m_notifier.notify_all();
					}
				}

				TimerStatus status() const { return m_status; }

				UnitTime interval() const { return m_interval; }

				template<typename Duration>
				void setInterval(Duration val)
				{
					unique_lock<Sync::StdMutex> lock(m_mutexLock);
					m_interval = std::chrono::duration_cast<UnitTime>(val);
				}
#endif
			};

			using HighPrecisionStandardTimerCallback
				= StandardTimerCallback<std::chrono::nanoseconds>;
			using RegularTimerCallback = StandardTimerCallback<std::chrono::milliseconds>;
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
