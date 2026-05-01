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

#include "Time.h"

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{

			class AccurateTime : public DGE::Core::Utility::Time
			{
			public:
				using AcTime = AccurateTime;
				AccurateTime();
				AccurateTime(uint32 hours, uint8 minutes, uint8 seconds, uint16 milliSeconds);
				AccurateTime(uint64 totalMilliSecond);

				AcTime operator+(int16 milliSeconds);

				AcTime operator+(AcTime time);

				AcTime& operator+=(int16 milliSeconds);

				AcTime& operator+=(AcTime time);

				AcTime operator-(AcTime time);

				AcTime& operator-=(AcTime time);

				bool operator>(AcTime time) const;

				bool operator<(AcTime time) const;

				bool operator==(AcTime time) const;

				bool operator!=(AcTime time) const;

				bool operator>=(AcTime time) const;

				bool operator<=(AcTime time) const;

				AcTime& addMilliSeconds(int32 milliSeconds);
				AcTime& minusMicroSeconds(int32 milliSeconds);

				uint16 milliSeconds() const;

				/// \brief Get representation of time in String
				string toString(string delimiter = ":") const;

#ifdef DGE_USE_QT
				/// \brief Get representation of time in QString
				QString toQString(bool hasSeconds = true, QString delimiter = ":") const;
#endif
				/// \brief Convert total milliseconds into AcTime
				AcTime	toAcTime(uint64 totalMilliSecond) const;
				AcTime& setMilliSeconds(uint64 totalMilliSecond);

				/// \brief Static function for converting total milliseconds into AcTime
				static AcTime fromMilliSeconds(uint64 totalMilliSecond);

				/// \brief Get total milliseconds of this Time object
				uint64 toMilliSecond() const;

				/// \brief Set zero Time object
				void setZero();

				/// \brief Check if Time object equal to 0:0:0 return true else return false
				bool isZero() const;

				/// \brief normalize value of the time in this class
				void normalize();

			protected:
				uint16 m_milliSeconds;
			};
			using AcTime = AccurateTime;
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
