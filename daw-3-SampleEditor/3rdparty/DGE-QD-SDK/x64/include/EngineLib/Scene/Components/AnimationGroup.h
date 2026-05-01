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
 *
 */
#pragma once

#include "Core/Utility/Singleton/SingletonMacrosDef.h"
#include <Core/System/DObject.h>
#include <Core/System/DObjectMacrosDef.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <GFX/GCore/Interfaces/IUpdate.h>
#include <Scene/Component.h>
#include <Scene/Components/AnimationComponent.h>
#include <memory>
#include <unordered_set>

namespace DGE
{
	namespace Scene
	{
		/// \brief Manage group of animations in the scene
		class DGE_SceneAPI AnimationGroup : public Scene::Component
		{
			D_OBJECT(AnimationGroup)
		public:
			AnimationGroup(std::shared_ptr<Core::System::Context> context);
			D_Destructor(AnimationGroup);

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			virtual bool update(GFX::GCore::InputParameters& input) override;

			/// \brief Add the specified animation component into this animation group
			void addAnimation(AnimationComponentRef animComponent);

			/// \brief Remove the specified animation component from this animation group
			void removeAnimation(AnimationComponentRef animComponent);

			/// \brief stop playing animation for now. Must call play() function to playing
			/// animation again
			void stop();

			/// \brief start playing animation. Must call stop() or pause() function to preventing
			/// of continuing playing animation
			///
			/// \param isReversePlaying if set this flag to true, animation play in reverse direction
			void play(bool isReversePlaying = false);

			/// \brief pause playing animation for now. Must call play() function to continue
			/// playing animation from current place of animation
			void pause();

			bool isStopped() const;

			bool isPaused() const;

			bool isPlaying() const;

			bool isReversePlaying() const;

			uint64 minKeyFrameTime() const;

			uint64 maxKeyFrameTime() const;

			/// \brief This function indicate whether all animations in this animation group are finished.
			bool isFinished();

		protected:
			/// \brief update min & max time in the animation group
			void updateMinTime(uint64 time);
			void updateMaxTime(uint64 time);

		protected:
			std::shared_ptr<AnimationGroup>			  m_this;

			bool							m_isReversePlaying;
			Core::Animation::AnimationState m_animationState;

			uint64 m_minKeyFrameTime;
			uint64 m_maxKeyFrameTime;

			D_PROPERTY(std::unordered_set<AnimationComponentRef>, animationsSet, AnimationsSet);
			D_PROPERTY(uint64, currentTime, CurrentTime, 0)
			D_PROPERTY(float, speed, Speed, 0)
		};

		D_DefRefType(AnimationGroup);

	} // namespace Scene
} // namespace DGE
