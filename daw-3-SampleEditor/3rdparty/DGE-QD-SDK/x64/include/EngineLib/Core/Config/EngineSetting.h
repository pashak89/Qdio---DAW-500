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

//? you can change this file but be careful!
#pragma region Engine Setting

// #define DGE_Use_Unicode
// #define DGE_Use_SIMD
// #define DGE_Enable_WindowsMessageDebug
#define DGE_UseWindowConsoleDebug
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
#define DGE_UseCRTDebugMemory
// #define DGE_UseNedMalloc
#else
// #define DGE_Track_Memory // enable Track memory
#endif

// #define DGE_EnableProfiler	 // Enable profiler
//  #define DGE_UseTracyProfiler // Set Tracy profiler for default profiler
#define DGE_DEBUG_THREAD 1

/// \brief With defining below macro, engine will produce debug message for destroying the engine
/// objects when executing destructor
#define DGE_DEBUG_DESTRUCTOR_OBJECTS

#define DGE_SINGLETON_MAPPED 1

/// \brief Enable Qt wrapper layer and classes
// #define DGE_USE_QT

//#define DGE_UseXmlLibrary
#define DGE_DisableDebugWarrning 1
#define DGE_Default_Array_Block_Size 16
#define DGE_UseGameComponent				//enable API in engine
#define DGE_Use_Directx 0
#define DGE_Use_OpenGL 1

//! -----------------------------------------------------------------
//! Support Unicode Mode
//! -----------------------------------------------------------------
#define DGE_Use_Unicode 0 //! Do not support engine Unicode mode in when equal to 0

#pragma region Enable/Disable API Engine
//! -----------------------------------------------------------------
//! Macro Definition for Enable/Disable API Engine
//! -----------------------------------------------------------------
#if defined(DGE_UseGameComponent)
#define DGE_UseGraphicAPI
#define DGE_UseInputAPI
#define DGE_UseSoundAPI
#define DGE_UseEngine
#define DGE_UsePhysicAPI
#endif  
#pragma endregion

#define FREEIMAGE_LIB
#define Dynamic

#pragma endregion
