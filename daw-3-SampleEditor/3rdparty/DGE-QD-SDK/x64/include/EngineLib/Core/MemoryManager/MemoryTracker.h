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
#include <Core/Sync/Mutex.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <functional>
#include <memory>
#include <string>

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{
			namespace Memory
			{
#ifdef DGE_Track_Memory
				struct DescriptionMemory
				{
					void*  dataMemory;
					char   sourceFile[256];
					uint8  lineNumber;
					uint64 sizeMemory;
					DescriptionMemory();
					DescriptionMemory(void* data, const char* file, uint8 line, size_t size);
					DescriptionMemory(const DescriptionMemory& copy);
					DescriptionMemory& operator=(const DescriptionMemory& copy);
					void init(void* data, const char* file, uint8 line, size_t size);
				};
				using MallocString
                    = std::basic_string<char, char_traits<char>, Memory::MemoryAllocator<char>>;
				struct CompareString
				{
					bool operator()(MallocString const a, MallocString const b) const
					{
						return a < b;
					}
				};

				using MemoryTable = unordered_map<
					void*, DescriptionMemory, hash<const void*>, equal_to<void*>,
					Memory::MemoryAllocator<std::pair<void* const, DescriptionMemory>>>;
				using MemoryTableFile
					= map<MallocString, int, CompareString,
						  Memory::MemoryAllocator<std::pair<const MallocString, int>>>;

				class MemoryTracker
					: public System::DCoreObject
					, public Utility::Singleton<MemoryTracker, std::shared_ptr<MemoryTracker>,
												Core::Utility::Deleter<MemoryTracker>::deleter,
												Core::Utility::Provider<MemoryTracker>,
												MallocAllocator<MemoryTracker>>
				{
					D_SINGLETON(MemoryTracker, std::shared_ptr<MemoryTracker>,
								Core::Utility::Deleter<MemoryTracker>::deleter,
								Core::Utility::Provider<MemoryTracker>,
								MallocAllocator<MemoryTracker>)
				protected:
				public:
					MemoryTracker();
					D_Destructor(MemoryTracker);
					void* allocate(size_t count, const char* file, int line);
					void  deallocate(void*& memory, const char* file, int line);

					bool registerMemory(size_t count, void* memory, const char* file, int line);
					bool unRegsterMemory(void*& memory, const char* file, int line);

					bool isInternalTrackerEnabled() const;
					void setIsInternalTrackerEnabled(bool newIsInternalTrackerEnabled);

				protected:
					MemoryTable										m_trackList;
					MemoryTable										m_removedMemList;
					MemoryTableFile									m_detectList;
					std::atomic_bool								m_isLocked;
					Sync::StdMutex									m_lock;
					std::atomic_bool								m_isInternalTrackerEnabled;
				};
				using MemoryTrackerRef = std::shared_ptr<MemoryTracker>;
				#endif
			} // namespace Memory
		}	  // namespace BaseType
	}		  // namespace Core
} // namespace DGE
