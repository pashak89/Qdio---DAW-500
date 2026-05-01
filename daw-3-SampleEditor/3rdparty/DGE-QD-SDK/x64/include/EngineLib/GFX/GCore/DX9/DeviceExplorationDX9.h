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
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>

#include <GFX/DeviceExploration.h>

namespace DGE
{
	using namespace DGE::ResourceManager;
	using namespace DGE::Core::Math;
	using namespace Core::Array;
	namespace GFX
	{
		namespace GCore
		{
			namespace DirectX9
			{
				// Display Mode Structure that use in engine.
				struct DGE_GFXAPI DisplayModeDX9
				{
#if DGE_Graphic_API == DGE_Use_Directx && defined(DGE_UseGraphicAPI)
					D3DDISPLAYMODE Mode; // Direct3D display mode.
#endif
					char Bpp[10]; // Color depth in string format for display!
				};

				struct DGE_GFXAPI MultiSampleFeature
				{
#if DGE_Graphic_API == DGE_Use_Directx && defined(DGE_UseGraphicAPI)
					D3DMULTISAMPLE_TYPE Type;
					DWORD				Quality;
#endif
				};

				struct DGE_GFXAPI DeviceDX9Feature
				{
#if DGE_Graphic_API == DGE_Use_Directx && defined(DGE_UseGraphicAPI)
					D3DDEVTYPE DeviceType;
					D3DFORMAT  AdapterFormat;
					D3DFORMAT  BackBufferFormat;
					D3DFORMAT  DepthStencilFormat;
#endif
					MultiSampleFeature MultiSample;
					uint32			   PresentationInterval;
				};

				// base class of Explore device of system & select best ones.
				class DGE_GFXAPI DeviceExploreDX9 : public DeviceExplore
				{
				public:
					DeviceExploreDX9(uint16 TotalBackBuffer);

					~DeviceExploreDX9();

					//!? Initialize DeviceExplore class & start to Explore device on this System.
					virtual bool Init();

					// Initialize RendererDX9 for current setup DisplayMode.
					virtual bool InitializeRenderer(void* param, bool AutoAntiAliased = false);

					// return DisplayMode structure render screen
					virtual DisplayMode* GetDisplayMode();

					// true when Render in window mode.
					virtual bool IsWindowed();

					// true when V-Sync enabled.
					virtual bool IsVSynced();

					// Get Renderer of DirectX9 API.
					virtual GFX::GCore::Renderer* GetRenderer();

				protected:
					DArray<DisplayModeDX9*> m_DisplayModes;
					DArray<DeviceDX9Feature>
						   ConditionAv; // store all available condition of possible feature.
					bool   FastDialog	   = false;
					bool   InitDisplayMode = false;
					uint16 m_TotalBackBuffer;
#if DGE_Graphic_API == DGE_Use_Directx && defined(DGE_UseGraphicAPI)
					D3DADAPTER_IDENTIFIER9 m_adapter;
					D3DDISPLAYMODE		   m_SelectedMode;
					IDirect3D9*			   m_pD3D;
					D3DCAPS9			   m_Caps;
					IDirect3DDevice9*	   m_device = nullptr;
#endif
				};
			} // namespace DirectX9
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
