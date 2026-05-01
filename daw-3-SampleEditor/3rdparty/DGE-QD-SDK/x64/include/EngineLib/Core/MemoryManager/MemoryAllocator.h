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
		namespace BaseType
		{
			namespace Memory
			{
#ifdef DGE_Track_Memory
				template<class T>
				struct MemoryAllocator
				{
					typedef size_t	  size_type;
					typedef ptrdiff_t difference_type;
					typedef T*		  pointer;
					typedef const T*  const_pointer;
					typedef T&		  reference;
					typedef const T&  const_reference;
					typedef T		  value_type;

					template<class U>
					struct rebind
					{
						typedef MemoryAllocator<U> other;
					};
					MemoryAllocator() throw() {}
					MemoryAllocator(const MemoryAllocator&) throw() {}

					template<class U>
					MemoryAllocator(const MemoryAllocator<U>&) throw()
					{
					}

					~MemoryAllocator() throw() {}

					pointer		  address(reference x) const { return &x; }
					const_pointer address(const_reference x) const { return &x; }

					pointer allocate(size_type s, void const* = 0)
					{
						if (0 == s)
							return NULL;
						pointer temp = (pointer) malloc(s * sizeof(T));
						if (temp == NULL)
							throw std::bad_alloc();
						return temp;
					}

					void deallocate(pointer p, size_type) { free(p); }

					size_type max_size() const throw()
					{
#if DGE_Platform==DGE_Windows_Platform
#undef max
#endif
						return std::numeric_limits<size_t>::max() / sizeof(T);
					}

					void construct(pointer p, const T& val) { new (((void*) p)) T(val); }

					void destroy(pointer p) { p->~T(); }
				};

				template<class T>
				struct MallocAllocator : public std::allocator<T>
				{
					typedef T* pointer;

					using std::allocator<T>::allocator;
					template<class U>
					struct rebind
					{
						using other = MallocAllocator<U>;
					};

					pointer allocate(size_t n) { return (pointer) std::malloc(n * sizeof(T)); }

					void deallocate(void* p, size_t) { std::free(p); }
				};
				#endif
			} // namespace Memory
		}	  // namespace BaseType
	}		  // namespace Core
} // namespace DGE
