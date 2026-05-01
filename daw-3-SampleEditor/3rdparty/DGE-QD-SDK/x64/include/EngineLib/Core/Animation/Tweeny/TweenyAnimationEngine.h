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
#include <Core/CoreModuleDef.h>
#include <Core/Math/Vector3D.h>

namespace DGE
{
	namespace Core
	{
		namespace Animation
		{
			namespace Tweeny
			{
				/// \brief base class for using tweeny as animation engine in the current animation pipeline
				template<typename Type>
				class TweenyAnimationEngine : public DGE::Core::Animation::AnimationEngine<Type>
				{
				public:
					// IAnimationEngine interface
				};

				template<>
				class DGE_CoreAPI TweenyAnimationEngine<Math::Vector3D>
					: public DGE::Core::Animation::AnimationEngine<Math::Vector3D>

				{
				public:
					TweenyAnimationEngine();
					~TweenyAnimationEngine();

					virtual bool initialize(
						Core::Utility::Interfaces::InitializeParams& parameters) override;

					bool nextFrame(uint64 elapsedTime) override;
					bool previousFrame(uint64 elapsedTime) override;
					bool seekToFrame(uint64 frameTime) override;
					/// \brief Indicate whether current animation is finished
					virtual bool isFinsihed() const override;

					/// \brief Retrieve current time of animation.
					virtual uint64 currentTime() const override;

					uint64 addKeyFrame(
						const Core::Utility::AcTime& frameTime, const Math::Vector3D& value,
						const TransitionType& transitionType,
						Core::Animation::TransformerRef<Core::Animation::TransitionType>
							transformerRef) override;

					/// \brief Modify a key-frame with specified id and changed property of key-frame
					virtual bool modifyKeyFrame(uint64 id, uint64 frameTime) override;
					virtual bool modifyKeyFrame(uint64 id, Core::Utility::AcTime frameTime) override;
					virtual bool modifyKeyFrame(uint64 id, const Math::Vector3D& value) override;
					virtual bool modifyKeyFrame(uint64 id, TransitionType transitionType) override;
					virtual bool modifyKeyFrame(uint64 id, Core::Utility::AcTime frameTime,
												const Math::Vector3D& value,
												TransitionType		  transitionType) override;

					/// \brief Remove a key-frame with specified id
					virtual bool removeKeyFrame(uint64 id) override;

					/// \brief Indicate whether a key-frame with specified id is existed.
					virtual bool isExistKeyFrame(uint64 id) const override;

					/// \brief Return first frame id else return -1.
					virtual int64 firstFrame() const override;

					/// \brief Return last frame id else return -1.
					virtual int64 lastFrame() const override;

					/// \brief Return previous key frame data of specified frame time.
					virtual std::optional<BaseKeyframe<Math::Vector3D>> getPreviousKeyFrame(
						uint64 frameTime) override;

					/// \brief Return next key frame data of specified frame time.
					virtual std::optional<BaseKeyframe<Math::Vector3D>> getNextKeyFrame(
						uint64 frameTime) override;

					/// \brief Return the key frame data at specified frame time if the key frame
					/// exist at specified time; otherwise returns std::nullopt
					virtual std::optional<BaseKeyframe<Math::Vector3D>> keyFrame(
						uint64 frameTime) override;

					virtual MappedValues getMappedValues(uint64 frameTime) const override;

					/// \brief Return the key frame data that is valid at specified time. This will
					/// return std::nullopt if the animation contains no key frame.
					virtual MappedKeys getMappedKeys(uint64 frameTime) const override;

					/// \brief Return value of the animation in the specified frame time.
					virtual Math::Vector3D peek(uint64 frameTime) const override;

					virtual Core::System::Event<bool, Math::Vector3D>& valueChanged() override;
					virtual Core::System::Event<void>&				   jumppedToBegin() override;
					virtual Core::System::Event<void>&				   jumppedToEnd() override;
					virtual Math::Vector3D currentValue() const override;
					virtual uint64		   size() const override;

					virtual KeyFrameList<Math::Vector3D> keyFrames() const override;

				protected:
					class AnimationEngineImp;
					std::unique_ptr<AnimationEngineImp> m_engine;
				};
				template<typename Type>
				using TweenyAnimationEngineWRef = std::weak_ptr<TweenyAnimationEngine<Type>>;
				template<typename Type>
				using TweenyAnimationEngineRef = std::shared_ptr<TweenyAnimationEngine<Type>>;
			} // namespace Tweeny
		}	  // namespace Animation
	}		  // namespace Core
} // namespace DGE
