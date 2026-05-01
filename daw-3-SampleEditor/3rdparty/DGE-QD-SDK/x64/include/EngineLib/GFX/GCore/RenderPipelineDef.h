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

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/BaseTypes/Color.h>
#include <Core/Math/Bound.h>
#include <GFX/GFXModuleDef.h>
#include <algorithm>
#include <iterator>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			/// \brief Type of the Basic graphical object in the Graphical core of engine
			enum class GPUObjectType
			{
				None,
				VertexArray,
				VertexBuffer,
				IndexBuffer,
				TextureCoordBuffer,
				TextureBuffer,
				FrameBuffer
			};

			/// \brief Type of the attribute of the Element
			enum class AttributeType
			{
				None,
				VertexArrayObject,
				Position,
				IndexBuffer,
				Normal,
				TextureCoord,
				Color1,
				Color2,
				Color3,
				Color4,
				Color5,
				Color6,
				Color7,
				Color8,
				Color9,
				Color10,
				Texture1,
				Texture2,
				Texture3,
				Texture4,
				Texture5,
				Texture6,
				Texture7,
				Texture8,
				Texture9,
				Texture10,
				VertexData,
				CustomData
			};

			/// \brief Type of values that is store in a attribute of the Element or vertex
			enum class AttributeValueType
			{
				Byte,
				UnsignedByte,
				Short,
				UnsignedShort,
				Int,
				UnsignedInt,
				HalfFloat,
				Float,
				Double,
				Fixed,
				Int_2_10_10_10_Rev,
				UnsignedInt_2_10_10_10_Rev,
				UnsignedInt_10F_11F_11F_Rev,
				Boolean
			};

			/// \brief Type of graphic buffer is activating in render pipeline
			enum class RenderBufferType
			{
				Stencil,
				Depth,
				Color
			};

			/// \brief Type of blend operation in the render pipeline
			enum class BlendMode
			{
				Replace = 0,
				Add,
				Multiply,
				Alpha,
				AddAlpha,
				PremulAlpha,
				InvertDestinationAlpha,
				Subtract,
				SubtractAlpha,
				MaxCount
			};

			/// \brief Compare mode for depth or stencil buffer
			enum class CompareMode
			{
				Always = 0,
				Equal,
				NotEqual,
				Less,
				LessEqual,
				Greater,
				GreaterEqual,
				MaxCount
			};

			/// \brief Type of Depth value is using in range
			enum class DepthValueType : uint8
			{
				/// Reserved value type
				Normal = 0,
				PinToFront,
				PinToBack,

				MaxCount,
				CustomValues

			};

			/// \brief Type of culling mode in the render pipeline
			enum class CullMode
			{
				None = 0,
				CCW,
				CW
			};

			/// \brief Fill mode.
			enum class FillMode
			{
				Solid = 0,
				Wireframe,
				Point
			};

			/// \brief Type of stencil operation.
			enum class StencilOperation
			{
				Keep,
				Zero,
				Ref,
				Increase,
				Decrease
			};

			/// \brief Show different type of Render API that use loading suitable Renderer implementation
			enum class RenderAPI
			{
				DirectX9,
				DirectX10,
				DirectX11,
				DirectX12,
				OpenGL2,
				OpenGL3,
				OpenGL4,
				OpenGLES3,
				OpenGLES2,
				Metal,
				Volkan,
				WebGL1,
				WebGL2,
				WebGPU
			};

			enum class RenderPrimitivesType
			{
				None,
				Points,
				Lines,
				LineLoop,
				LineStrip,
				Triangles,
				TriangleStrip,
				TriangleFAN,
				Quads,
				QuadStrip,
				Polygon
			};

			enum class BufferUsageType
			{
				Stream_Draw,
				Stream_Read,
				Stream_Copy,
				Static_Draw,
				Static_Read,
				Static_Copy,
				Dynamic_Draw,
				Dynamic_Read,
				Dynamic_Copy,
			};

			struct RenderApiVersion
			{
				RenderAPI renderApi;
				DString	  versionString;
				uint32_t  majorVersion;
				uint32_t  minorVersion;

				/// \brief Detect based on render API version
				bool isMobileBase;
				RenderApiVersion()
					: renderApi(RenderAPI::OpenGL4)
					, majorVersion(4)
					, minorVersion(0)
					, versionString("")
					, isMobileBase(false)
				{
				}
				RenderApiVersion(const RenderApiVersion&);
				RenderApiVersion& operator=(const RenderApiVersion& copy);
				DString			  toString() { return DString(majorVersion) + "." + minorVersion; }
			};

			struct RenderAPIInfo
			{
				RenderApiVersion versionApi;
				DString			 shaderVersion;

				/// \brief Detect Based on platform type
				bool isMobile;
				bool isWeb;
				RenderAPIInfo()
					: shaderVersion(400)
					, isMobile(false)
					, isWeb(false)
				{
				}
				RenderAPIInfo(const RenderAPIInfo&);
				RenderAPIInfo& operator=(const RenderAPIInfo& copy);
			};

			struct StyleFont;
			//! structure use for clear function.
			struct DGE_GFXAPI ClearParam
			{
				uint32			  count;
				Core::Math::Bound rect; // a array of rectangle use for clear area.
				/*	this list of type set how clear surface or other things
				Stencil Clear the stencil buffer.
				Color Clear a render target, or all targets in a multiple render target.
				Depth Clear the depth buffer.
				*/
				std::vector<RenderBufferType> listBufferType;
				bool						 isClearColor;
				Color						 color;
				float						 zValue;
				uint32						 stencilValue;

				bool						 clear();

				//			ClearParam()
				//			{
				//				count		 = 0;
				//				zValue		 = 0.0f;
				//				stencilValue = 0;
				//				isClearColor = false;
				//			}

				//			ClearParam(const ClearParam& clearParam)
				//			{
				//				count		 = clearParam.count;
				//				rect		 = clearParam.rect;
				//				isClearColor = clearParam.isClearColor;
				//				color		 = clearParam.color;
				//				zValue		 = clearParam.zValue;
				//				stencilValue = clearParam.stencilValue;
				//				listBufferType.clear();
				//				for (auto bufferTYpe : clearParam.listBufferType)
				//					listBufferType.push_back(bufferTYpe);
				//			}

				//			ClearParam(const ClearParam&& clearParam)
				//			{
				//				count		 = clearParam.count;
				//				rect		 = clearParam.rect;
				//				isClearColor = clearParam.isClearColor;
				//				color		 = clearParam.color;
				//				zValue		 = clearParam.zValue;
				//				stencilValue = clearParam.stencilValue;
				//				listBufferType.clear();
				//				for (auto bufferTYpe : clearParam.listBufferType)
				//					listBufferType.push_back(bufferTYpe);
				//			}

				const ClearParam& operator=(const ClearParam& clearParam)
				{
					count		 = clearParam.count;
					rect		 = clearParam.rect;
					isClearColor = clearParam.isClearColor;
					color		 = clearParam.color;
					zValue		 = clearParam.zValue;
					stencilValue = clearParam.stencilValue;
					listBufferType.clear();
					std::copy(clearParam.listBufferType.begin(),
							  clearParam.listBufferType.end(),
							  std::back_inserter(listBufferType));

					return *this;
				}
			};

			//! structure use for present function.
			struct PresentParam
			{
				Core::Math::Bound sourceRect;
				Core::Math::Bound destRect;
			};

			/// \brief Structure use for drawing an element
			struct ElementDrawInfo
			{
				RenderPrimitivesType m_renderPrimitivesType;
				uint64				 m_count;
				AttributeValueType	 m_valueType;
				void*				 m_offsetPointer;
			};

			/// \brief Structure use for drawing a vertex array
			struct VertexArrayDrawInfo
			{
				RenderPrimitivesType m_renderPrimitivesType;
				uint64				 m_beginIndex;
				uint64				 m_count;
			};
		} // namespace GCore
	}	  // namespace GFX
} // namespace DGE
