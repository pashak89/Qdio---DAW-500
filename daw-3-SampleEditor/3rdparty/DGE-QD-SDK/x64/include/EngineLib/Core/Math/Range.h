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

#include <Core/BaseTypes/BaseTypes.h>

namespace DGE
{
	namespace Core
	{
		namespace Math
		{
			enum class CompletePart
			{
				None,
				Low,
				High,
				Both
			};

			template<typename Value>
			class Range
			{
			public:
				Range(Value low = 0)
					: m_low(low)
					, m_high(0)
					, m_completePart(CompletePart::Low)
				{
				}
				Range(Value low, Value high, bool isIncomplete)
					: Range(low, high, isIncomplete ? CompletePart::None : CompletePart::Both)
				{
				}
				Range(Value low, Value high, CompletePart completePart = CompletePart::Both)
					: m_low(low)
					, m_high(high)
					, m_completePart(completePart)
				{
				}

				/// \brief Check whether a value is in this range.
				///
				/// \param isExclusiveLowValue determinate is high value of this range belong to
				/// only low value of range or not.
				bool isInRange(Value value, bool isExclusiveLowValue = false) const
				{
					return value >= m_low
						   && ((!isExclusiveLowValue && value == m_high) || value < m_high);
				}

				Value length()
				{
					if (isValid())
					{
						return m_high - m_low;
					}
					return 0;
				}
				// bool operator()(const Range &lhs, const Range &rhs) const
				// {
				//     if (lhs.m_isIncompleted && !rhs.m_isIncompleted)
				//         return lhs.m_high < rhs.m_low;
				// }

				bool operator>(const Value &value) const
				{
					if (!isInRange(value))
						return m_low > value;
					return false;
				}

				bool operator<(const Value &value) const
				{
					if (!isInRange(value))
						return m_low < value;
					return false;
				}

				bool operator>(const Range &rhs) const
				{
					if (!isIncompleted() && rhs.isIncompleted())
					{
						auto validValue = (rhs.m_completePart == CompletePart::High ? rhs.m_high
																					: rhs.m_low);
						if (!isInRange(validValue, true))
						{
							return m_low > validValue;
						}
						else
							return false;
					}
					else if (isIncompleted() && !rhs.isIncompleted())
					{
						auto validValue = (m_completePart == CompletePart::High ? m_high : m_low);
						if (!rhs.isInRange(validValue, true))
						{
							return validValue > rhs.m_low;
						}
						else
							return false;
					}
					else if (isIncompleted() && rhs.isIncompleted())
					{
						return false;
					}
					else
					{
						return m_low > rhs.m_low;
					}
				}

				bool operator<(const Range &rhs) const { return rhs > *this; }

				Value low() const { return m_low; }

				Value high() const { return m_high; }

				void setLow(const Value &newLow) { m_low = newLow; }

				void setHigh(const Value &newHigh) { m_high = newHigh; }

				bool isIncompleted() const { return m_completePart != CompletePart::Both; }
				void setIsIncompleted(bool isIncompleted)
				{
					m_completePart = isIncompleted ? CompletePart::None : CompletePart::Both;
				}

				bool isLowValid() const
				{
					return m_completePart != CompletePart::None
						   && m_completePart != CompletePart::High;
				}

				bool isHighValid() const
				{
					return m_completePart != CompletePart::None
						   && m_completePart != CompletePart::Low;
				}

				CompletePart completePart() const { return m_completePart; }
				void setCompletePart(CompletePart completePart) { m_completePart = completePart; }

				bool isValid() const { return m_low < m_high; }
				void nomalize()
				{
					if (m_high < m_low)
						std::swap(m_high, m_low);
				}

			protected:
				Value m_low;
				Value m_high;
				CompletePart m_completePart;
			};
		} // namespace Math
	}	  // namespace Core
} // namespace DGE
