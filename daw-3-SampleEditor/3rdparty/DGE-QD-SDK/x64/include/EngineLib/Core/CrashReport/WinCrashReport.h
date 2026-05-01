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

#include <Core/CrashReport/CrashReport.h>

namespace DGE
{
	namespace Core
	{
		namespace CrashReport
		{

			class DGE_CoreAPI WinCrashReport : public DGE::Core::CrashReport::CrashReportInterface
			{
			public:
				WinCrashReport();

				// IInitialize interface
				virtual bool initialize(Utility::Interfaces::InitializeParams& parameters) override;

				// IRelease interface
				virtual bool release() override;

			protected:
				void* m_lastExceptionFilter = nullptr;
			};

		} // namespace CrashReport
	}	  // namespace Core
} // namespace DGE
