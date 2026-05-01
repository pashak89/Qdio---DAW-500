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

#include <GFX/OrthoLens.h>
#include <Qt/QMLScene/Camera/QCameraLens.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			class QOrthoLens : public DGE::QtWrapper::QMLScene::QCameraLens
			{
				D_OBJECT(QOrthoLens);
				Q_OBJECT
				QML_ELEMENT

				D_QT_PROPERTY_BASE_TYPE_CONVERTER(GFX::OrthoLensRef, cameraLens, CameraLens,
												  GFX::CameraLensRef,
												  std::static_pointer_cast<GFX::OrthoLens>)

				D_QT_PROPERTY_BINDING(float, left, Left, cameraLens)
				D_QT_PROPERTY_BINDING(float, right, Right, cameraLens)
				D_QT_PROPERTY_BINDING(float, top, Top, cameraLens)
				D_QT_PROPERTY_BINDING(float, bottom, Bottom, cameraLens)
				D_QT_PROPERTY_BINDING(float, widthScreen, WidthScreen, cameraLens)
				D_QT_PROPERTY_BINDING(float, heightScreen, HeightScreen, cameraLens)
				// D_QT_PROPERTY_BINDING(float, fov, Fov, cameraLens)

			public:
				explicit QOrthoLens(GFX::OrthoLensRef orthoLens, QObject *parent = nullptr);
				explicit QOrthoLens(QObject *parent = nullptr);

			private:
				/// \brief Do common initialize tasks of this class constructor
				void commonInitConstructor();
			};
		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
