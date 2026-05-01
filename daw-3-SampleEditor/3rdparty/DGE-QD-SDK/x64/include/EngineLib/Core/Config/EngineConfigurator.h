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
//!? Do not change this file!!

#pragma region Engine Configurator
//! -----------------------------------------------------------------
//! Error Macro
//! -----------------------------------------------------------------
#define DGE_FileCodeInfo __FILE__
#define DGE_LineCodeInfo __LINE__
#define DGE_FunctionInfo __FUNCTION__

#include "EngineConfig.h"

#if DGE_Platform == DGE_Windows_Platform || DGE_Compiler == DGE_MSVC_Compiler
#include <Core/Config/OSConfig/VSConfigurator.h>
#include <Core/Config/OSConfig/WindowsConfigurator.h>
#elif DGE_Compiler == DGE_GNUC_Compiler
#include <Core/Config/OSConfig/GNUCConfigurator.h>
#else
#error "dge dosn't support this compiler"
#endif

#include <Core/Config/APIConfig/APIConfigurator.h>

//! -----------------------------------------------------------------
//! C++ Header File.
//! -----------------------------------------------------------------
#include <algorithm>
#include <array>
#include <chrono>
#include <exception> //for classes exception and bad_exception
#include <fstream>
#include <ios>		 //for I/O exceptions
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>	//for most logic and runtime error classes
#include <string>
#include <system_error> //for system errors (since C++11)
#include <tuple>
#include <typeinfo>		//for bad_cast and bad_typeid
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#ifndef _M_CEE
#include <atomic>
#include <condition_variable>
#include <future> //for errors with a sync() and futures (since C++11)
#include <mutex>
#endif

//! -----------------------------------------------------------------
//! C Header File.
//! -----------------------------------------------------------------
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctgmath>
#include <ctime>
#include <cwchar>
#include <cwctype>
#if DGE_Platform == DGE_Windows_Platform
#include <process.h>
#endif

#include <Core/CoreModuleDef.h>

using namespace std;
using namespace std::chrono;
#pragma endregion
