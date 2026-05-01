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

#include <GFX/GCore/AttributeGObjectDescriptor.h>
#include <GFX/GCore/Drawable.h>
#include <GFX/GCore/IndexBuffer.h>
#include <GFX/GCore/Interfaces/IGpuBuffer.h>
#include <GFX/GCore/RenderPipelineDef.h>
#include <GFX/GCore/Renderer.h>
#include <GFX/GCore/VertexArrayObject.h>
#include <GFX/GCore/VertexBuffer.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Attribute of Graphic object in CPU side
			struct AttributeGObject
			{
				GFX::GCore::Interfaces::IGpuBufferRef m_graphicObject;
				/// \brief Store details of graphic object
				AttributeGObjectDescriptor m_descriptor;
			};

			/// A dynamic structure for render any object with graphic pipeline with setting
			/// definition of the ElementAttribute
			class Element : public DGE::GFX::GCore::Drawable
			{
				D_OBJECT(Element)
			public:
				Element(GFX::GCore::RenderPrimitivesType primitiveType
						= RenderPrimitivesType::Triangles);
				Element(GFX::GCore::RenderPrimitivesType		  primitiveType,
						std::vector<AttributeGObjectDescriptor>&& elementAttributesDescriptor);
				Element(GFX::GCore::RenderPrimitivesType			   primitiveType,
						const std::vector<AttributeGObjectDescriptor>& elementAttributesDescriptor,
						GFX::GCore::RendererRef						   renderer);

				/// \brief Add a list of descriptor into this Element class
				bool addAttributes(
					const std::vector<GCore::AttributeGObjectDescriptor>& descriptorArray);

				/// \brief Add a descriptor into this Element class
				bool addAttribute(const GCore::AttributeGObjectDescriptor& descriptor);

				/// \brief Return type of registered buffer that was be added into this element
				/// previously
				///
				/// \return Return nullptr if don't find registered type with the specific type.
				GFX::GCore::Interfaces::IGpuBufferRef buffer(AttributeType type);

				// IInitialize interface
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				// IUpdate interface
				virtual bool update(InputParameters& input) override;

				// IPreProcessRender interface
				virtual bool preProcessRender(InputParameters& input) override;

				// IRender interface
				virtual bool render(InputParameters& input) override;

				// IPostProcessRender interface
				virtual bool postProcessRender(InputParameters& input) override;

				uint32 latestUsageIndex() const;

				GFX::GCore::RenderPrimitivesType primitiveType() const;
				void setPrimitiveType(GFX::GCore::RenderPrimitivesType newPrimitiveType);

				GFX::GCore::IndexBufferRef activeIndexBuffer() const;

				GFX::GCore::VertexBufferRef activeVertexBuffer() const;

				uint64_t totalNumberVertices() override;

				bool setGOBjectBuffer(AttributeType attributeType, DByteArray& newData);
				bool setGOBjectBuffer(AttributeType attributeType, DByteArray&& newData);

				bool isSeparatedDataBuffer() const;
				void setIsSeparatedDataBuffer(bool newIsSeparatedDataBuffer);

			protected:
				/// \brief convert type of base graphic object into attribute type
				static AttributeType convertAttributeType(GPUObjectType type);
				/// \brief convert attribute type into type of base graphic object
				static GPUObjectType convertAttributeType(AttributeType type);
				/// \brief Specified whether an attribute type is a variant of vertex buffer
				static bool isVertexBufferVariant(AttributeType type);
				/// \brief Retrieve the slot usage of specified type of graphic object
				uint32 slotUsageOfAttributeType(AttributeType type);

			protected:
				/// \brief the map of attribute objects and the type of them in the graphic buffer
				std::map<AttributeType, AttributeGObject> m_attributes;
				/// \brief Renderer object that will do basic rendering operation.
				GFX::GCore::RendererRef m_renderer;
				/// \brief Vertex array object
				GFX::GCore::VertexArrayObjectRef m_vao;
				/// \brief Indicate whether this element has vertex array object that usually has it
				bool							 m_hasVAO;
				/// \brief Indicate whether this element has separated data buffer for the vertices
				bool m_isSeparatedDataBuffer;
				/// \brief Latest usage index of graphic slot that is using for this element.
				uint32 m_latestUsageIndex;
				/// \brief Total stride of whole vertex data
				uint32 m_stride;
				/// \brief Specifies kind of primitives to render this element.
				GFX::GCore::RenderPrimitivesType m_primitiveType;
				/// \brief Active index buffer is used for rendering this element. If don't have any
				/// index buffer, use vertex array for rendering this element.
				GFX::GCore::IndexBufferRef m_activeIndexBuffer;
				/// \brief Active vertex buffer is used for rendering this element. If don't specify
				/// any vertex buffer, will be used first vertex buffer for rendering this element.
				GFX::GCore::VertexBufferRef m_activeVertexBuffer;
			};

			using ElementRef  = std::shared_ptr<GFX::GCore::Element>;
			using ElementWRef = std::weak_ptr<GFX::GCore::Element>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
