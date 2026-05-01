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
#include <Core/BaseTypes/DString.h>

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
#ifndef DGE_DEBUG_DESTRUCTOR_OBJECTS
#define D_Destructor(ClassName) virtual ~ClassName()
#else
#define D_Destructor(ClassName) void startDestroying()
#endif

#ifndef DGE_DEBUG_DESTRUCTOR_OBJECTS
#define D_Destructor_Implementation(ClassName) ClassName::~ClassName()
#else
#define D_Destructor_Implementation(ClassName) void ClassName::startDestroying()
#endif

#ifndef DGE_DEBUG_DESTRUCTOR_OBJECTS
#define D_OBJECT_DESTROYING_FUNC_DEBUG()
#else
#define D_OBJECT_DESTROYING_FUNC_DEBUG() \
	void startDestroying() {}
#endif

#ifndef DGE_DEBUG_DESTRUCTOR_OBJECTS
#define D_DESTRUCTOR_DEBUG(type)
#else
#define D_DESTRUCTOR_DEBUG(type)                                   \
public:                                                            \
	virtual ~type()                                                \
	{                                                              \
		startDestroying();                                         \
		if (DGE::Core::DCoreConfig::s_debugContext)                \
			LogConsole(DST(D_MakeString(type)) + " Is Destroyed"); \
	}                                                              \
                                                                   \
private:
#endif

#define D_SINGLETON_INTERNAL(type, ...)                              \
	friend class DGE::Core::Utility::Singleton<type, ##__VA_ARGS__>; \
	friend class DGE::Core::Utility::Deleter<type>;                  \
	friend class DGE::Core::Utility::Provider<type>;

#define D_SINGLETON(type, ...)                \
	D_SINGLETON_INTERNAL(type, ##__VA_ARGS__) \
	D_DESTRUCTOR_DEBUG(type)
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
