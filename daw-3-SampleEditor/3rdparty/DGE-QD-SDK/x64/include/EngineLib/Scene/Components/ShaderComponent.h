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

#include <Core/System/DProperty.h>
#include <GFX/GCore/Shader.h>
#include <Scene/Components/SystemComponent.h>
#include <memory>

namespace DGE
{
	namespace Scene
	{
		class DGE_SceneAPI ShaderComponent
			: public Scene::Component
			, public GFX::GCore::Interfaces::IActivable
		{
			D_OBJECT(ShaderComponent)
		public:
			ShaderComponent(DGE::Core::System::ContextRef context);
			D_Destructor(ShaderComponent);

			// IRenderPipeline interface
			/// \brief Call when object must be initialized.
			using Scene::Component::initialize;

			// IInitialize interface
			bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			bool update(GFX::GCore::InputParameters& input) override;

			// IActivable interface
			virtual bool activate() override;
			virtual bool deactivate() override;

			bool setBoolean(const DString name, bool value);
			bool setFloat(const DString name, float value);
			bool setInteger(const DString name, int value);
			bool setVector2(const DString name, const glm::vec2& value);
			bool setVector2(const DString name, float x, float y);
			bool setVector3(const DString name, const glm::vec3& value);
			bool setVector3(const DString name, float x, float y, float z);
			bool setVector4(const DString name, const glm::vec4& value);
			bool setVector4(const DString name, float x, float y, float z, float w);
			bool setMatrix2(const DString name, const glm::mat2& value);
			bool setMatrix3(const DString name, const glm::mat3& value);
			bool setMatrix4(const DString name, const glm::mat4& value);

			std::shared_ptr<GFX::GCore::Shader> shader() const;
			void setShader(const std::shared_ptr<GFX::GCore::Shader>& newShader);

			bool isReadyForUse();

		protected:
			/// \brief Weak reference to shared pointer of renderer object;
			GFX::GCore::RendererWRef			m_renderer;
			std::shared_ptr<GFX::GCore::Shader> m_shader;
			atomic_bool							m_isReadyForUse;

			/// TODO: must convert to ShaderResource and ShaderResourceLoader
			D_PROPERTY(DString, vertexSourceCode, VertexSourceCode, DST "");
			D_PROPERTY(DString, fragmentSourceCode, FragmentSourceCode, DST "");
		};
		using ShaderComponentRef = std::shared_ptr<DGE::Scene::ShaderComponent>;
		using ShaderComponentWRef = std::weak_ptr<DGE::Scene::ShaderComponent>;
	} // namespace Scene
} // namespace DGE
