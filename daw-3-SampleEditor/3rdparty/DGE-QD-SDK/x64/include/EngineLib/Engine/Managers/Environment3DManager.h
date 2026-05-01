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

#include "EngineModuleDef.h"
#include <Core/IO/Input/InputEvents.h>
#include <Core/System/DEvent.h>
#include <Scene/Components/AnimationComponent.h>
#include <Scene/Components/CameraController.h>
#include <Scene/DEntity.h>
#include <Scene/SceneNode.h>

namespace DGE
{
	namespace Managers
	{
		struct Environment3DInfo
		{
			Scene::CameraControllerWRef currentCameraController;
			bool						isAccepted = false;
			void*						extraInfo  = nullptr;
		};

		class DGE_EngineAPI Environment3DManager : public Core::IO::Input::InputEvents
		{
		public:
			Environment3DManager();

			/// \brief Get selected entity object event
			bool onSelectedEntityChanged(Scene::DEntityWRef entity);

			DGE::Scene::CameraControllerWRef cameraController() const;
			void									setCameraController(
												   const DGE::Scene::CameraControllerRef& newCameraController);

			static DGE::Scene::NodeWRef movingArrows();
			static void setMovingArrows(const Scene::NodeRef& newMovingArrows);

		protected:
			Vector3D getMappedPoint(Vector2D mousePosition, Vector3D helpPoint) const;
			Vector3D getMappedDirection(Vector2D mousePosition) const;

			void updateArrowAndEntity(Vector3D pos);

			///
			// void addKey(uint64 time, Vector3D position, Animation::TransitionType transition);

		protected:
			static DGE::Scene::NodeWRef s_movingArrows;
			static DGE::Scene::NodeWRef s_movingArrowsParent;

			D_Event(selectedEntityChangedEvent, bool, DGE::Scene::DEntityWRef);

			DGE::Scene::CameraControllerWRef m_cameraController;
			DGE::Scene::DEntityWRef			m_selectedEntity;

			/// \brief Last selected an arrow with hover mouse on a group of them
			Scene::DEntityWRef m_lastSelectedArrow;
			/// \brief Last Start 3d position that is using for calculating movement of object on
			/// every view
			Vector3D m_lastStart3DPosition;
		};
		using Environment3DManagerRef = std::shared_ptr<DGE::Managers::Environment3DManager>;
		using Environment3DManagerWRef = std::weak_ptr<DGE::Managers::Environment3DManager>;
	} // namespace Managers
} // namespace DGE
