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

#include "Core/BaseTypes/BaseTypes.h"
#include "Core/System/DObjectMacrosDef.h"
#include <Core/ResourceManager/ResourceLoader.h>
#include <Core/System/DProperty.h>
#include <GFX/GCore/Shader.h>
#include <GFX/GFXModuleDef.h>
#include <array>

namespace DGE
{
	namespace GFX
	{
		namespace Resource
		{
			struct DGE_GFXAPI ShaderFile
			{
				DPath	   path;
				DByteArray sourceCode;
				ShaderFile(DPath fullpath = DST "") { path = fullpath; }
				ShaderFile(DPath fullpath, DByteArray source)
				{
					path	   = fullpath;
					sourceCode = source;
				}

				void setSourceCode(DString sourceCode)
				{
					this->sourceCode = (DByteArray) sourceCode;
				}
				bool operator==(const ShaderFile& object);
			};
			using ShadersList = std::array<ShaderFile, (uint8) GFX::GCore::ShaderType::TotalNumber>;
			using ShaderTypeNames = std::array<DString, (uint8) GFX::GCore::ShaderType::TotalNumber>;
			using ShaderTypeNamesMap = std::map<DString, GFX::GCore::ShaderType>;

			bool operator==(const ShadersList& a, const ShadersList& b);

			struct DGE_GFXAPI ShadersListHasher
			{
				std::size_t operator()(const ShadersList& k) const;
			};

			/// \brief Shader resource that load from specified source file and is used by the
			/// objects in Engine
			class DGE_GFXAPI ShaderResource : public DGE::Core::ResourceManager::ObjectResource
			{
				D_OBJECT(ShaderResource);

			public:
				ShaderResource(const DString name, const DString path = DL("./"));
				D_Destructor(ShaderResource);
				virtual void unloadResource() override;
				virtual bool reloadResource(const DString& Name, const DString& Path) override;

				/// \brief Retrieve the source code of specified shader type
				DByteArray source(GFX::GCore::ShaderType type);
				/// \brief Set shader file data of specified shader type
				void setShaderFile(GFX::GCore::ShaderType type, ShaderFile sourceCode);
				void setShaderFile(GFX::GCore::ShaderType type, DPath path);

				/// \brief Convert shader type into int8 index value
				static int8 shaderTypeToInt8(GFX::GCore::ShaderType type);
				/// \brief Convert uint8 index value into shader type
				static GFX::GCore::ShaderType int8ToShaderType(int8 index);
				static DString				  int8ToStringOfShaderType(int8 index);
				static GFX::GCore::ShaderType stringToShaderType(DString shaderTypeName);
				/// \brief Indicate whether shader type is valid or not
				static bool isValidShaderType(GFX::GCore::ShaderType type);

			protected:
				static ShaderTypeNames s_shaderTypeNames;
				static ShaderTypeNamesMap s_shaderTypeNamesMapper;
				D_PROPERTY(ShadersList, sources, Sources);
			};
			D_DefRefType(ShaderResource)
		} // namespace Resource
	}	  // namespace GFX
} // namespace DGE
