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
#include "../EngineConfig.h"
#include "../EngineSetting.h"

//!? Do not change this file!!
#pragma region API Configuration

#pragma region Set SIMD Feature For Engine

//! -----------------------------------------------------------------
//! Set SIMD Feature For Engine
//! -----------------------------------------------------------------
#if(DGE_Use_SSE1+DGE_Use_SSE2+DGE_Use_SSE3+DGE_Use_SSE4+DGE_Use_AVX1+DGE_Use_AVX2+DGE_Use_3DNOW)==1
#define DGE_Use_SIMD 1
#elif(DGE_Use_SSE1+DGE_Use_SSE2+DGE_Use_SSE3+DGE_Use_SSE4+DGE_Use_AVX1+DGE_Use_AVX2+DGE_Use_3DNOW)>1
#error "more than one SIMD config enable!"
#else
#define DGE_Use_SIMD 0
#endif  

#pragma endregion

#pragma region Set Graphic API For Engine

//! -----------------------------------------------------------------
//! Set Graphic API For Engine
//! -----------------------------------------------------------------
#ifdef DGE_UseGraphicAPI

#if DGE_Use_Directx==1
#define DGE_Graphic_API DGE_Use_Directx
#elif DGE_Use_OpenGL==1
#define DGE_Graphic_API DGE_Use_OpenGL
#else
#error "You must select one Graphic API or disable this API."
#endif

#endif  

#pragma endregion

#pragma region Set Sound API For Engine

//! -----------------------------------------------------------------
//! Set Sound API For Engine
//! -----------------------------------------------------------------
#if DGE_Use_XAudio2==1
#define DGE_Sound_API DGE_Use_XAudio2
#elif DGE_Use_XACT==1
#define DGE_Sound_API DGE_Use_XACT
#elif DGE_Use_OpenAL==1
#define DGE_Sound_API DGE_Use_OpenAL
#elif DGE_Use_BASS==1
#define DGE_Sound_API DGE_Use_BASS
#elif DGE_Use_FMOD==1
#define DGE_Sound_API DGE_Use_FMOD
#elif DGE_Use_DirectMusic==1
#define DGE_Sound_API DGE_Use_DirectMusic
#else
#error "You must select one Sound API."
#endif  
#pragma endregion

#pragma region Set Input API For Engine

//! -----------------------------------------------------------------
//! Set Input API For Engine
//! -----------------------------------------------------------------
#if DGE_Use_DirectxInput==1
#define DGE_Input_API DGE_Use_DirectxInput
#elif DGE_Use_XInput==1
#define DGE_Input_API DGE_Use_XInput
#elif DGE_Use_OwnInput==1
#define DGE_Input_API DGE_Use_OwnInput
#else
#error "You must select one Input API."
#endif  
#pragma endregion

#pragma region Set Physic API For Engine

//! -----------------------------------------------------------------
//! Set Input API For Engine
//! -----------------------------------------------------------------
#if defined(DGE_UsePhysicAPI)
#if DGE_Use_PhysXEngine==1
#define DGE_Physic_API DGE_Use_PhysXEngine
#else
#define DGE_Physic_API -128
#endif

#if DGE_Use_Box2DEngine==1
#define DGE_2DPhysic_API DGE_Use_Box2DEngine
#endif

#if !defined(DGE_Physic_API)&&!defined(DGE_2DPhysic_API)
#error "You must select one Physic API."
#endif  
#endif
#pragma endregion

#include"GraphicAPIConfigurator.h"
#include"SoundAPIConfigurator.h"
#include"InputAPIConfigurator.h"
#include"PhysicAPIConfigurator.h"

#pragma endregion
