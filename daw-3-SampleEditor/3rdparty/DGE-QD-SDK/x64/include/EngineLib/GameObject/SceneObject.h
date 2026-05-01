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

#include <GFX/GCore/DX9/MeshDX9.h>
#include <GFX/Mesh.h>

// using namespace DGE::Physic;
// using namespace DGE::GFX;
namespace DGE
{
	namespace GameObjects
	{
		enum class TypeSceneObject
		{
			Base,Spawner,AnimatedObject,Viewer

		};

		enum class TypeBoundVolume
		{
			Box, Sphere, Ellipsoid,none,
		};

		class DGE_API SceneObject
		{
			Core::Math::Vector3D m_translation; //  Object's translation in 3D space.
			Core::Math::Vector3D m_rotation;	// Object's rotation in radians.

			Core::Math::Vector3D m_velocity; // Object's velocity in unit/second.
			Core::Math::Vector3D m_spin;	 // Object's spin in radians/second.

			Core::Math::Matrix m_translationMatrix; // Translation matrix.
			Core::Math::Matrix m_rotationMatrix;	// Rotation matrix.

			TypeSceneObject m_type;// Identifies the scene object's parent class.
			float m_friction;// Friction applied to object's velocity and spin.
			BaseType::uint32 m_collisionStamp;// Indicates the last frame when a collision occurred.
			bool m_visible;// Indicates it the object is visible. Invisible objects are not rendered.
			bool m_enabled;// Indicates if the object is enabled. Disabled objects are not updated.
			bool m_ghost;// Indicates if the object is a ghost. Ghost objects cannot physically collide with anything.
			bool m_ignoreCollisions;// Indicates if the object is to ignore collisions. Physical collisions can still occur, they're just not registered.
			bool m_touchingGround;// Indicates if the object is touching the ground.
			bool m_sharedMesh;// Indicates if the object is sharing the mesh or has exclusive access.

		protected:
			std::shared_ptr<GFX::Mesh> m_mesh; // Pointer to object's mesh.
			// Physic::Px_shape m_physicShape;

			Core::Math::Vector3D m_forward; // Object's forward vector.
			Core::Math::Vector3D m_right;	// Object's right vector.

			Core::Math::Matrix m_worldMatrix; // World matrix.
			Core::Math::Matrix m_viewMatrix;  // View matrix.
		public:
			SceneObject(const DChar* meshName = nullptr, const DChar* meshPath = DL(".\\"),
						bool				 IsStatic = true,
						Core::Math::Vector3D Position = Core::Math::Vector3D(0, 0, 0),
						Core::Math::Vector3D Rotation = Core::Math::Vector3D(0, 0, 0),
						TypeBoundVolume typeVolume = TypeBoundVolume::Box, bool sharedMesh = true,
						TypeSceneObject type = TypeSceneObject::Base);
			virtual ~SceneObject();

			void AddForce(Core::Math::Vector3D force);

			// Updates the Scene Object.
			virtual void Update(float elapsed, bool addVelocity = true);
			// Render the Scene Object.
			virtual void Render(Core::Math::Matrix* world = nullptr);

			// Called when something collides with the object.
			virtual void CollisionOccurred(SceneObject* object, BaseType::uint32 collisionStamp);

			// Applies the given force to the object in the forwards/backwards direction.
			void Drive(float force, bool lockYAxis = true);
			// Applies the given force to the object in the right/left direction.
			void Strafe(float force, bool lockYAxis = true);
			// Stops the object moving.
			void Stop();

			// Sets the object's translation.
			void SetTranslation(float x, float y, float z);
			void SetTranslation(Core::Math::Vector3D translation);

			// Adds the given translation to the object's current translation.
			void AddTranslation(float x, float y, float z);
			void AddTranslation(Core::Math::Vector3D translation);

			// Returns the object's translation.
			Core::Math::Vector3D GetTranslation();

			// Sets the object's rotation.
			void SetRotation(float x, float y, float z);
			void SetRotation(Core::Math::Vector3D rotation);

			// Adds the given rotation to the object's current rotation.
			void AddRotation(float x, float y, float z);
			void AddRotation(Core::Math::Vector3D rotation);

			// Returns the object's rotation.
			Core::Math::Vector3D GetRotation();

			// Sets the object's velocity.
			void SetVelocity(float x, float y, float z);
			void SetVelocity(Core::Math::Vector3D velocity);

			// Adds the given velocity to the object's current velocity.
			void AddVelocity(float x, float y, float z);
			void AddVelocity(Core::Math::Vector3D velocity);

			// Returns the object's velocity.
			Core::Math::Vector3D GetVelocity();

			// Sets the object's spin.
			void SetSpin(float x, float y, float z);
			void SetSpin(Core::Math::Vector3D spin);

			// Adds the given spin to the object's current spin.
			void AddSpin(float x, float y, float z);
			void AddSpin(Core::Math::Vector3D spin);

			// Returns the object's spin.
			Core::Math::Vector3D GetSpin();

			// Returns the object's forward vector.
			Core::Math::Vector3D GetForwardVector();
			// Returns the object's right vector.
			Core::Math::Vector3D GetRightVector();

			// Returns a pointer to the object's current translation matrix.
			Core::Math::Matrix* GetTranslationMatrix();
			// Returns a pointer to the object's current rotation matrix.
			Core::Math::Matrix* GetRotationMatrix();
			// Returns a pointer to the object's current world matrix.
			Core::Math::Matrix* GetWorldMatrix();
			// Returns a pointer to the object's current view matrix.
			Core::Math::Matrix* GetViewMatrix();

			// Sets the object's type.
			void SetType(TypeSceneObject type);
			// Returns the object's type.
			TypeSceneObject GetType();

			// Sets the object's friction.
			void SetFriction(float friction);

			// Returns the collision stamp.
			BaseType::uint32 GetCollisionStamp();
			
			// Sets the object's visible flag.
			void SetVisible(bool visible);
			// Returns the object's visible flag.
			bool GetVisible();

			// Sets the object's enabled flag.
			void SetEnable(bool enabled);
			// Returns the object's enabled flag.
			bool GetEnable();

			// Sets the object's ghost flag.
			void SetGhost(bool ghost);
			// Returns the object's ghost flag.
			bool GetGhost();

			// Sets the object's ignore collisions flag.
			void SetIgnoreCollisions(bool ignoreCollisions);
			// Returns the object's ignore collisions flag.
			bool GetIgnoreCollisions();

			// Set's the the flag for touching the ground.
			void SetTouchingGround(bool touchingGround);
			// Returns true if the object is touching the ground.
			bool IsTouchingGround();

			// Sets the mesh for this scene object.
			void SetMesh(const DChar* meshName = nullptr, const DChar* meshPath = DL("./"),
						 bool sharedMesh = true);
			// Returns a pointer to the object's mesh.
			std::shared_ptr<GFX::Mesh> GetMesh();
		};
	}
}

