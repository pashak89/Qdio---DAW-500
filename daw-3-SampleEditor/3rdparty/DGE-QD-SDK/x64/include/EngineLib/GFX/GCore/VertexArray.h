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

#include <Core/IO/LogSystem/Log.h>
#include <Core/System/DObject.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <GFX/GCore/Interfaces/IRenderPipeline.h>
#include <GFX/GCore/RenderPipelineDef.h>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// An abstract base class for Vertex array object
			class DGE_GFXAPI VertexArray
				: public Core::System::DObject
				, public Core::Utility::Interfaces::IInitialize
				, public Interfaces::IRenderPipeline
			{
				D_OBJECT(VertexArray)
			public:
				/// \brief Default constructor
				///
				/// \details Creates an empty vertex array
				VertexArray();

				/// \brief Construct the vertex array with the given type
				///
				/// \param type Type of primitives
				VertexArray(GFX::GCore::RenderPrimitivesType type);

				// IInitialize interface
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				/// \brief Get total number of vertex.
				///
				/// \return Number of vertices in the array
				std::size_t getVertextCount();

				/// \brief Get a read-only access to the vertex array by its index,
				///
				/// \details This function return the vertex at specified index. Be aware that this
				/// function doesn't check index and must be in range [0, getVertextCount()-1].
				///
				/// \param index Index of the vertex in the vertex array to get
				///
				/// \return Reference to the vertex that place at index position in the vertex array
				template<typename VertexType>
				VertexType operator[](std::size_t index)
				{
					if (m_sizeOfType == 0)
						m_sizeOfType = sizeof(VertexType);
					else if (m_sizeOfType != sizeof(VertexType))
						LogError("Access to vertex array with diffrent type.");
					return *(VertexType*) m_vertexData[index * m_sizeOfType];
				}

				/// \brief Get a read-only access to the vertex array by its index,
				///
				/// \details This function return the vertex at specified index. Be aware that this
				/// function doesn't check index and must be in range [0, getVertextCount()-1].
				///
				/// \param index Index of the vertex in the vertex array to get
				///
				/// \return Reference to the vertex that place at index position in the vertex array
				template<typename VertexType>
				const VertexType operator[](std::size_t index) const
				{
					if (m_sizeOfType == 0)
						m_sizeOfType = sizeof(VertexType);
					else if (m_sizeOfType != sizeof(VertexType))
						LogError("Access to vertex array with diffrent type.");
					return *(VertexType*) m_vertexData[index * m_sizeOfType];
				}

				/// \brief Add a vertex to the array.
				///
				/// \param vertex The vertex that will added to the vertex array
				template<typename VertexType>
				void append(const VertexType& vertex)
				{
					if (m_sizeOfType == 0)
						m_sizeOfType = sizeof(VertexType);
					else if (m_sizeOfType != sizeof(VertexType))
						LogError("Access to vertex array with diffrent type.");
					auto lastElement = m_vertexData.size();
					m_vertexData.resize(lastElement + m_sizeOfType);
					memcpy(m_vertexData.data() + lastElement, (char*) &vertex, sizeof(VertexType));
					syncGBuffer();
				}

				/// \brief Add a list of vertex to the array.
				///
				/// \param verteces The list of vertex that will added to the vertex array
				template<typename VertexType>
				void append(const std::vector<VertexType>& verteces)
				{
					if (m_sizeOfType == 0)
						m_sizeOfType = sizeof(VertexType);
					else if (m_sizeOfType != sizeof(VertexType))
						LogError("Access to vertex array with diffrent type.");
					auto lastElement  = m_vertexData.size();
					auto vertecesSize = verteces.size() * m_sizeOfType;
					m_vertexData.resize(lastElement + vertecesSize);
					memcpy(m_vertexData.data() + lastElement, (char*) verteces.data(), vertecesSize);
					syncGBuffer();
				}

				template<typename VertexType>
				void setData(const std::vector<VertexType>& vertices)
				{
					if (m_sizeOfType == 0)
						m_sizeOfType = sizeof(VertexType);
					else if (m_sizeOfType != sizeof(VertexType))
						LogError("Access to vertex array with diffrent type.");
					m_vertexData.clear();
					auto vertecesSize = vertices.size() * m_sizeOfType;
					m_vertexData.resize(vertecesSize);
					memcpy(m_vertexData.data(), (char*) vertices.data(), vertecesSize);
					syncGBuffer();
				}

				/// \brief Clear the vertex array
				///
				/// \details This function removes all the vertices from the array.
				void clear();

				/// \brief Get the type of primitives drawn by the vertex array.
				///
				/// \return Primitive type
				GFX::GCore::RenderPrimitivesType primitiveType() const;

				/// \brief Set the type of primitives to draw
				///
				/// \details This function defines how draw vertices
				///
				/// \param newPrimitiveType Type of primitives
				void setPrimitiveType(GFX::GCore::RenderPrimitivesType newPrimitiveType);

				/// \brief Get type of vertex buffer
				///
				/// \return Type of vertex buffer
				BufferUsageType bufferType() const;

				/// \brief Set type of vertex buffer
				///
				/// \param newBufferUsage
				void setBufferType(BufferUsageType newBufferUsage);

			protected:
				/// \brief Sync buffer in memory with graphical buffer
				virtual bool syncGBuffer() = 0;

			protected:
				/// \brief Reserved data
				static uint16	   s_reservedData;
				std::vector<uint8> m_vertexData;
				/// \brief Specifies kind of primitives to render this vertex array.
				GFX::GCore::RenderPrimitivesType m_primitiveType;

				/// \brief Specifies type of buffer that store verteces in GPU.
				BufferUsageType m_vertexBufferType;
				uint32			m_positionSize = 3;
				mutable uint64	m_sizeOfType   = 0;
			};

			using VertexArrayRef  = std::shared_ptr<GFX::GCore::VertexArray>;
			using VertexArrayWRef = std::weak_ptr<GFX::GCore::VertexArray>;
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
