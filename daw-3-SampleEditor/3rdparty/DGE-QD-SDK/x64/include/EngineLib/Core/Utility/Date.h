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
#if defined(DGE_Track_Memory) || defined(DGE_UseCRTDebugMemory)
#undef new
#undef delete
#endif
#ifdef DGE_USE_QT
#include <QObject>
#endif
#ifdef DGE_Track_Memory
#define new(param, ...) DBG_NEW(param, DGE_FileCodeInfo, DGE_LineCodeInfo)
#define delete(param, ...) DBG_Delete(param, DGE_FunctionInfo, DGE_FileCodeInfo, DGE_LineCodeInfo)
#elif defined(DGE_UseCRTDebugMemory)
#define _New DBG_NEW
#define new DBG_NEW
#endif

using namespace std;

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			/// \brief Use for converting date
			enum class ConvertType
			{
				NoneConvertJalali,
				NoneConvertMiladi,
				JalaliToMiladi,
				MiladiToJalali,
				Invalid
			};

			/// \brief Name of day of week
			enum class DayOfWeek : uint8
			{
				Saturday,
				Sunday,
				Monday,
				Tuesday,
				Wednesday,
				Thursday,
				Friday,
				WeekDayNum
			};

			/// \brief Use for showing current date type
			enum class DateType
			{
				Invaild,
				Jalali,
				Miladi
			};

			extern uint8 g_numberDayOfMonth[12];

			/// \brief Use for storing date and converting dates together
			class DGE_CoreAPI Date
			{
			public:
				Date();
				Date(const Date& date);
				Date(uint32 year, uint32 month, uint32 day);
				Date(uint32 year, uint32 month, uint32 day, ConvertType convertType);
				Date(uint32 byte, ConvertType convertType);
				Date(uint64 byte, ConvertType convertType);
				Date(string strDate, string delimiter = "/",
					 ConvertType convertType = ConvertType::NoneConvertJalali);
#ifdef DGE_USE_QT
                Date(QDate date, ConvertType convertType);
                Date(QString strDate, QString delimiter = "/",
					 ConvertType convertType = ConvertType::NoneConvertJalali);
#endif

				string toString(string delimiter = "/");
#ifdef DGE_USE_QT
                QString toQString(QString delimiter = "/");
#endif
				Date& operator=(const Date& date);

				bool operator==(Date obj) const;

				bool operator()(Date obj1, Date obj2);
				bool operator()(const Date& obj1, const Date& obj2) const;
				bool operator<(const Date& obj1) const;
				bool operator<=(Date obj1) const;
				bool operator>(Date obj1) const;
				bool operator>=(Date obj1) const;
				bool operator!=(Date obj1) const;

				/// \brief Convert content of this object to Jalali
				void  toJalali();

				/// \brief Convert content of this object to Miladi
				void  toMiladi();
#ifdef DGE_USE_QT
                QDate toQDate(bool convertToMiladi = true);
#endif
				uint32 getDay();
				uint32 getMonth();
				uint32 getYear();

				/// \brief Move forward date by numDays
				Date addDays(int32 numDays);

				/// \brief Move forward date by numMonths
				Date addMonths(int32 numMonths);
				// Date nextDate();

				/// \brief Get is current date is Friday or holiday
				bool   isHoliday();

				///
				/// \brief get comment of this date
				string getEventComment();

				DayOfWeek getDayOfWeek();

				void setDate(uint32 year, uint32 month, uint32 day, ConvertType convertType);
				void setDate(uint32 byte, ConvertType convertType);
				void setDate(uint64 byte, ConvertType convertType);
#ifdef DGE_USE_QT
				void setDate(QDate date, ConvertType convertType);
				void setEventComment(const QString&& comment);
#endif
				void setEventComment(const char* comment);
				void setIsHoliday(bool isHoliday);

				bool isValid(bool canFutureDate = false);

				/// \brief Check if given year is leap
				bool								   isLeap();
				static bool							   isLeap(int year, bool isJalali);
				static DGE::Core::Utility::Date toJalali(uint32 year, uint32 month,
																uint32 day);
#ifdef DGE_USE_QT
                static DGE::Core::Utility::Date toJalali(QDate date);
                static DGE::Core::Utility::Date toMiladi(QString date,
																QString delimiter = "/");
#endif
				static DGE::Core::Utility::Date toMiladi(uint32 year, uint32 month,
																uint32 day);
				static bool isValid(uint32 year, uint32 month, uint32 day, DateType type,
									bool canFutureDate = false);
				static Date now();

				/// \brief Compress date & time in 32 bits
				///
				/// \details compress date in 32 bits in follow format:
				/// 1 bit for Jalali or milady
				/// 8 bit for year (+1357 for Jalali and 1900 for Milady)
				/// 4 bit for month
				/// 5 bit for day
				/// 5 bit for hour
				/// 6 bit for minute
				/// 3 bit for second(0-6!!)10 second precision
				uint32 getCompressDateTime(Time time, bool isJalali);

				/// \brief Compress date in 64 bits
				uint32 getCompressDateTime(bool isJalali);

				/// \brief Compress date & time in 64 bits
				///
				/// \details Compress date & time in 64 bits in follow format:
				/// 1 bit for Jalali or milady
				/// 16 bit for year (+1357 for Jalali and 1900 for Milady)
				/// 4 bit for month
				/// 5 bit for day
				/// 5 bit for hour
				/// 6 bit for minute
				/// 6 bit for second
				/// 10 bit for millisecond
				uint64 getCompressDateTime64(Time time, bool isJalali);

				/// \brief Compress date in 64 bits
				uint64 getCompressDateTime64(bool isJalali);

				//        Date QDateConvertor::Today()
				//        {
				//           set_days();
				//           QStringList Date=  this->ToJalali( today.toString("yyyy"),
				//           today.toString("MM"),today.toString("dd"));
				//           Date<<Day[today.toString("dddd")]; return Date;
				//        }
			private:
				static bool isLeapYear(int y);

			private:
				uint32 m_year;
				uint32 m_month;
				uint32 m_day;

				DateType m_dateType;
				string	 m_eventComment;
				bool	 m_isHoliday;
			};
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
#ifdef DGE_USE_QT
Q_DECLARE_METATYPE(DGE::Core::Utility::Date);
#endif
