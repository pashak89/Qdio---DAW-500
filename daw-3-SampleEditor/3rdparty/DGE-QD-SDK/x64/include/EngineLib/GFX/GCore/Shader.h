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

#include <Core/System/DSubsystem.h>
#include <GFX/GCore/Interfaces/IActivable.h>
#include <GFX/GFXModuleDef.h>
#include <glm/glm.hpp>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			enum class DGE_GFXAPI ShaderType : uint8
			{
				Vertex				   = 0x0001,
				Fragment			   = 0x0002,
				Geometry			   = 0x0004,
				TessellationControl	   = 0x0008,
				TessellationEvaluation = 0x0010,
				Compute				   = 0x0020,

				FirstShader = 0x0000,
				TotalNumber = 0x0006
			};

			/// \brief Contain information about the attribute that we want to set in shader
			struct AttributePointerInfo
			{
				void* pointerData;
			};

			/// \brief base class for shader codes
			class DGE_GFXAPI Shader
				: public Core::System::DObject
				, public Interfaces::IActivable
			{
				D_OBJECT(Shader)
			public:
				Shader(std::shared_ptr<Core::System::Context> context);

				virtual bool setSourceCode(ShaderType shaderType, const DString sourceCode) = 0;
				virtual bool setSourceCode(ShaderType shaderType, const char* sourceCode)  = 0;
				virtual const DString				  sourceCode(ShaderType shaderType)		= 0;
				virtual std::map<ShaderType, DString> sourceCodes()							= 0;
				virtual bool setAttributeLocation(const DString name, uint32_t location)	= 0;
				// virtual bool setAttributePointer(const String name,)
				using IActivable::activate;
				using IActivable::deactivate;
				virtual bool link()	   = 0;

				virtual bool setBoolean(const DString name, bool value)							= 0;
				virtual bool setFloat(const DString name, float value)							= 0;
				virtual bool setInteger(const DString name, int value)							= 0;
				virtual bool setVector2(const DString name, const glm::vec2& value)				= 0;
				virtual bool setVector2(const DString name, float x, float y)					= 0;
				virtual bool setVector3(const DString name, const glm::vec3& value)				= 0;
				virtual bool setVector3(const DString name, float x, float y, float z)			= 0;
				virtual bool setVector4(const DString name, const glm::vec4& value)				= 0;
				virtual bool setVector4(const DString name, float x, float y, float z, float w) = 0;
				virtual bool setMatrix2(const DString name, const glm::mat2& value)				= 0;
				virtual bool setMatrix3(const DString name, const glm::mat3& value)				= 0;
				virtual bool setMatrix4(const DString name, const glm::mat4& value)				= 0;
			};
			using ShaderRef	 = std::shared_ptr<GFX::GCore::Shader>;
			using ShaderWRef = std::weak_ptr<GFX::GCore::Shader>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
