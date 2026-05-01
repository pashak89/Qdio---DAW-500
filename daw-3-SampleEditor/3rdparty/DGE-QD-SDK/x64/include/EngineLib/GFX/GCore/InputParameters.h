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
#include <Core/Utility/Interfaces/IInitialize.h>
#include <GFX/GCore/RenderPipelineDef.h>
#include <any>
#include <memory>
#include <optional>
#ifdef DGE_USE_GLM
#include <glm/common.hpp>
#endif

namespace DGE
{
	namespace GFX
	{
		class Camera;
	}
	namespace Scene
	{
		class Node;
		class Component;
		class DEntity;
	} // namespace Scene
	namespace GFX
	{
		class View;
		namespace GCore
		{
			class Renderer;
			namespace Interfaces
			{
				class IGObjectDrawable;
			}
			/// \brief input parameter for render pipeline
			struct DGE_CoreAPI InputParameters
			{
				/// \brief elapsed from last rendered frame
				chrono::milliseconds m_elapsedTime;
				/// \brief Current view-port ID
				uint8 m_currentViewportID;
				/// \brief Shared pointer to renderer
				std::shared_ptr<GFX::GCore::Renderer> m_renderer;
				/// \brief Shared pointer to clear parameters
				GFX::GCore::ClearParam m_clearParam;
				/// \brief No. number of current frame
				uint64 m_frameNumber = 0;
				/// \brief Indicate whether The mouse is clicked
				bool m_isMouseClicked = false;
				/// \brief Indicate whether The mouse is pressed
				bool m_isMousePressed = false;
				/// \brief Indicate whether The mouse is hovered
				bool m_isMouseHovered = false;
				/// \brief The current position of mouse
				Core::Math::Vector2D m_mousePosition;
				/// \brief Map name of initialize parameters and its values
				Core::Utility::Interfaces::InitializeParams m_initializeParameters;
				/// \brief Shared pointer to current mesh;
				std::shared_ptr<GFX::GCore::Interfaces::IGObjectDrawable> m_currentMesh;
				/// \brief Shared pointer to current view
				std::shared_ptr<GFX::View> m_currentView;
				/// \brief Shared pointer to current root node that is used for rendering in current view
				std::shared_ptr<Scene::Node> m_currentSceneNode;
				/// \brief Shared pointer to current node that is process in rendering pipeline
				std::shared_ptr<Scene::Node> m_thisNode;
				/// \brief Shared pointer to current component that is process in rendering pipeline
				std::shared_ptr<Scene::Component> m_thisComponent;
				/// \brief Shared pointer to current activated camera
				std::shared_ptr<Scene::Component> m_currentCamera;
				/// \brief A list of rendered DEntity
				std::shared_ptr<std::vector<std::shared_ptr<Scene::DEntity>>> m_renderedDEntity;
#ifdef DGE_USE_GLM
				/// \brief Current view matrix
				glm::mat4 m_currentViewMatrix;
				/// \brief Current projection matrix
				glm::mat4 m_currentProjectionMatrix;
#endif
				/// \brief distance to camera from current DEntity
				float m_currentDistanceToCamera;
			};

		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
