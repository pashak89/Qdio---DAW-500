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

#include <GFX/GCore/Interfaces/IGpuBuffer.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			class Renderer;
			/// \brief Abstract base class for vertex array object
			class VertexArrayObject : public Interfaces::IGpuBuffer
			{
			public:
				// IBaseGObject interface
				virtual GPUObjectType type() override;

				virtual AttributeGObjectDescriptor descriptor() override;
				virtual void setDescriptor(const AttributeGObjectDescriptor& descriptor) override;

			protected:
				AttributeGObjectDescriptor m_descriptor;
			};

			using VertexArrayObjectRef	= std::shared_ptr<GFX::GCore::VertexArrayObject>;
			using VertexArrayObjectWRef = std::weak_ptr<GFX::GCore::VertexArrayObject>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
