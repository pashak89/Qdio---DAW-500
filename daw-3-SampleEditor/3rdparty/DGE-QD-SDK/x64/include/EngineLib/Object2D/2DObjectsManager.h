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

#include "2DObject.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>

#include <Engine/GFX/GCore/Renderer.h>

#include <Engine/Object2D/GUI/Control.h>
#include <Engine/Object2D/GUI/Image.h>

#include <Engine/Core/IO/Events.h>
#include <Engine/Core/IO/InputManager.h>

namespace DGE
{
	namespace Objects2D
	{
		using namespace DGE::IO;
		using namespace DGE::IO::Input;
		using namespace DGE::Resource;
		using namespace DGE::Math;
		using namespace DGE::GFX;
		using namespace DGE::Objects2D::GUI;
		// using namespace DGE::Objects2D::PhysicBase;
#if defined(DGE_UseGraphicAPI)

		class DGE_API _2DObjectsManager
		{
			// Bound<int> m_BShowRectangle;
		public:
			_2DObjectsManager(
				GFX::GCore::Renderer* renderer /*, Mouse *mouse, Bound<int> ShowRectangle*/);
			virtual ~_2DObjectsManager(void);

			Image* AddImage(const DChar* TextureFile, Bound rect);

			Image* AddImage(const DChar* TextureFile, Vector2D Position,
							Core::BaseType::Color color = {1, 1, 1, 1});

			//			StaticObject* AddStaticObject(DChar* TextureFile, Bound rect, PhysicMaterial
			// physicMaterial);

			//			StaticObject* AddStaticObject(DChar* TextureFile, Vector2D Position,
			// PhysicMaterial physicMaterial, Color color = 0xffffffff);

			//			DynamicObject* AddDynamicObject(DChar* TextureFile, Bound rect,
			// PhysicMaterial physicMaterial);

			//			DynamicObject* AddDynamicObject(DChar* TextureFile, Vector2D Position,
			// PhysicMaterial physicMaterial, 											Color color
			// = 0xffffffff);

			//			PlayerController* AddPlayerController(DChar* TextureFile, Vector2D Position,
			//int
			// numberTextures, 												  PhysicMaterial
			// physicMaterial, Color color = 0xffffffff);

			//			PlayerController* AddPlayerController(DChar* TextureFile, Vector2D Position,
			// int
			// numberTextures, 												  PhysicMaterial
			// physicMaterial, Bound physicBountShape, Color color = 0xffffffff);

			/*PaintBoard* AddPaintBoard(Bound<int> Position);

			Player* AddPlayer(DChar* TextureFile, Bound rect = Bound(), char left = 'a', char right
			= 'd');

			HumanPlayer* AddHumanPlayer(DChar* TextureFile, Bound rect);

			EnemyMover* AddEnemyMover(DChar* TextureFile, Bound rect = Bound(), float index = 1);

			Button* AddButton(DChar* Caption, Bound rect, DChar* TextureFile, float index = 1);

			PushButton* AddPushButton(DChar* Caption, Bound rect, DChar* TextureFile, float index =
			1);

			PushButtonList* AddPushButtonList(vector<PushButtonData>* PushButtons, Core::Point
			Position, uint rows = 2, int DefaultSelected = -1);*/

			void AddControl(Control* control) { m_vObject.push_back(control); }

			void RemoveObject(uint32 ID) { m_vObject.erase(m_vObject.begin() + ID - 1); }

			void Render();
			void Update(float elapsed);

		protected:
			// True if Clicked Occurred
			bool m_bClicked,
				// True if Mouse Moved.
				m_bMouseMotion,
				// True if Mouse Button go to up state.
				m_bMouseUp,
				// True if Mouse Button go to down state.
				m_bMouseDown,
				// True if a Key Press.
				m_bKeyDown;
			// Store Data event of mouse click.
			MouseEventArgs m_DataEventClick,
				// Store Data event of button of mouse,(up/down)(left/middle/right).
				m_DataEventMouse,
				// Store Data event of Movement of mouse.
				m_DataEventMotion;
			// Store Data event of key press.
			KeyEventArgs m_DataEventKey;

			GFX::GCore::Renderer*						   m_pRenderer;
			vector<_2DObject*>							   m_vObject;
			std::shared_ptr<Core::IO::Input::InputManager> m_pInput;

			virtual void OnMouseMotion(EventArgs* event);
			virtual void OnMouseButton(EventArgs* event);
			virtual void OnMouseWheel(EventArgs* event);
			virtual void OnKeyPress(EventArgs* event);
		};
#endif
	} // namespace Objects2D
} // namespace DGE
