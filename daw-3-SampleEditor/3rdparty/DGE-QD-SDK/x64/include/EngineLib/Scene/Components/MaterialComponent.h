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

#include <Core/System/DObjectMacrosDef.h>
#include <Engine/Managers/AssetsManager.h>
#include <GFX/GCore/Interfaces/IGPUProgram.h>
#include <GFX/GCore/Technique.h>
#include <GFX/Resource/MaterialResource.h>

#include <Scene/Component.h>
#include <vector>

namespace DGE
{
	namespace Scene
	{
#define SetTechniqueProp(Technique, PropsList, PropFunc) \
	for (auto prop : PropsList)                          \
		Technique->PropFunc(prop.first, prop.second);    \
	PropsList.clear();

		/// TODO: Must be added a way that every class received changes in the resource that is
		/// using from it
		///
		/// Suggestion:
		/// * Added an interface for User Class of Resources: template<typename
		/// ResourceType>IUserResource
		/// * Added register in AssetsManager to get signal of changing resource to update the user
		/// class
		///

		/// \brief Base class for applying material to object 3d
		class MaterialComponent
			: public DGE::Scene::Component
			, public GFX::GCore::Interfaces::IGPUProgram
		{
			D_OBJECT(MaterialComponent)
		public:
			MaterialComponent(DGE::Core::System::ContextRef context);

			D_Destructor(MaterialComponent);

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

			// IGPUProgram interface
			virtual void setModelMatrix(glm::mat4& modelMatrix) override;
			virtual void setViewMatrix(glm::mat4& viewMatrix) override;
			virtual void setProjectionMatrix(glm::mat4& projectionMatrix) override;
			virtual void setMVPMatrix(glm::mat4& mvpMatrix) override;
			virtual void setColor(Core::BaseType::Color color);

			virtual bool setBoolean(const DString name, bool value) override;
			virtual bool setFloat(const DString name, float value) override;
			virtual bool setInteger(const DString name, int value) override;
			virtual bool setVector2(const DString name, const glm::vec2& value) override;
			virtual bool setVector2(const DString name, float x, float y) override;
			virtual bool setVector3(const DString name, const glm::vec3& value) override;
			virtual bool setVector3(const DString name, float x, float y, float z) override;
			virtual bool setVector4(const DString name, const glm::vec4& value) override;
			virtual bool setVector4(const DString name, float x, float y, float z, float w) override;
			virtual bool setMatrix2(const DString name, const glm::mat2& value) override;
			virtual bool setMatrix3(const DString name, const glm::mat3& value) override;
			virtual bool setMatrix4(const DString name, const glm::mat4& value) override;

			/// \brief Add an existed technique to this material
			bool addTechnique(GFX::GCore::TechniqueRef technique);
			/// \brief Create a technique and add it to this material with specified name and if
			/// useDefaultPropertyNames set to true, use default names of property
			GFX::GCore::TechniqueRef createTechnique(DString name,
													 bool	 useDefaultPropertyNames = true);
			GFX::GCore::TechniqueRef createTechnique(GFX::Resource::ShaderResourceRef shaderResource,
													 DString name,
													 bool	 useDefaultPropertyNames = true);

		protected:
			virtual void updateFromResource(GFX::Resource::MaterialResourceRef resource);

		protected:
			/// \brief Default model matrix name in the technique code to update in every frame
			static DString s_defaultMatrixModelName;
			/// \brief Default view matrix name in the technique code to update in every frame
			static DString s_defaultMatrixViewName;
			/// \brief Default projection matrix  name in the technique code to update in every frame
			static DString s_defaultMatrixProjectionName;
			/// \brief Default projection * view * model matrix name in the technique code to
			/// update in every frame
			static DString s_defaultMatrixMVPName;
			/// \brief Default name of color property in the technique
			static DString s_defaultColorName;

			D_PROPERTY(GFX::Resource::MaterialResourceRef, materialResource, MaterialResource);

			/// \brief List of all technique in this Material
			std::vector<GFX::GCore::TechniqueRef> m_techniques;
			/// \brief List of all uninitialized technique in this Material that must be initialized
			std::vector<GFX::GCore::TechniqueRef> m_uninitializedTechniques;

			/// \brief The weak shared pointer to AssetsManager class.
			Managers::AssetsManagerWRef m_assetsManager;

			glm::mat4 m_modelMatrix;
			glm::mat4 m_viewMatrix;
			glm::mat4 m_projectionMatrix;
			glm::mat4 m_mvpMatrix;
			std::map<DString, bool>	 m_boolProperties;
			std::map<DString, float> m_floatProperties;
			std::map<DString, int>	 m_intProperties;

			std::map<DString, glm::vec2> m_vec2Properties;
			std::map<DString, glm::vec3> m_vec3Properties;
			std::map<DString, glm::vec4> m_vec4Properties;

			std::map<DString, glm::mat2> m_mat2Properties;
			std::map<DString, glm::mat3> m_mat3Properties;
			std::map<DString, glm::mat4> m_mat4Properties;
		};
		D_DefRefType(MaterialComponent);
	} // namespace Scene
} // namespace DGE
