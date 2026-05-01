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
#include <GFX/GCore/Renderer.h>
#include <ScriptSet/Script.h>

namespace DGE
{
	namespace GFX
	{
		struct DGE_GFXAPI DisplayMode
		{
			uint32 Width;		// Width of screen render
			uint32 Height;		// Height of screen render
			uint16 RefreshRate; // Refresh rate of monitor
			bool IsFullScreen;//true when render in full screen mode.
			char DisplayFormat[20];//format of Display in string format.
			char ColorDepth[6];//Depth of Color of screen.
		};

		//base class of Explore device of system & select best ones.
		class DGE_GFXAPI DeviceExplore
		{
		protected:
			char* DeviceAPI;
			ScriptSet::ScriptResource* m_SettingScript = nullptr;
			bool m_Windowed;
			bool m_VSynced;
			GFX::DisplayMode* m_CurrentDisplaymode = nullptr;
			GFX::GCore::Renderer*	   m_Renderer			= nullptr;

		public:
			DeviceExplore(const char* NameDeviceAPI = nullptr, int length = 0);
			virtual ~DeviceExplore();

			//!? Initialize DeviceExplore class & start to Explore device on this System.
			virtual bool Init()=0;

			//Initialize Renderer for current setup DisplayMode.
			virtual bool InitializeRenderer(void* parameter,bool AutoAntiAliased=false) = 0;//antielyas

			//return DisplayMode structure render screen
			virtual DisplayMode* GetDisplayMode() = 0;

			//true when Render in window mode.
			virtual bool IsWindowed()=0;

			//true when V-Sync enabled.
			virtual bool IsVSynced() = 0;

			//Get Renderer of current API.
			virtual GFX::GCore::Renderer* GetRenderer() = 0;
		};
		extern DGE_GFXExtern DeviceExplore* g_DeviceExplorer;
	}
}
