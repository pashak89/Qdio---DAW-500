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
#include <Engine/Core/Math/Matrix.h>

#include <GFX/GCore/DX9/MeshDX9.h>
#include <GFX/Mesh.h>
#include <GameObject/SceneObject.h>

// using namespace DGE::Physic;
// using namespace DGE::GFX;
namespace DGE
{
	namespace GameObjects
	{

		class DGE_API GameObject
		{
		protected:
			Core::Math::Matrix m_translationMatrix; // Translation matrix.
			Core::Math::Matrix m_rotationMatrix;	// Rotation matrix.
			Core::Math::Matrix m_worldMatrix;		// World matrix.
			Core::Math::Matrix m_viewMatrix;		// View matrix.

			Core::Math::Vector3D m_translation; //  Object's translation in 3D space.
			Core::Math::Vector3D m_rotation;	// Object's rotation in radians.
			Core::Math::Vector3D m_forward;		// Object's forward vector.
			Core::Math::Vector3D m_right;		// Object's right vector.

			TypeSceneObject m_type;// Identifies the scene object's parent class.
			bool m_visible;// Indicates it the object is visible. Invisible objects are not rendered.
			bool m_enabled; // Indicates if the object is enabled. Disabled objects are not updated.

			std::shared_ptr<GFX::Mesh> m_mesh; // Pointer to object's mesh.
											   // Physic::Px_shape m_physicShape;
		public:
			GameObject(const DChar* meshName = nullptr, const DChar* meshPath = DL(".\\"),
					   bool IsStatic = true, Core::Math::Vector3D Position = Vector3D(0, 0, 0),
					   Core::Math::Vector3D Rotation   = Vector3D(0, 0, 0),
					   TypeBoundVolume		typeVolume = TypeBoundVolume::Box,
					   TypeSceneObject		type	   = TypeSceneObject::Base);
			virtual ~GameObject();

			void AddForce(Core::Math::Vector3D force);

			// Updates the Scene Object.
			virtual void Update(float elapsed, bool addVelocity = true);
			// Render the Scene Object.
			virtual void Render(Core::Math::Matrix* world = nullptr);

			// Called when something collides with the object.
			virtual void OnCollision(GameObject* object);

			// Sets the object's translation.
			void SetTranslation(float x, float y, float z);
			void SetTranslation(Core::Math::Vector3D translation);

			// Returns the object's translation.
			Core::Math::Vector3D GetTranslation();

			// Sets the object's rotation.
			void SetRotation(float x, float y, float z);
			void SetRotation(Core::Math::Vector3D rotation);

			// Returns the object's rotation.
			Core::Math::Vector3D GetRotation();

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
			
			// Sets the object's visible flag.
			void SetVisible(bool visible);
			// Returns the object's visible flag.
			bool GetVisible();

			// Sets the object's enabled flag.
			void SetEnable(bool enabled);
			// Returns the object's enabled flag.
			bool GetEnable();

			// Sets the mesh for this scene object.
			void SetMesh(const DChar* meshName = nullptr, const DChar* meshPath = DL("./"));
			// Returns a pointer to the object's mesh.
			std::shared_ptr<GFX::Mesh> GetMesh();
		};
	}
}

