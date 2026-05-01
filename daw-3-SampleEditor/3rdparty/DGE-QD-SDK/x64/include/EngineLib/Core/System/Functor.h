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
#include <Core/Config/DCoreConfig.h>
#include <Core/IO/LogSystem/Log.h>
#include <memory>
#include <optional>
#include <type_traits>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			namespace Helper
			{
				/// \ref https://stackoverflow.com/a/22863957
				template<typename T, typename... Args>
				struct return_type;

				template<typename T, typename... Args>
				struct return_type
					: return_type<std::conditional_t<!std::is_invocable_v<T, Args...>,
													 decltype(&T::operator()),
													 void (*)(Args...)>>
				{
				};

				// For generic types, directly use the result of the signature of its 'operator()'
				template<typename ClassType, typename ReturnType, typename... Args>
				struct return_type<ReturnType (ClassType::*)(Args...) const>
				{
					using type = ReturnType;
				};

				template<typename ReturnType, typename... Args>
				struct return_type<ReturnType (*)(Args...)>
				{
					using type = ReturnType;
				};

				/// \ref https://stackoverflow.com/a/35348334
				template<typename Ret, typename... Rest>
				void first_argument_helper(Ret (*)(Rest...));

				template<typename Ret, typename Arg, typename... Rest>
				Arg first_argument_helper(Ret (*)(Arg, Rest...));

				template<typename Ret, typename F, typename Arg, typename... Rest>
				Arg first_argument_helper(Ret (F::*)(Arg, Rest...));

				template<typename Ret, typename F, typename... Rest>
				void first_argument_helper(Ret (F::*)(Rest...));

				template<typename Ret, typename F, typename Arg, typename... Rest>
				Arg first_argument_helper(Ret (F::*)(Arg, Rest...) const);

				template<typename Ret, typename F, typename... Rest>
				void first_argument_helper(Ret (F::*)(Rest...) const);

				template<typename F>
				decltype(first_argument_helper(&F::operator())) first_argument_helper(F);

				template<typename T>
				using first_argument = decltype(first_argument_helper(std::declval<T>()));
			} // namespace Helper
			/// \brief Base abstract class for function pointer without template
			class Functor
			{
			public:
				virtual bool operator!=(void *m) = 0;
				virtual bool operator==(void *m) = 0;
				virtual		 operator void *()	 = 0;
				virtual void assign(void *mem)	 = 0;
				virtual ~Functor() {}
				void *owner() { return m_owner; }
				DString name() { return m_name; }

			protected:
				void *m_owner = nullptr;
				DString		m_name;
				friend auto ptr_fun(auto &&lambda);
			};

			/// \brief Abstract class for storing pointer to function either member function or not
			/// member function
			template<class Treturn, typename... Args>
			class Function : public Functor
			{
			};
			template<class Treturn, class Arg1, typename... Args>
			class Function<Treturn, Arg1, Args...> : public Functor
			{
			public:
				virtual Treturn operator()(Arg1 arg1, Args... args) = 0;
				virtual bool	operator!=(void *m)		 = 0;
				virtual bool	operator==(void *m)		 = 0;
				virtual			operator void *()		 = 0;
				virtual void	assign(void *mem)		 = 0;
				virtual ~Function() {}
			};

			/// Specialize Function for callable object that their return type are 'void' and don't
			/// have any argument
			template<>
			class Function<void> : public Functor
			{
			public:
				virtual void operator()()			  = 0;
				virtual bool operator!=(void *m)	  = 0;
				virtual bool operator==(void *m)	  = 0;
				virtual		 operator void *()		  = 0;
				virtual void assign(void *mem)		  = 0;
				virtual ~Function() {}
			};

			template<class Arg1, typename... Args>
			class Function<void, Arg1, Args...> : public Functor
			{
			public:
				virtual void operator()(Arg1 arg1, Args... args) = 0;
				virtual bool operator!=(void *m)				 = 0;
				virtual bool operator==(void *m)				 = 0;
				virtual		 operator void *()					 = 0;
				virtual void assign(void *mem)					 = 0;
				virtual ~Function() {}
			};

			template<class Treturn>
			class Function<Treturn> : public Functor
			{
			public:
				virtual Treturn operator()()		= 0;
				virtual bool	operator!=(void *m) = 0;
				virtual bool	operator==(void *m) = 0;
				virtual			operator void *()	= 0;
				virtual void	assign(void *mem)	= 0;
				virtual ~Function() {}
			};

			/// \brief Base class for storing pointer to not member function
			template<class Treturn, class... Args>
			class FunctionPointer : public Function<Treturn, Args...>
			{
			public:
				typedef Treturn (*FuncPointer)(Args... arg1);
				using FuncPointerType = FuncPointer;

				FunctionPointer() { m_pFun = nullptr; }
				FunctionPointer(Treturn (*_fun)(Args... arg1))
					: m_pFun(_fun)
				{
					this->m_owner = (void *) m_pFun;
					this->m_name  = typeid(_fun).name();
				}

				FunctionPointer(std::function<Treturn(Args...)> _fun)
					: m_pFun(_fun)
				{
					this->m_owner = (void *) m_pFun;
					this->m_name  = typeid(_fun).name();
				}
				FunctionPointer(void *mem) { assign(mem); }
				virtual ~FunctionPointer(void)
				{
					if (DGE::Core::DCoreConfig::s_debugContext)
						LogDebug(DST("FunctionPointer ") + typeid(m_pFun).name() + " Is Destroyed");
				}
				virtual Treturn operator()(Args... args) override { return ((m_pFun) (args...)); }

				virtual bool operator!=(void *mem) override
				{
					return mem != *static_cast<void **>(static_cast<void *>(&m_pFun));
				}

				virtual bool operator==(void *mem) override
				{
					return mem == *static_cast<void **>(static_cast<void *>(&m_pFun));
				}

				virtual operator void *() override
				{
					return *static_cast<void **>(static_cast<void *>(&m_pFun));
				}
				virtual void assign(void *mem) override
				{
					m_pFun		  = *(FuncPointerType *) (&mem);
					this->m_owner = (void *) m_pFun;
				}

			protected:
				FuncPointerType m_pFun;
			};

			/// \brief Base class for storing pointer to member function of the class
			template<typename ClassType, typename Treturn, typename... Args>
			class MemberFunctionPointer : public Function<Treturn, Args...>
			{
				typedef Treturn (ClassType::*m_type)(Args... arg1);
				m_type	   m_pFun  = nullptr;
				ClassType *m_class = nullptr;

			public:
				MemberFunctionPointer(ClassType *obj, Treturn (ClassType::*_fun)(Args... arg1))
					: m_pFun(_fun)
					, m_class(obj)
				{
					this->m_owner = obj;
					this->m_name  = typeid(_fun).name();
				}

				virtual ~MemberFunctionPointer(void)
				{
					if (DGE::Core::DCoreConfig::s_debugContext)
						LogDebug(DST("MemberFunctionPointer ") + typeid(m_pFun).name()
								 + " Is Destroyed");
				}

				virtual inline Treturn operator()(Args... args) override
				{
					return ((m_class->*m_pFun)(args...));
				}

				virtual bool operator!=(void *mem) override
				{
					return mem != *static_cast<void **>(static_cast<void *>(&m_pFun));
				}

				virtual bool operator==(void *mem) override
				{
					return mem == *static_cast<void **>(static_cast<void *>(&m_pFun));
				}

				virtual operator void *() override
				{
					return *static_cast<void **>(static_cast<void *>(&m_pFun));
				}

				virtual void assign(void *mem) override { m_pFun = *(m_type *) (&mem); }
			};

			/// \brief Base class for storing pointer to lambda type
			template<typename LambdaType, typename Treturn, typename... Args>
			struct LambdaFunctionPointer;

			template<typename LambdaType, typename Treturn, typename Arg1, typename... Args>
			struct LambdaFunctionPointer<LambdaType, Treturn, Arg1, Args...>
				: public Function<Treturn, Arg1, Args...>
			{
				std::optional<LambdaType> m_object;
				Treturn (LambdaType::*m_function)(Arg1, Args...) const;

				LambdaFunctionPointer(const LambdaType &object)
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) &object);
					this->m_owner = (void *) typeid(object).hash_code();
					this->m_name  = typeid(object).name();
				}
				LambdaFunctionPointer(LambdaType &&object)
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) &object);
					this->m_owner = (void *) typeid(object).hash_code();
					this->m_name  = typeid(object).name();
				}
				virtual ~LambdaFunctionPointer()
				{
					if (DGE::Core::DCoreConfig::s_debugContext)
						LogDebug(DST("LambdaFunctionPointer ") + typeid(m_object.value()).name()
								 + " Is Destroyed");
				}
				virtual bool operator!=(void *mem) override
				{
					return mem != *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual bool operator==(void *mem) override
				{
					return mem == *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual operator void *() override
				{
					return *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual void assign(void *mem) override
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) mem);
					this->m_owner = (void *) mem;
					// m_function = &decltype(m_object.value())::operator();
				}

				LambdaFunctionPointer &operator=(const LambdaFunctionPointer &object)
				{
					assign(&object.m_object);
					return *this;
				}

				virtual Treturn operator()(Arg1 arg1, Args... args) override
				{
					return (*m_object)(arg1, args...);
				}
			};

			template<typename LambdaType>
			struct LambdaFunctionPointer<LambdaType, void> : public Function<void>
			{
				std::optional<LambdaType> m_object;

				LambdaFunctionPointer(LambdaType &object)
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) &object);
					this->m_owner = (void *) typeid(object).hash_code();
					this->m_name  = typeid(object).name();
				}
				LambdaFunctionPointer(LambdaType &&object)
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) &object);
					this->m_owner = (void *) typeid(object).hash_code();
					this->m_name  = typeid(object).name();
				}
				virtual ~LambdaFunctionPointer()
				{
					if (DGE::Core::DCoreConfig::s_debugContext)
						LogDebug(DST("LambdaFunctionPointerVoidReturnType ")
								 + typeid(m_object.value()).name() + " Is Destroyed");
				}

				virtual bool operator!=(void *mem) override
				{
					return mem != *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual bool operator==(void *mem) override
				{
					return mem == *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual operator void *() override
				{
					return *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual void assign(void *mem) override
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) mem);
					this->m_owner = (void *) mem;
					// m_function = &decltype(m_object.value())::operator();
				}

				LambdaFunctionPointer &operator=(const LambdaFunctionPointer &object)
				{
					assign(&object.m_object);
					return *this;
				}

				virtual void operator()(void) override { (*m_object)(); }
			};

			template<typename LambdaType, typename Arg1, typename... Args>
			struct LambdaFunctionPointer<LambdaType, void, Arg1, Args...>
				: public Function<void, Arg1, Args...>
			{
				std::optional<LambdaType> m_object;

				LambdaFunctionPointer(const LambdaType &object)
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) &object);
					this->m_owner = (void *) typeid(object).hash_code();
					this->m_name  = typeid(object).name();
				}
				LambdaFunctionPointer(LambdaType &&object)
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) &object);
					this->m_owner = (void *) typeid(object).hash_code();
					this->m_name  = typeid(object).name();
				}
				virtual ~LambdaFunctionPointer()
				{
					if (DGE::Core::DCoreConfig::s_debugContext)
						LogDebug(DST("LambdaFunctionPointerVoidReturnType ")
								 + typeid(m_object.value()).name() + " Is Destroyed");
				}

				virtual bool operator!=(void *mem) override
				{
					return mem != *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual bool operator==(void *mem) override
				{
					return mem == *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual operator void *() override
				{
					return *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual void assign(void *mem) override
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) mem);
					this->m_owner = (void *) mem;
					// m_function = &decltype(m_object.value())::operator();
				}

				LambdaFunctionPointer &operator=(const LambdaFunctionPointer &object)
				{
					assign(&object.m_object);
					return *this;
				}

				virtual void operator()(Arg1 arg1, Args... args) override
				{
					(*m_object)(arg1, args...);
				}
			};

			template<typename LambdaType, typename Treturn>
			struct LambdaFunctionPointer<LambdaType, Treturn> : public Function<Treturn>
			{
				std::optional<LambdaType> m_object;

				LambdaFunctionPointer(const LambdaType &object)
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) &object);
					this->m_owner = (void *) typeid(object).hash_code();
					this->m_name  = typeid(object).name();
				}
				LambdaFunctionPointer(LambdaType &&object)
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) &object);
					this->m_owner = (void *) typeid(object).hash_code();
					this->m_name  = typeid(object).name();
				}
				virtual ~LambdaFunctionPointer()
				{
					if (DGE::Core::DCoreConfig::s_debugContext)
						LogDebug(DST("LambdaFunctionPointerVoidReturnType ")
								 + typeid(m_object.value()).name() + " Is Destroyed");
				}

				virtual bool operator!=(void *mem) override
				{
					return mem != *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual bool operator==(void *mem) override
				{
					return mem == *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual operator void *() override
				{
					return *static_cast<void **>(static_cast<void *>(&m_object));
				}
				virtual void assign(void *mem) override
				{
					m_object.reset();
					m_object.emplace(*(LambdaType *) mem);
					this->m_owner = (void *) mem;
					// m_function = &decltype(m_object.value())::operator();
				}

				LambdaFunctionPointer &operator=(const LambdaFunctionPointer &object)
				{
					assign(&object.m_object);
					return *this;
				}

				virtual Treturn operator()() override { return (*m_object)(); }
			};

			template<class Treturn, class... Args>
			std::shared_ptr<Function<Treturn, Args...>> ptr_fun(Treturn (*_fun)(Args... arg1))
			{
				return std::static_pointer_cast<Function<Treturn, Args...>>(
					std::make_shared<FunctionPointer<Treturn, Args...>>(_fun));
			}

			/// \brief Create pointer to function with this helper function
			template<class Treturn, class... Args>
			std::shared_ptr<Function<Treturn, Args...>> ptr_fun(Function<Treturn, Args...> *space,
																void					   *mem)
			{
				UNUSED(space);
				return std::static_pointer_cast<Function<Treturn, Args...>>(
					std::make_shared<FunctionPointer<Treturn, Args...>>(mem));
			}

			template<typename LambdaType>
			auto ptr_fun(const LambdaType &lambda) -> typename std::enable_if<
				!std::is_void<Helper::first_argument<decltype(lambda)>>::value,
				std::shared_ptr<Function<typename Helper::return_type<LambdaType>::type,
										 Helper::first_argument<decltype(lambda)>>>>::type
			{
				return std::static_pointer_cast<Function<typename Helper::return_type<LambdaType>::type,
														 Helper::first_argument<decltype(lambda)>>>(
					std::make_shared<
						LambdaFunctionPointer<LambdaType,
											  typename Helper::return_type<LambdaType>::type,
											  Helper::first_argument<decltype(lambda)>>>(lambda));
			}

			template<class LambdaType>
			auto ptr_fun(const LambdaType &lambda) -> typename std::enable_if<
				!std::is_void<Helper::first_argument<decltype(lambda)>>::value
					&& !std::is_void<typename Helper::return_type<LambdaType>::type>::value,
				std::shared_ptr<Function<typename Helper::return_type<LambdaType>::type,
										 Helper::first_argument<decltype(lambda)>>>>::type
			{
				return std::static_pointer_cast<Function<typename Helper::return_type<LambdaType>::type,
														 Helper::first_argument<decltype(lambda)>>>(
					std::make_shared<
						LambdaFunctionPointer<LambdaType,
											  typename Helper::return_type<LambdaType>::type,
											  Helper::first_argument<decltype(lambda)>>>(lambda));
			}

			template<class LambdaType>
			auto ptr_fun(const LambdaType &lambda) -> typename std::enable_if<
				!std::is_void<Helper::first_argument<decltype(lambda)>>::value
					&& std::is_void<typename Helper::return_type<LambdaType>::type>::value,
				std::shared_ptr<Function<void, Helper::first_argument<decltype(lambda)>>>>::type
			{
				return std::static_pointer_cast<
					Function<void, Helper::first_argument<decltype(lambda)>>>(
					std::make_shared<LambdaFunctionPointer<LambdaType,
														   void,
														   Helper::first_argument<decltype(lambda)>>>(
						lambda));
			}

			template<class LambdaType>
			auto ptr_fun(const LambdaType &lambda) -> typename std::enable_if<
				std::is_void<Helper::first_argument<decltype(lambda)>>::value
					&& std::is_void<typename Helper::return_type<LambdaType>::type>::value,
				std::shared_ptr<Function<Helper::first_argument<decltype(lambda)>>>>::type
			{
				return std::static_pointer_cast<Function<Helper::first_argument<decltype(lambda)>>>(
					std::make_shared<
						LambdaFunctionPointer<LambdaType, Helper::first_argument<decltype(lambda)>>>(
						lambda));
			}

			template<class LambdaType>
			auto ptr_fun(const LambdaType &lambda) -> typename std::enable_if<
				std::is_void<Helper::first_argument<decltype(lambda)>>::value
					&& !std::is_void<typename Helper::return_type<LambdaType>::type>::value,
				std::shared_ptr<Function<typename Helper::return_type<LambdaType>::type>>>::type
			{
				return std::static_pointer_cast<
					Function<typename Helper::return_type<LambdaType>::type>>(
					std::make_shared<
						LambdaFunctionPointer<LambdaType,
											  typename Helper::return_type<LambdaType>::type>>(
						lambda));
			}

			template<class LambdaType>
			auto ptr_fun(LambdaType &lambda) -> typename std::enable_if<
				std::is_void<Helper::first_argument<decltype(lambda)>>::value
					&& std::is_void<typename Helper::return_type<LambdaType>::type>::value,
				std::shared_ptr<Function<Helper::first_argument<decltype(lambda)>>>>::type
			{
				return std::static_pointer_cast<Function<Helper::first_argument<decltype(lambda)>>>(
					std::make_shared<
						LambdaFunctionPointer<LambdaType, Helper::first_argument<decltype(lambda)>>>(
						lambda));
			}

			/// \brief Create pointer to function with this helper function for lambda function types
			template<class LambdaType>
			auto _ptr_fun(LambdaType &&lambda) -> typename std::enable_if<
				!std::is_void<Helper::first_argument<decltype(lambda)>>::value
					&& !std::is_void<typename Helper::return_type<LambdaType>::type>::value,
				std::shared_ptr<Function<typename Helper::return_type<LambdaType>::type,
										 Helper::first_argument<decltype(lambda)>>>>::type
			{
				return std::static_pointer_cast<Function<typename Helper::return_type<LambdaType>::type,
														 Helper::first_argument<decltype(lambda)>>>(
					std::make_shared<
						LambdaFunctionPointer<LambdaType,
											  typename Helper::return_type<LambdaType>::type,
											  Helper::first_argument<decltype(lambda)>>>(lambda));
			}

			template<class LambdaType>
			auto _ptr_fun(LambdaType &&lambda) -> typename std::enable_if<
				!std::is_void<Helper::first_argument<decltype(lambda)>>::value
					&& std::is_void<typename Helper::return_type<LambdaType>::type>::value,
				std::shared_ptr<Function<void, Helper::first_argument<decltype(lambda)>>>>::type
			{
				return std::static_pointer_cast<
					Function<void, Helper::first_argument<decltype(lambda)>>>(
					std::make_shared<LambdaFunctionPointer<LambdaType,
														   void,
														   Helper::first_argument<decltype(lambda)>>>(
						lambda));
			}

			template<class LambdaType>
			auto _ptr_fun(LambdaType &&lambda) -> typename std::enable_if<
				std::is_void<Helper::first_argument<decltype(lambda)>>::value
					&& std::is_void<typename Helper::return_type<LambdaType>::type>::value,
				std::shared_ptr<Function<Helper::first_argument<decltype(lambda)>>>>::type
			{
				return std::static_pointer_cast<Function<Helper::first_argument<decltype(lambda)>>>(
					std::make_shared<
						LambdaFunctionPointer<LambdaType, Helper::first_argument<decltype(lambda)>>>(
						lambda));
			}

			template<class LambdaType>
			auto _ptr_fun(LambdaType &&lambda) -> typename std::enable_if<
				std::is_void<Helper::first_argument<decltype(lambda)>>::value
					&& !std::is_void<typename Helper::return_type<LambdaType>::type>::value,
				std::shared_ptr<Function<typename Helper::return_type<LambdaType>::type>>>::type
			{
				return std::static_pointer_cast<
					Function<typename Helper::return_type<LambdaType>::type>>(
					std::make_shared<
						LambdaFunctionPointer<LambdaType,
											  typename Helper::return_type<LambdaType>::type>>(
						lambda));
			}

			auto ptr_fun(auto &&lambda)
			{
				return _ptr_fun(std::move(lambda));
			}

			/// \brief Create pointer to function with this helper function
			template<class ClassType, class Treturn, class... Args>
			std::shared_ptr<Function<Treturn, Args...>> ptr_fun(
				ClassType *pclass, Treturn (ClassType::*_fun)(Args... arg1))
			{
				return std::static_pointer_cast<Function<Treturn, Args...>>(
					std::make_shared<MemberFunctionPointer<ClassType, Treturn, Args...>>(pclass,
																						 _fun));
			}
			/// \brief Create pointer to function with this helper function
			template<class ClassType, class SecondClass, class Treturn, class... Args>
			std::shared_ptr<Function<Treturn, Args...>> ptr_fun(
				SecondClass *pclass, Treturn (ClassType::*_fun)(Args... arg1))
			{
				return std::static_pointer_cast<Function<Treturn, Args...>>(
					std::make_shared<MemberFunctionPointer<ClassType, Treturn, Args...>>(pclass,
																						 _fun));
			}

			template<class Treturn, class... Args>
			using FunctionRef = std::shared_ptr<Function<Treturn, Args...>>;

#define D_PFUNC(...) DGE::Core::System::ptr_fun(__VA_ARGS__)
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
