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

namespace DGE
{
	namespace SFX
	{
		using namespace DGE::Resource;
		using namespace DGE::Math;
		struct PlayParam;
		//bass class for 3D Sound that use for different API Sound
		class DGE_API AudioPath3D
		{
		#if defined(DGE_UseSoundAPI)
		public:
			virtual ~AudioPath3D(){};

			virtual void SetPosition(Vector3D position)=0;
			virtual void SetVelocity(Vector3D velocity)=0;
			virtual void SetMode(BaseType::uint32 mode)=0;

			virtual void Play(void* segment, bool loop = false, BaseType::uint32 flags = DMUS_SEGF_SECONDARY) = 0;

		private:
		#endif
		};
		
	}
}
