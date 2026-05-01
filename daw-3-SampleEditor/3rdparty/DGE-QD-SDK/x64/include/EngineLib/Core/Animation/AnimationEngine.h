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

#include <Core/Animation/IAnimationController.h>
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/System/Event.h>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace Animation
		{
			template<typename Type>
			using KeyFrameList = std::vector<BaseKeyframeRef<Type>>;
			template<typename Type>
			class AnimationEngine
				: public IAnimationController
				, public Utility::Interfaces::IInitialize
			{
			public:
				using MappedValues = std::pair<std::optional<Type>, std::optional<Type>>;
				using MappedKeys   = std::pair<std::optional<BaseKeyframe<Type>>,
											   std::optional<BaseKeyframe<Type>>>;
				virtual ~AnimationEngine() {}

				/// \brief Add a key-frame with specified frameTime,value and transitionType
				virtual uint64 addKeyFrame(const Core::Utility::AcTime& frameTime,
										   const Type& value, const TransitionType& transitionType,
										   TransformerRef<TransitionType> transformerRef = nullptr)
					= 0;
				virtual uint64 addKeyFrame(const uint64 frameTime, const Type& value,
										   const TransitionType&		  transitionType,
										   TransformerRef<TransitionType> transformerRef = nullptr)
				{
					Core::Utility::AcTime ac = frameTime;
					return addKeyFrame(ac, value, transitionType, transformerRef);
				}
				/// \brief Add a key-frame with specified keyFrame object
				uint64 addKeyFrame(const BaseKeyframe<Type>& keyFrame)
				{
					return addKeyFrame(keyFrame.frameTime(),
									   keyFrame.value(),
									   keyFrame.transitionType());
				}

				/// \brief Modify a key-frame with specified id and changed property of key-frame
				virtual bool modifyKeyFrame(uint64 id, uint64 frameTime)				= 0;
				virtual bool modifyKeyFrame(uint64 id, Core::Utility::AcTime frameTime) = 0;
				virtual bool modifyKeyFrame(uint64 id, const Type& value)				= 0;
				virtual bool modifyKeyFrame(uint64 id, TransitionType transitionType)	= 0;
				virtual bool modifyKeyFrame(uint64 id, Core::Utility::AcTime frameTime,
											const Type& value, TransitionType transitionType)
					= 0;

				/// \brief Remove a key-frame with specified id
				virtual bool removeKeyFrame(uint64 id) = 0;

				/// \brief Indicate whether a key-frame with specified id is existed.
				virtual bool isExistKeyFrame(uint64 id) const = 0;

				/// \brief Return first frame id else return -1.
				virtual int64 firstFrame() const = 0;

				/// \brief Return last frame id else return -1.
				virtual int64 lastFrame() const = 0;

				/// \brief Return previous key frame data of specified frame time.
				virtual std::optional<BaseKeyframe<Type>> getPreviousKeyFrame(uint64 frameTime) = 0;

				/// \brief Return next key frame data of specified frame time.
				virtual std::optional<BaseKeyframe<Type>> getNextKeyFrame(uint64 frameTime) = 0;

				/// \brief Return key frame data at specified frame time.
				virtual std::optional<BaseKeyframe<Type>> keyFrame(uint64 frameTime) = 0;

				/// \brief Return value of the animation in the specified frame time.
				virtual Type peek(uint64 frameTime) const = 0;

				/// \brief Get previous and next value of keys that is related to a virtual key in
				/// specified time
				virtual MappedValues getMappedValues(uint64 frameTime) const = 0;

				/// \brief Return the keys frames data that is valid at specified time. This will
				/// return std::nullopt if the animation contains no key frame.
				virtual MappedKeys getMappedKeys(uint64 frameTime) const = 0;

				virtual Core::System::Event<bool, Type>& valueChanged()		  = 0;
				virtual Core::System::Event<void>&		 jumppedToBegin()	  = 0;
				virtual Core::System::Event<void>&		 jumppedToEnd()		  = 0;
				virtual Type							 currentValue() const = 0;
				virtual uint64							 size() const		  = 0;

				/// \brief Return list of key-frames that is available in this animation engine.
				///
				/// \note This list isn't updated automatically and need to call this function to
				/// generate the updated list of key-frames.
				virtual KeyFrameList<Type> keyFrames() const = 0;
			};
			template<typename Type>
			using AnimationEngineWRef = std::weak_ptr<AnimationEngine<Type>>;
			template<typename Type>
			using AnimationEngineRef = std::shared_ptr<AnimationEngine<Type>>;
			template<typename Type>
			using AnimationEngineURef = std::unique_ptr<AnimationEngine<Type>>;
		} // namespace Animation
	}	  // namespace Core
} // namespace DGE
