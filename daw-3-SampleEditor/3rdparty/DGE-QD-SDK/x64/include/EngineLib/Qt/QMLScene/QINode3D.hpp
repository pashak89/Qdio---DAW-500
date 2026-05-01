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

#include <Qt/QMLScene/QINode3D.h>
#include <Qt/QMLScene/QScene3D.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			template<class QNode3DType, class NodeType, class... Args>
			QNode3DType* QINode3D::createChild(NodeType nodeObject, const DString name, Args... args)
			{
				auto node3d = new QNode3DType(args...);
				node3d->setNodeRef(nodeObject);
				node3d->setParent(this);

				this->onChildAdded(node3d);
				return node3d;
			}

			template<class QNode3DType, class NodeType, class... Args>
			QNode3DType* QINode3D::createChild(NodeType nodeObject, Args... args)
			{
				return createChild<QNode3DType>(nodeObject, "", args...);
			}

			template<class QComponent3DType, class ComponentType, class... Args>
			QComponent3DType* QINode3D::createComponent(ComponentType componentObject,
														const DString  name, Args... args)
			{
				auto component3D = new QComponent3DType(args...);
				component3D->setComponentRef(componentObject);
				component3D->setParentNode(this);

				this->onComponentAdded(component3D);
				return component3D;
			}

			template<class QComponent3DType, class ComponentType, class... Args>
			QComponent3DType* QINode3D::createComponent(ComponentType componentObject, Args... args)
			{
				return createComponent<QComponent3DType>(componentObject, "", args...);
			}

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
