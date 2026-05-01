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

#include "GFX/GCore/Interfaces/IRender.h"
#include <Core/System/DObject.h>
#include <Core/System/DProperty.h>
#include <GFX/AbstractModel.h>
#include <Scene/Component.h>
#include <Scene/Components/SystemComponent.h>
#include <memory>
namespace DGE
{
	namespace Scene
	{
		/// \brief A Model component that use built-in basic object or 3d object
		class DGE_SceneAPI ModelComponent
			: public Scene::Component
			, public GFX::GCore::Interfaces::IRender
		{
			D_OBJECT(ModelComponent)
		public:
			ModelComponent(DGE::Core::System::ContextRef context);

			ModelComponent(const std::shared_ptr<GFX::AbstractModel> &model,
						   DGE::Core::System::ContextRef			  context);
			D_Destructor(ModelComponent);

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

			// IUpdate interface
			virtual bool update(GFX::GCore::InputParameters &input) override;

			// IRender interface
			virtual bool render(GFX::GCore::InputParameters &input) override;

			std::shared_ptr<GFX::AbstractModel> model() const;
			void setModel(const std::shared_ptr<GFX::AbstractModel> &newModel);

			std::vector<Core::Math::Vector3D> vertices() const;

		protected:
			/// \brief model object use for wrapping built-in basic object or
			/// 3d object and then render it
			std::shared_ptr<GFX::AbstractModel> m_model;
			/// \brief Indicate whether draw items for debugging boundaries.
			D_PROPERTY(bool, debugBoundaries, DebugBoundaries, false);
		};
		D_DefRefType(ModelComponent);
	} // namespace Scene
} // namespace DGE
