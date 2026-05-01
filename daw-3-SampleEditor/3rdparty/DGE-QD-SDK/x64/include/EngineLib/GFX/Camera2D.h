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
#include "Camera.h"
#include <Core/BaseTypes/BaseTypes.h>

namespace DGE
{
	namespace GFX
	{
#if defined(DGE_UseGraphicAPI)
		/// \brief Base class for working with camera in 2d space
		class DGE_GFXAPI Camera2D : public Camera
		{
		public:
			Camera2D(
				DGE::Core::System::ContextRef context = Core::System::Context::getInstance());
			Camera2D(
				float	  zPosition,
				glm::vec2 position = glm::vec2(0, 0),
				glm::vec2 speed	   = glm::vec2(10, 10),
				DGE::Core::System::ContextRef context = Core::System::Context::getInstance());
			virtual ~Camera2D();
			void		 setPosition(glm::vec2 position);
			glm::vec2	 getPosition();

		protected:
			virtual void updateProjectionMatrix() override;

		protected:
			glm::vec2 m_speed;
		};
#endif
	}
}
