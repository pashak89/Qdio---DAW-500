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
#include <GameObject/SceneObject.h>

#include <Engine/ScriptSet/Script.h>

namespace DGE
{
	namespace GameObjects
	{
		class DGE_API SpawnerObject : public SceneObject
		{
			DChar *m_name; // Name of the object that is spawned at this spawner.
			float m_frequency; // How often the spawner spawns its object (in seconds).
			float m_spawnTimer; // Timer used for spawning the object.

			// SFX::Sound *m_sound; // Sound to play when the object is collected.
			// SFX::AudioPath3D *m_audioPath; // Audio path to play the sound on.
			std::shared_ptr<ScriptSet::Script> m_objectScript; // Script for the spawner's object.

		public:
			SpawnerObject(const DChar *meshName = nullptr, const DChar *meshPath = DL(".\\"),
						  bool IsStatic = true, Vector3D Position = Vector3D(0, 0, 0),
						  Vector3D		  Rotation	 = Vector3D(0, 0, 0),
						  TypeBoundVolume typeVolume = TypeBoundVolume::Box, bool sharedMesh = true,
						  TypeSceneObject type = TypeSceneObject::Spawner);
			virtual ~SpawnerObject();

			virtual void Update(float elapsed, bool addVelocity = true);

			virtual void CollisionOccurred(SceneObject *object, BaseType::uint32 collisionStamp);

			ScriptSet::ScriptRef GetObjectScript();
		};
	}
}
