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

#include <GFX/PerspectiveLens.h>
#include <Qt/QMLScene/Camera/QCameraLens.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			class QPerspectiveLens : public DGE::QtWrapper::QMLScene::QCameraLens
			{
				D_OBJECT(QPerspectiveLens)

				Q_OBJECT
				QML_ELEMENT

				D_QT_PROPERTY_BASE_TYPE_CONVERTER(GFX::PerspectiveLensRef, cameraLens, CameraLens,
												  GFX::CameraLensRef,
												  std::static_pointer_cast<GFX::PerspectiveLens>)

				D_QT_PROPERTY_BINDING(float, widthScreen, WidthScreen, cameraLens)
				D_QT_PROPERTY_BINDING(float, heightScreen, HeightScreen, cameraLens)
				D_QT_PROPERTY_BINDING(float, zNearPlane, ZNearPlane, cameraLens)
				D_QT_PROPERTY_BINDING(float, zFarPlane, ZFarPlane, cameraLens)
				D_QT_PROPERTY_BINDING(float, fov, Fov, cameraLens)
			public:
				explicit QPerspectiveLens(GFX::PerspectiveLensRef perspectiveLens,
												QObject				   *parent = nullptr);
				explicit QPerspectiveLens(QObject *parent = nullptr);

			private:
				/// \brief Do common initialize tasks of this class constructor
				void commonInitConstructor();
			};
		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
