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

#include <Qt/QMLScene/Model/QDModelSource.h>
#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtQml/QQmlEngine>
#else
#endif

#include <GFX/Primitives/Cylinder.h>
#include <Qt/QEngineDef.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			namespace Model
			{
				namespace BuiltIn
				{
					/// \brief The Cylinder builtIn object model source
					class QDCylinder : public DGE::QtWrapper::QMLScene::Model::QDModelSource
					{
						Q_OBJECT
						QML_ELEMENT

						D_QT_PROPERTY(GFX::Primitives::CylinderRef, cylinder, Cylinder)
						/// \brief greater radius of cylinder
						D_QT_PROPERTY_BINDING(float, radius1, Radius1, cylinder)
						/// \brief smaller radius of cylinder
						D_QT_PROPERTY_BINDING(float, radius2, Radius2, cylinder)
						/// \brief number of steps for generating circle of cylinder
						D_QT_PROPERTY_BINDING(float, thetaSteps, ThetaSteps, cylinder)
						/// \brief number of steps for generating body of cylinder
						D_QT_PROPERTY_BINDING(float, radiusSteps, RadiusSteps, cylinder)

					public:
						explicit QDCylinder(QObject *parent = nullptr);

					protected:
						virtual void loadModel() override;
					};

				} // namespace BuiltIn
			}	  // namespace Model
		}		  // namespace QMLScene
	}			  // namespace QtWrapper
} // namespace DGE
