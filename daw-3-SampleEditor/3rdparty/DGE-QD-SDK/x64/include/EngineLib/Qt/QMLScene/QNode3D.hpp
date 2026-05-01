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
#include <Qt/QMLScene/QNode3D.h>
#include <Qt/QMLScene/QScene3D.h>

#include <Scene/SceneNode.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			template<typename QNodeType, typename NodeType>
			void QNode3D::registerNode(QObject *parentNode)
			{
				auto sceneRoot		  = QMLScene::QScene3D::getInstance();
				auto sceneRootNodeRef = Scene::SceneNode::getInstance();
				auto parentNode3D	  = qobject_cast<QNode3D *>(parentNode);
				if (m_nodeRef)
				{
					auto parentNodeRef = m_nodeRef->parent().lock();
					auto newParentnodeRef = parentNode3D->nodeRef();
					if (parentNodeRef == newParentnodeRef)
						return;
					if (parentNodeRef)
					{
						parentNodeRef->removeChild(m_nodeRef.value());
					}
					else
					{
						LogError("NodeRef of QScene root is null");
					}
					if (parentNode3D == nullptr)
						parentNode3D = sceneRoot.get();
					newParentnodeRef->addChild(m_nodeRef.value());
					this->setSceneRoot(parentNode3D->sceneRoot());
					sceneRoot->registerSceneObject(m_nodeRef->id(), this, m_nodeRef);
				}
				else
				{
					if (parentNode3D != nullptr)
					{
						auto rootScene3D = QMLScene::QScene3D::getInstance();
						m_nodeRef = rootScene3D->attachNode<QNodeType, NodeType>(parentNode3D,
																				 (QNodeType *) this);
					}
					else
					{
						LogWarning("Parent of QNode3D should be a QNode3D");
						auto rootScene3D = QMLScene::QScene3D::getInstance();
						m_nodeRef = rootScene3D->attachNode<QNodeType, NodeType>(rootScene3D.get(),
																				 (QNodeType *) this);
					}
				}
			}
		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
