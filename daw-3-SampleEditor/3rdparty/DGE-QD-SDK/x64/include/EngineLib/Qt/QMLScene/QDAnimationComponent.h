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

#include <Scene/Components/AnimationComponent.h>
#include <Qt/QMLScene/QComponent3D.h>

namespace DGE
{
	namespace QtWrapper
	{
		namespace QMLScene
		{
			/// \brief Animation component for using with a QDNode to animate it
			class QDAnimationComponent : public DGE::QtWrapper::QMLScene::QComponent3D
			{
				D_OBJECT(QDAnimationComponent)
				D_RegisterComponent(QDAnimationComponent, Scene::AnimationComponent);
				Q_PROPERTY(PropertyType propertyType READ propertyType WRITE setPropertyType NOTIFY
							   propertyTypeChanged FINAL)

				Q_OBJECT
				QML_ELEMENT
			public:
				enum PropertyType
				{
					Vec2,
					Vec3,
					Vec4
				};
				Q_ENUM(PropertyType);
				QDAnimationComponent(QNode3D* parent = nullptr);

				PropertyType propertyType() const;

			protected:
				// QComponent3D interface
				virtual void init() override;

			public slots:
				void setPropertyType(const PropertyType& newPropertyType);

			signals:
				void propertyTypeChanged(PropertyType propertyType);

			protected:
				PropertyType m_propertyType;
			};

		} // namespace QMLScene
	}	  // namespace QtWrapper
} // namespace DGE
