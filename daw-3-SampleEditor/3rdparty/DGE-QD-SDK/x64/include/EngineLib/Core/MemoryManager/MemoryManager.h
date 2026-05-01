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

#ifdef DGE_Track_Memory
#undef new
#undef delete
extern DGE_CoreExtern int MemCounter;
// try to allocate count bytes for an array
extern void* operator new[](size_t count) noexcept(false);

extern void operator delete[](void* memory) noexcept;
extern void operator delete[](void* memory, size_t count) noexcept;

// construct array with placement at _Where
extern void* operator new(size_t count) noexcept(false);

extern void* operator new(size_t count, const char* file, int line) noexcept(false);

extern void operator delete(void* memory) noexcept;

extern void operator delete(void* memory, size_t count) noexcept;

extern void operator delete(void* memory, const char* file, int line) noexcept;

// construct array with placement at _Where
extern void* operator new(size_t count, void* _Where) throw();

extern void* operator new(size_t count, void* _Where, const char* file, int line) throw();

#define DBG_NEW(param, ...) new (param, DGE_FileCodeInfo, DGE_LineCodeInfo)
#define new(param, ...) DBG_NEW(param, DGE_FileCodeInfo, DGE_LineCodeInfo)
#define DBG_Delete(param, ...) delete (param, DGE_FileCodeInfo, DGE_LineCodeInfo)
#define delete(param, ...) DBG_Delete(param, DGE_FunctionInfo, DGE_FileCodeInfo, DGE_LineCodeInfo)
#else
#if defined(DGE_UseCRTDebugMemory)&&!defined(DGE_Track_Memory)
#if DGE_Platform == DGE_Windows_Platform
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#define DBG_NEW new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define _New DBG_NEW
#define new DBG_NEW
#endif

#ifdef FORCEINLINE
#undef FORCEINLINE
#endif
#else
#define DBG_NEW new
#define _New DBG_NEW
#endif  // _DEBUG

#endif

#include <Core/Utility/Utility.h>

#if DGE_Platform == DGE_Windows_Platform
#include <crtdbg.h>
#endif

#include <stdlib.h>

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{
			namespace Memory
			{
#define SafeDelete(p)    \
	{                    \
		if (p)           \
		{                \
			delete p;    \
			p = nullptr; \
		}                \
	} // free memory of a simple pointer.
#define SafeDeleteArray(p) \
	{                      \
		if (p)             \
		{                  \
			delete[] p;    \
			p = nullptr;   \
		}                  \
	} // free memory of array pointer.
#define SafeRelease(p)      \
	{                       \
		if (p)              \
		{                   \
			(p)->Release(); \
			p = nullptr;    \
		}                   \
	} // free memory of COM Object.

#ifndef reinterpret_pointer_cast
				template<class T, class U>
				std::shared_ptr<T> reinterpret_pointer_cast(const std::shared_ptr<U>& r) noexcept
				{
					auto p = reinterpret_cast<typename std::shared_ptr<T>::element_type*>(r.get());
					return std::shared_ptr<T>{r, p};
				}
#else
				template<class T, class U>
				std::shared_ptr<T> reinterpret_pointer_cast(const std::shared_ptr<U>& r) noexcept
				{
					return std::reinterpret_pointer_cast<T>(r);
				}

#endif
			}
		}
	}
}

using namespace DGE::Core::BaseType::Memory;
