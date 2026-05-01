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

#include "Core/System/DObject.h"
#include "Core/System/DObjectMacrosDef.h"
#include <QObject>
#include <QQmlComponent>
#include <QQuickItem>
#include <Qt/QEngineDef.h>
#include <QtQml/qqml.h>

#include <Scene/Node.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			class QNode3D;
			class QScene3D;
			class QINode3D
				: public QObject
				, public System::DObject
			{
				D_OBJECT(QINode3D)

				Q_OBJECT
				QML_ELEMENT

				Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

				friend class DGE::QtWrapper::QMLScene::QNode3D;
				friend class DGE::QtWrapper::QMLScene::QScene3D;

			public:
				explicit QINode3D(QObject *parent = nullptr);

				virtual DGE::QtWrapper::QMLScene::QScene3D *sceneRoot() const = 0;

				template<class QNode3DType, class NodeType, class... Args>
				QNode3DType *createChild(NodeType nodeObject, const DString name, Args... args);

				template<class QNode3DType, class NodeType, class... Args>
				QNode3DType *createChild(NodeType nodeObject, Args... args);

				template<class QComponent3DType, class ComponentType, class... Args>
				QComponent3DType *createComponent(ComponentType componentObject, const DString name,
												  Args... args);

				template<class QComponent3DType, class ComponentType, class... Args>
				QComponent3DType *createComponent(ComponentType componentObject, Args... args);

				uint32 nodeID() const;

				virtual Scene::NodeRef nodeRef() const = 0;
				virtual void		   setNodeRef(Scene::NodeRef nodeRef) = 0;

				QString name() const;
				void	setName(const QString &newName);

				QINode3D *parentNode() const;

			protected:
				virtual void setSceneRoot(DGE::QtWrapper::QMLScene::QScene3D *) = 0;
				// virtual QINode3D *createQNode3D(std::shared_ptr<DGE::Scene::Node>) = 0;

			public slots:
				virtual void onChildAdded(QINode3D *node)	  = 0;
				virtual void onComponentAdded(QINode3D *node) = 0;
				void		 setParent(QINode3D *);

			signals:
				void nameChanged();

			private:
				void setParent(QObject *) = delete;
			};

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
