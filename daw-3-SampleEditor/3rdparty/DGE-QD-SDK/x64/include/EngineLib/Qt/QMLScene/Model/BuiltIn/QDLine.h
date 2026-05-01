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

#include <QVector3D>
#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtQml/QQmlEngine>
#else
#endif
#include <Qt/QMLScene/Model/QDModelSource.h>

#include <GFX/Primitives/Line.h>
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
					/// \brief The Line builtIn object model source
					class QDLine : public DGE::QtWrapper::QMLScene::Model::QDModelSource
					{
						Q_OBJECT
						QML_ELEMENT

						D_QT_PROPERTY(GFX::Primitives::LineRef, line, Line)
						D_QT_PROPERTY_BINDING(QVector3D, Core::Math::Vector3D, point1, Point1, line,
											  convertVector3D)
						D_QT_PROPERTY_BINDING(QVector3D, Core::Math::Vector3D, point2, Point2, line,
											  convertVector3D)
					public:
						explicit QDLine(QObject *parent = nullptr);

						// QDModelSource interface
					protected:
						virtual void loadModel() override;
					};

				} // namespace BuiltIn
			}	  // namespace Model
		}		  // namespace QMLScene
	}			  // namespace QtWrapper
} // namespace DGE
