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
#include <GameObject/GameObject.h>

#include <GFX/Camera.h>

// using namespace DGE::Physic;
// using namespace DGE::GFX;
namespace DGE
{
	namespace GameObjects
	{
		class DGE_API CameraObject
			: public GameObject
			, public GFX::Camera
		{
		
		public:
			CameraObject(const DChar* meshName = nullptr, const DChar* meshPath = DL(".\\"),
						 bool IsStatic = true, Vector3D Position = Vector3D(0, 0, 0),
						 Vector3D		 Rotation	= Vector3D(0, 0, 0),
						 TypeBoundVolume typeVolume = TypeBoundVolume::Box,
						 TypeSceneObject type		= TypeSceneObject::Base);
			virtual ~CameraObject();

			void AddForce(Vector3D force);

			// Updates the Scene Object.
			virtual void Update(float elapsed, bool addVelocity = true) override;
			// Render the Scene Object.
			virtual void Render(Matrix* world = nullptr) override;

			// Called when something collides with the object.
			virtual void OnCollision(GameObject* object) override;
		};
	}
}

