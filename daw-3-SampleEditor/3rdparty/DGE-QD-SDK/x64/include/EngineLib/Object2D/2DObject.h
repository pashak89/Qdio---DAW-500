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
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>

#include <Engine/GFX/GCore/Renderer.h>
#include <Engine/GFX/Texture.h>

namespace DGE
{
	using namespace DGE::Math;
	// using namespace DGE::IO;
	using namespace DGE::GFX;

	namespace Objects2D
	{
		class _2DObjectsManager;

#if defined(DGE_UseGraphicAPI)
		class DGE_API _2DObject
		{
			friend class Objects2D::_2DObjectsManager;
		protected:

			Matrix m_TransformMatrix;
			Vector2D m_Position,m_centerPoint;
			float m_zPlane=0;
			Bound m_rectangleScreen;//Rectangle of Control For Collision detection.
			Core::BaseType::Color m_Color;
			bool m_bMouseOver=false;//True When Mouse is Over this Control.
			uint32	 ID			 = -1;	// id of this Gui object!

			// renderer
			GFX::GCore::Renderer* m_pRenderer = nullptr;

			std::shared_ptr<Texture> m_pBackImage = nullptr;
			Bound m_ImageBound;//Bound in image for render
			Bound* m_fastRenderBound = nullptr;
		public:
			_2DObject();
			virtual ~_2DObject();

			//Render Control.
			virtual void Render() = 0;
			virtual void Update(float elapsed) = 0;

			//Check Click is in Rectangle
			virtual bool CheckClick(int X, int Y);

			// Set Position Of Control.
			virtual void SetPosition(int X, int Y);
			virtual void SetPosition(Vector2D Position);

			// Return Position Of Control.
			virtual Vector2D& GetPosition()
			{
				return m_Position;
			}

			//return Size of Object in Screen
			virtual Bound& GetSizeOnScreen();

			//Set Size of Control.
			virtual void SetSize(int Width, int Height);

			//Set Rectangle in Around of Control.
			virtual void SetRectangleScreen(int X, int Y, int Width, int Height);
			virtual void SetRectangleScreen(Bound bound);

			virtual void SetColor(Core::BaseType::Color color);

			virtual void SetCenterPoint(int x, int y);

			inline void FitSizeToImage();

			void SetImage(std::shared_ptr<Texture> pTexture);
		};
#endif

	}
}
