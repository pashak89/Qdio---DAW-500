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
#include <Core/MemoryManager/UndefMemoryMacros.h>
#include <QQuickItem>
#include <QtCore/QObject>

#include <Qt/QEngineDef.h>

#include <Core/MemoryManager/RedefMemoryMacros.h>

#include <GFX/CameraLens.h>
#include <Core/System/DObject.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			/// \brief A QML type for assign camera lens into QCameraComponent
			class QCameraLens
				: public QObject
				, public Core::System::DObject
			{
				D_OBJECT(QCameraLens)

				Q_OBJECT
				QML_ELEMENT
			public:
				explicit QCameraLens(GFX::CameraLensRef cameraLens, QObject* parent = nullptr);
				explicit QCameraLens(QObject* parent = nullptr);

				virtual GFX::CameraLensRef cameraLens() const;
				virtual void			   setCameraLens(const GFX::CameraLensRef& newCameraLens);

			signals:
			};
		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
