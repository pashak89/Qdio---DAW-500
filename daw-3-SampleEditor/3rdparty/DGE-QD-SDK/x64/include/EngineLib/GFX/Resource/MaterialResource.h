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

#include <Core/Math/Vector3D.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <Core/Serializers/Serialize.h>
#include <Core/System/DObjectMacrosDef.h>
#include <GFX/GFXModuleDef.h>

namespace DGE
{
	namespace GFX
	{
		namespace Resource
		{
			/// \brief Material resource that load from specified source file and is used by the
			/// objects in Engine
			class DGE_GFXAPI MaterialResource : public DGE::Core::ResourceManager::ObjectResource
			{
				D_OBJECT(MaterialResource)
			public:
				MaterialResource(const DString name, const DString path = DL("./"));
				D_Destructor(MaterialResource);
				virtual void unloadResource() override;
				virtual bool reloadResource(const DString& Name, const DString& Path) override;

				bool addVariable(DString name, Serializers::DVariant value);

			protected:
				using ShaderVariables = Core::Serializers::DMapStringVariant;
				D_PROPERTY(DPath, shaderResourcePath, ShaderResourcePath);
				D_PROPERTY(DPath, textureResourcePath, TextureResourcePath);
				D_PROPERTY(Color, color, Color);
				D_PROPERTY(bool, isUsingRandomColoring, IsUsingRandomColoring)
				D_PROPERTY(ShaderVariables, shaderVariables);
			};
			D_DefRefType(MaterialResource)
		} // namespace Resource
	}	  // namespace GFX
} // namespace DGE
