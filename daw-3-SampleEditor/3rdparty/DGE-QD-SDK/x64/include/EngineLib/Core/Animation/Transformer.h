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

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/IO/LogSystem/Log.h>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace Animation
		{
			/// \brief Transition type that is supported by backbone animation
			enum class TransitionType : uint8
			{
				None,
				Linear,
				Stepped,
				QuadraticIn,
				QuadraticOut,
				QuadraticInOut,
				CubicIn,
				CubicOut,
				CubicInOut,
				QuarticIn,
				QuarticOut,
				QuarticInOut,
				QuinticIn,
				QuinticOut,
				QuinticInOut,
				SinusoidalIn,
				SinusoidalOut,
				SinusoidalInOut,
				ExponentialIn,
				ExponentialOut,
				ExponentialInOut,
				CircularIn,
				CircularOut,
				CircularInOut,
				BounceIn,
				BounceOut,
				BounceInOut,
				ElasticIn,
				ElasticOut,
				ElasticInOut,
				BackIn,
				BackOut,
				BackInOut
			};

			template<typename ValueType = float>
			class ITransformerController
			{
			public:
				// ITransformerController();
				virtual ValueType run(float position, ValueType start, ValueType end) = 0;
				virtual vector<vector<DString>> toString() const					  = 0;
				virtual vector<vector<ValueType>> data() const							= 0;
			};

			/// \brief Base class for controlling converting values of an animation. It specifies
			/// how move from one key-frame to another key-frame
			template<typename TimeTransition = TransitionType, typename ValueType = float>
			class Transformer : public ITransformerController<ValueType>
			{
			public:
				Transformer(TimeTransition timeTransiation)
					: m_timeTransiation(timeTransiation)
				{
				}

				Transformer(TimeTransition										timeTransiation,
							std::shared_ptr<ITransformerController<ValueType>>& controller)
					: m_timeTransiation(timeTransiation)
				{
					m_controllers.push_back(std::move(controller));
				}

				Transformer(TimeTransition										 timeTransiation,
							std::shared_ptr<ITransformerController<ValueType>>&& controller)
					: m_timeTransiation(timeTransiation)
				{
					m_controllers.push_back(std::move(controller));
				}

				virtual ValueType run(float position, ValueType start, ValueType end) override
				{
					LogWarning("Call empty controller run() function!");
					return 0;
				}

				std::function<ValueType(float, ValueType, ValueType)> callableTransformer(
					uint8 index = 0)
				{
					ITransformerController<ValueType>* callee = this;
					if (!m_controllers.empty() && m_controllers.size() > index)
						callee = m_controllers[index].get();
					else
						LogWarning("Will be called empty controller run() function!");
					using namespace std::placeholders;
					return std::bind(&ITransformerController<ValueType>::run, callee, _1, _2, _3);
				}

				auto callableTransformers()
				{
					vector<std::function<ValueType(float, ValueType, ValueType)>> result;
					for (int i = 0; i < m_controllers.size(); i++)
					{
						result.push_back(callableTransformer(i));
					}
					return result;
				}

				auto controllersCount() const { return m_controllers.size(); }
				auto controllers() { return m_controllers; }

				TimeTransition timeTransiation() const { return m_timeTransiation; }
				void		   setTimeTransiation(TimeTransition newTimeTransiation)
				{
					m_timeTransiation = newTimeTransiation;
				}
				void addController(std::shared_ptr<ITransformerController<ValueType>>&& controller)
				{
					addController(controller);
				}

				void addController(std::shared_ptr<ITransformerController<ValueType>>& controller)
				{
					m_controllers.push_back(std::move(controller));
				}

				vector<vector<DString>> toString() const override
				{
					vector<vector<DString>> result;
					for (auto controller : m_controllers)
					{
						auto vec = controller->toString();
						if (!vec.empty())
							result.push_back(*vec.begin());
					}
					return result;
				}

				vector<vector<ValueType>> data() const override
				{
					vector<vector<ValueType>> result;
					for (auto controller : m_controllers)
					{
						auto vec = controller->data();
						if (!vec.empty())
							result.push_back(*vec.begin());
					}
					return result;
				}

			protected:
				TimeTransition											   m_timeTransiation;
				vector<std::shared_ptr<ITransformerController<ValueType>>> m_controllers;
			};
			template<typename ValueType = float>
			using IControllerRef = std::shared_ptr<ITransformerController<ValueType>>;
			template<typename TimeTransition = TransitionType, typename ValueType = float>
			using TransformerRef = std::shared_ptr<Transformer<TimeTransition, ValueType>>;
			using StandardTransformerRef = TransformerRef<DGE::Core::Animation::TransitionType>;
		} // namespace Animation
	}	  // namespace Core
} // namespace DGE
