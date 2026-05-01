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
#include "../../RenderCache.h"

namespace DGE
{
	using namespace DGE::ResourceManager;
	using namespace DGE::Core::Math;
	using namespace Math;
	// using namespace Physic;
	namespace GFX
	{
		namespace DirectX9
		{
			// RenderCache class use for optimize render process!
			class DGE_GFXAPI RenderCacheDX9 : public RenderCache
			{
			public:
				RenderCacheDX9(Material* material);
				virtual ~RenderCacheDX9();

				virtual void AddFace() override;
				virtual void Prepare(BaseType::uint32 totalVertices) override;

				virtual void Begin() override;
				virtual void RenderFace(uint16 vertex0, uint16 vertex1, uint16 vertex2) override;
				virtual void End() override;
			};
		} // namespace DirectX9
	}
}
