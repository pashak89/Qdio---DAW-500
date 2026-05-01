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
#include <Core/Animation/Transformer.h>
#include <Core/System/DCoreObject.h>
#include <Core/System/DObjectMacrosDef.h>
#include <Core/System/DProperty.h>
#include <Core/System/Event.h>
#include <Core/Utility/AccurateTime.h>

namespace DGE
{
	namespace Core
	{
		namespace Animation
		{
			/// \brief Base class for storing data of a key-frame
			template<typename Type>
			class BaseKeyframe : public System::DCoreObject
			{
				D_CORE_OBJECT(BaseKeyframe, System::DCoreObject);
				BaseKeyframe(const DGE::Core::Animation::BaseKeyframe<Type>& refValue)
					: System::DCoreObject()
				{
					m_frameTime		 = (refValue.m_frameTime);
					m_value			 = (refValue.m_value);
					m_transitionType = (refValue.m_transitionType);
				}

				BaseKeyframe(DGE::Core::Animation::BaseKeyframe<Type>&& refValue)
					: System::DCoreObject()
				{
					m_frameTime		 = (refValue.m_frameTime);
					m_value			 = (refValue.m_value);
					m_transitionType = (refValue.m_transitionType);
				}

				BaseKeyframe& operator=(const BaseKeyframe& rhs) { return *this; }
				BaseKeyframe& operator=(BaseKeyframe&& rhs) { return *this; }

			public:
				BaseKeyframe()
					: BaseKeyframe(0, TransitionType::None)
				{
				}

				BaseKeyframe(Core::Utility::AcTime time, TransitionType transitionType)
					: System::DCoreObject()
				{
					m_frameTime		 = (time);
					m_transitionType = (transitionType);
				}

				BaseKeyframe(Core::Utility::AcTime time, const Type& newValue,
							 TransformerRef<TransitionType> transformer = nullptr)
					: BaseKeyframe(time, newValue, TransitionType::Linear, transformer)
				{
				}

				BaseKeyframe(Core::Utility::AcTime time, const Type& newValue,
							 TransitionType					transitionType,
							 TransformerRef<TransitionType> transformer = nullptr)
					: System::DCoreObject()
				{
					m_frameTime		 = (time);
					m_value			 = (newValue);
					m_transitionType = (transitionType);
					m_transformer	 = transformer;
				}

				virtual ~BaseKeyframe() { LogDebug("Destroyed BaseKeyframe"); }

				bool operator()(const BaseKeyframe& lhs, const BaseKeyframe& rhs) const
				{
					return lhs.m_frameTime.value() < rhs.m_frameTime.value();
				}

				bool operator<(const BaseKeyframe& rhs) const
				{
					return m_frameTime.value() < rhs.m_frameTime.value();
				}

				bool operator>(const BaseKeyframe& rhs) const
				{
					return m_frameTime.value() > rhs.m_frameTime.value();
				}

			protected:
				/// \brief The value of this key-frame
				D_PROPERTY(Type, value, Value);
				/// \brief Store time of this key-frame
				D_PROPERTY(Core::Utility::AcTime, frameTime, FrameTime);
				/// \brief Type of Transition for this key-frame
				D_PROPERTY(TransitionType, transitionType, TransitionType);
				/// \brief Transformer object
				D_PROPERTY(TransformerRef<TransitionType>, transformer, Transformer);
			};

			D_DefRefTypeTemplate(BaseKeyframe)
		} // namespace Animation
	}	  // namespace Core
} // namespace DGE
