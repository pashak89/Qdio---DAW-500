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

#include <Core/Math/Vector3D.h>
#include <GFX/GCore/Element.h>
#include <GFX/GCore/Interfaces/IGObjectDrawable.h>
#include <GFX/GCore/RenderPipelineDef.h>
#include <GFX/GCore/VertexArray.h>

namespace DGE
{
	namespace GFX
	{
		namespace Primitives
		{
			/// \brief Base class for rendering Geometry object
			class DGE_GFXAPI Geometry : public GFX::GCore::Interfaces::IGObjectDrawable
			{
				D_OBJECT(Geometry)
			public:
				Geometry(bool				isUsedIndexBuffer = true,
						 System::ContextRef context = Core::System::Context::getInstance());

				// IInitialize interface
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

				template<typename VertexType>
				void setVertices(const std::vector<VertexType> &vertices)
				{
					if (m_element)
					{
						m_element->setGOBjectBuffer(GCore::AttributeType::Position,
													GCore::AttributeGBDesc::toDByteArray<VertexType>(
														m_vertices));
						m_element->setGOBjectBuffer(GCore::AttributeType::IndexBuffer,
													GCore::AttributeGBDesc::toDByteArray<uint32>(
														m_indexes));
					}
				}

				// IUpdate interface
				virtual bool update(GCore::InputParameters &input) override;

				// IPostProcessRender interface
				virtual bool postProcessRender(GCore::InputParameters &input) override;

				// IRender interface
				virtual bool render(GCore::InputParameters &input) override;

				// IPreProcessRender interface
				virtual bool preProcessRender(GCore::InputParameters &input) override;

				/// \brief Retrieve list of vertices for this geometry object
				virtual std::vector<DGE::Core::Math::Vector3D> vertices() const override;

				void setRenderPrimitivesType(GFX::GCore::RenderPrimitivesType renderType);
				GFX::GCore::RenderPrimitivesType renderPrimitivesType() const;

				virtual uint64 totalNumberVertices() override;

				Core::Math::Vector3D offset() const;
				void				 setOffset(const Core::Math::Vector3D &newOffset);

				Color color() const;
				void  setColor(const Color &newColor);

			protected:
				/// \brief regenerate vertices for this geometry
				virtual void regenerateVertices(bool mustBeSync = false);

			protected:
				std::atomic_bool					   m_isChanged;
				std::atomic_bool					   m_isUsedIndexBuffer;
				std::shared_ptr<GFX::GCore::Element>   m_element;
				std::vector<DGE::Core::Math::Vector3D> m_vertices;
				std::vector<uint32>					   m_indexes;
				bool								   m_hasColor;
				Color								   m_color;
				/// \brief Offset of all vertex
				Core::Math::Vector3D m_offset;
			};

			using GeometryRef = std::shared_ptr<GFX::Primitives::Geometry>;
		} // namespace Primitives
	}	  // namespace GFX
} // namespace DGE
