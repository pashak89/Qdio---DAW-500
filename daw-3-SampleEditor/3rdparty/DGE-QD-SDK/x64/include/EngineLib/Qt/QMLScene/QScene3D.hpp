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
#include <GFX/OrthoLens.h>
#include <GFX/PerspectiveLens.h>
#include <Scene/Component.h>
#include <Scene/Components/AnimationComponent.h>
#include <Scene/Components/DrawableComponent.h>
#include <Scene/Components/FpsCameraController.h>
#include <Scene/Components/ModelComponent.h>
#include <Scene/Components/PredefinedComponents.h>
#include <Scene/Components/ShaderComponent.h>
#include <Scene/Components/ThirdPersonCameraController.h>
#include <Scene/DEntity.h>
#include <Scene/Node.h>

#include <Core/MemoryManager/UndefMemoryMacros.h>

#include <Qt/QMLScene/Camera/QCameraComponent.h>
#include <Qt/QMLScene/Camera/QFpsCameraController.h>
#include <Qt/QMLScene/Camera/QOrthoLens.h>
#include <Qt/QMLScene/Camera/QPerspectiveLens.h>
#include <Qt/QMLScene/Camera/QThirdPersonController.h>
#include <Qt/QMLScene/Model/QDModel.h>
#include <Qt/QMLScene/QDAnimationComponent.h>
#include <Qt/QMLScene/QDShader.h>
#include <Qt/QMLScene/QEntity3D.h>
#include <Qt/QMLScene/QNode3D.hpp>

#include <Core/MemoryManager/RedefMemoryMacros.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			Q_NAMESPACE
			template<class Node3DType, class... Args>
			std::shared_ptr<Scene::Node> QScene3D::node3DBinder(std::shared_ptr<Scene::Node> nodeRef,
																Args... args)
			{
				if (*(Node3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QNode3D::staticTypeInfo().typeInfoRef.get()))
				{
					return nodeRef->createChild<Scene::Node>(args...);
				}
				if (*(Node3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QEntity3D::staticTypeInfo().typeInfoRef.get()))
				{
					return nodeRef->createChild<Scene::DEntity>(args...);
				}

				if (*(Node3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QScene3D::staticTypeInfo().typeInfoRef.get()))
				{
					return Scene::SceneNode::getInstance(args...);
				}
				return nullptr;
			}

			template<class NodeType, class... Args>
			QINode3D* QScene3D::nodeBinder(Args... args)
			{
				if (*(NodeType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::Node::staticTypeInfo().typeInfoRef.get()))
				{
					return new QNode3D(args...);
				}
				if (*(NodeType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::DEntity::staticTypeInfo().typeInfoRef.get()))
				{
					return new QEntity3D(args...);
				}

				if (*(NodeType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::SceneNode::staticTypeInfo().typeInfoRef.get()))
				{
					return new QScene3D(args...);
				}
				return nullptr;
			}

			template<class Component3DType, class... Args>
			std::shared_ptr<Scene::Component> QScene3D::component3DBinder(
				std::shared_ptr<Scene::Node> nodeRef, Args... args)
			{
				if (*(Component3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QComponent3D::staticTypeInfo().typeInfoRef.get()))
				{
					return nodeRef->createComponent<Scene::Component>(args...);
				}
				if (*(Component3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QCameraComponent::staticTypeInfo().typeInfoRef.get()))
				{
					return std::static_pointer_cast<Scene::Component>(
						nodeRef->createComponent<Scene::CameraComponent>(args...));
				}

				if (*(Component3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QFpsCameraController::staticTypeInfo().typeInfoRef.get()))
				{
					return std::static_pointer_cast<Scene::Component>(
						nodeRef->createComponent<Scene::FpsCameraController>(args...));
				}
				if (*(Component3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QThirdPersonController::staticTypeInfo().typeInfoRef.get()))
				{
					return std::static_pointer_cast<Scene::Component>(
						nodeRef->createComponent<Scene::ThirdPersonCameraController>(args...));
				}
				if (*(Component3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QDShader::staticTypeInfo().typeInfoRef.get()))
				{
					return std::static_pointer_cast<Scene::Component>(
						nodeRef->createComponent<Scene::ShaderComponent>(args...));
				}
				if (*(Component3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QDModel::staticTypeInfo().typeInfoRef.get()))
				{
					return std::static_pointer_cast<Scene::Component>(
						nodeRef->createComponent<Scene::ModelComponent>(args...));
				}
				if (*(Component3DType::staticTypeInfo().typeInfoRef.get())
					== *(QMLScene::QDAnimationComponent::staticTypeInfo().typeInfoRef.get()))
				{
					return std::static_pointer_cast<Scene::Component>(
						nodeRef->createComponent<Scene::AnimationComponent>(args...));
				}
				//					if (*(Component3DType::staticTypeInfo().typeInfoRef.get())
				//						==
				//*(QMLScene::QOrthoLens::staticTypeInfo().typeInfoRef.get()))
				//					{
				//						return std::static_pointer_cast<Scene::Component>(
				//							nodeRef->createComponent<Scene::DrawableComponent<GFX::OrthoLens>>(
				//								args...));
				//					}

				return nullptr;
			}

			template<class ComponentType, class... Args>
			QObject* QScene3D::componentBinder(Args... args)
			{
				if (*(ComponentType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::Component::staticTypeInfo().typeInfoRef.get()))
				{
					return new QMLScene::QComponent3D(args...);
				}
				if (*(ComponentType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::CameraComponent::staticTypeInfo().typeInfoRef.get()))
				{
					return new QMLScene::QCameraComponent(args...);
				}

				if (*(ComponentType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::FpsCameraController::staticTypeInfo().typeInfoRef.get()))
				{
					return new QMLScene::QFpsCameraController(args...);
				}
				if (*(ComponentType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::ThirdPersonCameraController::staticTypeInfo().typeInfoRef.get()))
				{
					return new QMLScene::QThirdPersonController(args...);
				}
				if (*(ComponentType::staticTypeInfo().typeInfoRef.get())
					== *(GFX::OrthoLens::staticTypeInfo().typeInfoRef.get()))
				{
					return new QMLScene::QOrthoLens(args...);
				}
				if (*(ComponentType::staticTypeInfo().typeInfoRef.get())
					== *(GFX::PerspectiveLens::staticTypeInfo().typeInfoRef.get()))
				{
					return new QMLScene::QPerspectiveLens(args...);
				}
				if (*(ComponentType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::ShaderComponent::staticTypeInfo().typeInfoRef.get()))
				{
					return new QMLScene::QDShader(args...);
				}
				if (*(ComponentType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::ModelComponent::staticTypeInfo().typeInfoRef.get()))
				{
					return new QMLScene::QDModel(args...);
				}
				if (*(ComponentType::staticTypeInfo().typeInfoRef.get())
					== *(Scene::AnimationComponent::staticTypeInfo().typeInfoRef.get()))
				{
					return new QMLScene::QDAnimationComponent(args...);
				}

				return nullptr;
			}

			template<class Node3DType, class NodeType, class... Args>
			QINode3D* QScene3D::createNode3D(QINode3D* parent, Args... args)
			{
				auto nodePtr = parent->nodeRef();
				auto nodeObj = node3DBinder<Node3DType>(nodePtr, args...);
				auto node3D	 = parent->createChild<Node3DType>(nodeObj);
				registerSceneObject(nodeObj->id(), node3D, nodeObj);
				return node3D;
			}

			template<class Node3DType, class NodeType, class... Args>
			std::shared_ptr<NodeType> QScene3D::attachNode(QINode3D* parent, QINode3D* self,
														   Args... args)
			{
				auto nodePtr = parent->nodeRef();
				auto nodeObj = node3DBinder<Node3DType>(nodePtr, args...);
				self->setParent(parent);
				self->setSceneRoot(parent->sceneRoot());
				registerSceneObject(nodeObj->id(), self, nodeObj);
				return std::dynamic_pointer_cast<NodeType>(nodeObj);
			}

			template<class QComponent3DType, class ComponentType, class... Args>
			QComponent3DType* QScene3D::createComponent3D(QINode3D* parent, Args... args)
			{
				auto nodeRefPtr	  = parent->nodeRef();
				auto componentObj = component3DBinder<QComponent3DType>(nodeRefPtr, args...);
				auto component3D  = parent->createComponent<QComponent3DType>(componentObj);
				registerSceneComponent(component3D, componentObj);
				return component3D;
			}

			template<class QComponent3DType, class ComponentType, class... Args>
			std::shared_ptr<ComponentType> QScene3D::attachComponent(QINode3D*		   parent,
																	 QComponent3DType* self,
																	 Args... args)
			{
				auto nodeRefPtr	  = parent->nodeRef();
				auto componentObj = component3DBinder<QComponent3DType>(nodeRefPtr, args...);
				self->setParent(parent);
				registerSceneComponent(self, componentObj);
				return std::dynamic_pointer_cast<ComponentType>(componentObj);
			}
		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
