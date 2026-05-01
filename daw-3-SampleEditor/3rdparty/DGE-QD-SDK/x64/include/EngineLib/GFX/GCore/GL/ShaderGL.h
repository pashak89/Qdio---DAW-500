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

#include <GFX/GCore/GL/RendererGL.h>
#include <GFX/GCore/Shader.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace GL
			{
				struct SourceObject
				{
					DString sourceCode;
					uint32	id;
				};

				struct ProgramObject
				{
					uint32 id;
					bool   isValid = false;
				};

				class DGE_GFXAPI ShaderGL : public GFX::GCore::Shader
				{
					D_OBJECT(ShaderGL)
				public:
					ShaderGL(RendererGLRef renderer, std::shared_ptr<Core::System::Context> context);

					// Shader interface
					virtual bool		  setSourceCode(ShaderType	  shaderType,
														const DString sourceCode) override;
					virtual bool		  setSourceCode(ShaderType	shaderType,
														const char *sourceCode) override;
					virtual const DString sourceCode(ShaderType shaderType) override;
					virtual std::map<ShaderType, DString> sourceCodes() override;

					virtual bool setAttributeLocation(const DString name,
													  uint32_t		location) override;
					virtual bool activate() override;
					virtual bool deactivate() override;
					virtual bool link() override;
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
					uint32 uniformLocation(const DString &name);
					uint32 shaderTypeToShaderTypeGL(ShaderType);

				protected:
					RendererGLRef			  m_renderer;
					SourceObject			  m_shaderObjects[(uint32) ShaderType::TotalNumber];
					std::map<DString, uint32> m_locationCache;
					ProgramObject			  m_program;
					bool					  m_isCompiled;
				};
			} // namespace GL
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
