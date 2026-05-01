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
#include <Core/MemoryManager/UndefMemoryMacros.h>

#include "qqmlparserstatus.h"
#include <QVector3D>
#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtQml/QQmlEngine>
#else
#endif

#include <Core/MemoryManager/RedefMemoryMacros.h>

#include <Core/Math/Vector3D.h>
#include <Core/System/DObjectMacrosDef.h>
#include <Core/System/DProperty.h>
#include <GFX/AbstractModel.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			namespace Model
			{
				/// \brief The Base class of source model
				class QDModelSource
					: public QObject
					, public Core::System::DObject
					, public QQmlParserStatus
				{
					Q_OBJECT
					QML_ELEMENT
					Q_INTERFACES(QQmlParserStatus)
					D_OBJECT(QDModelSource)

					D_PROPERTY(GFX::AbstractModelRef, model, Model)

				public:
					enum ModelLoader
					{
						BuiltIn,
						File,
						Memory
					};
					Q_ENUM(ModelLoader);
					enum BuiltInModel
					{
						Line,
						Triangle,
						Cube,
						Capsule,
						Cylinder,
						Sphere,
						ThreeAxes,
						Arrow
					};
					Q_ENUM(BuiltInModel);
					explicit QDModelSource(QObject *parent = nullptr);

					virtual void classBegin() override;
					virtual void componentComplete() override;

				protected:
					virtual void loadModel();
					Core::Math::Vector3D convertVector3D(QVector3D inputValue) const;
					QVector3D			 convertVector3D(Core::Math::Vector3D inputValue) const;

				signals:

				protected:
					ModelLoader			   m_loaderType;
					BuiltInModel		   m_builtInType;
				};
			} // namespace Model
		}	  // namespace QMLScene
	}		  // namespace QtWrapper
} // namespace DGE
