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

#include <Qt/QMLScene/QComponent3D.h>

#include <Scene/Components/ShaderComponent.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			/// \brief The shader component in QML side
			class QDShader : public DGE::QtWrapper::QMLScene::QComponent3D
			{
				D_OBJECT(QDShader)
				D_RegisterComponent(QDShader, Scene::ShaderComponent);
				D_QT_PROPERTY(DGE::Scene::ShaderComponentRef, shaderObject, ShaderObject)
				D_QT_PROPERTY_BINDING(QString, DString, vertexSourceCode, VertexSourceCode,
									  shaderObject, convertString)
				D_QT_PROPERTY_BINDING(QString, DString, fragmentSourceCode, FragmentSourceCode,
									  shaderObject, convertString)
				Q_OBJECT
				QML_ELEMENT

			public:
				QDShader(QNode3D* parent = nullptr);

			protected:
				DString	convertString(QString inputValue) const;
				QString convertString(DString inputValue) const;

				// QComponent3D interface
				virtual void init() override;
			};

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
