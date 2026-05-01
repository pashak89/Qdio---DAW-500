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

#include "Qt/QEngineDef.h"
#include <Qt/QMLScene/Model/QDModelSource.h>
#include <Qt/QMLScene/QComponent3D.h>

#include <Scene/Components/ModelComponent.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			/// \brief The model type in QML side for using as a component
			class QDModel : public DGE::QtWrapper::QMLScene::QComponent3D
			{
				D_OBJECT(QDModel)
				D_RegisterComponent(QDModel, Scene::ModelComponent);

				Q_OBJECT
				QML_ELEMENT
				Q_PROPERTY(DGE::QtWrapper::QMLScene::Model::QDModelSource* modelSource READ
							   modelSource WRITE setModelSource NOTIFY modelSourceChanged FINAL)
				D_QT_PROPERTY(DGE::QtWrapper::QMLScene::Model::QDModelSource*, modelSource,
							  ModelSource, nullptr)
				D_QT_PROPERTY_BINDING(GFX::AbstractModelRef, model, Model, modelSource)

			public:
				QDModel(QNode3D* parent = nullptr);

			protected:
				// QComponent3D interface
				virtual void init() override;
			};

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
