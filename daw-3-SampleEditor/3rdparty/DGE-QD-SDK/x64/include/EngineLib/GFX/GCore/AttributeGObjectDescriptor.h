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

#include "Core/BaseTypes/BaseTypes.h"
#include "glm/gtx/compatibility.hpp"
#include <GFX/GCore/RenderPipelineDef.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Predefined types of attribute layout for graphic object
			///
			/// \note Character 'D'  at end of every type with a number before of it is indicated to
			/// dimensions of that type.
			/// For example Float3D, has represent array with 3 cells of
			/// type float that is equal to Vector3D!
			enum class AttributeLayoutType
			{
				Float1D,
				Float2D,
				Float3D,
				Float4D,
				Matrix3,
				Matrix4,
				Int1D,
				Int2D,
				Int3D,
				Int4D,
				UInt1D,
				UInt2D,
				UInt3D,
				UInt4D,
				Boolean
			};

			/// \brief A Descriptor class for every attribute of the graphic object that is used to
			/// create related base basic graphic Object
			class AttributeGObjectDescriptor
			{
			public:
				AttributeGObjectDescriptor(AttributeType type = AttributeType::Position);
				AttributeGObjectDescriptor(AttributeType type, uint64 elementCount,
										   DByteArray&& data);
				AttributeGObjectDescriptor(AttributeType type, uint64 elementCount,
										   const DByteArray& data, uint32 location = 0,
										   AttributeValueType valueType = AttributeValueType::Float,
										   bool mustBeNormalized = false, uint32 strideSize = 0,
										   void*		   offsetPointer = nullptr,
										   BufferUsageType usage = BufferUsageType::Static_Draw);

				/// \note If value of location equal to 0 then location value automatically will be set
				AttributeGObjectDescriptor(AttributeLayoutType layout, Core::BaseType::DString name,
										   DByteArray&& data, uint32 location = 0,
										   AttributeType type = AttributeType::Position);
				AttributeGObjectDescriptor(AttributeLayoutType layout, DString name,
										   DByteArray&&	 data,
										   AttributeType type = AttributeType::Position);
				AttributeGObjectDescriptor(AttributeLayoutType layout, DByteArray&& data,
										   AttributeType type = AttributeType::Position);
				AttributeGObjectDescriptor(AttributeLayoutType layout);

				AttributeType type() const;
				void				 setType(AttributeType newType);

				/// \brief The type of the this type of graphic object attribute
				uint64 size() const;

				uint32 location() const;
				void   setLocation(uint32 newLocation);

				DByteArray data() const;
				void	   setData(const DByteArray &newData);

				uint64 elementCount() const;
				void   setElementCount(uint64 newElementCount);

				BufferUsageType usageType() const;
				void			setUsageType(BufferUsageType newUsageType);

				AttributeValueType valueType() const;
				void			   setValueType(AttributeValueType newValueType);

				bool isNormalized() const;
				void setIsNormalized(bool newIsNormalized);

				uint32 strideSize() const;
				void   setStrideSize(uint32 newStrideSize);

				void* offsetPointer() const;
				void  setOffsetPointer(void* newOffsetPointer);

				/// \brief Return number of total vertices in this Attribute descriptor
				uint32 vertexCount() const;

				template<typename VertexType>
				static DByteArray toDByteArray(std::vector<VertexType>&& vertices)
				{
					return toDByteArray(vertices);
				}

				template<typename VertexType>
				static DByteArray toDByteArray(const std::vector<VertexType>& vertices)
				{
					auto	   sizeOfType = sizeof(VertexType);
					DByteArray convertedByteArray;
					auto	   vertecesSize = vertices.size() * sizeOfType;
					convertedByteArray.resize(vertecesSize);
					memcpy(convertedByteArray.data(), (char*) vertices.data(), vertecesSize);
					return convertedByteArray;
				}

				/// \brief Convert Type of attribute layout to type of attribute value
				static AttributeValueType convertLayoutTypeToValueType(
					AttributeLayoutType layoutType);

				/// \brief Convert Type of attribute layout to number of elements
				static uint64 convertLayoutTypeToElementCount(AttributeLayoutType layoutType);

				/// \brief Return size of Type of attribute layout
				static uint64 sizeOfLayoutType(AttributeLayoutType layoutType);

			protected:
				/// \brief The name of this graphic object attribute
				Core::BaseType::DString m_name;
				/// \brief The type of the graphic object attribute
				AttributeType m_type;
				/// \brief The location of the graphic object attribute in graphic buffer
				uint32 m_location;
				/// \brief The size of an element of the graphic object attribute in graphic buffer
				uint64 m_elementCount;
				/// \brief The data of the graphic object attribute in graphic buffer
				DByteArray m_data;
				/// \brief Type of using the graphic object attribute in graphic buffer
				BufferUsageType m_usageType;
				/// \brief type of an element of the graphic object attribute in graphic buffer
				AttributeValueType m_valueType;
				/// \brief Indicate whether the element of the graphic object attribute in graphic
				/// buffer must be normalized
				bool m_isNormalized;
				/// \brief Size of stride of the element of the graphic object attribute in graphic buffer
				uint32 m_strideSize;
				/// \brief The offset pointer of the element of the graphic object attribute in
				/// graphic buffer
				void* m_offsetPointer;
			};

			using AttributeGBDesc = AttributeGObjectDescriptor;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
