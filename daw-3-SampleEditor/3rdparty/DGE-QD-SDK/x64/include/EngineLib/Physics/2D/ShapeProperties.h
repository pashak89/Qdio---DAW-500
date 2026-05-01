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

#include "../../Core/BaseTypes/BaseTypes.h"
#include "../../Core/MemoryManager/MemoryManager.h"

#include "../../Core/Math/Vector.h"

namespace DGE
{
	namespace Physic
	{
		namespace _2D
		{
			//Physic Material need in ShapeProperties
			enum class PhysicMaterial:unsigned short
			{
				Wood=0, Iron, Plastic,Stone,Water,Grass,Count
			};

			enum class Physic2dShape
			{
				none, Line, Box, Circle, Capsule, Polygon
			};

			//set properties of variant shapes.
			class DGE_API ShapeProperties
			{
				b2Shape* baseShape;
				b2FixtureDef fixtureDef;
				bool m_isFixedRotation=false;
				bool m_hasMassData = false;
				b2MassData m_MassData;
				friend class Physics2D;
				friend class Physic2dObject;
				//friction,restitution,density
				const static float material[static_cast<int>(PhysicMaterial::Count)][3];

				void SetUserData(void* data);

				void* GetUserData();
			public:
				~ShapeProperties();
				/// hx: the half - width.
				/// hy: the half-height.
				void SetAsBox(float width, float height);
				/// hx: the half - width.
				/// hy: the half-height.
				/// center: the center of the box in local coordinates.
				/// angle: the rotation of the box in local coordinates.
				void SetAsBox(float width, float height, Math::Vector2D center, float angle);


				/// radius: the radius of circle.
				void SetAsCircle(float radius);

				/// xRadius: radius of capsule in x axis.
				/// yRadius: radius of capsule in y axis.
				void SetAsCapsule(float radius, float width);

				/// xRadius: radius of capsule in x axis.
				/// yRadius: radius of capsule in y axis.
				/// center: indicate center of Capsule.
				/// rotationAngle: angle of Rotation about center axis.
				void SetAsCapsule(float xRadius, float yRadius, Core::Math::Vector2D center,
								  float rotationAngle);

				/// The friction coefficient, usually in the range [0,1].
				/// The restitution (elasticity) usually in the range [0,1].
				/// The density, usually in kg/m^2.
				/// A sensor shape collects contact information but never generates a collision response.
				/// Contact filtering data.
				void SetFixture(float friction,float restitution,float density,
					 b2Filter filter, bool isSensor=false, void* userData=nullptr);
				void SetFixture(float friction, float restitution,float density,
					 bool isSensor=false, void* userData=nullptr);
				void SetFixture(PhysicMaterial phMaterial = PhysicMaterial::Wood, bool isSensor = false, void* userData = nullptr);

				void SetFixture(PhysicMaterial phMaterial, b2Filter filter, bool isSensor = false, void* userData = nullptr);

				void SetFixRotation(bool flag);

				void SetMassData(float mass, Core::Math::Vector2D center, float rotationAngle);
			};
		}
	}
}
