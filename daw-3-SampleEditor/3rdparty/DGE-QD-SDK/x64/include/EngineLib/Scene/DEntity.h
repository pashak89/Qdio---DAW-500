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
#include "Core/Math/Vector3D.h"
#include <Core/Math/AABB.h>
#include <Core/Math/Transformation.h>
#include <Scene/Components/DebugVolume.h>
#include <Scene/Components/ModelComponent.h>
#include <Scene/Components/PredefinedComponents.h>
#include <Scene/Components/ShaderComponent.h>
#include <Scene/SceneNode.h>

namespace DGE
{
	namespace Scene
	{
		/// \brief Base class for Entity object in Scene with location properties
		class DGE_SceneAPI DEntity : public Scene::Node
		{
			D_OBJECT(DEntity)
		public:
			DEntity(DGE::Core::System::ContextRef context);

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			virtual bool update(GFX::GCore::InputParameters& input) override;

			// IRender interface
			virtual bool render(GFX::GCore::InputParameters& input) override;

			// std::shared_ptr<Scene::ShaderComponent> mainShader() const;
			// void setMainShader(const std::shared_ptr<Scene::ShaderComponent>& newMainShader);

			std::map<ModelComponentRef, std::shared_ptr<Core::Math::AABB>> boundaries() const;

			Color currentColor() const;
			void  setCurrentColor(const Color& newCurrentColor);

			Color selectionColor() const;
			void  setSelectionColor(const Color& newSelectionColor);

			Core::Math::BoundaryVolumeType boundaryType() const;
			void setBoundaryType(Core::Math::BoundaryVolumeType newBoundaryType);

		protected:
			/// \brief A private helper function to add a boundary for specified ModelComponent that
			/// is useful for debug purpose.
			Core::Math::BoundaryRef addDebugBoundary(ModelComponentRef modelComponent);

		public:
			/// \brief Default name for distance to camera variable
			static DString s_distanceToCameraName;

		protected:
			/// \brief Default color for the entity when it is not selected
			static Color s_defaultColor;
			/// \brief Default selection effect color for the entity when it is selected
			static Color s_defaultSelectionColor;

			/// \brief Type of boundary is used for this entity
			Core::Math::BoundaryVolumeType m_boundaryType;
			// /// \brief Main shader component use for general rendering entity and applying
			// transformation. std::shared_ptr<Scene::ShaderComponent> m_mainShader;
			/// \brief Indicate whether this entity is hit-able
			D_PROPERTY(bool, isHitable, IsHitable, false);
			/// \brief Indicate whether this entity is hover-able
			D_PROPERTY(bool, isHoverable, IsHoverable, false);
			/// \brief Indicate whether this entity has boundaries
			D_PROPERTY(bool, hasBoundaries, HasBoundaries, false);
			/// \brief Indicate whether draw items for debugging boundaries.
			D_PROPERTY(bool, debugBoundaries, DebugBoundaries, false);
			/// \brief Indicate whether this entity has random coloring
			D_PROPERTY(bool, isRandomColoring, IsRandomColoring, false);
			/// \brief Current color for this entity
			Color m_currentColor;
			/// \brief Selection effect color for this entity
			Color m_selectionColor;
			/// \brief Indicate whether this entity is selected
			D_PROPERTY(bool, isSelected, IsSelected, false);
			/// \brief shared pointer to debug volume component is used for render debug items
			D_PROPERTY(Scene::DebugVolumeRef, debugVolumeComponent, DebugVolumeComponent);
			/// \brief scale value of bounding box is using for calculating surrounding bounding box
			/// of this entity
			D_PROPERTY(Core::Math::Vector3D, boundingBoxScale, BoundingBoxScale, 1.01f);
			/// \brief Map of model component and it Boundary
			std::map<ModelComponentRef, std::shared_ptr<Core::Math::AABB>> m_boundaries;
		};

		D_DefRefType(DEntity);
	} // namespace Scene
} // namespace DGE
