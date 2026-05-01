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

namespace DGE
{
	namespace GameObjects
	{
#ifdef DGE_UseEngine
		class DGE_API AnimatedObject : public SceneObject //, public ID3DXAnimationCallbackHandler
		{
			// ID3DXAnimationController *m_animationController; // Controller for managing mesh
			// animation playback.
			uint32 m_currentTrack; // The track of the currently playing animation.
			float m_currentTime; // Timer used for animation.

			// Animation callback handler.
			// virtual HRESULT CALLBACK HandleCallback(THIS_ uint32 Track, void* pCallbackData);

		public:
			AnimatedObject(const DChar* meshName = nullptr, const DChar* meshPath = DL(".\\"),
						   bool					IsStatic   = true,
						   Core::Math::Vector3D Position   = Core::Math::Vector3D(0, 0, 0),
						   Core::Math::Vector3D Rotation   = Core::Math::Vector3D(0, 0, 0),
						   TypeBoundVolume		typeVolume = TypeBoundVolume::Box,
						   bool					sharedMesh = true,
						   TypeSceneObject		type	   = TypeSceneObject::AnimatedObject);
			virtual ~AnimatedObject();

			// Updates the Scene Object.
			virtual void Update(float elapsed, bool addVelocity = true);

			// Plays the given animation with the given transition time.
			void PlayAnimation(uint32 animation, float transitionTime, bool loop = true);
			// Returns a pointer to the object's animation controller.
			// ID3DXAnimationController *GetAnimationController();
#endif
		};

	}
}
