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
#include <Core/MemoryManager/MemoryAllocator.h>
#include <mutex>
#include <utility>

namespace DGE
{
	namespace Core
	{
		namespace Sync
		{
#ifdef DGE_Track_Memory
			using ThreadLockIDSet = set<uint32, std::less<uint32>, Memory::MemoryAllocator<uint32>>;
			#else
			using ThreadLockIDSet = set<uint32, std::less<uint32>>;
			#endif
			template<class mutexType>
			class Mutex
			{
#ifndef _M_CEE
			public:
				Mutex()
				{
#ifdef DGE_DEBUG_THREAD
					m_id = m_countLock;
					m_countLock++;
					m_locked		= false;
					m_ownerThreadId = std::this_thread::get_id();
#endif
				}
				bool try_lock() {
					return m_mutexRaw.try_lock();
				}

				void lock()
				{
					m_mutexRaw.lock();
#ifdef DGE_DEBUG_THREAD
					/*if (m_mutexRaw.try_lock() != !m_locked)
					{
						printf("Incorrect Lock flag is detected:%d\n", (bool) m_locked);
#ifdef WIN32
						DebugBreak();
#endif
					}*/

					if (m_locked)
					{
						printf("DeadLock\n");
#ifdef WIN32
						DebugBreak();
#endif
					}
					m_numLockingCounter++;
					m_locked = true;
					if (m_locked)
					{
						auto id = std::this_thread::get_id();
						if (m_threadsLock.empty() || m_threadsLock.find(id) == m_threadsLock.end())
							m_threadsLock.emplace(id, ThreadLockIDSet{});
						m_threadsLock[id].insert(m_id);
						// }
					}
#endif
				}
				void unlock()
				{
#ifdef DGE_DEBUG_THREAD
					if (!m_locked)
					{
						printf("Incorrect Lock Sequentioal!\n");
#ifdef WIN32
						DebugBreak();
#endif
					}

					auto& list = m_threadsLock[std::this_thread::get_id()];
					if (list.find(m_id) != list.end())
						list.erase(m_id);
					else
					{
						printf("OW\n");
					}
					m_locked = false;
#endif
					m_mutexRaw.unlock();
				}

			protected:
#ifdef DGE_DEBUG_THREAD
				static inline uint32 m_countLock = 0;
#ifdef DGE_Track_Memory
				map<thread::id, ThreadLockIDSet, std::less<thread::id>,
					Memory::MemoryAllocator<std::pair<const thread::id, ThreadLockIDSet>>>
					#else
				map < thread::id, ThreadLockIDSet,
					std::less<thread::id>>
				#endif
									  m_threadsLock;
				atomic_bool			  m_locked;
				uint32				  m_id;
				thread::id			  m_ownerThreadId;
				std::hash<thread::id> m_hasher;
				atomic_int64_t		  m_numLockingCounter = 0;
				// static set<uint> m_mutexLocked;
#endif
				mutexType m_mutexRaw;
#endif
			};

#ifndef _M_CEE
#ifdef DGE_DEBUG_THREAD
			using StdMutex = Mutex<std::mutex>;
#else
			using StdMutex = std::mutex;
#endif
#endif

		} // namespace Sync
	}	  // namespace Core
} // namespace DGE
