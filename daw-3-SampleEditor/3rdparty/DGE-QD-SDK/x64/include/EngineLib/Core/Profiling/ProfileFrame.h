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
#include <Core/OS/Timer.h>

namespace DGE
{
	namespace Core
	{
		namespace Profiling
		{
			class DGE_CoreAPI ProfileFrame
			{
			public:
				using ProfilerChild = std::pair<std::shared_ptr<ProfileFrame>, uint64>;

				ProfileFrame(std::shared_ptr<ProfileFrame> parent, std::string name);
				ProfileFrame(ProfileFrame* parent, std::string name);
				~ProfileFrame();

				/// \brief Begin profiling with specified count parameter
				///
				/// \param count specified number of call
				void begin(uint32 count = 1);

				/// \brief End timing
				void end();

				/// \brief Finish profiling the frame and calculate all parameters
				void endFrame();

				/// \brief Clear all accumulated data in accumulation period
				void resetAccumulated();

				/// \brief Return an existed child profile frame with specified name or create one
				std::shared_ptr<ProfileFrame> getChild(std::string name);

				/// \brief Return name of this profile frame
				std::string name() const;
				/// \brief Set name of this profile frame
				void setName(const std::string& newName);
				/// \brief Reset name of this profile frame to default value (empty value!).
				void resetName();

				/// \brief Return current time of this profile frame
				uint64 currentTime() const;
				/// \brief Set current time of this profile frame.
				void setCurrentTime(uint64 newCurrentTime);
				/// \brief Reset current time of this profile frame to zero
				void resetCurrentTime();

				/// \brief Return number of times this profile was called on current frame
				uint64 calls() const;
				/// \brief Set number of times this profile was called on current frame
				void setCalls(uint64 newCalls);
				/// \brief Reset number of times this profile was called on current frame to zero
				void resetCalls();

				/// \brief Return sum of all samples together in accumulation period
				uint64 accumulatedTime() const;
				/// \brief Set sum of all samples together in accumulation period
				void setAccumulatedTime(uint64 newAccumulatedTime);
				/// \brief Rest sum of all samples together in accumulation period to zero
				void resetAccumulatedTime();

				/// \brief Return number of times this profile was called in accumulation period
				uint64 accumulatedCalls() const;
				/// \brief Set number of times this profile was called in accumulation period
				void setAccumulatedCalls(uint64 newAccumulatedCalls);
				/// \brief Reset number of times this profile was called in accumulation period to zero
				void resetAccumulatedCalls();

				/// \brief Return parent of this profile frame
				std::shared_ptr<ProfileFrame> parent() const;
				/// \brief Set parent of this profile frame
				void setParent(const std::shared_ptr<ProfileFrame>& newParent);
				/// \brief Reset parent of this profile frame to no parent
				void resetParent();

				/// \brief Return time of this profile frame on the previous frame
				uint64 previousTime() const;
				/// \brief Set time of this profile frame on the previous frame
				void setPreviousTime(uint64 newPreviousTime);
				/// \brief Reset time of this profile frame on the previous frame to zero
				void resetPreviousTime();

				/// \brief Return number of times this profile was called on the previous frame
				uint64 previousCalls() const;
				/// \brief Set number of times this profile was called on the previous frame
				void setPreviousCalls(uint64 newPreviousCalls);
				/// \brief Reset number of times this profile was called on the previous frame to zero
				void resetPreviousCalls();

				/// \brief Return total time of this profile frame
				uint64 totalTime() const;
				/// \brief Set total time of this profile frame
				void setTotalTime(uint64 newTotalTime);
				/// \brief Reset total time of this profile frame to zero
				void resetTotalTime();

				/// \brief Return number of times this profile was called
				uint64 totalCalls() const;
				/// \brief Set number of times this profile was called
				void setTotalCalls(uint64 newTotalCalls);
				/// \brief Reset number of times this profile was called to zero
				void resetTotalCalls();

				/// \brief Return children of this profile frame
				std::unordered_map<string, ProfilerChild> childern() const;
				/// \brief Set children of this profile frame
				void setChildern(const std::unordered_map<string, ProfilerChild>& newChildern);
				/// \brief Reset children of this profile frame to zero
				void resetChildern();

				/// \brief Indicate whether this class is root.
				bool isRoot();

			protected:
				/// \brief The name of the profile.
				std::string m_name;
				/// \brief The precise timer for measuring the block duration
				DGE::Core::OS::PreciseTimer m_timer;
				/// \brief The time on current frame
				uint64 m_currentTime;
				/// \brief The number of times this profile was called on current frame
				uint64 m_calls;

				/// \brief Sum of all samples together in accumulation period
				uint64 m_accumulatedTime;
				/// \brief The number of times this profile was called in accumulation period
				uint64 m_accumulatedCalls;

				/// \brief The time on previous frame
				uint64 m_previousTime;
				/// \brief The number of times this profile was called on previous frame
				uint64 m_previousCalls;

				/// \brief Sum of all samples together
				uint64 m_totalTime;
				/// \brief The total number of times this profile was called
				uint64 m_totalCalls;

				/// \brief The parent of this profile frame
				std::shared_ptr<ProfileFrame> m_parent;
				/// \brief The children of this profile frame
				std::unordered_map<std::string, ProfilerChild> m_childern;
			};
		} // namespace Profiling
	}	  // namespace Core
} // namespace DGE
