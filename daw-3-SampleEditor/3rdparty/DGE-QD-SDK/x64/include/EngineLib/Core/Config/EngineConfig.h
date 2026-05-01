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

#include "EngineSetting.h"

//!? Do not change this file!!
#pragma region EngineConfig

#pragma region EngineDefinition
//! -----------------------------------------------------------------
//! Compile Mode Engine
//! -----------------------------------------------------------------
#define DGE_Debug 0
#define DGE_Release 1

//! -----------------------------------------------------------------
//! Platform
//! -----------------------------------------------------------------
#define DGE_Unknown_Platform 0	  //? Unknown platform
#define DGE_Windows_Platform 1	  //! Windows platform
#define DGE_Linux_Platform 2	  //! Linux platform
#define DGE_Mac_Platform 3		  //! Apple platform
#define DGE_Android_Platform 4	  //! Android platform
#define DGE_IOS_Platform 5		  //! IOS platform
#define DGE_Emscripten_Platform 6 //! IOS platform

//! -----------------------------------------------------------------
//! Compiler
//! -----------------------------------------------------------------
#define DGE_Unknown_Compiler 0 //? Unknown Compiler
#define DGE_MSVC_Compiler 1	   //! Microsoft Visual C++ Compiler
#define DGE_GNUC_Compiler 2	   //! GNU C/C++ Compiler
#define DGE_Borland_Compiler 3 //! Borland Compiler
#define DGE_NDK_Compiler 4	   //! NDK build system

//! -----------------------------------------------------------------
//! Architecture Type
//! -----------------------------------------------------------------
#define DGE_Unknown_Architecture 0 //? Unknown Architecture
#define DGE_X86_Architecture 1	   //! 32 bit Architecture System
#define DGE_X64_Architecture 2	   //! 64 bit Architecture System

//! -----------------------------------------------------------------
//! Memory Tracker
//! -----------------------------------------------------------------
#define DGE_Memory_NedMalloc 0
#define DGE_Memory_CRT 1
#define DGE_DGEProfiler 2
#define DGE_TracyProfiler 3

//! -----------------------------------------------------------------
//! Set SIMD Feature For Engine
//! -----------------------------------------------------------------
#define DGE_Use_SSE1 0	// Enable/Disable SSE2
#define DGE_Use_SSE2 0	// Enable/Disable SSE2
#define DGE_Use_SSE3 0	// Enable/Disable SSE3
#define DGE_Use_SSE4 0	// Enable/Disable SSE4
#define DGE_Use_AVX1 0	// Enable/Disable AVX1
#define DGE_Use_AVX2 0	// Enable/Disable AVX2
#define DGE_Use_3DNOW 0 // Enable/Disable 3DNOW

//! -----------------------------------------------------------------
//! Graphic API Config
//! -----------------------------------------------------------------
#define DGE_DirectX9_API 1
#define DGE_DirectX10_API 0
#define DGE_DirectX11_API 0
#define DGE_OpenGL1_API 0
#define DGE_OpenGL2_API 0
#define DGE_OpenGL3_API 0
#define DGE_OpenGL4_API 0

//! -----------------------------------------------------------------
//! Sound API Config
//! -----------------------------------------------------------------
#define DGE_Use_FMOD 0
#define DGE_Use_BASS 0
#define DGE_Use_OpenAL 0
#define DGE_Use_XAudio2 0
#define DGE_Use_XACT 0
#define DGE_Use_DirectMusic 1

//! -----------------------------------------------------------------
//! Input API Config
//! -----------------------------------------------------------------
#define DGE_Use_DirectxInput 1
#define DGE_Use_XInput 0
#define DGE_Use_OwnInput 0

//! -----------------------------------------------------------------
//! Physic API Config
//! -----------------------------------------------------------------
#define DGE_Use_PhysXEngine 0
#define DGE_Use_Box2DEngine 0
#pragma endregion EngineDefinition

#pragma region CompilerModeConfig
//! -----------------------------------------------------------------
//! Find Compiler Mode
//! -----------------------------------------------------------------
#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
#define DGE_Compiler_Mode DGE_Debug
#ifndef DEBUG
#define DEBUG
#endif
#else
#define DGE_Compiler_Mode DGE_Release
#define NDEBUG 1
#endif
#pragma endregion CompilerModeConfig

#pragma region CompilerNameAndVersion
//! -----------------------------------------------------------------
//! Find Compiler Name & Version
//! -----------------------------------------------------------------
#if defined(_MSC_VER) //! Microsoft Visual C++ Compiler
#define DGE_Compiler DGE_MSVC_Compiler
#define DGE_Version_Compiler _MSC_VER

#elif defined(__GNUC__) //! GNU C/C++ Compiler
#define DGE_Compiler DGE_GNUC_Compiler
#define DGE_Version_Compiler (((__GNUC__) *100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__)

#elif defined(__BORLANDC__) //! Borland Compiler
#define DGE_Compiler DGE_Borland_Compiler
#define DGE_Version_Compiler __BCPLUSPLUS__

#elif defined(NDK) //! NDK build system
#define DGE_Compiler DGE_NDK_Platform
#define DGE_Version_Compiler 7

#else //? Unknown Compiler
#define DGE_Compiler DGE_Unknown_Platform
#define DGE_Version_Compiler 0
#pragma error "No known compiler. Compiling Abort!"
#endif
#pragma endregion CompilerNameAndVersion

#pragma region PlatformType
//! -----------------------------------------------------------------
//! Find platform Type
//! -----------------------------------------------------------------
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) //! Windows platform
#define DGE_Platform DGE_Windows_Platform
#elif defined(__APPLE__) || defined(__APPLE_CC__)
#ifdef TARGET_OS_IPHONE
#define DGE_Platform DGE_IOS_Platform
#else
#define DGE_Platform DGE_Mac_Platform
#endif
#elif defined(ANDROID)
#define DGE_Platform DGE_Android_Platform
#elif defined(__EMSCRIPTEN__) //! Emscripten platform
#define DGE_Platform DGE_Emscripten_Platform
#elif defined(__unix__) //! Linux platform
#define DGE_Platform DGE_Linux_Platform
#else //? Unknown platform
#define DGE_Platform DGE_Unknown_Platform
#error "dge cannot run at this platform!"
#endif
#pragma endregion PlatformType

#pragma region ArchitectureType
//! -----------------------------------------------------------------
//! Find Architecture Type
//! -----------------------------------------------------------------
#if defined(__x86_x64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) \
	|| defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#define DGE_Architecture_Type DGE_X64_Architecture
#else
#define DGE_Architecture_Type DGE_X86_Architecture
#endif
#pragma endregion ArchitectureType

#pragma region MemoryTracker
//! -----------------------------------------------------------------
//! Memory Tracker
//! -----------------------------------------------------------------
#if defined(DGE_UseNedMalloc)
#define DGE_Memory_Allocator DGE_Memory_NedMalloc
#endif

#if defined(DGE_UseTracyProfiler)
#define DGE_DefaultProfiler DGE_TracyProfiler
#else
#define DGE_DefaultProfiler DGE_Memory_DGEProfiler
#endif

#pragma endregion MemoryTracker

#pragma region UnicodeMode
//! -----------------------------------------------------------------
//! Support Unicode Mode
//! -----------------------------------------------------------------
#if (DGE_Use_Unicode == 1)
#define DGE_Support_Unicode 1
#define UseWideChar //! Active Unicode in Engine.
#define UNICODE
#define _UNICODE
#else
#define DGE_Support_Unicode 0
#endif
#pragma endregion UnicodeMode

#pragma region EngineVersion
//! -----------------------------------------------------------------
//! Version Engine
//! -----------------------------------------------------------------
#define D_AttachTwoString(x, y) x##y
#define D_MakeString(x) #x
#define D_AttachString(x, y) D_MakeString(x) D_MakeString(y)
#define D_Attach2Parameters(x, y) D_MakeString(x) D_MakeString(y)
#define D_Attach3Parameters(x, y, z) D_Attach2Parameters(x, y) D_MakeString(z)
#define D_Attach4Parameters(x, y, z, w) D_Attach3Parameters(x, y, z) D_MakeString(w)
#define D_Attach5Parameters(x, y, z, w, u) D_Attach4Parameters(x, y, z, w) D_MakeString(u)
#define D_Attach6Parameters(x, y, z, w, u, v) D_Attach5Parameters(x, y, z, w, u) D_MakeString(v)
#define D_Attach7Parameters(x, y, z, w, u, v, a) \
	D_Attach6Parameters(x, y, z, w, u, v) D_MakeString(a)
#define DGE_Version_Num                                                                     \
	D_Attach7Parameters(DGE_Version_Major, ., DGE_Version_Minor, ., DGE_Version_Release, ., \
						DGE_Version_Suffix)
#define DGE_Version_NumW _T(DGE_Version_Num)
#define DGE_Version_NameA D_AttachString("Digital Dreams Game Engine version ", DGE_Version_Num)
#define DGE_Version_NameW \
	D_AttachString(L"Digital Dreams Game Engine version ", _T(DGE_Version_Num))

#define DGE_Version ((DGE_Version_Major << 16) | (DGE_Version_Minor << 8) | DGE_Version_Release)

#pragma endregion EngineVersion

#pragma endregion EngineConfig
