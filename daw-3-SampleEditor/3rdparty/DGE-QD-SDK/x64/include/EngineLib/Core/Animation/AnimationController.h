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
#include <Core/Animation/AnimationEngine.h>
#include <Core/Animation/BaseKeyframe.h>
#include <Core/Animation/IAnimationController.h>
#include <Core/Animation/KeyFrame.h>
#include <Core/Math/Vector3D.h>
#include <memory>

namespace DGE
{
    namespace Core
    {
        namespace Animation
        {
            enum class AnimationState
            {
                Stopped,
                Paused,
                Playing
            };

            template<typename Type>
            using KeysList = std::map<uint64, BaseKeyframe<Type>>;

            /// \brief Base Class for controlling an animation
			template<typename Type>
			class AnimationController
				: public IAnimationController
				, public Utility::Interfaces::IInitialize
			{
            public:
				AnimationController(AnimationEngineURef<Type> uniquePtr)
					: m_animEngine(std::move(uniquePtr))
				{
					m_animationState = AnimationState::Stopped;
				}
				virtual ~AnimationController() {}

				// IInitialize interface
				bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override
				{
                    return m_animEngine->initialize(parameters);
                }

				uint64 addKeyFrame(
					Core::Utility::AcTime& frameTime, const Type& value,
					TransitionType&													 transitionType,
					Core::Animation::TransformerRef<Core::Animation::TransitionType> transformerRef
					= nullptr)
				{
					uint64 id = frameTime.toMilliSecond();
					if (!isExistKeyFrame(id))
                    {
						id = addKeyFrame(id, value, transitionType, transformerRef);
					}
					return id;
				}

				uint64 addKeyFrame(
					uint64 frameTime, const Type& value, TransitionType& transitionType,
					Core::Animation::TransformerRef<Core::Animation::TransitionType> transformerRef
					= nullptr)
				{
					return m_animEngine->addKeyFrame(frameTime, value, transitionType,
													 transformerRef);
				}
				uint64 addKeyFrame(const BaseKeyframe<Type>& keyFrame)
				{
                    uint64 id = m_animEngine->addKeyFrame(keyFrame);
                    return id;
                }

				bool removeKeyFrame(uint64 id) { return m_animEngine->removeKeyFrame(id); }
				bool isExistKeyFrame(uint64 id) { return m_animEngine->isExistKeyFrame(id); }
				/// \brief Modify a key-frame with specified id and changed property of key-frame
				bool modifyKeyFrame(uint64 id, uint64 frameTime)
				{
					return m_animEngine->modifyKeyFrame(id, frameTime);
				}
				bool modifyKeyFrame(uint64 id, Core::Utility::AcTime frameTime)
				{
					return m_animEngine->modifyKeyFrame(id, frameTime);
				}
				bool modifyKeyFrame(uint64 id, const Type& value)
				{
                    return m_animEngine->modifyKeyFrame(id, value);
                }
				bool modifyKeyFrame(uint64 id, TransitionType transitionType)
				{
					return m_animEngine->modifyKeyFrame(id, transitionType);
				}
				bool modifyKeyFrame(uint64 id, Core::Utility::AcTime frameTime, const Type& value,
									TransitionType transitionType)
				{
                    return m_animEngine->modifyKeyFrame(id, frameTime, value, transitionType);
                }

                /// \brief Go to next frame with specified elapsed time
                bool nextFrame(uint64 elapsedTime) override
                {
                    return m_animEngine->nextFrame(elapsedTime);
                }
                bool previousFrame(uint64 elapsedTime) override
                {
                    return m_animEngine->previousFrame(elapsedTime);
                }
                bool seekToFrame(uint64 frameTime) override
                {
                    return m_animEngine->seekToFrame(frameTime);
                }

                void stop() { m_animationState = AnimationState::Stopped; }
                void play() { m_animationState = AnimationState::Playing; }
                void pause() { m_animationState = AnimationState::Paused; }

                void setCurrentTime(uint64 currentTime) { m_animEngine->seekToFrame(currentTime); }
                void setCurrentTime(Utility::AcTime currentTime)
                {
                    setCurrentTime(currentTime.toMilliSecond());
                }

                bool isStopped() const { return m_animationState == AnimationState::Stopped; }
                void setIsStopped(bool newIsStopped)
                {
                    if (m_animationState != AnimationState::Stopped)
                        m_animationState = AnimationState::Stopped;
                }

                bool isPaused() const { return m_animationState == AnimationState::Paused; }
                void setIsPaused(bool newIsPaused)
                {
                    if (m_animationState != AnimationState::Paused)
                        m_animationState = AnimationState::Paused;
                }

                bool isPlaying() const { return m_animationState == AnimationState::Playing; }

				void setAnimEngine(const AnimationEngineURef<Type>& newAnimEngine)
				{
					if (m_animEngine != newAnimEngine)
                        m_animEngine = newAnimEngine;
				}

				// IAnimationController interface
				bool isFinsihed() const override { return m_animEngine->isFinsihed(); }

				Core::System::Event<bool, Type>& valueChanged()
				{
					return m_animEngine->valueChanged();
				}
				Core::System::Event<void>& jumppedToBegin()
				{
					return m_animEngine->jumppedToBegin();
				}
				Core::System::Event<void>& jumppedToEnd() { return m_animEngine->jumppedToEnd(); }

				uint64		   currentTime() const override { return m_animEngine->currentTime(); }
				Type		   currentValue() const { return m_animEngine->currentValue(); }
                uint64		   size() const { return m_animEngine->size(); }

				typename AnimationEngine<Type>::MappedValues getMappedValues(uint64 frameTime)
				{
					return m_animEngine->getMappedValues(frameTime);
				}

				typename AnimationEngine<Type>::MappedKeys getMappedKeys(uint64 frameTime)
				{
					return m_animEngine->getMappedKeys(frameTime);
				}
				/// \brief Return first frame id else return -1.
				int64 firstFrame() const { return m_animEngine->firstFrame(); }

				/// \brief Return last frame id else return -1.
				int64 lastFrame() const { return m_animEngine->lastFrame(); }

				/// \brief Return previous key frame data of specified frame time.
				std::optional<BaseKeyframe<Type>> getPreviousKeyFrame(uint64 frameTime)
				{
					return m_animEngine->getPreviousKeyFrame(frameTime);
				}

				/// \brief Return next key frame data of specified frame time.
				std::optional<BaseKeyframe<Type>> getNextKeyFrame(uint64 frameTime)
				{
					return m_animEngine->getNextKeyFrame(frameTime);
				}

				/// \brief Return the key frame data at specified frame time.
				std::optional<BaseKeyframe<Type>> keyFrame(uint64 frameTime)
				{
					return m_animEngine->keyFrame(frameTime);
				}

				/// \brief Return value of the animation in the specified frame time.
				Type peek(uint64 frameTime) const { return m_animEngine->peek(frameTime); }

				/// \brief Return list of key-frames that is available in this animation engine.
				///
				/// \note This list isn't updated automatically and need to call this function to
				/// generate the updated list of key-frames.
				virtual KeyFrameList<Type> keyFrames() const { return m_animEngine->keyFrames(); }

			protected:
				AnimationState				  m_animationState;
				AnimationEngineURef<Type>	  m_animEngine;
			};
			D_DefRefTypeTemplate(AnimationController)
		} // namespace Animation
    }	  // namespace Core
} // namespace DGE
