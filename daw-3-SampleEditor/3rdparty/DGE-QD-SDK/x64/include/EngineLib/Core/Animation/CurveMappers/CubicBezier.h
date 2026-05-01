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

#include "Core/BaseTypes/DString.h"
#include "Core/IO/LogSystem/LogMacrosDef.h"
#include <Core/Animation/Transformer.h>
#include <Core/IO/LogSystem/Log.h>
#include <concepts>

namespace DGE
{
	namespace Core
	{
		namespace Animation
		{
			namespace CurveMappers
			{
				/// \brief Check whether Type T can be converted to string or not
				template<typename T>
				concept ConvertableToString = requires(T v) {
					{
						v.toString()
					} -> std::convertible_to<DGE::Core::BaseType::DString>;
				};

				/// \brief Cubic bezier transformer is using in Animation system to control values
				/// of animation steps
				template<typename ValueType>
				class CubicBezier : public ITransformerController<ValueType>
				{
				public:
					CubicBezier(ValueType startTangent, ValueType endTangent)
						: m_startTangent(startTangent)
						, m_endTangent(endTangent)
					{
						m_isFirstTime = true;
					}
					virtual ValueType run(float position, ValueType start, ValueType end) override
					{
						if (m_isFirstTime)
						{
							m_isFirstTime = false;
							//LogDebug(DST "Start value:" + start + " End value:" + end);
						}
						return map(position, start, end, m_startTangent, m_endTangent);
					}

					static ValueType map(float position, ValueType start, ValueType end,
										 ValueType startTangent, ValueType endTangent)
					{
						return bezierMap(start,
										 end,
										 position,
										 start - startTangent,
										 end + endTangent);
					}

					ValueType startTangent() const { return m_startTangent; }
					void	  setStartTangent(ValueType newStartTangent)
					{
						m_startTangent = newStartTangent;
					}

					ValueType endTangent() const { return m_endTangent; }
					void setEndTangent(ValueType newEndTangent) { m_endTangent = newEndTangent; }

				protected:
					template<typename Type>
						requires ConvertableToString<Type>
					static Type bezierMap(Type	 start, // Start value
										  Type	 end,	// Ending value
										  double t,
										  Type	 startTangent, // First control value
										  Type	 endTangent	   // Second control value
										  )					   // Parameter 0 <= t <= 1
					{
						// LogDebug(DST "Start Tangent:" + startTangent.toString());
						// LogDebug(DST "End Tangent:" + endTangent.toString());
						double	  s	  = 1 - t;
						Type	  AB  = start * s + startTangent * t;
						Type	  BC  = startTangent * s + endTangent * t;
						Type	  CD  = endTangent * s + end * t;
						Type	  ABC = AB * s + CD * t;
						Type	  BCD = BC * s + CD * t;
						return ABC * s + BCD * t;
					}
					template<typename Type>
						requires std::floating_point<Type>
					static Type bezierMap(Type	 start, // Start value
										  Type	 end,	// Ending value
										  double t,
										  Type	 startTangent, // First control value
										  Type	 endTangent	   // Second control value
										  )					   // Parameter 0 <= t <= 1
					{
						// LogDebug(DST "Start Tangent:" + startTangent);
						// LogDebug(DST "End Tangent:" + endTangent);
						double s   = 1 - t;
						Type   AB  = start * s + startTangent * t;
						Type   BC  = startTangent * s + endTangent * t;
						Type   CD  = endTangent * s + end * t;
						Type   ABC = AB * s + CD * t;
						Type   BCD = BC * s + CD * t;
						return ABC * s + BCD * t;
					}

					// ITransformerController interface
					virtual vector<vector<DString>> toString() const override 
					{
						return internalToString();
					}

					virtual vector<vector<ValueType>> data() const override
					{
						return {{m_startTangent, m_endTangent}};
					}

					protected:
					template<typename Type=ValueType>
						requires ConvertableToString<Type>
					vector<vector<DString>> internalToString() const
					{
						return {{m_startTangent.toString(), m_endTangent.toString()}};
					}
					template<typename Type = ValueType>
						requires std::floating_point<Type>
					vector<vector<DString>> internalToString() const
					{
						return {{DString(m_startTangent), DString(m_endTangent)}};
					}
				protected:
					ValueType m_startTangent;
					ValueType m_endTangent;
					bool	  m_isFirstTime;
				};

			} // namespace CurveMappers
		}	  // namespace Animation
	}		  // namespace Core
} // namespace DGE
