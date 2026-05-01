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
	using namespace DGE::Physic::_2D;

	namespace Objects2D
	{
		class _2DObjectsManager;
		namespace PhysicBase
		{
#if defined(DGE_UseGraphicAPI)
			class DGE_API DynamicObject : public StaticObject
			{

			public:
				DynamicObject();
				DynamicObject(Renderer* pRenderer, Bound BoundObject, Vector2D& centerPoint, PhysicMaterial physicMaterial, Bound RectImage);
				DynamicObject(Renderer* pRenderer, Bound BoundObject, PhysicMaterial physicMaterial, Bound RectImage);
				DynamicObject(Renderer* pRenderer, Bound BoundObject, PhysicMaterial physicMaterial);

				virtual ~DynamicObject();
				virtual void Render();
				virtual void Update(float elapsed);

				virtual void Move(Vector2D direction);
				virtual void AddForce(Vector2D direction);
				virtual void EnableGravity(bool flag);

				/// Set the position of the body's origin and rotation.
				/// Manipulating a body's transform may cause non-physical behavior.
				/// Note: contacts are updated on the next call to b2World::Step.
				/// @param position the world position of the body's local origin.
				/// @param angle the world rotation in radians.
				void SetTransform(const Vector2D& position, float angle);

				/// Get the world body origin position.
				/// @return the world position of the body's origin.
				Vector2D& GetPosition();

				/// Get the angle in radians.
				/// @return the current world rotation angle in radians.
				float GetAngle() const;

				/// Get the world position of the center of mass.
				const Vector2D& GetWorldCenter() const;

				/// Get the local position of the center of mass.
				const Vector2D& GetLocalCenter() const;

				/// Set the linear velocity of the center of mass.
				/// @param v the new linear velocity of the center of mass.
				void SetLinearVelocity(const Vector2D& v);

				/// Get the linear velocity of the center of mass.
				/// @return the linear velocity of the center of mass.
				const Vector2D& GetLinearVelocity() const;

				/// Set the angular velocity.
				/// @param omega the new angular velocity in radians/second.
				void SetAngularVelocity(float omega);

				/// Get the angular velocity.
				/// @return the angular velocity in radians/second.
				float GetAngularVelocity() const;

				/// Get the world coordinates of a point given the local coordinates.
				/// @param localPoint a point on the body measured relative the the body's origin.
				/// @return the same point expressed in world coordinates.
				Vector2D GetWorldPoint(const Vector2D& localPoint) const;

				/// Get the world coordinates of a vector given the local coordinates.
				/// @param localVector a vector fixed in the body.
				/// @return the same vector expressed in world coordinates.
				Vector2D GetWorldVector(const Vector2D& localVector) const;

				/// Gets a local point relative to the body's origin given a world point.
				/// @param a point in world coordinates.
				/// @return the corresponding local point relative to the body's origin.
				Vector2D GetLocalPoint(const Vector2D& worldPoint) const;

				/// Gets a local vector given a world vector.
				/// @param a vector in world coordinates.
				/// @return the corresponding local vector.
				Vector2D GetLocalVector(const Vector2D& worldVector) const;

				/// Get the world linear velocity of a world point attached to this body.
				/// @param a point in world coordinates.
				/// @return the world velocity of a point.
				Vector2D GetLinearVelocityFromWorldPoint(const Vector2D& worldPoint) const;

				/// Get the world velocity of a local point.
				/// @param a point in local coordinates.
				/// @return the world velocity of a point.
				Vector2D GetLinearVelocityFromLocalPoint(const Vector2D& localPoint) const;

				/// Get the linear damping of the body.
				float GetLinearDamping() const;

				/// Set the linear damping of the body.
				void SetLinearDamping(float linearDamping);

				/// Get the angular damping of the body.
				float GetAngularDamping() const;

				/// Set the angular damping of the body.
				void SetAngularDamping(float angularDamping);
				//virtual Vector2D Get
			};
#endif
		}
	}
}