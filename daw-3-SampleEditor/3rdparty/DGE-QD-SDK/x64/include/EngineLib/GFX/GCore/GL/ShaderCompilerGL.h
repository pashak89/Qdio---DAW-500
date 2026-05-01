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

#include <GFX/GCore/ShaderCompiler.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace GL
			{
				class DGE_GFXAPI ShaderCompilerGL : public DGE::GFX::GCore::ShaderCompiler
				{
				public:
					ShaderCompilerGL(std::shared_ptr<Renderer> renderer,
									 std::shared_ptr<Core::System::Context>
										 context);

					// ShaderCompiler interface
					virtual ShaderRef compile(Resource::ShaderResourceRef shaderResource) override;
					virtual ShaderRef compile(const Resource::ShadersList& shaderCodes) override;

					virtual DString getInitCommand(
						const GFX::GCore::RenderAPIInfo& renderAPIInfo) const override;

				protected:
					bool prepareAndCompile(ShaderRef					shaderObject,
										   const Resource::ShadersList& list,
										   DString						glslInitString);
				};

			} // namespace GL
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
