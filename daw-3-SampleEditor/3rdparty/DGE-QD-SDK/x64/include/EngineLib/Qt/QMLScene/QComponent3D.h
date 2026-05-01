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

#include "Core/System/DObjectMacrosDef.h"
#include <Core/MemoryManager/UndefMemoryMacros.h>

#include <QQuickItem>
#include <Qt/QEngineDef.h>
#include <Qt/QMLScene/QComponentMacros.h>
#include <Qt/QMLScene/QNode3D.h>
#include <Qt/QMLScene/QScene3D.h>

#include <Core/MemoryManager/RedefMemoryMacros.h>

#include <Scene/Component.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			/// \brief Base class for Component 3d in QML codes
			class DGE_QtAPI QComponent3D
				: public QObject
				, public System::DObject
			{
				D_OBJECT(QComponent3D)
				D_MapComponent(QComponent3D, Scene::Component);

				Q_OBJECT
				QML_ELEMENT
				// Q_PROPERTY(QNode3D* parentNode READ parentNode WRITE setParentNode NOTIFY
				// 			   parentNodeChanged)
				friend class QMLScene::QNode3D;

			public:
				explicit QComponent3D(QNode3D* parent = nullptr);

				void setParent(QINode3D* parent);

			protected:
				/// \brief Register of specified type of component in the scene system automatically
				template<typename QComponentType, typename ComponentType>
				void registerComponent(QObject* parentNode)
				{
					auto parentNode3D = qobject_cast<QNode3D*>(parentNode);
					if (m_componentRef)
					{
						if (parentNode3D)
						{
							auto newParent = parentNode3D->nodeRef();
							auto oldParent = m_componentRef->node();
							if (!oldParent.expired() && newParent && oldParent.lock() == newParent)
								return;
						}
					}
					if (parentNode3D != nullptr)
					{
						auto rootScene3D = QMLScene::QScene3D::getInstance();
						m_componentRef	 = rootScene3D->attachComponent<
							  QComponentType, ComponentType>(parentNode3D, (QComponentType*) this);
					}
					else
					{
						auto rootScene3D = QMLScene::QScene3D::getInstance();
						m_componentRef = rootScene3D->attachComponent<QComponentType, ComponentType>(
							rootScene3D.get(), (QComponentType*) this);
					}
				}

				virtual void init();

			private:
				void setParent(QObject*) = delete;

			protected:
				D_PROPERTY(DGE::Scene::ComponentRef, componentRef);
				D_QT_PROPERTY(QNode3D*, parentNode, ParentNode, nullptr);
			};
		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
