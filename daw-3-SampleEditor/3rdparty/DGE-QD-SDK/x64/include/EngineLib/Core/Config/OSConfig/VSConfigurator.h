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
#pragma region Visual Studio Setting(VSVersion >= 2017)

#pragma region Disable Warning
//! -----------------------------------------------------------------
//! Disable Warning
//! -----------------------------------------------------------------
#if(DGE_DisableDebugWarrning)
// Unwanted VC++ level 4 warnings to disable.
#pragma warning(disable : 4049)
#pragma warning(disable : 4311)
#pragma warning(disable : 4312)
#pragma warning(disable : 4244) // conversion to float, possible loss of data
#pragma warning(disable : 4699) // creating precompiled header
#pragma warning(disable : 4200) // Zero-length array item at end of structure, a VC-specific extension
#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable : 4710) // inline function not expanded
#pragma warning(disable : 4714) // __forceinline function not expanded
#pragma warning(disable : 4702) // unreachable code in inline expanded function
#pragma warning(disable : 4711) // function selected for autmatic inlining
#pragma warning(disable : 4127) // Conditional expression is constant
#pragma warning(disable : 4512) // assignment operator could not be generated
#pragma warning(disable : 4245) // conversion from 'enum ' to 'unsigned long', signed/unsigned mismatch
#pragma warning(disable : 4389) // signed/unsigned mismatch
#pragma warning(disable : 4251) // needs to have dll-interface to be used by clients of class
#pragma warning(disable : 4275) // non dll-interface class used as base for dll-interface class
#pragma warning(disable : 4511) // copy constructor could not be generated
#pragma warning(disable : 4284) // return type is not a UDT or reference to a UDT
#pragma warning(disable : 4355) // this used in base initializer list
#pragma warning(disable : 4291) // typedef-name '' used as synonym for class-name ''
#pragma warning(disable : 4324) // structure was padded due to __declspec(align())
#pragma warning(disable : 4996) // 'function': was declared deprecated
#pragma warning(disable : 4995) // 'function': was declared deprecated
#pragma warning(disable : 4800) // 'function': was declared deprecated
#if(DGE_Compiler_Mode==DGE_Debug)
#pragma warning(disable : 4189) // local variable is initialized but not referenced
#pragma warning(disable : 4505) // unreferenced local function has been removed
#pragma warning(disable : 4099) // unfounded '.pdb' file!
#endif
#endif  
#pragma endregion

#pragma region WindowsSetting
#if DGE_Platform==DGE_Windows_Platform
#pragma comment(linker,"/manifestdependency:\"" \
	"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' " \
	"version='6.0.0.0' " \
	"processorArchitecture='*' "  \
	"publicKeyToken='6595b64144ccf1df' " \
	"language='*'\"")
#endif  
#pragma endregion

#pragma region Set Special Macro for Debug/Release Mode in Compile Time
//! -----------------------------------------------------------------
//! Set Special Macro for Debug/Release Mode in Compile Time.
//! -----------------------------------------------------------------
#if DGE_Compiler_Mode==DGE_Debug
#elif DGE_Compiler_Mode==DGE_Release
#define _CRT_SECURE_NO_WARNINGS 1
#endif  
#pragma endregion

#pragma region C++11 Support is checking
//! -----------------------------------------------------------------
//! C++11 (C++0x) Support is checking
//! -----------------------------------------------------------------
#if _MSC_VER>=1600
#define DGE_CPP11_Compiler 1
#else
#define DGE_CPP11_Compiler 0
#endif  
#pragma endregion

#pragma region DLL Export / Import support feature.
//! -----------------------------------------------------------------
//! DLL export/import support feature.
//! -----------------------------------------------------------------
#define DGE_Import __declspec(dllimport)
#define DGE_Export __declspec(dllexport)

#pragma endregion

#pragma region LIB Exp / Imp support feature.
//! -----------------------------------------------------------------
//! LIB export/import support feature.
//! -----------------------------------------------------------------
#define DGE_LIB_API DGE_Extern

#pragma endregion

#pragma region Check Compiler for wrong setting!
//! -----------------------------------------------------------------
//! Check Compiler for wrong setting!
//! -----------------------------------------------------------------
#ifndef _CPPUNWIND
#error "Bad VC++ option: C++ exception handling must be enabled."
#endif

#ifdef _Char_UNSIGNED //! Make sure Characters are unsigned.
#error "Bad VC++ option: Characters must be signed."
#endif  
#pragma endregion

#pragma region Set Align for Use SIMD
//! -----------------------------------------------------------------
//! Set Align for Use SIMD
//! -----------------------------------------------------------------
#ifdef DGE_Use_SIMD
#define DGE_Align __declspec(align(16))
#else
#define DGE_Align
#endif  
#pragma endregion

#pragma region Some macro
//! -----------------------------------------------------------------
//! Make Macro definition
//! -----------------------------------------------------------------
#define DGE_Force_Inline __forceinline
#define DGE_No_Inline __declspec(noinline)
#define DGE_Naked_Function void __declspec(naked)

//! -----------------------------------------------------------------
//! Define Macro for use Set/Get Properties
//! -----------------------------------------------------------------
#define GetSetMacro(G,T) __declspec(property(get = G, put = T))
#pragma endregion

#pragma endregion
