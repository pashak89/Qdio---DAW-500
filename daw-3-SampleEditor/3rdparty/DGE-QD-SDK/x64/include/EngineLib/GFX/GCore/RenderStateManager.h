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

#include <GFX/GCore/Interfaces/IActivable.h>
#include <GFX/GCore/RenderStateImp.h>
#include <GFX/GFXModuleDef.h>
#include <stack>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			class RenderState;
			class Renderer;
			/// \brief Manage render states in the render pipeline
			class DGE_GFXAPI RenderStateManager : public DGE::Core::System::DCoreObject
			{
			public:
				RenderStateManager(std::shared_ptr<GFX::GCore::Renderer> renderer);

				bool activate(std::shared_ptr<GFX::GCore::RenderStateImp> newState);
				bool deactivate(std::shared_ptr<GFX::GCore::RenderStateImp> newState);

			protected:
				std::stack<std::shared_ptr<RenderStateData>>			m_activatedStates;
				std::shared_ptr<GFX::GCore::RenderStateImp>				m_currentState;
			};
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
