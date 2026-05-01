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
#include <Core/Animation/BaseKeyframe.h>
#include <Core/Animation/Transformer.h>
#include <Core/IO/LogSystem/Log.h>
#include <Core/Math/Range.h>
#include <Core/Utility/Utility.h>
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
				struct TweenKeyFrame
				{
					using TupleValue		   = std::tuple<Value...>;
					using TweenKeyFrameTypeRef = std::shared_ptr<
						TweenKeyFrame<TweenType, TweenTransitionType, TimeType, Value...>>;
					struct KeyPoint
					{
						TimeType					   time;
						TupleValue					   value;
						TransitionType				   transition;
						std::shared_ptr<TweenKeyFrame> attachTween = nullptr;
						TransformerRef<TransitionType> transformer;
						bool						   isInitialized;

						KeyPoint(bool isInitialized = true)
							: KeyPoint(0, {}, (TransitionType) 0)
						{
							this->isInitialized = isInitialized;
						}

						KeyPoint(TimeType						timeValue,
								 TupleValue						tupleValues,
								 TransitionType					transitionValue,
								 std::shared_ptr<TweenKeyFrame> attachTweenValue = nullptr,
								 TransformerRef<TransitionType> transformerValue = nullptr)
							: time(timeValue)
							, value(tupleValues)
							, transition(transitionValue)
							, attachTween(attachTweenValue)
							, transformer(transformerValue)
						{
							this->isInitialized = true;
						}

						void swap(KeyPoint &key)
						{
							std::swap(time, key.time);
							std::swap(value, key.value);
							std::swap(transition, key.transition);
							std::swap(attachTween, key.attachTween);
							std::swap(transformer, key.transformer);
							this->isInitialized = true;
						}
						KeyPoint &operator=(const KeyPoint &key)
						{
							time				= key.time;
							value				= key.value;
							transition			= key.transition;
							attachTween			= key.attachTween;
							transformer			= key.transformer;
							this->isInitialized = true;
							return *this;
						}
						void setValues(TimeType time, TupleValue value, TransitionType transition,
									   std::shared_ptr<TweenKeyFrame> attachTween,
									   TransformerRef<TransitionType> transformerValue)
						{
							this->time			= time;
							this->value			= value;
							this->transition	= transition;
							this->attachTween	= attachTween;
							this->transformer	= transformerValue;
							this->isInitialized = true;
						}
					};

					KeyPoint					   fromKey;
					KeyPoint					   toKey;
					TweenType<Value...>			   tween;

					TweenKeyFrame() {}

					void setKeys(const Range<TimeType> &newRange, TupleValue fromValue,
								 TupleValue toValue, TransitionType fromTransitionType,
								 TransitionType		  toTransitionType,
								 TweenKeyFrameTypeRef previousTween, TweenKeyFrameTypeRef nextTween)
					{
						fromKey = {newRange.low(), fromValue, fromTransitionType, previousTween};
						toKey	= {newRange.high(), toValue, toTransitionType, nextTween};
					}

					TweenKeyFrame &from(std::tuple<Value...> &tuple)
					{
						return setFrom(tuple, std::index_sequence_for<Value...>());
					}
					TweenKeyFrame &from(Value... values)
					{
						tween = TweenType<Value...>::from(values...);
						return *this;
					}

					TweenKeyFrame &to(std::tuple<Value...> &tuple)
					{
						return setTo(tuple, std::index_sequence_for<Value...>());
					}
					TweenKeyFrame &to(Value... values)
					{
						tween.to(values...);
						return *this;
					}

					void normalize()
					{
						if (fromKey.time > toKey.time)
						{
							fromKey.swap(toKey);
						}
					}
					void normalizeAndUpdate()
					{
						normalize();
						// auto [fvalue] = fromKey.value;
						from(fromKey.value).to(toKey.value);
						if (toKey.transformer)
						{
							toKey.transformer->setTimeTransiation(toKey.transition);
							auto callables = toKey.transformer->callableTransformers();

							auto callablesTuple = Utility::gen_tuple<std::tuple_size_v<TupleValue>>(
								[&](size_t i) { return callables[i]; });

							std::apply([this](auto &&...args)
									   { tween.during(lengthTime()).via(args...); },
									   callablesTuple);
						}
						else
						{
							tween.during(lengthTime()).via((TweenTransitionType) toKey.transition);
						}
					}

					Range<TimeType> timeRange() { return {fromKey.time, toKey.time}; }

					inline TimeType lengthTime() { return toKey.time - fromKey.time; }

					bool containTime(TimeType value) const
					{
						return (value >= fromKey.time) && (value <= toKey.time);
					}

				protected:
					template<std::size_t... Is>
					TweenKeyFrame &setFrom(std::tuple<Value...> &tuple, std::index_sequence<Is...>)
					{
						return from(std::get<Is>(tuple)...);
					}

					template<std::size_t... Is>
					TweenKeyFrame &setTo(std::tuple<Value...> &tuple, std::index_sequence<Is...>)
					{
						return to(std::get<Is>(tuple)...);
					}
				};
			} // namespace Tweeny
		}	  // namespace Animation
	}		  // namespace Core
} // namespace DGE
