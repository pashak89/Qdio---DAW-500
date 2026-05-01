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

#include <Core/System/DModule.h>
#include <Scene/SceneModuleDef.h>
#include <Scene/SceneNode.h>
#include <Scene/ViewportManager.h>

namespace DGE
{
	namespace Scene
	{

		class DGE_SceneAPI SceneModule
			: public DGE::Core::System::DModule
			, public Core::Utility::Singleton<SceneModule>
		{
			D_OBJECT_SINGLETON(SceneModule)
			SceneModule(std::shared_ptr<Core::System::Context> context
						= DGE::Core::System::Context::getInstance());

		public:
			D_Destructor(SceneModule);

			/// IInitialize interface
			///
			/// \brief Use default implementation of
			/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
			/// function from IInitialize interface.
			using Core::Utility::Interfaces::IInitialize::initialize;

			/// \brief Initialize CoreModule and if it is initialized successfully return true
			///
			/// \return Return true if initializing is successfully else return false
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual bool release() override;

			Scene::SceneNodeWRef mainScene() const;

			Scene::ViewportManagerWRef viewportManager() const;

		protected:
			virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

		protected:
			/// \brief Main root of the scene node
			Scene::SceneNodeRef		  m_mainScene;
			Scene::ViewportManagerRef m_viewportManager;
		};

		using SceneModuleRef  = std::shared_ptr<Scene::SceneModule>;
		using SceneModuleWRef = std::weak_ptr<Scene::SceneModule>;

	} // namespace Scene
} // namespace DGE
