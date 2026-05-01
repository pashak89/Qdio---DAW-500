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

#include <Core/CoreModuleDef.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <Core/Utility/Interfaces/IRelease.h>

namespace DGE
{
	namespace Core
	{
		namespace CrashReport
		{
			class CrashReportInterface
				: public Core::Utility::Interfaces::IInitialize
				, public Core::Utility::Interfaces::IRelease
			{
			};

			/// \brief Base class for detecting crash in the app and create the reports
			class DGE_CoreAPI CrashReporter : public CrashReportInterface
			{
			public:
				CrashReporter();
				~CrashReporter();

				// IInitialize interface
				using Core::Utility::Interfaces::IInitialize::initialize;
				virtual bool initialize(Utility::Interfaces::InitializeParams& parameters) override;
				virtual bool release() override;

			protected:
				std::unique_ptr<CrashReportInterface> m_crashReportHandler;
			};
		} // namespace CrashReport
	}	  // namespace Core
} // namespace DGE
