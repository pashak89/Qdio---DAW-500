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
#include <Core/System/Functor.h>
#include <GFX/GeometryModel.h>
#include <Scene/Component.h>
#include <Scene/Components/MaterialComponent.h>
#include <Scene/Components/ModelComponent.h>
#include <Scene/Components/ShaderComponent.h>
#include <Scene/Components/SolidRenderer.h>
#include <Scene/DEntity.h>
#include <Scene/Node.hpp>
#include <memory>
#include <unordered_map>

namespace DGE
{
	namespace Scene
	{
#define OpenBracket [
#define CloseBracket ]

#define tupleVars(name)                                                                            \
	auto OpenBracket		  name##Entity, name##Component, name##GeometryModel, name##Primitive, \
		name##ComponentsTable CloseBracket

		using ComponentSet = std::unordered_multiset<Scene::ComponentRef>;

		template<typename ModelType, typename PrimitiveType>
		using EntityTuple
			= std::tuple<Scene::DEntityRef, DGE::Scene::ModelComponentRef,
						 std::shared_ptr<ModelType>, std::shared_ptr<PrimitiveType>, ComponentSet>;

		template<typename PrimitiveType>
		using GoemetryEntityTuple = DGE::Scene::EntityTuple<DGE::GFX::GeometryModel, PrimitiveType>;
		/// \brief A helper class for creating the scene objects
		class DGE_SceneAPI SceneHelper
		{
			static std::shared_ptr<System::Function<bool, GFX::GCore::InputParameters>>
				s_defaultUpdateFunctionPtr;

		public:
			static DString s_defaultEntityName;
			SceneHelper();

			static Scene::MaterialComponentRef createMaterialComponent(
				DGE::Scene::NodeRef node, DString techniqueName,
				GFX::Resource::ShaderResourceRef shaderResource);
			static Scene::MaterialComponentRef createMaterialComponent(DGE::Scene::NodeRef node,
																	   DString materialResource);
			static Scene::MaterialComponentRef createMaterialComponent(DGE::Scene::NodeRef node,
																	   DString techniqueName,
																	   DString vertexShader,
																	   DString fragmentShader);

			template<
				class PrimitiveType, class ModelType = GFX::GeometryModel, bool isHitable = true,
				bool useUpdateEvent = false, bool isWireframe = false, bool debugBoundaries = false,
				const DString& entityName = SceneHelper::s_defaultEntityName,
				std::shared_ptr<System::Function<bool, GFX::GCore::InputParameters>>& updateFunctionPtr
				= SceneHelper::s_defaultUpdateFunctionPtr,
				typename... Args>
			static EntityTuple<ModelType, PrimitiveType> createEntityNode(Scene::NodeRef parentNode,
																		  DString vertexShader,
																		  DString fragmentShader,
																		  Args... args)
			{
				using namespace DGE::Scene;
				using namespace DGE::GFX;
				using namespace DGE::Core;
				auto cubeEntity = parentNode->createChild<DGE::Scene::DEntity>();
				cubeEntity->setIsHitable(isHitable);
				cubeEntity->setName(entityName);
				cubeEntity->setDebugBoundaries(debugBoundaries);
				// cubeEntity->addOrderType(DGE::Scene::ShaderComponent::staticTypeInfo(), 0);

				auto cubeMaterial = createMaterialComponent(cubeEntity, "SolidTechnique",
															vertexShader, fragmentShader);

				auto cubeModelComponent = cubeEntity->createComponent<DGE::Scene::ModelComponent>();
				auto cubeMesh  = std::make_shared<PrimitiveType>(args...);
				if (isWireframe)
					cubeMesh->setRenderPrimitivesType(GFX::GCore::RenderPrimitivesType::LineLoop);
				auto cubeModel = std::make_shared<ModelType>(cubeMesh);
				cubeModelComponent->setModel(cubeModel);
				if (useUpdateEvent)
				{
					if (updateFunctionPtr)
						cubeMesh->updated() += updateFunctionPtr;
				}
				auto solidRenderer = cubeEntity->createComponent<SolidRenderer>();
				solidRenderer->setMaterialComponent(cubeMaterial);
				solidRenderer->setModelComponent(cubeModelComponent);
				solidRenderer->setDebugVolume(cubeEntity->debugVolumeComponent());

				return {cubeEntity, cubeModelComponent, cubeModel, cubeMesh,
						ComponentSet{solidRenderer}};
			}

			template<
				class PrimitiveType, class ModelType = GFX::GeometryModel, bool isHitable = true,
				bool useUpdateEvent = false, bool isWireframe = false, bool debugBoundaries = false,
				const DString& entityName = SceneHelper::s_defaultEntityName,
				std::shared_ptr<System::Function<bool, GFX::GCore::InputParameters>>& updateFunctionPtr
				= SceneHelper::s_defaultUpdateFunctionPtr,
				typename... Args>
			static EntityTuple<ModelType, PrimitiveType> createEntityNode(
				Scene::NodeRef parentNode, DGE::GFX::Resource::ShaderResourceRef shaderResource,
				Args... args)
			{
				using namespace DGE::Scene;
				using namespace DGE::GFX;
				using namespace DGE::Core;
				auto cubeEntity = parentNode->createChild<DGE::Scene::DEntity>();
				cubeEntity->setIsHitable(isHitable);
				cubeEntity->setName(entityName);
				cubeEntity->setDebugBoundaries(debugBoundaries);
				// cubeEntity->addOrderType(DGE::Scene::ShaderComponent::staticTypeInfo(), 0);

				auto cubeMaterial = createMaterialComponent(cubeEntity, "SolidTechnique",
															shaderResource);

				auto cubeModelComponent = cubeEntity->createComponent<DGE::Scene::ModelComponent>();
				auto cubeMesh			= std::make_shared<PrimitiveType>(args...);
				if (isWireframe)
					cubeMesh->setRenderPrimitivesType(GFX::GCore::RenderPrimitivesType::LineLoop);
				auto cubeModel = std::make_shared<ModelType>(cubeMesh);
				cubeModelComponent->setModel(cubeModel);
				if (useUpdateEvent)
				{
					if (updateFunctionPtr)
						cubeMesh->updated() += updateFunctionPtr;
				}
				auto solidRenderer = cubeEntity->createComponent<SolidRenderer>();
				solidRenderer->setMaterialComponent(cubeMaterial);
				solidRenderer->setModelComponent(cubeModelComponent);
				solidRenderer->setDebugVolume(cubeEntity->debugVolumeComponent());

				return {cubeEntity, cubeModelComponent, cubeModel, cubeMesh,
						ComponentSet{solidRenderer}};
			}

			template<
				class PrimitiveType, class ModelType = GFX::GeometryModel, bool isHitable = true,
				bool useUpdateEvent = false, bool isWireframe = false, bool debugBoundaries = false,
				const DString& entityName = SceneHelper::s_defaultEntityName,
				std::shared_ptr<System::Function<bool, GFX::GCore::InputParameters>>& updateFunctionPtr
				= SceneHelper::s_defaultUpdateFunctionPtr,
				typename... Args>
			static EntityTuple<ModelType, PrimitiveType> createEntityNode(
				Scene::NodeRef parentNode, DString materialResourcePath, Args... args)
			{
				using namespace DGE::Scene;
				using namespace DGE::GFX;
				using namespace DGE::Core;
				auto cubeEntity = parentNode->createChild<DGE::Scene::DEntity>();
				cubeEntity->setIsHitable(isHitable);
				cubeEntity->setName(entityName);
				cubeEntity->setDebugBoundaries(debugBoundaries);
				// cubeEntity->addOrderType(DGE::Scene::ShaderComponent::staticTypeInfo(), 0);

				auto cubeMaterial = createMaterialComponent(cubeEntity, materialResourcePath);

				auto cubeModelComponent = cubeEntity->createComponent<DGE::Scene::ModelComponent>();
				auto cubeMesh			= std::make_shared<PrimitiveType>(args...);
				if (isWireframe)
					cubeMesh->setRenderPrimitivesType(GFX::GCore::RenderPrimitivesType::LineLoop);
				auto cubeModel = std::make_shared<ModelType>(cubeMesh);
				cubeModelComponent->setModel(cubeModel);
				if (useUpdateEvent)
				{
					if (updateFunctionPtr)
						cubeMesh->updated() += updateFunctionPtr;
				}
				auto solidRenderer = cubeEntity->createComponent<SolidRenderer>();
				solidRenderer->setMaterialComponent(cubeMaterial);
				solidRenderer->setModelComponent(cubeModelComponent);
				solidRenderer->setDebugVolume(cubeEntity->debugVolumeComponent());

				return {cubeEntity, cubeModelComponent, cubeModel, cubeMesh,
						ComponentSet{solidRenderer}};
			}

		public:
		};

	} // namespace Scene
} // namespace DGE
