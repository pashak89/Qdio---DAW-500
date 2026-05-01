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

#include "Platform.h"

namespace DGE
{
	namespace Core
	{
		namespace Platforms
		{
			// TODO: Must implement NativePlatform class and related tests
			/// \brief Implementation of platform class for native platform system type that is requested
			class NativePlatform : public DGE::Core::Platforms::Platform
			{
				D_OBJECT(NativePlatform)
			public:
				NativePlatform(DGE::Core::System::ContextRef context);

				D_Destructor(NativePlatform);

				// IInitialize interface
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				// IProcessEvents interface
				virtual bool process() override;

				// Platform interface
				virtual bool  release() override;
				virtual void* createWindow(const WindowSpec& params) override;
				virtual bool  destroyWindow(void* handle) override;

				// DSubsystem interface
			protected:
				virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;
			};

		} // namespace Platforms
	}	  // namespace Core
} // namespace DGE
