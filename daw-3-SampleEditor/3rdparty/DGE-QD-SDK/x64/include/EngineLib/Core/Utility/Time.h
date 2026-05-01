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
#include <Core/MemoryManager/MemoryManager.h>

#ifdef DGE_USE_QT
#include <Core/MemoryManager/UndefMemoryMacros.h>

#include <QStringList>
#include <QTime>

#include <Core/MemoryManager/RedefMemoryMacros.h>
#endif

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			/// \brief Use for storing time and working with it
			class DGE_CoreAPI Time
			{
			public:
				Time();
				Time(uint32 hours, uint8 minutes, uint8 seconds);
				Time(uint32 byte);
#ifdef DGE_USE_QT
				Time(QTime time);
				Time(QString str, QString splitter = ":");
#endif
				Time operator+(int32 seconds);

				Time operator+(Time time);

				Time& operator+=(int32 seconds);

				Time& operator+=(Time time);

				Time operator-(Time time);

				Time& operator-=(Time time);

				bool operator>(Time time) const;

				bool operator<(Time time) const;

				bool operator==(Time time) const;

				bool operator!=(Time time) const;

				bool operator>=(Time time) const;

				bool operator<=(Time time) const;

				uint8  seconds() const;
				uint8  minutes() const;
				uint32 hours() const;

				Time& addSeconds(int32 seconds);
				Time& minusSeconds(int32 seconds);

				/// \brief Get representation of time in String
				string toString(string delimiter = ":") const;

#ifdef DGE_USE_QT
				/// \brief Get representation of time in QString
				QString toQString(bool hasSeconds = true, QString delimiter = ":") const;

				/// \brief Converting Time into QTime
				QTime toQTime() const;
#endif
				/// \brief Convert total seconds into Time
				Time toTime(uint32 totalSecond) const;

				/// \brief Static function for converting total seconds into Time
				static Time fromSeconds(uint32 totalSecond);

				/// \brief Get total second of this Time object
				uint32 toSecond() const;

				/// \brief Set zero Time object
				void setZero();

				/// \brief Check if Time object equal to 0:0:0 return true else return false
				bool isZero() const;
				
				/// \brief normalize value of the time in this class
				void normalize();

				/// \brief Static function for getting current time
				static Time now();
				
			protected:
				uint8  m_seconds;
				uint8  m_minutes;
				uint32 m_hours;				
			};
			// #pragma pack(pop)
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
