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
#include <Core/Utility/Singleton/Singleton.h>

namespace DGE
{
	namespace Core
	{
		namespace Profiling
		{
			class DGE_CoreAPI FPS
			{
			public:
				/// \brief FPS Constructor that initialize its counter
				FPS();

				/// \brief Update inner counter and fps counter
				void update();

				/// \brief Return fps
				///
				/// \return FPS counter value
				float getFPS() const;

			protected:
				float			 m_fps;
				uint32			 m_fpscount;
				OS::PreciseTimer m_fpsinterval;
			};

		} // namespace Profiling
	}	  // namespace Core
} // namespace DGE
