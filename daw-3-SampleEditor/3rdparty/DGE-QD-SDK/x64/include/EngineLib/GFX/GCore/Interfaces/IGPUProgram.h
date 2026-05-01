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

#include "IActivable.h"
#include <QtCore/qglobal.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace Interfaces
			{
				/// \brief Interface for program that must be execute on GPU
				class IGPUProgram : public DGE::GFX::GCore::Interfaces::IActivable
				{
				public:
					/// \brief Set model matrix in the program
					virtual void setModelMatrix(glm::mat4& modelMatrix) = 0;

					/// \brief Set view matrix in the program. This function can be ignored in
					/// render pipeline.
					virtual void setViewMatrix(glm::mat4& viewMatrix) = 0;

					/// \brief Set projection matrix in the program. This function can be ignored in
					/// render pipeline.
					virtual void setProjectionMatrix(glm::mat4& projectionMatrix) = 0;

					/// \brief Set MVP matrix (model * view * projection) matrix in the program
					virtual void setMVPMatrix(glm::mat4& mvpMatrix) = 0;

					virtual bool setBoolean(const DString name, bool value)				   = 0;
					virtual bool setFloat(const DString name, float value)				   = 0;
					virtual bool setInteger(const DString name, int value)				   = 0;
					virtual bool setVector2(const DString name, const glm::vec2& value)	   = 0;
					virtual bool setVector2(const DString name, float x, float y)		   = 0;
					virtual bool setVector3(const DString name, const glm::vec3& value)	   = 0;
					virtual bool setVector3(const DString name, float x, float y, float z) = 0;
					virtual bool setVector4(const DString name, const glm::vec4& value)	   = 0;
					virtual bool setVector4(const DString name, float x, float y, float z, float w)
						= 0;
					virtual bool setMatrix2(const DString name, const glm::mat2& value) = 0;
					virtual bool setMatrix3(const DString name, const glm::mat3& value) = 0;
					virtual bool setMatrix4(const DString name, const glm::mat4& value) = 0;
				};

			} // namespace Interfaces
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
