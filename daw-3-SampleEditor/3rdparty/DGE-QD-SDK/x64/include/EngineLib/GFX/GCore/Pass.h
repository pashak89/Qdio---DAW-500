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

#include <Core/System/DProperty.h>
#include <GFX/GCore/Renderer.h>
#include <GFX/GCore/Shader.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Base class for storing data about a render phase of everything in the render pipeline
			class Pass
				: public Core::System::DObject
				, public Core::Utility::Interfaces::IInitialize
				, public Interfaces::IActivable
			{
				D_OBJECT(Pass)
			public:
				Pass(DString name, std::shared_ptr<GFX::GCore::Renderer> renderer,
					 DGE::Core::System::ContextRef context);
				D_Destructor(Pass);

				// IInitialize interface
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				// IActivable interface
				virtual bool activate() override;
				virtual bool deactivate() override;

			protected:
				/// \brief Name of this technique
				DString m_name;
				/// \brief Shared pointer of renderer object;
				std::shared_ptr<GFX::GCore::Renderer> m_renderer;
			};
			using PassRef  = std::shared_ptr<DGE::GFX::GCore::Pass>;
			using PassWRef = std::weak_ptr<DGE::GFX::GCore::Pass>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
