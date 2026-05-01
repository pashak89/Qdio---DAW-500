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
#include "../../Physics/2D/Physics2D.h"
namespace DGE
{
	using namespace DGE::Math;
	using namespace DGE::IO;
	using namespace DGE::GFX;

	namespace Objects2D
	{
		class _2DObjectsManager;
		namespace PhysicBase
		{
			using namespace Physic::_2D;
#if defined(DGE_UseGraphicAPI)
			class Physical2DObject : public Physic2dObject, public _2DObject
			{
			protected:
				
			public:
				DGE_Force_Inline Vector2D& ConvertFromPhysicToRender(Vector2D& InOutPosition)
				{
					InOutPosition *= fPRV;
					InOutPosition.x -= m_rectangleScreen.Width()/2;
					InOutPosition.y -= m_rectangleScreen.Height()/2;
					return InOutPosition;
				}
				
				//************************************
				// Method:    ConvertFromPhysicToRender
				// FullName:  DGE::Object2dClass::PhysicBase::PhysicBasedObject2D::ConvertFromPhysicToRender
				// Access:    protected 
				// Returns:   DGE_Force_Inline void
				// Qualifier:
				// Parameter: Vector2D & InOutPosition
				//				use for Input & output position of result of this function.
				// Parameter: Bound<int> & RenctngleShape
				//				use for calculate width & height of this shape.
				//************************************
				DGE_Force_Inline Vector2D& ConvertFromPhysicToRender(Vector2D& InOutPosition, Bound& RenctngleShape)
				{
					InOutPosition *= fPRV;
					InOutPosition.x -= RenctngleShape.Width();
					InOutPosition.y -= RenctngleShape.Height();
					return InOutPosition;
				}
			
				Physical2DObject();
				virtual ~Physical2DObject();
				virtual void Render()=0;
				virtual void Update(float elapsed)=0;

				virtual void EnableEvent() = 0;
				virtual void DisableEvent() = 0;
				virtual void SetMass(float mass, Vector2D center = Vector2D(0, 0), float rotationAngle = 0);
				virtual float GetMass();
			};
#endif
		}
	}
}