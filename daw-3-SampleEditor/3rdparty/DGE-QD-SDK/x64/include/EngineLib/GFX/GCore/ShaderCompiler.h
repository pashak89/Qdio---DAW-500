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

#include "GFX/GFXModuleDef.h"
#include <Core/System/DObject.h>
#include <GFX/GCore/Shader.h>
#include <GFX/Resource/ShaderResource.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			class Renderer;
			/// \brief Shader compiler class that compile shader resource and convert it to shader object
			class DGE_GFXAPI ShaderCompiler : public DGE::Core::System::DObject
			{
				D_OBJECT(ShaderCompiler)

			public:
				ShaderCompiler(std::shared_ptr<Renderer> renderer,
							   std::shared_ptr<Core::System::Context>
								   context);
				D_Destructor(ShaderCompiler);

				virtual GFX::GCore::ShaderRef compile(GFX::Resource::ShaderResourceRef shaderResource)
					= 0;
				virtual GFX::GCore::ShaderRef compile(const Resource::ShadersList& shaderCodes) = 0;

				virtual DString getInitCommand(const GFX::GCore::RenderAPIInfo& renderAPIInfo) const
					= 0;

			protected:
				std::shared_ptr<Renderer> m_renderer;
			};
			D_DefRefType(ShaderCompiler)
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
