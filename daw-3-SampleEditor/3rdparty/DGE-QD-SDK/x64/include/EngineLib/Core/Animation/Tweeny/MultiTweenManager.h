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
#include <Core/Animation/Tweeny/TweensContainer.h>
#include <Core/System/Event.h>
#include <tuple>

namespace DGE
{
	namespace Core
	{
		namespace Animation
		{
			namespace Tweeny
			{
				enum class SeekDirection
				{
					Forward,
					Backward
				};

				template<template<typename... TweenValues> class TweenType,
						 typename TweenTransitionType,
						 typename TimeType = uint64,
						 typename... Value>
				class MultiTweenManager
					: public TweensContainer<TweenType, TweenTransitionType, TimeType, Value...>
					, public IAnimationController
				{
					using TweensContainerParent
						= TweensContainer<TweenType, TweenTransitionType, TimeType, Value...>;
					using TweenKeyFrameTypeRef = typename TweensContainerParent::TweenKeyFrameTypeRef;
					using TupleValue = typename TweensContainerParent::TupleValue;
					using ParentTweensContainer
						= TweensContainer<TweenType, TweenTransitionType, TimeType, Value...>;

				public:
					MultiTweenManager()
					{
						m_numberLoops = 0;
						m_isFinished  = true;
					}

					virtual bool onStep(Value... value)
					{
						m_currentKeyValue = {value...};
						return false;
					}

					virtual bool onSeek(Value... value)
					{
						m_currentKeyValue = {value...};
						return false;
					}

					// IAnimationController interface
					virtual bool nextFrame(uint64 elapsedTime) override
					{
						if (elapsedTime == 0)
							return false;
						updateKey();
						if (m_currentActiveKey)
						{
							m_currentActiveKey->tween.step((uint32) elapsedTime);
							return true;
						}
						return false;
					}
					virtual bool previousFrame(uint64 elapsedTime) override
					{
						if (elapsedTime == 0)
							return false;
						updateKey(SeekDirection::Backward);
						if (m_currentActiveKey)
						{
							m_currentActiveKey->tween.step(-(int32) elapsedTime);
							return true;
						}
						return false;
					}

					TweenKeyFrameTypeRef findFrame(uint64 frameTime) const
					{
						if (!m_currentActiveKey->containTime(frameTime))
						{
							auto it = this->m_tweensMap.find({frameTime});
							if (it != this->m_tweensMap.end())
							{
								return it->second;
							}
							else if (!this->m_tweensMap.empty())
							{
								if (frameTime <= this->m_wholeRange.low()
									&& this->m_wholeRange.isLowValid())
									return this->m_tweensMap.begin()->second;
								else if (frameTime >= this->m_wholeRange.high()
										 && this->m_wholeRange.isHighValid())
								{
									auto endIt = --this->m_tweensMap.end();
									return endIt->second;
								}
								else
									return this->m_tweensMap.begin()->second;
							}
							else
								return nullptr;
						}

						return m_currentActiveKey;
					}

					virtual bool seekToFrame(uint64 frameTime) override
					{
						if (m_currentActiveKey)
						{
							if (!m_currentActiveKey->containTime(frameTime))
							{
								auto it = this->m_tweensMap.find({frameTime});
								if (it != this->m_tweensMap.end())
								{
									m_currentActiveKey = it->second;
								}
								else if (!this->m_tweensMap.empty())
								{
									if (frameTime <= this->m_wholeRange.low()
										&& this->m_wholeRange.isLowValid())
										m_currentActiveKey = this->m_tweensMap.begin()->second;
									else if (frameTime >= this->m_wholeRange.high()
											 && this->m_wholeRange.isHighValid())
									{
										auto endIt		   = --this->m_tweensMap.end();
										m_currentActiveKey = endIt->second;
									}
									else
										return false;
								}
								else
									return false;
							}
							/// \note Here we calculate distance to begin of active (current tween)
							/// key-frame because we have multiple independent tween object as a
							/// key-frame. So we must active current one and set correct time
							/// position for it.
							uint32_t exactTime = frameTime > m_currentActiveKey->fromKey.time
													 ? (frameTime - m_currentActiveKey->fromKey.time)
													 : 0;
							peekCurrent(exactTime);
							m_currentActiveKey->tween.seek(exactTime);
							return true;
						}
						return false;
					}

					virtual bool isFinsihed() const override
					{
						if (m_currentActiveKey
							&& m_currentActiveKey->toKey.attachTween == this->m_firstKey)
						{
							return m_currentActiveKey->tween.isFinished();
						}
						return false;
					}
					virtual uint64 currentTime() const override
					{
						if (m_currentActiveKey)
						{
							return (m_currentActiveKey->fromKey.time
									+ m_currentActiveKey->tween.currentTimePoint());
						}
						return 0;
					}

					virtual std::tuple<Value...> currentTweenValue() const { return {}; }

					int32 numberLoops() const { return m_numberLoops; }
					void  setNumberLoops(int32 newNumberLoops)
					{
						if (m_numberLoops != newNumberLoops)
							m_numberLoops = newNumberLoops;
					}
					virtual bool modifyKeyFrame(TimeType timeId, const TupleValue &value) override
					{
						return ParentTweensContainer::modifyKeyFrame(timeId, value);
					}
					virtual bool modifyKeyFrame(TimeType timeId, Value... value) override
					{
						auto key = ParentTweensContainer::findFromKeyFrame(timeId);
						if (key)
						{
							typename ParentTweensContainer::KeyPoint fromKey = key->fromKey;
							typename ParentTweensContainer::KeyPoint toKey	 = key->toKey;
							// TweenType<Value...>			   tween	   = key->tween;
							if (ParentTweensContainer::removeKeyFrame(timeId))
							{
								if (fromKey.time == timeId)
								{
									TransformerRef<TransitionType> transformer = key->fromKey
																					 .transformer;
									if (fromKey.isInitialized)
										return ParentTweensContainer::addTweenKeyFrame(
											fromKey.time, value..., fromKey.transition, transformer);
									else
										LogError(DST "Was found a key frame with uninitialized "
													 "fromKey value by id: "
												 + timeId);
								}
								else if (toKey.time == timeId)
								{
									TransformerRef<TransitionType> transformer = key->toKey
																					 .transformer;
									if (toKey.isInitialized)
										return ParentTweensContainer::addTweenKeyFrame(
											toKey.time, value..., toKey.transition, transformer);
									else
										LogError(DST "Was found a key frame with uninitialized "
													 "toKey value by id: "
												 + timeId);
								}
								else
									LogError(DST "Was found a key frame with mismatched from and "
												 "to key frame value in time value by id: "
											 + timeId);
							}
							else
							{
								LogError(DST "Can't find key frame with id: " + timeId);
							}
						}
						return false;
					}
					virtual bool modifyKeyFrame(TimeType timeId, TimeType frameTime) override
					{
						auto testKey = ParentTweensContainer::findFromKeyFrame(frameTime);
						if (testKey
							&& (testKey->fromKey.time == frameTime
								|| testKey->toKey.time == frameTime))
							return false;
						auto key = ParentTweensContainer::findFromKeyFrame(timeId);
						if (key)
						{
							typename ParentTweensContainer::KeyPoint fromKey = key->fromKey;
							typename ParentTweensContainer::KeyPoint toKey	 = key->toKey;
							// TweenType<Value...>			   tween	   = key->tween;
							if (ParentTweensContainer::removeKeyFrame(timeId))
							{
								if (fromKey.time == timeId)
								{
									TransformerRef<TransitionType> transformer = key->fromKey
																					 .transformer;
									if (fromKey.isInitialized)
										return addKeyFrame(frameTime,
														   fromKey.value,
														   fromKey.transition,
														   transformer);
									else
										LogError(DST "Was found a key frame with uninitialized "
													 "fromKey value by id: "
												 + timeId);
								}
								else if (toKey.time == timeId)
								{
									TransformerRef<TransitionType> transformer = key->toKey
																					 .transformer;
									if (toKey.isInitialized)
										return addKeyFrame(frameTime,
														   toKey.value,
														   toKey.transition,
														   transformer);
									else
										LogError(DST "Was found a key frame with uninitialized "
													 "toKey value by id: "
												 + timeId);
								}
								else
									LogError(DST "Was found a key frame with mismatched from and "
												 "to key frame value in time value by id: "
											 + timeId);
							}
							else
							{
								LogError(DST "Can't find key frame with id: " + timeId);
							}
						}
						return false;
					}
					virtual bool modifyKeyFrame(TimeType			  timeId,
												Core::Utility::AcTime frameTime) override
					{
						return modifyKeyFrame(timeId, frameTime.toMilliSecond());
					}
					virtual bool modifyKeyFrame(TimeType	   timeId,
												TransitionType transitionType) override
					{
						return true;
					}
					virtual bool modifyKeyFrame(TimeType timeId, Core::Utility::AcTime frameTime,
												const TupleValue &value,
												TransitionType	  transitionType) override
					{
						return true;
					}

				protected:
					void gotoBegin()
					{
						m_isFinished	   = false;
						m_currentActiveKey = this->m_firstKey;
						m_currentActiveKey->tween.seek(0.0f);
						peekCurrent();
					}
					void peekCurrent(float time)
					{
						auto [x, y, z]	  = m_currentActiveKey->tween.peek(time);
						m_currentKeyValue = {x, y, z};
					}
					void peekCurrent(uint32 time = 0)
					{
						auto [x, y, z]	  = m_currentActiveKey->tween.peek(time);
						m_currentKeyValue = {x, y, z};
					}
					TupleValue peekFrame(uint32 time) const
					{
						auto frame = findFrame(time);
						if (frame)
						{
							if (time < frame->fromKey.time)
								return frame->fromKey.value;
							if (time > frame->toKey.time)
								return frame->toKey.value;
							auto [x, y, z] = frame->tween.peek(uint32(time - frame->fromKey.time));
							return {x, y, z};
						}
						return {0, 0, 0};
					}
					TupleValue peekFrame(float time) { return peekFrame((uint32) time); }

					bool addKeyFrame(TimeType time, const TupleValue &value,
									 TransitionType transitionType = TransitionType::Linear,
									 TransformerRef<TransitionType> transformerRef = nullptr)
					{
						return addKeyFrameWithTuple(time, value, transitionType, transformerRef,
													std::index_sequence_for<Value...>());
					}

					template<std::size_t... Is>
					bool addKeyFrameWithTuple(TimeType time, const TupleValue &value,
											  TransitionType				 transitionType,
											  TransformerRef<TransitionType> transformerRef,
											  std::index_sequence<Is...>)
					{
						return ParentTweensContainer::addTweenKeyFrame(time, std::get<Is>(value)...,
																	   transitionType,
																	   transformerRef);
					}

					virtual void setEventFunctions(TweenKeyFrameTypeRef newKey) override
					{
						newKey->tween.onStep([this](Value... value)
											 { return this->onStep(value...); });
						newKey->tween.onSeek([this](Value... value)
											 { return this->onSeek(value...); });
					}
					void updateKey(SeekDirection seekDirection = SeekDirection::Forward)
					{
						bool isUpdated = false;
						if (!m_currentActiveKey)
						{
							isUpdated = true;
							gotoBegin();
						}
						auto switchPoint = seekDirection == SeekDirection::Forward ? 1.0f : 0.0f;
						auto beginPoint	 = seekDirection == SeekDirection::Forward ? 0.0f : 1.0f;
						if (m_currentActiveKey
							&& m_currentActiveKey->tween.progress() == switchPoint)
						{
							auto moveKey = seekDirection == SeekDirection::Forward
											   ? m_currentActiveKey->toKey.attachTween
											   : m_currentActiveKey->fromKey.attachTween;
							if (moveKey)
							{
								isUpdated		   = true;
								if (moveKey == this->m_firstKey)
									m_isFinished = true;
								m_currentActiveKey = moveKey;
							}
							else if (m_numberLoops == 0)
							{
								m_isFinished = true;
							}
							else
							{
								if (m_numberLoops > 0)
									m_numberLoops--;
								isUpdated = true;
								gotoBegin();
							}
						}
						else
						{
							m_isFinished = false;
						}
						if (m_currentActiveKey && m_currentActiveKey->tween.progress() != beginPoint
							&& isUpdated)
						{
							m_currentActiveKey->tween.seek(beginPoint);
							peekCurrent(beginPoint);
						}
					}

					virtual void updateCurrentFrame()
					{
						m_currentActiveKey = this->m_firstKey;
						seekToFrame(currentTime());
					}

				protected:
					TweenKeyFrameTypeRef						m_currentActiveKey;
					bool										m_isFinished;
					int32										m_numberLoops;
					TupleValue									m_currentKeyValue;
				};
			} // namespace Tweeny
		}	  // namespace Animation
	}		  // namespace Core
} // namespace DGE
