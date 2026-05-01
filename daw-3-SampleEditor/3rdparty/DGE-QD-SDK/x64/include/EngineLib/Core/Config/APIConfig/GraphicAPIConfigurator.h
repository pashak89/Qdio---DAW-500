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

#include "../EngineConfigurator.h"

#pragma region Graphic API Configorator

	#ifdef DGE_UseGraphicAPI

		//! -----------------------------------------------------------------
		//! Portable Library Header File.
		//! -----------------------------------------------------------------
#pragma region Free Image Libarary

#if DGE_Platform == DGE_Windows_Platform
#ifdef Dynamic
#if defined(DEBUG) || defined(_DEBUG)
		//#pragma comment( lib, "FreeImage_Dynamic_Debug.lib" )

#else
		//#pragma comment( lib, "FreeImage_Dynamic_Release.lib" )
#endif
#else
#define FREEIMAGE_LIB
#if defined(DEBUG) || defined(_DEBUG)
		//#pragma comment( lib, "FreeImage_Static_Debug.lib" )
#else
		//#pragma comment( lib, "FreeImage_Static.lib" )
#endif
#endif
#else

#endif
		//#include <FreeImage.h>
#pragma endregion

#pragma region DirectX Config
		//! -----------------------------------------------------------------
		//! Some Tools For Directx
		//! -----------------------------------------------------------------
#if DGE_Graphic_API == DGE_Use_Directx

#define DGE_UseDirectxMath

#pragma region Some Macro for Directx
		//! -----------------------------------------------------------------
		//! Some Macro for Directx
		//! Helper macros to build member functions that
		//! access member variables with thread safety(SDK DirectX)
		//! -----------------------------------------------------------------
#define SET_ACCESSOR(x, y)  \
	inline void Set##y(x t) \
	{                       \
		DXUTLock l;         \
		m_state.m_##y = t;  \
	};
#define GET_ACCESSOR( x, y )       inline x Get##y()           { DXUTLock l; return m_state.m_##y; };
					#define GET_SET_ACCESSOR( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR( x, y )

					#define SETP_ACCESSOR( x, y )      inline void Set##y( x* t )  { DXUTLock l; m_state.m_##y = *t; };
					#define GETP_ACCESSOR( x, y )      inline x* Get##y()          { DXUTLock l; return &m_state.m_##y; };
					#define GETP_SETP_ACCESSOR( x, y ) SETP_ACCESSOR( x, y ) GETP_ACCESSOR( x, y )
#pragma endregion

		//! -----------------------------------------------------------------
		//! Portable Library Header File.
		//! -----------------------------------------------------------------
#pragma region Direct3D Libarary

		//#pragma comment( lib, "dxerr.lib" )
		//#pragma comment(lib, "dxerr9.lib")
		//#pragma comment(lib, "dxguid.lib")
		//#pragma comment(lib, "d3d9.lib")
		//#pragma comment(lib, "d3d10.lib")
		//#pragma comment(lib, "d3dxof.lib")

#if defined(DEBUG) || defined(_DEBUG)
#define D3D_DEBUG_INFO
#define D3DX_DEBUG
		//#pragma comment(lib, "d3dx9d.lib")
		//#pragma comment(lib, "d3dx10d.lib")
#else
		//#pragma comment(lib, "d3dx9.lib")
		//#pragma comment(lib, "d3dx10.lib")
#endif

		//#pragma comment(lib, "d3dcompiler.lib")
		//#pragma comment(lib, "winmm.lib")
		//#pragma comment(lib, "comctl32.lib")
#pragma warning(push)
#pragma warning(disable : 4005)

#if DGE_Platform == DGE_Windows_Platform

#include <d3d11.h>
#include <d3d10.h>
#include <d3d9.h>
		//#include <D3dx9core.h>
#include <dxgi.h>
		//#include<d3dx9.h>
#include <d3dcommon.h>

#pragma warning(pop)

#pragma endregion
#endif
#endif
#pragma endregion

#endif

#pragma endregion
