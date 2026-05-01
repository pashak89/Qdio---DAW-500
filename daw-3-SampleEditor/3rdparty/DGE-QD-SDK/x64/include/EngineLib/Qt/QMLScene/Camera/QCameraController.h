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

#include "qobjectdefs.h"
#include <Qt/QMLScene/Camera/QCameraComponent.h>
#include <Qt/QMLScene/QComponent3D.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			class DGE_QtAPI QCameraController : public QMLScene::QComponent3D
			{
				D_OBJECT(QCameraController)
				Q_OBJECT
				QML_ELEMENT
				Q_PROPERTY(bool isEnableRotating READ isEnableRotating WRITE setIsEnableRotating
							   NOTIFY isEnableRotatingChanged FINAL)
			public:
				QCameraController(QNode3D* parent = nullptr);

				virtual QCameraComponent* getCameraComponnent();

				Q_INVOKABLE virtual void rotate(float x, float y);
				Q_INVOKABLE virtual void zoom(float zoomValue);
				Q_INVOKABLE virtual void moveLeftRight(float value);
				Q_INVOKABLE virtual void moveUpDown(float value);

				bool isEnableRotating() const;

			public slots:
				virtual void onMousePressed(QMouseEvent*)		 = 0;
				virtual void onMouseMoved(QMouseEvent* event)	 = 0;
				virtual void onMouseReleased(QMouseEvent* event) = 0;
				virtual void onMouseWheeled(QWheelEvent* event)	 = 0;
				virtual void onKeyPressed(QKeyEvent* event)		 = 0;
				virtual void onKeyReleased(QKeyEvent* event)	 = 0;

				void setIsEnableRotating(bool newIsEnableRotating);

			signals:
				void isEnableRotatingChanged(bool isEnableRotating);

			protected:
				bool m_isEnableRotating;
			};

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
