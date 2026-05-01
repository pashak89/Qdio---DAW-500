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
#include "Core/System/DSubsystem.h"
#include <Core/BaseTypes/BaseTypes.h>

#include <Core/IO/LogSystem/Log.h>
#include <Core/Profiling/ProfileFrame.h>
#include <Core/Utility/Interfaces/IInitialize.h>

#ifdef DGE_Track_Memory
#undef new
#undef delete
#endif
#if DGE_DefaultProfiler == DGE_TracyProfiler
#include <tracy/Tracy.hpp>
#endif
#ifdef DGE_Track_Memory
#define new(param, ...) DBG_NEW(param, DGE_FileCodeInfo, DGE_LineCodeInfo)
#define delete(param, ...) DBG_Delete(param, DGE_FunctionInfo, DGE_FileCodeInfo, DGE_LineCodeInfo)
#endif

namespace DGE
{
	namespace Core
	{
		namespace Profiling
		{
			/// \brief Subsystem for profiling in engine.
			class DGE_CoreAPI Profiler
				: public Core::System::DSubsystem
				, public Utility::Singleton<Profiler>
			{
				D_OBJECT_SINGLETON(Profiler)

				Profiler(DGE::Core::System::ContextRef context
						 = DGE::Core::System::Context::getInstance());

			public:
				D_Destructor(Profiler);

				/// IInitialize interface
				///
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
				/// function from IInitialize interface.
				using Core::Utility::Interfaces::IInitialize::initialize;

				/// \brief Initialize Profiler and if it is initialized successfully return true
				///
				/// \return Return true if initializing is successfully else return false
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				virtual bool release() override;

				/// \brief Begin timing a profiling block with specified count parameter
				void beginBlock(const DString&, uint32 count = 1);
				/// \brief End timing the current profiling block
				void endBlock();

				/// \brief Begin the profiling frame
				void beginFrame();
				/// \brief End the profiling frame and calculate all parameters
				void endFrame();
				/// \brief Clear all accumulated data in accumulation period
				void resetAccumulated();

				/// \brief Return current profiling frame object
				std::shared_ptr<ProfileFrame> currentProfile() const;

				/// \brief Return profile statistic data as string
				std::string toString(bool showAccumulatedData = false);

				bool isBegunFrame() const;

				uint64 accumulatedFrames() const;

			protected:
				virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

			private:
				/// \brief Return profile statistic data as string for the specified profile
				std::string getProfilingData(std::shared_ptr<ProfileFrame> profile,
											 uint32 indentNumber, bool showAccumulatedData = false);

			protected:
				/// \brief Default name of root profile that use only for one profile frame
				static std::string s_defaultRootName;
				static uint8	   s_defaultAddedTabNumber;

				/// \brief Current profile frame object that call in our codes
				std::shared_ptr<ProfileFrame> m_currentProfile;
				/// \brief Root node of profiling tree
				std::shared_ptr<ProfileFrame> m_rootProfile;

				/// \brief Specifies whether profiling the frame is begun or not.
				atomic_bool m_isBegunFrame;

				/// \brief Total number of frame in accumulated period
				uint64 m_accumulatedFrames;
				/// \brief Total number of frame in whole profiling time
				uint64 m_totalFrames;

				/// \brief Size of name column without added tabs
				uint8 m_columnNameSize;
				/// \brief Total Tabs added to Name Column
				uint8 m_addedTabNumber;
			};

			class DGE_CoreAPI AutoProfileFrame
			{
			public:
				AutoProfileFrame(const DString& name, uint32 count = 1);
				AutoProfileFrame(const DString& varName, const DString& name,
								 uint32 count = 1);
				~AutoProfileFrame();

			protected:
				std::weak_ptr<Profiler> m_profiler;
			};
#ifdef DGE_EnableProfiler
#if DGE_DefaultProfiler == DGE_TracyProfiler
#define DGE_Profile(name) ZoneScopedN(#name)
#define DGE_ProfileVarNamed(varName, nameString) ZoneNamedN(varName, #varName, true)
#else
#define DGE_Profile(name) DGE::Core::Profiling::AutoProfileFrame profile_##name(#name)
#define DGE_ProfileVarNamed(varName, nameString) \
	DGE::Core::Profiling::AutoProfileFrame profile_##varName(nameString, #varName)
#endif
#else
#define DGE_Profile(name)
#define DGE_ProfileVarNamed(varName, nameString)
#endif
		} // namespace Profiling
	}	  // namespace Core
} // namespace DGE
