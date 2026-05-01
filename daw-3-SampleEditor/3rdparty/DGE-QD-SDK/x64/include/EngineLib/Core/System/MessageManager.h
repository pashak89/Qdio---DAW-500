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
#include "Event.h"
#include <Core/IO/LogSystem/Log.h>
#include <Core/OS/Timer.h>
#include <Core/Sync/BaseThread.h>
#include <Core/Sync/Mutex.h>
#include <any>
#include <condition_variable>
#include <vector>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			template<typename Treturn, typename... MessageType>
			class MessageManager : public Sync::BaseThread
			{
#ifndef _M_CEE
				Core::System::Event<Treturn, MessageType...> m_OnMessage;
				Core::System::Event<void>					 m_checkNewMessage;
				vector<std::tuple<MessageType...>>			 m_ThreadMessages;
				Sync::StdMutex								 m_MutexLock;
				condition_variable_any						 m_Notifier;
				std::string									 m_messageManagerName;
				std::atomic_uint64_t m_maxExcuteTimeThread = 10; // in millisecond
				thread::id			 m_currentThreadId;

				bool m_isPaused	 = false;
				bool m_isCreated = false;
				bool m_exited	 = false;

			public:
				MessageManager(std::string name = "")
					: m_messageManagerName(name)
				{
				}
				MessageManager(const Core::System::Event<Treturn, MessageType...>& eventData,
							   std::string										   name = "")
					: m_messageManagerName(name)
				{
					m_OnMessage = eventData;
				}
				virtual ~MessageManager()
				{
					// cout << "Destroy MessageManager" << endl;
				}

				void setName(std::string name) { m_messageManagerName = name; }

				void start()
				{
					if (!m_isCreated)
					{
						create();
						m_isCreated = true;
					}
					else
						m_isPaused = false;
				}

				void* threadProcFunc() override
				{
					m_currentThreadId = std::this_thread::get_id();
					Core::OS::PreciseTimer timer;
					timer.start();
					try
					{
						while (1)
						{
							std::tuple<MessageType...> msg;
							{
								unique_lock<Sync::StdMutex> lock(m_MutexLock);
								while ((m_ThreadMessages.size() == 0 || m_isPaused) && !m_exited)
								{
									if (!m_isPaused)
										m_checkNewMessage();
									m_Notifier.wait_for(lock,
														std::chrono::milliseconds(
															m_maxExcuteTimeThread));
								}
								if (m_ThreadMessages.size() == 0 && m_exited)
									return nullptr;
								else
								{
									do
									{
										if (m_ThreadMessages.size() == 0)
											break;
										msg = m_ThreadMessages[0];

										m_ThreadMessages.erase(m_ThreadMessages.begin(),
															   m_ThreadMessages.begin() + 1);
										callMessageEvent(msg,
														 std::index_sequence_for<MessageType...>());
#ifdef DEBUG
										auto queueSize = m_ThreadMessages.size();
										if (queueSize != 0)
											spdlog::debug("MessageManager with name {}:{}",
														  m_messageManagerName.c_str(),
														  queueSize);
#endif
									} while (std::chrono::milliseconds(m_maxExcuteTimeThread)
#ifdef WIN32
													 .count()
#endif
												 / 2
											 >= timer.getTimeAndUpdate<milliseconds>());
								}
							}

							//;
						}
					}
					catch (std::exception& except)
					{
						LogError(DST("exception occured:\n ") + except.what());
					}
					catch (...)
					{
						LogError(DST("exception occured:\n Unknow error"));
					}
					return nullptr;
				}

				void sendMessage(MessageType&&... message)
				{
					if (m_eThreadStatus == Sync::ThreadStatus::Running)
					{
						try
						{
							if (m_currentThreadId == this_thread::get_id())
							{
								m_ThreadMessages.push_back({message...});
								// printf("add one message\n");
								if (!m_isPaused)
									m_Notifier.notify_one();
								return;
							}
							{
								unique_lock<Sync::StdMutex> lock(m_MutexLock);
								{
									m_ThreadMessages.push_back({message...});
									// printf("add one message\n");
									if (!m_isPaused)
										m_Notifier.notify_one();
								}
							}
						}
						catch (...)
						{
							LogError(DST("exception occured:\n Unknow error"));
						}
					}
				}
				void sendMessage(const MessageType&... message)
				{
					if (m_eThreadStatus == Sync::ThreadStatus::Running)
					{
						try
						{
							if (m_currentThreadId == this_thread::get_id())
							{
								m_ThreadMessages.push_back({message...});
								// printf("add one message\n");
								if (!m_isPaused)
									m_Notifier.notify_one();
								return;
							}
							{
								unique_lock<Sync::StdMutex> lock(m_MutexLock);
								{
									m_ThreadMessages.push_back({message...});
									// printf("add one message\n");
									if (!m_isPaused)
										m_Notifier.notify_one();
								}
							}
						}
						catch (...)
						{
							LogError(DST("exception occured:\n Unknow error"));
						}
					}
				}
				Core::System::Event<Treturn, MessageType...>& OnMessage() { return m_OnMessage; }

				void operator+=(Core::System::Function<Treturn, MessageType...>* functionPointer)
				{
					m_OnMessage += functionPointer;
				}

				Core::System::Event<void>& checkNewMessage() { return m_checkNewMessage; }

				void stop()
				{
					if (m_eThreadStatus == Sync::ThreadStatus::Running)
					{
						if (m_currentThreadId != this_thread::get_id())
						{
							unique_lock<Sync::StdMutex> lock(m_MutexLock);
							{
								m_isPaused = false;
								m_exited   = true;
								m_Notifier.notify_one();
							}
						}
						else
						{
							m_isPaused = false;
							m_exited   = true;
							m_Notifier.notify_one();
						}
						if (m_hThreadHandle.joinable())
						{
							m_hThreadHandle.join();
						}
					}
				}

				void pause()
				{
					if (m_eThreadStatus == Sync::ThreadStatus::Running)
					{
						if (m_currentThreadId != this_thread::get_id())
						{
							unique_lock<Sync::StdMutex> lock(m_MutexLock);
							{
								m_isPaused = true;
								// m_Notifier.notify_one();
							}
						}
						else
							m_isPaused = true;
					}
				}
#else
				void* ThreadProcFunc() override;
#endif
				std::chrono::milliseconds maxExcuteTimeThread() const
				{
					return std::chrono::milliseconds(m_maxExcuteTimeThread);
				}
				void setMaxExcuteTimeThread(std::chrono::milliseconds val)
				{
					m_maxExcuteTimeThread = val.count();
				}
				const Core::System::Event<void>& checkNewMessage() const;

			private:
				void executeEvent(MessageType... values) { m_OnMessage(values...); }
				template<std::size_t... Is>
				void callMessageEvent(std::tuple<MessageType...>& tuple, std::index_sequence<Is...>)
				{
					executeEvent(std::get<Is>(tuple)...);
				}
			};
		} // namespace System
	}	  // namespace Core
} // namespace DGE
