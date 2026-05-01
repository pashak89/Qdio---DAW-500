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

#include "../BaseTypes/BaseTypes.h"
#include "../MemoryManager/MemoryManager.h"

namespace DGE
{
	namespace Core
	{
		namespace OS
		{
			/// \brief class foe measure time
			class PreciseTimer
			{
#ifdef WIN32
			public:
				LARGE_INTEGER m_StartingTime, m_EndingTime, m_Frequency;
				uint64_t	  getFrequency() const
				{
					return m_Frequency.QuadPart;
				}
				static uint64_t getFrequencyCPU()
				{
					LARGE_INTEGER Frequency;
					QueryPerformanceFrequency(&Frequency);
					return Frequency.QuadPart;
				}
				static uint64_t getCurrentTickCount()
				{
					LARGE_INTEGER tick;
					QueryPerformanceCounter(&tick);
					return tick.QuadPart;
				}
				PreciseTimer()
				{
					QueryPerformanceFrequency(&m_Frequency);
					QueryPerformanceCounter(&m_StartingTime);
				}
				// Start Timer
				void start()
				{
					QueryPerformanceFrequency(&m_Frequency);
					QueryPerformanceCounter(&m_StartingTime);
				}

				// Get Time Between Start & Now.
				uint64_t getTime()
				{
					QueryPerformanceFrequency(&m_Frequency);
					QueryPerformanceCounter(&m_EndingTime);
					return (m_EndingTime.QuadPart - m_StartingTime.QuadPart) * 1000.0
						   / m_Frequency.QuadPart;
				}

				// Get Time Between Start & Now.
				template<class typeTime>
				typeTime getTime()
				{
					QueryPerformanceFrequency(&m_Frequency);
					QueryPerformanceCounter(&m_EndingTime);
					auto micro = chrono::microseconds(
						(m_EndingTime.QuadPart - m_StartingTime.QuadPart) * 1000000
						/ m_Frequency.QuadPart);
					return duration_cast<typeTime>(micro);
				}

				// Get Time Between Start To Now & Update Timer to Now.
				uint64_t getTimeUpdate()
				{
					QueryPerformanceFrequency(&m_Frequency);
					QueryPerformanceCounter(&m_EndingTime);
					auto mili = (m_EndingTime.QuadPart - m_StartingTime.QuadPart) * 1000.0
								/ m_Frequency.QuadPart;
					QueryPerformanceCounter(&m_StartingTime);
					return mili;
				}

				// Get Time Between Start To Now & Update Timer to Now.
				template<class typeTime>
				typeTime getTimeAndUpdate()
				{
					QueryPerformanceFrequency(&m_Frequency);
					QueryPerformanceCounter(&m_EndingTime);
					auto micro = chrono::microseconds(
						(m_EndingTime.QuadPart - m_StartingTime.QuadPart) * 1000000
						/ m_Frequency.QuadPart);
					QueryPerformanceCounter(&m_StartingTime);
					return duration_cast<typeTime>(micro);
				}
				uint64 getTimeAndUpdate()
				{
					QueryPerformanceFrequency(&m_Frequency);
					QueryPerformanceCounter(&m_EndingTime);
					auto micro = chrono::microseconds(
						(m_EndingTime.QuadPart - m_StartingTime.QuadPart) * 1000000
						/ m_Frequency.QuadPart);
					QueryPerformanceCounter(&m_StartingTime);
					return duration_cast<milliseconds>(micro).count();
				}

#else

				high_resolution_clock::time_point m_start;

			public:
				PreciseTimer()
				{
					m_start = high_resolution_clock::time_point();
				}
				// Start Timer
				void start()
				{
					m_start = high_resolution_clock::now();
				}

				// Get Time Between Start & Now.
				uint64_t getTime()
				{
					return duration_cast<milliseconds>(high_resolution_clock::now() - m_start)
						.count();
				}

				// Get Time Between Start & Now.
				template<class typeTime>
				typeTime getTime()
				{
					return duration_cast<typeTime>(high_resolution_clock::now() - m_start);
				}

				// Get Time Between Start To Now & Update Timer to Now.
				uint64_t getTimeAndUpdate()
				{
					uint64_t t = duration_cast<milliseconds>(high_resolution_clock::now() - m_start)
									 .count();
					m_start = high_resolution_clock::now();
					return t;
				}

				// Get Time Between Start To Now & Update Timer to Now.
				template<class typeTime>
				typeTime getTimeAndUpdate()
				{
					typeTime t = duration_cast<typeTime>(high_resolution_clock::now() - m_start);
					m_start	   = high_resolution_clock::now();
					return t;
				}
#endif
			};
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
