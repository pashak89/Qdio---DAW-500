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
#include <GFX/GCore/Renderer.h>
#include <Object2D/2DObject.h>

namespace DGE
{
	using namespace DGE::Resource;
	using namespace DGE::Core::Math;
	using namespace GFX;
	namespace Objects2D
	{
		namespace GUI
		{
#if defined(DGE_UseGraphicAPI)
			class DGE_API Image :public _2DObject
			{
			public:
				Image(GFX::GCore::Renderer* pRenderer, Bound BoundObject, Bound RectImage);
				Image(GFX::GCore::Renderer* pRenderer, Bound BoundObject);
				virtual ~Image(void);
				virtual void Render() override;
				virtual void Update(float elapsed) override;
				virtual void SetPosition(Vector3D vec);
				virtual void SetPosition(Vector2D vec) override;
				virtual void SetPosition(float x, float y, float z);
			};
#endif
		}
	}
}
