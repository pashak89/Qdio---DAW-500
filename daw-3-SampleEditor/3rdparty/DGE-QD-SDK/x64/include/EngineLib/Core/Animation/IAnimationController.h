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
#include <Core/Animation/BaseKeyframe.h>
#include <Core/Utility/Interfaces/IInitialize.h>

namespace DGE
{
	namespace Core
	{
		namespace Animation
		{
			class IAnimationController
			{
			public:
				/// \brief Go to next frame with specified elapsed time
				virtual bool nextFrame(uint64 elapsedTime) = 0;
				/// \brief Go to previous frame with specified elapsed time
				virtual bool previousFrame(uint64 elapsedTime) = 0;
				/// \brief Go to special frame with specified frame time
				virtual bool seekToFrame(uint64 frameTime) = 0;

				/// \brief Indicate whether current animation is finished
				virtual bool isFinsihed() const = 0;

				/// \brief Retrieve current time of animation.
				virtual uint64 currentTime() const = 0;
			};

			using IAnimationControllerWRef = std::weak_ptr<IAnimationController>;
			using IAnimationControllerRef  = std::shared_ptr<IAnimationController>;

		} // namespace Animation
	}	  // namespace Core
} // namespace DGE
