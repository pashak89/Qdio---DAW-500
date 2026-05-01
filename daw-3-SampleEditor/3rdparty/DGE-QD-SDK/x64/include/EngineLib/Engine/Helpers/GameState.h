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

#include <EngineModuleDef.h>
#include <GFX/GCore/DX9/RendererDX9.h>
// #include <GameObject/GameObject.h>

namespace DGE
{
	class Engine;
	namespace GameTools
	{
		struct ViewerSetup
		{
			DGE::GFX::GCore::ClearParam m_clearParam
				= {}; // Parameter needed for clear viewer screen.
			// DGE::GameObjects::SceneObject *mainCamera = nullptr;
			ViewerSetup() { m_clearParam.clear(); };
		};
		enum class StatusGameState
		{
			NotLoaded = 0, Loaded, Switched, Resumed, Destroyed,
		};

		//! a interface class for definition finite state machine(FSM) for engine!
		class DGE_EngineAPI GameState
		{
			static uint32	m_statesCounter;
			uint32			m_id; // Application defined ID that must be unique for state switching.
			StatusGameState m_status;
			friend class DGE::Engine;
			void SetStatus(StatusGameState flage);
			StatusGameState GetStatus();

			void GotoLoading();
			void GotoSwitching();
			void GotoResuming();
			void GotoDestroying();
			uint32 GetGlobalId();

		public:
			GameState(uint32 id = 0);

			virtual ~GameState();

			/*
			\brief	Engine Call this function when Completely loaded and with
			this function,state load.
			*/
			virtual void OnLoad();

			/*
			\brief	Engine Call this function when resume State from last switching.
			*/
			virtual void OnResume();

			/*
			\brief	Engine Call this function when switch to another State and pause using this state.
			*/
			virtual void OnSwitch();

			/*
			\brief	Engine Call this function when Completely loaded and with
			this function state destroy.
			*/
			virtual void OnDestroy();

			/*
			\brief	return \c ViewerSetup struct for given frame.
			*/
			virtual void RequestViewer(ViewerSetup *viewer);

			/*
			\brief	the state can update own work (maybe relate to render
			function).
			*/
			virtual void Update(float elapsed);

			/*
			\brief	the Engine Call this function after own render.
			state can affect on render.
			*/
			virtual void Render();

			/*
			\brief	return ID of this state.
			\return \c uint
			*/
			uint32 GetID();
		};

	}
}

