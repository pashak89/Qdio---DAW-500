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
 *
 */
#pragma once

#include <Core/System/DEvent.h>
#include <Core/IO/Input/Mouse.h>
#include <Core/System/DObjectMacrosDef.h>
#include <Scene/Component.h>
#include <Scene/Components/CameraController.h>
#include <Scene/DEntity.h>
#include <Scene/SceneModuleDef.h>
#include <atomic>

namespace DGE
{
	namespace Scene
	{
		/// \brief This component could handle input from a mouse and control
		/// the movement of a 3D object based on user interaction with the mouse.
		class DGE_SceneAPI MouseController : public DGE::Scene::Component
		{
			D_OBJECT(MouseController)
		public:
			MouseController(DGE::Core::System::ContextRef context);

			D_Destructor(MouseController);

			// IRenderPipeline interface
			/// \brief Call when object must be initialized.
			using Scene::Component::initialize;

			// IInitialize interface
			bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			bool update(GFX::GCore::InputParameters& input) override;

			/// \brief Get clicked event of mouse
			bool onMouseClicked(Core::Math::Vector2D pos, void* eventData);
			/// \brief Get key pressed event of mouse
			bool onMouseHoverMoved(Core::Math::Vector2D pos, void* eventData);
			/// \brief Get moved event of mouse
			bool onMousePressed(Core::Math::Vector2D   pos,
								Core::IO::MouseButtons mouseButton,
								void*				   eventData);
			/// \brief Get moved event of mouse with pressing any key
			bool onMouseMoved(Core::Math::Vector2D pos, Core::IO::MouseButtons mouseButton,
							  void* eventData);
			/// \brief Get key released event of mouse
			bool onMouseReleased(Core::Math::Vector2D	pos,
								 Core::IO::MouseButtons mouseButton,
								 void*					eventData);
			/// \brief Get wheeled event of mouse
			bool onMouseWheeled(Core::Math::Vector2D pos, Core::IO::MouseButtons mouseButton,
								Core::Math::Vector2D angleDelta, void* eventData);
			/// \brief Get key pressed event of key of keyboard
			bool onKeyPressed(int key, IO::KeyboardModifiers modifires, void* eventData);
			/// \brief Get key released event of key of keyboard
			bool onKeyReleased(int key, IO::KeyboardModifiers modifires, void* eventData);
			/// \brief Get touch event
			bool onTouchEvent(Core::Math::Vector2D pos, void* eventData);

			/// \brief Get entity hovered by mouse event
			bool onEntityHoveredByMouse(bool isClicked, Core::Math::Vector2D pos,
										Scene::DEntityWRef hoveredEntity = {});

			/// \brief Retrieve focus flag
			bool hasFocus();
			/// \brief Set focus flag
			void setFocus(bool focus);

		protected:
			/// \brief Detect whether the mouse position is in boundary of the 3d object
			bool	 isInBound(Vector2D mousePosition);
			Vector3D getMappedPoint(DGE::Scene::CameraControllerRef cameraController,
									Vector2D mousePosition, Vector3D helpPoint) const;

			Vector3D getMappedDirection(DGE::Scene::CameraControllerRef cameraController,
										Vector2D						mousePosition) const;

			void updateEntity(Vector3D movement);

		protected:
			std::atomic_bool m_hasFocus;
			/// \brief Last selected an object with hover mouse on a group of objects
			Scene::DEntityWRef m_lastHoveredEntity;
			/// \brief Last Start 3d position that is using for calculating movement of object on
			/// every view
			Vector3D m_lastStart3DPosition;
			D_PROPERTY(bool, useChildrenOnly, UseChildrenOnly, false);
			D_Event(entityMoved, void, Scene::DEntityWRef, Vector3D);
		};

	} // namespace Scene
} // namespace DGE
