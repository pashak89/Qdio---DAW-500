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

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/System/Event.h>

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			template<typename EnumType, typename Value>
			class Transition
			{
			public:
				Transition(EnumType state)
					: m_currentState(state)
				{
				}
				Transition(EnumType state, Value newValue)
					: m_currentState(state)
					, m_newValue(newValue)
				{
				}
				Transition(EnumType state, Value newValue, EnumType nextState)
					: m_currentState(state)
					, m_newValue(newValue)
					, m_nextState(nextState)
				{
				}
				EnumType currentState() const { return m_currentState; }
				void	 setCurrentState(const EnumType& newCurrentState)
				{
					m_currentState = newCurrentState;
				}

				Value newValue() const { return m_newValue; }
				void  setNewValue(const Value& newNewValue) { m_newValue = newNewValue; }

				EnumType nextState() const { return m_nextState; }
				void	 setNextState(const EnumType& newNextState) { m_nextState = newNextState; }

				bool operator()(const Transition& lhs, const Transition& rhs) const
				{
					return (lhs.m_currentState == rhs.m_currentState)
							   ? (lhs.m_newValue < rhs.m_newValue)
							   : (lhs.m_currentState < rhs.m_currentState);
				}

			protected:
				EnumType m_currentState;
				Value	 m_newValue;
				EnumType m_nextState;
			};

			template<typename EnumType, typename Value>
			class StateMachine
			{
			public:
				StateMachine(EnumType initialValue = 0) { m_currentState = initialValue; }
				bool addTransition(EnumType state, Value newValue, EnumType nextState)
				{
					auto [it, result] = m_statesMap.insert(
						{Transition<EnumType, Value>(state, newValue, nextState), nextState});
					return result;
				}

				bool removeTransition(EnumType state, Value newValue, EnumType nextState)
				{
					return m_statesMap.erase(Transition<EnumType, Value>(state, newValue, nextState))
						   == 1;
				}

				bool transite(Value newValue)
				{
					auto transition = Transition<EnumType, Value>(m_currentState, newValue);
					auto it			= m_statesMap.find(transition);
					if (it != m_statesMap.end())
					{
						m_currentState = it.first.nextState();
						transition.setNextState(m_currentState);
						m_stateChanged(m_currentState, transition);
						return true;
					}
					else
						return false;
				}

				Core::System::Event<void, EnumType, Transition<EnumType, Value>>& stateChanged() const
				{
					return m_stateChanged;
				}

			protected:
				EnumType										m_currentState;
				std::map<Transition<EnumType, Value>, EnumType> m_statesMap;
				/// \brief State changed event
				///
				/// \param nextState value of next state
				/// \param transition transition value
				mutable Core::System::Event<void, EnumType, Transition<EnumType, Value>>
					m_stateChanged;
			};
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
