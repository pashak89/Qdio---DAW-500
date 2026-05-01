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
#include "Core/IO/LogSystem/LogMacrosDef.h"
#include "Functor.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/IO/LogSystem/Log.h>
#include <algorithm>
#include <future>
#include <memory>
#include <vector>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			extern DGE_CoreExtern void warnDuplicateFuncObject(Functor *pfunc);
			extern DGE_CoreExtern void destroyingEventObject(void *eventObject);

			class DGE_CoreAPI BaseEvent
			{
			public:
				BaseEvent();
				virtual bool   empty() const = 0;
				virtual void   clear() const = 0;
				virtual uint64 size() const	 = 0;
			};

			template<class Treturn, typename... Args>
			class Event;
			template<class Treturn, class Arg1, typename... Args>
			class Event<Treturn, Arg1, Args...> : public BaseEvent
			{
			protected:
				const size_t m_cArgumentSize = (sizeof...(Args)) + 1;

				mutable std::map<void *,
								 std::shared_ptr<Core::System::Function<Treturn, Arg1, Args...>>>
					m_functionsList;
				mutable std::map<void *, std::shared_ptr<Core::System::Function<Treturn>>>
					m_noArgumentFunctions;

			public:
				Event() {}

				Event(const Core::System::Event<Treturn, Arg1, Args...> &copy)
				{
					m_functionsList		  = copy.m_functionsList;
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
				}

				Event(Core::System::Event<Treturn, Arg1, Args...> &&copy)
				{
					m_functionsList		  = copy.m_functionsList;
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
				}

				explicit Event(
					std::shared_ptr<Core::System::Function<Treturn, Arg1, Args...>> functionPointer)
				{
					m_functionsList.insert({functionPointer->owner(), functionPointer});
				}

				explicit Event(std::shared_ptr<Core::System::Function<Treturn>> functionPointer)
				{
					m_noArgumentFunctions.insert({functionPointer->owner(), functionPointer});
				}

				virtual ~Event() { destroyingEventObject(this); }

				virtual bool empty() const override
				{
					return m_functionsList.empty() && m_noArgumentFunctions.empty();
				}

				virtual void clear() const override
				{
					m_functionsList.clear();
					m_noArgumentFunctions.clear();
				}

				virtual uint64 size() const override
				{
					return m_functionsList.size() + m_noArgumentFunctions.size();
				}

				virtual inline std::vector<std::shared_future<Treturn>> asyncCall(Arg1 arg1,
																				  Args... args)
				{
					std::vector<std::shared_future<Treturn>> results;

					for (const auto &[key, func] : m_functionsList)
					{
						results.push_back(
							std::async([arg1, args..., func]() { return (*func)(arg1, args...); }));
					}

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						results.push_back(std::async([arg1, args..., func]() { return (*func)(); }));
					}
					return results;
				}

				virtual inline std::vector<std::shared_future<Treturn>> asyncCall()
				{
					std::vector<std::shared_future<Treturn>> results;
					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						// must fix return result from multiple functions call.
						results.push_back(std::async([&]() { return (*func)(); }));
					}
					return results;
				}

				virtual Treturn operator()(const Arg1 &arg1, const Args &...args)
				{
					std::vector<Treturn> results;

					for (const auto &[key, func] : m_functionsList)
					{
						results.push_back((*func)(arg1, args...));
					}

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						results.push_back((*func)());
					}

					return results.empty() ? Treturn() : results[0];
				}

				virtual Treturn operator()()
				{
					std::vector<Treturn> results;

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						results.push_back((*func)());
					}

					return results.empty() ? Treturn() : results[0];
				}

				virtual inline Treturn operator()(const Arg1 &&arg1, const Args &&...args)
				{
					return (*this)(arg1, args...);
				}

				virtual std::vector<Treturn> call(const Arg1 &arg1, const Args &...args)
				{
					std::vector<Treturn> results;

					for (const auto &[key, func] : m_functionsList)
					{
						results.push_back((*func)(arg1, args...));
					}

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						results.push_back((*func)());
					}

					return results;
				}

				virtual std::vector<Treturn> call()
				{
					std::vector<Treturn> results;

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						results.push_back((*func)());
					}

					return results;
				}

				virtual inline std::vector<Treturn> call(const Arg1 &&arg1, const Args &&...args)
				{
					return (*this).call(arg1, args...);
				}

				virtual Core::System::Event<Treturn, Arg1, Args...> &operator=(
					const Core::System::Event<Treturn, Arg1, Args...> &copy)
				{
					m_functionsList		  = copy.m_functionsList;
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
					return *this;
				}

				virtual void operator+=(
					Core::System::Function<Treturn, Arg1, Args...> *functionPointer)
				{
					if (m_functionsList.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer);
						return;
					}
					{
						std::shared_ptr<Core::System::Function<Treturn, Arg1, Args...>> func;
						func.reset(functionPointer,
								   [](Core::System::Function<Treturn, Arg1, Args...> *p)
								   { UNUSED(p); });
						m_functionsList.insert({func->owner(), func});
					}
				}
				virtual void operator+=(Core::System::Function<Treturn> *functionPointer)
				{
					if (m_noArgumentFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer);
						return;
					}
					{
						std::shared_ptr<Core::System::Function<Treturn>> func;
						func.reset(functionPointer,
								   [](Core::System::Function<Treturn> *p) { UNUSED(p); });
						m_noArgumentFunctions.insert({func->owner(), func});
					}
				}

				virtual void operator+=(
					std::shared_ptr<Core::System::Function<Treturn, Arg1, Args...>> functionPointer)
				{
					if (m_functionsList.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer.get());
						return;
					}
					m_functionsList.insert({functionPointer->owner(), functionPointer});
				}

				virtual void operator+=(
					std::shared_ptr<Core::System::Function<Treturn>> functionPointer)
				{
					if (m_noArgumentFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer.get());
						return;
					}
					m_noArgumentFunctions.insert({functionPointer->owner(), functionPointer});
				}

				virtual void operator+=(
					const Core::System::Event<Treturn, Arg1, Args...> &EventPointer)
				{
					for (auto [key, pFunc] : EventPointer.m_functionsList)
					{
						if (m_functionsList.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							continue;
						}
						m_functionsList.insert({key, pFunc});
					}
					for (auto [key, pFunc] : EventPointer.m_noArgumentFunctions)
					{
						if (m_noArgumentFunctions.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							continue;
						}
						m_noArgumentFunctions.insert({key, pFunc});
					}
				}

				virtual auto operator+=(const Core::System::Event<Treturn> &EventPointer) ->
					typename std::enable_if<!std::is_void<Treturn>::value, void>::type
				{
					for (auto [key, pFunc] : EventPointer.noArgumentFunctions())
					{
						if (m_noArgumentFunctions.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							continue;
						}
						m_noArgumentFunctions.insert({key, pFunc});
					}

					for (auto [key, pFunc] : EventPointer.functionsList())
					{
						if (m_functionsList.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							continue;
						}
						m_functionsList.insert(
							{key, std::reinterpret_pointer_cast<
									  Core::System::Function<Treturn, Arg1, Args...>>(pFunc)});
					}
				}

				auto noArgumentFunctions() const { return m_noArgumentFunctions; }
				auto functionsList() const { return m_functionsList; }
			};

			template<>
			class Event<void> : public BaseEvent
			{
			protected:
				mutable std::map<void *, std::shared_ptr<Core::System::Function<void>>>
					m_noArgumentFunctions;

			public:
				Event() {}

				Event(const Core::System::Event<void> &copy)
				{
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
				}

				Event(Core::System::Event<void> &&copy)
				{
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
				}

				explicit Event(std::shared_ptr<Core::System::Function<void>> functionPointer)
				{
					m_noArgumentFunctions.insert({functionPointer->owner(), functionPointer});
				}

				virtual ~Event() { destroyingEventObject(this); }

				virtual bool   empty() const override { return m_noArgumentFunctions.empty(); }
				virtual void   clear() const override { m_noArgumentFunctions.clear(); }
				virtual uint64 size() const override { return m_noArgumentFunctions.size(); }

				virtual inline std::vector<std::shared_future<void>> asyncCall()
				{
					std::vector<std::shared_future<void>> results;

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						// must fix return result from multiple functions call.
						results.push_back(std::async([&]() { return (*func)(); }));
					}
					return results;
				}

				virtual inline void operator()()
				{
					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						(*func)();
					}

					return;
				}

				virtual inline void call()
				{
					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						(*func)();
					}

					return;
				}

				virtual Core::System::Event<void> &operator=(const Core::System::Event<void> &copy)
				{
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
					return *this;
				}

				virtual void operator+=(Core::System::Function<void> *functionPointer)
				{
					if (m_noArgumentFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer);
						return;
					}
					{
						std::shared_ptr<Core::System::Function<void>> func;
						func.reset(functionPointer,
								   [](Core::System::Function<void> *p) { UNUSED(p); });
						m_noArgumentFunctions.insert({func->owner(), func});
					}
				}

				virtual void operator+=(std::shared_ptr<Core::System::Function<void>> functionPointer)
				{
					if (m_noArgumentFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer.get());
						return;
					}
					m_noArgumentFunctions.insert({functionPointer->owner(), functionPointer});
				}

				virtual void operator+=(const Core::System::Event<void> &EventPointer)
				{
					for (const auto &[key, pFunc] : EventPointer.noArgumentFunctions())
					{
						if (m_noArgumentFunctions.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							continue;
						}
						m_noArgumentFunctions.insert({key, pFunc});
					}
				}

				std::map<void *, std::shared_ptr<Core::System::Function<void>>> noArgumentFunctions()
					const
				{
					return m_noArgumentFunctions;
				}
				std::set<std::shared_ptr<Core::System::Function<void>>> functionsList() const
				{
					return {};
				}
			};

			template<class Arg1, typename... Args>
			class Event<void, Arg1, Args...> : public BaseEvent
			{
			protected:
				const size_t m_cArgumentSize = (sizeof...(Args));
				const size_t m_cReturnSize	 = 0;
				mutable std::map<void *, std::shared_ptr<Core::System::Function<void, Arg1, Args...>>>
					m_functionsList;
				mutable std::map<void *, std::shared_ptr<Core::System::Function<void>>>
					m_noArgumentFunctions;

			public:
				Event() {}

				Event(const Core::System::Event<void, Arg1, Args...> &copy)
				{
					m_functionsList		  = copy.m_functionsList;
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
				}

				Event(Core::System::Event<void, Arg1, Args...> &&copy)
				{
					m_functionsList		  = copy.m_functionsList;
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
				}

				explicit Event(
					std::shared_ptr<Core::System::Function<void, Arg1, Args...>> functionPointer)
				{
					m_functionsList.insert({functionPointer->owner(), functionPointer});
				}

				explicit Event(std::shared_ptr<Core::System::Function<void>> functionPointer)
				{
					m_noArgumentFunctions.insert({functionPointer->owner(), functionPointer});
				}

				virtual ~Event() { destroyingEventObject(this); }

				virtual bool empty() const override
				{
					return m_functionsList.empty() && m_noArgumentFunctions.empty();
				}

				virtual void clear() const override
				{
					m_functionsList.clear();
					m_noArgumentFunctions.clear();
				}

				virtual uint64 size() const override
				{
					return m_functionsList.size() + m_noArgumentFunctions.size();
				}

				virtual inline std::vector<std::shared_future<void>> asyncCall(Arg1 arg1,
																			   Args... args)
				{
					std::vector<std::shared_future<void>> results;

					for (const auto &[key, func] : m_functionsList)
					{
						// must fix return result from multiple functions call.
						results.push_back(std::async([&]() { return (*func)(arg1, args...); }));
					}

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						// must fix return result from multiple functions call.
						results.push_back(std::async([&]() { return (*func)(); }));
					}
					return results;
				}

				virtual inline std::vector<std::shared_future<void>> asyncCall()
				{
					std::vector<std::shared_future<void>> results;

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						// must fix return result from multiple functions call.
						results.push_back(std::async([&]() { return (*func)(); }));
					}
					return results;
				}

				virtual void operator()(const Arg1 &arg1, const Args &...args)
				{
					for (const auto &[key, func] : m_functionsList)
					{
						(*func)(arg1, args...);
					}

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						(*func)();
					}

					return;
				}

				virtual void operator()()
				{
					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						(*func)();
					}

					return;
				}

				virtual inline void operator()(Arg1 &&arg1, Args &&...args)
				{
					return (*this)(arg1, args...);
				}

				virtual void call(const Arg1 &arg1, const Args &...args)
				{
					(*this)(arg1, args...);

					return;
				}

				virtual void call()
				{
					(*this)();

					return;
				}

				virtual inline void call(Arg1 &&arg1, Args &&...args)
				{
					(*this)(arg1, args...);
					return;
				}

				virtual Core::System::Event<void, Arg1, Args...> &operator=(
					const Core::System::Event<void, Arg1, Args...> &copy)
				{
					m_functionsList		  = copy.m_functionsList;
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
					return *this;
				}

				//				virtual Core::System::Event<void> &operator=(
				//					const Core::System::Event<void> &copy)
				//				{
				//					m_functionsList		  = copy.m_functionsList;
				//					m_noArgumentFunctions = copy.m_noArgumentFunctions;
				//					return *this;
				//				}

				virtual void operator+=(Core::System::Function<void, Arg1, Args...> *functionPointer)
				{
					if (m_functionsList.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer);
						return;
					}
					{
						std::shared_ptr<Core::System::Function<void, Arg1, Args...>> func;
						func.reset(functionPointer,
								   [](Core::System::Function<void, Arg1, Args...> *p)
								   { UNUSED(p); });
						m_functionsList.insert({func->owner(), func});
					}
				}
				virtual void operator+=(
					Core::System::FunctionRef<void, Arg1, Args...> functionPointer)
				{
					if (m_functionsList.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer.get());
						return;
					}
					m_functionsList.insert({functionPointer->owner(), functionPointer});
				}
				virtual void operator+=(Core::System::Function<void> *functionPointer)
				{
					if (m_noArgumentFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer);
						return;
					}
					{
						std::shared_ptr<Core::System::Function<void>> func;
						func.reset(functionPointer,
								   [](Core::System::Function<void> *p) { UNUSED(p); });
						m_noArgumentFunctions.insert({func->owner(), func});
					}
				}

				virtual void operator+=(
					const std::shared_ptr<Core::System::Function<void, Arg1, Args...>>
						&functionPointer) const
				{
					if (m_functionsList.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer.get());
						return;
					}
					m_functionsList.insert({functionPointer->owner(), functionPointer});
				}
				virtual void operator+=(
					const std::shared_ptr<Core::System::Function<void>> &functionPointer) const
				{
					if (m_noArgumentFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer.get());
						return;
					}
					m_noArgumentFunctions.insert({functionPointer->owner(), functionPointer});
				}

				virtual void operator+=(const Core::System::Event<void, Arg1, Args...> &EventPointer)
				{
					for (auto [key, pFunc] : EventPointer.m_functionsList)
					{
						if (m_functionsList.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							continue;
						}
						m_functionsList.insert({key, pFunc});
					}
					for (auto [key, pFunc] : EventPointer.m_noArgumentFunctions)
					{
						if (m_noArgumentFunctions.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							return;
						}
						m_noArgumentFunctions.insert({key, pFunc});
					}
				}
				virtual void operator+=(const Core::System::Event<void> &eventPointer)
				{
					for (const auto &[key, pFunc] : eventPointer.noArgumentFunctions())
					{
						if (m_noArgumentFunctions.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							return;
						}
						m_noArgumentFunctions.insert({key, pFunc});
					}
				}

				auto noArgumentFunctions() const { return m_noArgumentFunctions; }
				auto functionsList() const { return m_functionsList; }
			};

			template<class Treturn>
			class Event<Treturn> : public BaseEvent
			{
			protected:
				mutable std::map<void *, std::shared_ptr<Core::System::Function<Treturn>>>
					m_noArgumentFunctions;
				mutable std::map<void *, std::shared_ptr<Core::System::Function<void>>>
					m_noReturnTypeFunctions;

			public:
				Event() {}

				Event(const Core::System::Event<Treturn> &copy)
				{
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
				}

				Event(Core::System::Event<Treturn> &&copy)
				{
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
				}

				Event(const Core::System::Event<void> &copy)
				{
					m_noReturnTypeFunctions = copy.m_noArgumentFunctions;
				}

				Event(Core::System::Event<void> &&copy)
				{
					m_noReturnTypeFunctions = copy.m_noArgumentFunctions;
				}

				explicit Event(std::shared_ptr<Core::System::Function<Treturn>> functionPointer)
				{
					m_noArgumentFunctions.insert({functionPointer->owner(), functionPointer});
				}

				explicit Event(std::shared_ptr<Core::System::Function<void>> functionPointer)
				{
					m_noReturnTypeFunctions.insert({functionPointer->owner(), functionPointer});
				}

				virtual ~Event() { destroyingEventObject(this); }

				virtual bool empty() const override
				{
					return m_noArgumentFunctions.empty() && m_noReturnTypeFunctions.empty();
				}

				virtual void clear() const override
				{
					m_noReturnTypeFunctions.clear();
					m_noArgumentFunctions.clear();
				}

				virtual uint64 size() const override
				{
					return m_noReturnTypeFunctions.size() + m_noArgumentFunctions.size();
				}

				virtual inline std::vector<std::shared_future<Treturn>> asyncCall()
				{
					std::vector<std::shared_future<Treturn>> results;

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						// must fix return result from multiple functions call.
						results.push_back(std::async([&]() { return (*func)(); }));
					}
					return results;
				}

				virtual inline Treturn operator()()
				{
					std::vector<Treturn> results;

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						results.push_back((*func)());
					}
					for (const auto &[key, func] : m_noReturnTypeFunctions)
					{
						(*func)();
					}

					return results.empty() ? Treturn() : results[0];
				}

				virtual inline std::vector<Treturn> call()
				{
					std::vector<Treturn> results;

					for (const auto &[key, func] : m_noArgumentFunctions)
					{
						results.push_back((*func)());
					}
					for (const auto &[key, func] : m_noReturnTypeFunctions)
					{
						(*func)();
					}

					return results;
				}

				virtual Core::System::Event<Treturn> &operator=(
					const Core::System::Event<Treturn> &copy)
				{
					m_noArgumentFunctions = copy.m_noArgumentFunctions;
					return *this;
				}

				virtual Core::System::Event<Treturn> &operator=(const Core::System::Event<void> &copy)
				{
					m_noReturnTypeFunctions = copy.noArgumentFunctions();
					return *this;
				}

				virtual void operator+=(Core::System::Function<Treturn> *functionPointer)
				{
					if (m_noArgumentFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer);
						return;
					}
					{
						std::shared_ptr<Core::System::Function<Treturn>> func;
						func.reset(functionPointer,
								   [](Core::System::Function<Treturn> *p) { UNUSED(p); });
						m_noArgumentFunctions.insert({func->owner(), func});
					}
				}

				virtual void operator+=(Core::System::Function<void> *functionPointer)
				{
					if (m_noReturnTypeFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer);
						return;
					}
					{
						std::shared_ptr<Core::System::Function<void>> func;
						func.reset(functionPointer,
								   [](Core::System::Function<void> *p) { UNUSED(p); });
						m_noReturnTypeFunctions.insert({func->owner(), func});
					}
				}

				virtual void operator+=(
					std::shared_ptr<Core::System::Function<Treturn>> functionPointer)
				{
					if (m_noArgumentFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer.get());
						return;
					}
					m_noArgumentFunctions.insert({functionPointer->owner(), functionPointer});
				}

				virtual void operator+=(std::shared_ptr<Core::System::Function<void>> functionPointer)
				{
					if (m_noReturnTypeFunctions.contains(functionPointer->owner()))
					{
						warnDuplicateFuncObject(functionPointer.get());
						return;
					}
					m_noReturnTypeFunctions.insert({functionPointer->owner(), functionPointer});
				}

				virtual void operator+=(const Core::System::Event<Treturn> &EventPointer)
				{
					for (const auto &[key, pFunc] : EventPointer.m_noArgumentFunctions)
					{
						if (m_noArgumentFunctions.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							continue;
						}
						m_noArgumentFunctions.insert({pFunc->owner(), pFunc});
					}
				}

				virtual void operator+=(const Core::System::Event<void> &EventPointer)
				{
					for (const auto &[key, pFunc] : EventPointer.noArgumentFunctions())
					{
						if (m_noReturnTypeFunctions.contains(key))
						{
							warnDuplicateFuncObject(pFunc.get());
							continue;
						}
						m_noReturnTypeFunctions.insert({pFunc->owner(), pFunc});
					}
				}

				auto noArgumentFunctions() const { return m_noArgumentFunctions; }
				auto functionsList() const { return m_noReturnTypeFunctions; }
			};

		} // namespace System
	}	  // namespace Core
} // namespace DGE
