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
#include "Core/System/DObjectMacrosDef.h"
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <GFX/GCore/ShaderCompiler.h>
#include <GFX/Resource/ShaderResource.h>
#include <memory>
#include <unordered_map>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			class Renderer;
		}

		using ShadersTable = std::unordered_map<Resource::ShadersList,
												GFX::GCore::ShaderRef,
												Resource::ShadersListHasher>;

		class DGE_GFXAPI ShaderLibrary
			: public DGE::Core::System::DSubsystem
			, public Core::Utility::Singleton<ShaderLibrary>
		{
			D_OBJECT_SINGLETON(ShaderLibrary)
			ShaderLibrary(std::shared_ptr<GCore::Renderer> renderer,
						  std::shared_ptr<Core::System::Context>
							  context);

		public:
			D_Destructor(ShaderLibrary);

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual bool release() override;

			// DSubsystem interface
			virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

			GFX::GCore::ShaderRef generate(GFX::Resource::ShaderResourceRef shaderResource);
			GFX::GCore::ShaderRef generate(const Resource::ShadersList& shaderCodes);

		protected:
			std::shared_ptr<GCore::Renderer> m_renderer;
			GCore::ShaderCompilerRef		 m_shaderCompiler;

			ShadersTable m_shaders;
		};
		D_DefRefType(ShaderLibrary);
	} // namespace GFX
} // namespace DGE
