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

#include <Core/Math/Matrix.h>
#include <Core/Math/Vector.h>
#include <Core/System/DObjectMacrosDef.h>
#include <Core/System/Event.h>
#include <glm/glm.hpp>

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
			/// \brief Control transformation data
			class DGE_CoreAPI Transformation
			{
			public:
				/// \brief
				Transformation();

				/// \brief Get local position
				const glm::vec3& localPosition() const;
				/// \brief Set local position
				void setLocalPosition(const glm::vec3& newPosition);

				/// \brief
				const glm::vec3& localRotation() const;
				/// \brief
				void setLocalRotation(const glm::vec3& newRotation);

				/// \brief
				const glm::vec3& localScale() const;
				/// \brief
				void setLocalScale(const glm::vec3& newScale);

				/// \brief
				const glm::vec3 worldRight() const;
				/// \brief
				const glm::vec3 worldUp() const;
				/// \brief
				const glm::vec3 worldBackward() const;
				/// \brief
				const glm::vec3 worldForward() const;

				/// \brief
				const glm::vec3 worldPosition() const;
				/// \brief
				const glm::vec3 worldRotation() const;
				/// \brief
				const glm::vec3 worldScale() const;

				/// \brief
				const glm::mat4& transform() const;

				/// \brief
				bool isChanged() const;
				/// \brief
				void setIsChanged(bool newIsChanged);

				/// \brief
				void updateWorldMatrix();
				/// \brief
				void updateWorldMatrix(const glm::mat4& parentWorldMatrix);

				glm::mat4 getLocalMatrix();

				Vector3D pivot() const;
				void	 setPivot(const Vector3D& newPivot);

				bool operator==(const Transformation& transformation) const;
				bool operator==(Transformation&& transformation) const;
				bool operator!=(const Transformation& transformation) const;
				bool operator!=(Transformation&& transformation) const;

				Core::System::Event<void, glm::vec3>& positionChanged() const;

				Core::System::Event<void, glm::vec3>& rotationChanged() const;

				Core::System::Event<void, glm::vec3>& scaleChanged() const;

				Core::System::Event<void, glm::vec3>& moved() const;

				Core::System::Event<void, glm::vec3>& localPositionChanged() const;

				Core::System::Event<void, glm::vec3>& localRotationChanged() const;

				Core::System::Event<void, glm::vec3>& localScaleChanged() const;

				Core::System::Event<void, glm::vec3>& locallyMoved() const;

				Core::System::Event<void>& transformationChanged() const;

			protected:
				/// \brief Check if position,rotation or scale has been changed to emit events
				void checkChangePRS();

			protected:
				/// \brief The local position of entity
				glm::vec3 m_position;

				/// \brief The local rotation of entity
				glm::vec3 m_rotation;
				/// \brief The local scale of entity
				glm::vec3 m_scale;

				/// \brief Store world transform matrix
				glm::mat4 m_transform;

				/// \brief Store previous world transform matrix
				glm::mat4 m_previousTransform;

				/// \brief The previous world rotation of entity
				glm::vec3 m_previousRotation;
				/// \brief The previous world scale of entity
				glm::vec3 m_previousScale;

				/// \brief The pivot of entity
				Vector3D m_pivot;

				/// \brief Indicate the transformation data is changed and must update m_transform matrix
				bool m_isChanged;

				/// \brief Indicate whether It's first time that properties of transformation is updated
				bool m_isFirstTime;

				mutable Core::System::Event<void, glm::vec3> m_positionChanged;
				mutable Core::System::Event<void, glm::vec3> m_rotationChanged;
				mutable Core::System::Event<void, glm::vec3> m_scaleChanged;
				mutable Core::System::Event<void, glm::vec3> m_moved;

				mutable Core::System::Event<void, glm::vec3> m_localPositionChanged;
				mutable Core::System::Event<void, glm::vec3> m_localRotationChanged;
				mutable Core::System::Event<void, glm::vec3> m_localScaleChanged;
				mutable Core::System::Event<void, glm::vec3> m_locallyMoved;

				mutable Core::System::Event<void> m_transformationChanged;
			};

		} // namespace Math
	}	  // namespace Core
} // namespace DGE
