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

#include "GFX/Resource/ShaderResource.h"
#include "GFX/ShaderLibrary.h"
#include <Core/System/DProperty.h>
#include <GFX/GCore/Interfaces/IGPUProgram.h>
#include <GFX/GCore/Pass.h>
#include <GFX/GCore/Renderer.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief The base class for implementing render technique
			class Technique
				: public Core::System::DObject
				, public Core::Utility::Interfaces::IInitialize
				, public Interfaces::IGPUProgram
			{
			public:
				Technique(DString name, DGE::Core::System::ContextRef context);
				Technique(GFX::Resource::ShaderResourceRef shaderResource, DString name,
						  DGE::Core::System::ContextRef context);
				D_Destructor(Technique);

				// IInitialize interface
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				// IActivable interface
				virtual bool activate() override;
				virtual bool deactivate() override;

				// IGPUProgram interface
				virtual void setModelMatrix(glm::mat4& modelMatrix) override;
				virtual void setViewMatrix(glm::mat4& viewMatrix) override;
				virtual void setProjectionMatrix(glm::mat4& projectionMatrix) override;
				virtual void setMVPMatrix(glm::mat4& mvpMatrix) override;

				virtual bool setBoolean(const DString name, bool value) override;
				virtual bool setFloat(const DString name, float value) override;
				virtual bool setInteger(const DString name, int value) override;
				virtual bool setVector2(const DString name, const glm::vec2& value) override;
				virtual bool setVector2(const DString name, float x, float y) override;
				virtual bool setVector3(const DString name, const glm::vec3& value) override;
				virtual bool setVector3(const DString name, float x, float y, float z) override;
				virtual bool setVector4(const DString name, const glm::vec4& value) override;
				virtual bool setVector4(const DString name, float x, float y, float z,
										float w) override;
				virtual bool setMatrix2(const DString name, const glm::mat2& value) override;
				virtual bool setMatrix3(const DString name, const glm::mat3& value) override;
				virtual bool setMatrix4(const DString name, const glm::mat4& value) override;

				Resource::ShaderResourceRef shaderResource() const;

			protected:
				/// \brief Name of this technique
				DString m_name;
				/// \brief Shared pointer of shader library object;
				GFX::ShaderLibraryWRef m_shaderLibrary;
				/// \brief The shared pointer to shader object
				GFX::GCore::ShaderRef m_shader;
				/// \brief It's a flag that is indicated to whether this shader object is ready for
				/// using in render pipeline
				atomic_bool m_isReadyForUse;
				/// \brief Shader resource that will be used for loading and creating shader object
				GFX::Resource::ShaderResourceRef m_shaderResource;

				/// \brief Model matrix name in main shader code to update in every frame
				D_PROPERTY(DString, modelMatrixName, ModelMatrixName, DST "");
				/// \brief View matrix name in main shader code to update in every frame
				D_PROPERTY(DString, viewMatrixName, ViewMatrixName, DST "");
				/// \brief Projection matrix  name in main shader code to update in every frame
				D_PROPERTY(DString, projectionMatrixName, ProjectionMatrixName, DST "");
				/// \brief Projection * view * model matrix name in main shader code to
				/// update in every frame
				D_PROPERTY(DString, mvpMatrixName, MVPMatrixName, DST "");

				D_PROPERTY(DString, vertexSourceCode, VertexSourceCode, DST "");
				D_PROPERTY(DString, fragmentSourceCode, FragmentSourceCode, DST "");
			};
			using TechniqueRef	= std::shared_ptr<DGE::GFX::GCore::Technique>;
			using TechniqueWRef = std::weak_ptr<DGE::GFX::GCore::Technique>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
