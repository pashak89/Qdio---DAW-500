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

#include <GFX/GCore/Shader.h>

class QOpenGLShaderProgram;

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace QtWrapper
			{
#if defined(DGE_USE_QT)
				/// \brief Use this class for working with shader. It use of Qt backend for shader
				class DGE_GFXAPI QtShader : public Shader
				{
					D_OBJECT(QtShader)
				public:
					QtShader(std::shared_ptr<Core::System::Context> context);
					D_Destructor(QtShader);

					// Shader interface
					/// \brief do activate shader for rendering
					virtual bool activate() override;

					virtual bool setAttributeLocation(const DString name,
													   uint32_t		location) override;

					/// \brief do compiling and linking shader codes
					virtual bool link() override;

					/// \brief set source code \param sourceCode for specific shader type \param shaderType
					virtual bool						 setSourceCode(ShaderType  shaderType,
																	   const char *sourceCode) override;
					virtual bool						 setSourceCode(ShaderType	shaderType,
																	   const DString sourceCode) override;
					virtual const DString				 sourceCode(ShaderType shaderType) override;
					virtual std::map<ShaderType, DString> sourceCodes() override;

					/// \brief deactivate resource of this shader
					virtual bool deactivate() override;

					// Shader interface
					virtual bool setBoolean(const DString name, bool value) override;
					virtual bool setFloat(const DString name, float value) override;
					virtual bool setInteger(const DString name, int value) override;
					virtual bool setVector2(const DString name, const glm::vec2 &value) override;
					virtual bool setVector2(const DString name, float x, float y) override;
					virtual bool setVector3(const DString name, const glm::vec3 &value) override;
					virtual bool setVector3(const DString name, float x, float y, float z) override;
					virtual bool setVector4(const DString name, const glm::vec4 &value) override;
					virtual bool setVector4(const DString name, float x, float y, float z,
											float w) override;
					virtual bool setMatrix2(const DString name, const glm::mat2 &value) override;
					virtual bool setMatrix3(const DString name, const glm::mat3 &value) override;
					virtual bool setMatrix4(const DString name, const glm::mat4 &value) override;

				protected:
					std::map<GFX::GCore::ShaderType, DString> m_shaderSourceCode;
					std::shared_ptr<QOpenGLShaderProgram>	 m_program;
				};
#endif
			} // namespace QtWrapper
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
