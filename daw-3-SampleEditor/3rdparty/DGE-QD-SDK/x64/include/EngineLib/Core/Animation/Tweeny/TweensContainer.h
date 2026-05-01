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
#include "Core/Animation/Transformer.h"
#include "Core/Math/Range.h"
#include <Core/Animation/IAnimationController.h>
#include <Core/Animation/Tweeny/TweenKeyFrame.h>
#include <memory>
#include <utility>

using namespace DGE::Core::Math;

namespace DGE
{
	namespace Core
	{
		namespace Animation
		{
			namespace Tweeny
			{

				template<template<typename... TweenValues> class TweenType,
						 typename TweenTransitionType, typename TimeType = uint64, typename... Value>
				class TweensContainer
				{
				public:
					using TweenKeyFrameType
						= TweenKeyFrame<TweenType, TweenTransitionType, TimeType, Value...>;
					using KeyPoint			   = typename TweenKeyFrameType::KeyPoint;
					using TweenKeyFrameTypeRef = typename TweenKeyFrameType::TweenKeyFrameTypeRef;
					using TupleValue		   = typename TweenKeyFrameType::TupleValue;
					using TweenMapType		   = std::map<Range<TimeType>, TweenKeyFrameTypeRef>;
					using TweenMapIteratorType =
						typename std::map<Range<TimeType>, TweenKeyFrameTypeRef>::iterator;

				public:
					TweensContainer() { m_wholeRange = {0}; }

					/*tweeny::tween<Value...>&*/

					bool addTweenKeyFrame(TimeType		 time, Value... value,
										  TransitionType transitionType = TransitionType::Linear,
										  TransformerRef<TransitionType> transformerRef = nullptr)
					{
						auto it = m_tweensMap.find({time});
						if (it != m_tweensMap.end())
						{
							if (it->first.isIncompleted())
							{
								auto changeKey = it->second;
								m_tweensMap.erase(m_wholeRange);
								// m_wholeKey.normalize();
								//   m_wholeRange.nomalize();
								//  auto maxValue = m_wholeKey.fromKey.time > m_wholeKey.toKey.time
								//  					? m_wholeKey.fromKey.time
								//  					: m_wholeKey.toKey.time;
								auto compareValue = m_wholeRange.completePart()
															== CompletePart::High
														? m_wholeRange.high()
														: m_wholeRange.low();
								if (time > m_wholeRange.high())
									m_wholeRange.setHigh(time);
								else if (time > m_wholeRange.low())
									m_wholeRange.setLow(time);
								if (!m_wholeKey.fromKey.isInitialized)
								{
									m_wholeKey.fromKey.time			 = time;
									m_wholeKey.fromKey.value		 = {value...};
									m_wholeKey.fromKey.transition	 = transitionType;
									m_wholeKey.fromKey.transformer	 = transformerRef;
									m_wholeKey.fromKey.isInitialized = true;
								}
								else
								{
									m_wholeKey.toKey.time		   = time;
									m_wholeKey.toKey.value		   = {value...};
									m_wholeKey.toKey.transition	   = transitionType;
									m_wholeKey.toKey.transformer   = transformerRef;
									m_wholeKey.toKey.isInitialized = true;
								}
								if (!changeKey->fromKey.isInitialized)
								{
									changeKey->fromKey.time			 = time;
									changeKey->fromKey.value		 = {value...};
									changeKey->fromKey.transition	 = transitionType;
									changeKey->fromKey.isInitialized = true;
									changeKey->fromKey.transformer	 = transformerRef;
								}
								else
								{
									changeKey->toKey.time		   = time;
									changeKey->toKey.value		   = {value...};
									changeKey->toKey.transition	   = transitionType;
									changeKey->toKey.isInitialized = true;
									changeKey->toKey.transformer   = transformerRef;
								}

								m_wholeRange.setIsIncompleted(false);

								m_wholeKey.normalize();
								m_wholeRange = m_wholeKey.timeRange();
								changeKey->normalizeAndUpdate();
								m_firstKey = changeKey;

								setEventFunctions(changeKey);

								insertKeyFrame(changeKey);
							}
							else if (it->first.low() != time && it->first.high() != time)
							{
								/// [low ... time ... high]
								auto	 oldRange		  = it->first;
								auto	 oldTweenKeyFrame = it->second;
								KeyPoint middleKey		  = {time, {value...}, transitionType};
								splitKeyFrame(middleKey, oldRange, transformerRef, oldTweenKeyFrame);
							}
							else
							{
								printf("Why\n");
								return false;
							}
						}
						else
						{
							if (m_tweensMap.empty())
							{
								if (m_wholeRange.high() == m_wholeRange.low()
									&& m_wholeRange.high() == 0)
								{
									m_wholeRange				= {0, time, CompletePart::High};
									m_wholeKey.toKey.time		= time;
									m_wholeKey.toKey.value		= {value...};
									m_wholeKey.toKey.transition = transitionType;
									m_wholeKey.toKey.transformer	 = transformerRef;
									m_wholeKey.fromKey.isInitialized = false;
									auto newKey					  = makeKeyFrame(m_wholeRange,
																				 m_wholeKey.fromKey,
																				 m_wholeKey.toKey);

									newKey->fromKey.attachTween = newKey->toKey.attachTween = newKey;
									newKey->fromKey.isInitialized							= false;
									newKey->toKey.isInitialized								= true;
									// newKey->normalizeAndUpdate();

									m_firstKey = newKey;
									setEventFunctions(newKey);

									insertKeyFrame(m_wholeRange, newKey);
								}
								else
								{
									printf("Why empty map with not initialize list!\n");
									return false;
								}
							}
							else if (m_wholeRange > time)
							{
								/// (new key)[time ... low] <-> [low ... high]

								auto high = m_wholeRange.low();
								m_wholeRange.setLow(time);
								auto			firstKeyFrame = m_tweensMap.begin();
								auto			lastKeyFrame  = (--m_tweensMap.end());
								Range<TimeType> newRange	  = {time, high};

								auto newKeyFrame = makeKeyFrame(newRange, {value...},
																m_wholeKey.fromKey.value,
																transitionType,
																m_wholeKey.fromKey.transition,
																lastKeyFrame->second,
																firstKeyFrame->second,
																transformerRef,
																m_wholeKey.fromKey.transformer);
								newKeyFrame->normalizeAndUpdate();
								setEventFunctions(newKeyFrame);
								m_firstKey								   = newKeyFrame;
								firstKeyFrame->second->fromKey.attachTween = newKeyFrame;
								lastKeyFrame->second->toKey.attachTween	   = newKeyFrame;
								m_wholeKey.fromKey.time					   = time;
								m_wholeKey.fromKey.value				   = {value...};
								m_wholeKey.fromKey.transition			   = transitionType;
								m_wholeKey.fromKey.transformer			   = transformerRef;
								insertKeyFrame(newKeyFrame);
							}
							else if (m_wholeRange < time)
							{
								///  [low ... high] <-> [high ... time](new key)

								auto low = m_wholeRange.high();
								m_wholeRange.setHigh(time);
								auto			lastKeyFrame  = (--m_tweensMap.end());
								auto			firstKeyFrame = m_tweensMap.begin();
								Range<TimeType> newRange	  = {low, time};

								auto newKeyFrame = makeKeyFrame(newRange, m_wholeKey.toKey.value,
																{value...},
																m_wholeKey.toKey.transition,
																transitionType,
																lastKeyFrame->second,
																firstKeyFrame->second,
																m_wholeKey.toKey.transformer,
																transformerRef);
								newKeyFrame->normalizeAndUpdate();
								setEventFunctions(newKeyFrame);
								lastKeyFrame->second->toKey.attachTween	   = newKeyFrame;
								firstKeyFrame->second->fromKey.attachTween = newKeyFrame;
								m_wholeKey.toKey.time					   = time;
								m_wholeKey.toKey.value					   = {value...};
								m_wholeKey.toKey.transition				   = transitionType;
								m_wholeKey.toKey.transformer			   = transformerRef;
								insertKeyFrame(newKeyFrame);
							}
							else
							{
								printf("Why Equal\n");
								return false;
							}
						}

						return true;
					}

					auto begin() const { return m_tweensMap.begin(); }
					auto end() const { return m_tweensMap.end(); }

					/// \brief Modify a key-frame with specified id and changed property of key-frame
					virtual bool modifyKeyFrame(TimeType timeId, const TupleValue &value)
					{
						return modifyKeyFrameValue(timeId, value,
												   std::index_sequence_for<Value...>());
					}
					virtual bool modifyKeyFrame(TimeType timeId, TimeType frameTime)
					{
						return true;
					}
					virtual bool modifyKeyFrame(TimeType timeId, Value... value) { return true; }
					virtual bool modifyKeyFrame(TimeType timeId, Core::Utility::AcTime frameTime)
					{
						return true;
					}
					virtual bool modifyKeyFrame(TimeType timeId, TransitionType transitionType)
					{
						return true;
					}
					virtual bool modifyKeyFrame(TimeType timeId, Core::Utility::AcTime frameTime,
												const TupleValue &value,
												TransitionType	  transitionType)
					{
						return true;
					}

					/// \brief Remove a key-frame with specified id
					virtual bool removeKeyFrame(TimeType timeId)
					{
						auto it = findFromKeyFrame(timeId, true);
						if (it == m_tweensMap.end())
							return false;
						auto key = it->second;
						if (key)
						{
							/// Is exactly equal timeId with value of fromKey
							if (key->fromKey.time == timeId || key->toKey.time == timeId)
							{
								// Check whether must be changed neighbor key-frame
								bool needChangeNeighbor = key->fromKey.time == timeId
														  && key->fromKey.attachTween
																 != m_firstKey->fromKey.attachTween;
								/// Is last or first link?
								if (key->toKey.attachTween == m_firstKey || key == m_firstKey)
								{
									/// Check this link is not latest exist link
									if (key->toKey.attachTween != key)
									{
										key->toKey.attachTween->fromKey.attachTween
											= key->fromKey.attachTween;
										key->fromKey.attachTween->toKey.attachTween
											= key->toKey.attachTween;
										if (key == m_firstKey)
										{
											m_firstKey = key->toKey.attachTween;
											m_wholeRange.setLow(m_firstKey->fromKey.time);
											m_wholeKey.fromKey = m_firstKey->fromKey;
										}
										else if (key->toKey.attachTween == m_firstKey) /// last node
										{
											auto newLastKey = key->fromKey.attachTween;
											if (key->toKey.time == timeId)
											{
												m_wholeRange.setHigh(newLastKey->toKey.time);
												m_wholeKey.toKey = newLastKey->toKey;
											}
											else
											{
												newLastKey->toKey.time	= key->toKey.time;
												newLastKey->toKey.value = key->toKey.value;
												newLastKey->toKey.transition = key->toKey.transition;
												newLastKey->toKey.transformer = key->toKey
																					.transformer;
												newLastKey->normalizeAndUpdate();
												setEventFunctions(newLastKey);
											}
										}
										else
										{
											printf("Why!\n");
											return false;
										}
									}
									else
									{
										/// this link is latest link
										// m_wholeRange = {0};
										// m_wholeKey	 = {};
										if (key->toKey.time == timeId)
										{
											key->fromKey.attachTween = key->toKey.attachTween = key;
											m_wholeRange = {key->fromKey.time, 0, CompletePart::Low};
											if (key->fromKey.isInitialized)
											{
												m_wholeKey.fromKey.time		  = key->fromKey.time;
												m_wholeKey.fromKey.value	  = key->fromKey.value;
												m_wholeKey.fromKey.transition = key->fromKey
																					.transition;
												m_wholeKey.fromKey.transformer = key->fromKey
																					 .transformer;
											}
											else
											{
												/// \note Reset this container to show an empty one.
												m_wholeKey.fromKey.isInitialized = false;
												m_firstKey						 = nullptr;
												key->toKey.attachTween			 = nullptr;
												key->fromKey.attachTween		 = nullptr;
												m_wholeRange					 = {0};
											}
											m_wholeKey.toKey.isInitialized = false;
											key->toKey.isInitialized	   = false;
											m_tweensMap.erase(it);
											if (key->fromKey.isInitialized)
											{
												m_firstKey = key;
												m_tweensMap.insert({m_wholeRange, key});
												setEventFunctions(key);
											}
											updateCurrentFrame();
											return true;
										}
										else if (key->fromKey.time == timeId)
										{
											key->fromKey.attachTween = key->toKey.attachTween = key;
											m_wholeRange = {key->toKey.time, 0, CompletePart::Low};
											if (key->toKey.isInitialized)
											{
												m_wholeKey.fromKey.time	 = key->toKey.time;
												m_wholeKey.fromKey.value = key->toKey.value;
												m_wholeKey.fromKey.transition = key->toKey.transition;
												m_wholeKey.fromKey.transformer = key->toKey
																					 .transformer;
											}
											else
											{
												/// \note Reset this container to show an empty one.
												m_wholeKey.fromKey.isInitialized = false;
												m_firstKey						 = nullptr;
												key->toKey.attachTween			 = nullptr;
												key->fromKey.attachTween		 = nullptr;
												m_wholeRange					 = {0};
											}
											m_wholeKey.toKey.isInitialized = false;
											key->fromKey.isInitialized	   = false;
											m_tweensMap.erase(it);
											if (key->toKey.isInitialized)
											{
												m_firstKey = key;
												setEventFunctions(key);
												m_tweensMap.insert({m_wholeRange, key});
											}
											updateCurrentFrame();
											return true;
										}
										return false;
									}
								}
								else
								{
									/// middle link!
									key->fromKey.attachTween->toKey				= key->toKey;
									key->toKey.attachTween->fromKey.attachTween = key->fromKey
																					  .attachTween;
									key->fromKey.attachTween->normalizeAndUpdate();
									setEventFunctions(key->fromKey.attachTween);
								}
								if (needChangeNeighbor)
								{
									/// (changeKey)[from ... to]  (key)[from=timeId ... to]
									///
									/// change into:
									///
									/// [from ... to of key] and remove key frame
									auto changeKey	 = key->fromKey.attachTween;
									auto changeRange = changeKey->timeRange();
									auto itHandler	 = m_tweensMap.extract(changeRange);
									changeRange.setHigh(key->toKey.time);
									itHandler.key() = changeRange;
									m_tweensMap.insert(std::move(itHandler));
								}
								m_tweensMap.erase(it);
								updateCurrentFrame();
								return true;
							}
						}
						return false;
					}

					TweenKeyFrameTypeRef findFromKeyFrame(TimeType timeId)
					{
						auto it = findFromKeyFrame(timeId, true);
						if (it == m_tweensMap.end())
							return nullptr;
						return it->second;
					}

					TweenMapIteratorType findFromKeyFrame(TimeType timeId, bool catchLastLink)
					{
						auto it = m_tweensMap.find({timeId});
						if (it == m_tweensMap.end())
						{
							if (!m_tweensMap.empty() && catchLastLink)
							{
								auto nextKey = it;
								nextKey--;
								if (nextKey->second->toKey.time == timeId)
									return nextKey;
							}
						}
						return it;
					}

					TweenKeyFrameTypeRef findEndKeyFrame(TimeType timeId)
					{
						auto it = findEndKeyFrame(timeId, true);
						if (it == m_tweensMap.end())
							return nullptr;
						if (it->second != m_firstKey && it->second->fromKey.time == timeId)
						{
							/// return previous key to catch to key
							return it->second->fromKey.attachTween;
						}
						return it->second;
					}

					TweenMapIteratorType findEndKeyFrame(TimeType timeId, bool catchLastLink)
					{
						auto it = m_tweensMap.find({0, timeId, CompletePart::High});
						if (it == m_tweensMap.end())
						{
							if (!m_tweensMap.empty())
							{
								auto nextKey = it;
								nextKey--;
								if (nextKey->second->toKey.time == timeId)
									return nextKey;
							}
						}
						return it;
					}

					TweenKeyFrameTypeRef findMappedKeyFrames(TimeType time) const
					{
						auto it = m_tweensMap.find({time});
						if (it != m_tweensMap.end())
						{
							if (it->first.isIncompleted())
							{
								TweenKeyFrameTypeRef newKeyFrame
									= std::make_shared<TweenKeyFrameType>();
								auto compareValue = m_wholeRange.completePart()
															== CompletePart::High
														? m_wholeRange.high()
														: m_wholeRange.low();
								if (compareValue > time)
								{
									newKeyFrame->toKey				   = m_wholeRange.completePart()
																 == CompletePart::High
																			 ? m_wholeKey.toKey
																			 : m_wholeKey.fromKey;
									newKeyFrame->fromKey.isInitialized = false;
								}
								else
								{
									newKeyFrame->fromKey			 = m_wholeRange.completePart()
																   == CompletePart::High
																		   ? m_wholeKey.toKey
																		   : m_wholeKey.fromKey;
									newKeyFrame->toKey.isInitialized = false;
								}
								return newKeyFrame;
							}
							else if (it->first.low() != time && it->first.high() != time)
							{
								/// [low ... time ... high]
								return it->second;
							}
							else
							{
								printf("Why\n");
								return nullptr;
							}
						}
						else
						{
							if (m_tweensMap.empty())
							{
								return nullptr;
							}
							else if (m_wholeRange > time)
							{
								/// (new key)[time ... low] <-> [low ... high]
								TweenKeyFrameTypeRef newKeyFrame
									= std::make_shared<TweenKeyFrameType>();
								newKeyFrame->toKey				   = m_wholeKey.fromKey;
								newKeyFrame->fromKey.isInitialized = false;
								return newKeyFrame;
							}
							else if (m_wholeRange < time)
							{
								///  [low ... high] <-> [high ... time](new key)

								TweenKeyFrameTypeRef newKeyFrame
									= std::make_shared<TweenKeyFrameType>();
								newKeyFrame->fromKey			 = m_wholeKey.toKey;
								newKeyFrame->toKey.isInitialized = false;
								return newKeyFrame;
							}
							else
							{
								printf("Why Equal\n");
							}
						}

						return nullptr;
					}

					uint64 size() const { return m_tweensMap.size(); }

					/// \brief Indicate whether a key-frame with specified id is existed.
					virtual bool isExistKeyFrame(TimeType id) const
					{
						auto keyFrame = m_tweensMap.find({id});
						return keyFrame != m_tweensMap.end() && !keyFrame->first.isIncompleted();
					}

				protected:
					virtual void setEventFunctions(TweenKeyFrameTypeRef newKey) {}
					virtual void updateCurrentFrame() {}

				private:
					void splitKeyFrame(const KeyPoint		 &newKey,
									   const Range<TimeType> &oldRange,
									   TransformerRef<TransitionType>
															transformerRef,
									   TweenKeyFrameTypeRef oldTweenData)
					{
						/// [low ... time ... high] = [low ... time] <-> [time ... high](new key)
						Range<TimeType> range1		= {oldRange.low(), newKey.time};
						Range<TimeType> range2		= {newKey.time, oldRange.high()};
						auto			newKeyFrame = makeKeyFrame(range2, newKey.value,
																   oldTweenData->toKey.value,
																   newKey.transition,
																   oldTweenData->toKey.transition,
																   oldTweenData,
																   oldTweenData->toKey.attachTween,
																   transformerRef,
																   oldTweenData->toKey.transformer);

						if (oldTweenData->toKey.attachTween)
							oldTweenData->toKey.attachTween->fromKey.attachTween = newKeyFrame;
						oldTweenData->toKey.setValues(newKey.time, newKey.value, newKey.transition,
													  newKeyFrame, transformerRef);
						oldTweenData->normalizeAndUpdate();
						newKeyFrame->normalizeAndUpdate();
						setEventFunctions(oldTweenData);
						setEventFunctions(newKeyFrame);
						m_tweensMap.erase(oldRange);
						m_tweensMap.insert({range1, oldTweenData});
						m_tweensMap.insert({range2, newKeyFrame});
					}

					void insertKeyFrame(TweenKeyFrameTypeRef keyFrame)
					{
						insertKeyFrame(keyFrame->timeRange(), keyFrame);
					}
					void insertKeyFrame(const Range<TimeType> &newRange,
										TweenKeyFrameTypeRef   keyFrame)
					{
						m_tweensMap.insert({newRange, keyFrame});
					}

					/*void insertKeyFrame(const Range<TimeType> &newRange, Value... from, Value... to,
										TransitionType		   fromTransitionType,
										TransitionType		   toTransitionType,
										TweenKeyFrameTypeRef   previousTween,
										TweenKeyFrameTypeRef   nextTween)
					{
						auto newKeyFrame = makeKeyFrame(newRange, {from...}, {to...},
														fromTransitionType, toTransitionType,
														previousTween, nextTween);
						newKeyFrame.normalizeAndUpdate();

						insertKeyFrame(newKeyFrame);
					}*/

					TweenKeyFrameTypeRef makeKeyFrame(const Range<TimeType> &newRange,
													  KeyPoint				&from,
													  KeyPoint				&to)
					{
						return makeKeyFrame(newRange, from.value, to.value, from.transition,
											to.transition, from.attachTween, to.attachTween,
											from.transformer, to.transformer);
					}

					TweenKeyFrameTypeRef makeKeyFrame(const Range<TimeType> &newRange,
													  TupleValue			 fromValue,
													  TupleValue			 toValue,
													  TransitionType		 fromTransitionType,
													  TransitionType		 toTransitionType,
													  TweenKeyFrameTypeRef	 previousTween,
													  TweenKeyFrameTypeRef	 nextTween,
													  TransformerRef<TransitionType>
														  fromTransformer,
													  TransformerRef<TransitionType>
														  toTransformer)
					{
						auto newKeyFrame = std::make_shared<TweenKeyFrameType>();
						newKeyFrame->setKeys(newRange,
											 fromValue,
											 toValue,
											 fromTransitionType,
											 toTransitionType,
											 previousTween,
											 nextTween);
						newKeyFrame->fromKey.transformer = fromTransformer;
						newKeyFrame->toKey.transformer	 = toTransformer;

						return newKeyFrame;
					}

					template<std::size_t... Is>
					bool modifyKeyFrameValue(TimeType timeId, const TupleValue &tuple,
											 std::index_sequence<Is...>)
					{
						return modifyKeyFrame(timeId, std::get<Is>(tuple)...);
					}

				protected:
					TweenMapType		 m_tweensMap;
					TweenKeyFrameType	 m_wholeKey;
					Range<TimeType>		 m_wholeRange;
					TweenKeyFrameTypeRef m_firstKey;
				};
			} // namespace Tweeny
		}	  // namespace Animation
	}		  // namespace Core
} // namespace DGE
