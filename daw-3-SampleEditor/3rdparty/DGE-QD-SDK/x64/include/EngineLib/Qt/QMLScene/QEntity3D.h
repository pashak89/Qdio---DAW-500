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

#include "QNode3D.h"
#include <Scene/DEntity.h>
#include <Qt/QMLScene/QNodeMacros.h>
#include <Qt/QMLScene/QScene3D.h>
Q_DECLARE_METATYPE(DGE::Scene::DEntityRef);

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			class QDShader;
			class QEntity3D : public QNode3D
			{
				D_OBJECT(QEntity3D)
				D_RegisterNode(QEntity3D, Scene::DEntity);

				Q_OBJECT
				QML_ELEMENT

				D_QT_PROPERTY(Scene::DEntityRef, entity, Entity)
				D_QT_PROPERTY_BINDING(bool, isHitable, IsHitable, entity)
				D_QT_PROPERTY_BINDING(bool, debugBoundaries, DebugBoundaries, entity)
			public:
				explicit QEntity3D(QINode3D* parent = nullptr);

				void setMainShader(QDShader* shader);

			protected:
				virtual void init() override;
			};

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
