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
#include <type_traits>
#include <utility>

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{

			namespace detail
			{
				template<class T>
				struct is_reference_wrapper : std::false_type
				{
				};
				template<class U>
				struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
				{
				};

				template<class T>
				struct invoke_impl
				{
					template<class F, class... Args>
					static auto call(F&& f, Args&&... args)
						-> decltype(std::forward<F>(f)(std::forward<Args>(args)...));
				};

				template<class B, class MT>
				struct invoke_impl<MT B::*>
				{
					template<class T,
							 class Td = typename std::decay<T>::type,
							 class = typename std::enable_if<std::is_base_of<B, Td>::value>::type>
					static auto get(T&& t) -> T&&;

					template<class T,
							 class Td = typename std::decay<T>::type,
							 class = typename std::enable_if<is_reference_wrapper<Td>::value>::type>
					static auto get(T&& t) -> decltype(t.get());

					template<class T,
							 class Td = typename std::decay<T>::type,
							 class = typename std::enable_if<!std::is_base_of<B, Td>::value>::type,
							 class = typename std::enable_if<!is_reference_wrapper<Td>::value>::type>
					static auto get(T&& t) -> decltype(*std::forward<T>(t));

					template<class T,
							 class... Args,
							 class MT1,
							 class = typename std::enable_if<std::is_function<MT1>::value>::type>
					static auto call(MT1 B::*pmf, T&& t, Args&&... args)
						-> decltype((invoke_impl::get(std::forward<T>(t))
									 .*pmf)(std::forward<Args>(args)...));

					template<class T>
					static auto call(MT B::*pmd, T&& t)
						-> decltype(invoke_impl::get(std::forward<T>(t)).*pmd);
				};

				template<class F, class... Args, class Fd = typename std::decay<F>::type>
				auto INVOKE(F&& f, Args&&... args)
					-> decltype(invoke_impl<Fd>::call(std::forward<F>(f),
													  std::forward<Args>(args)...));
			} // namespace detail

			template<class F, class T, class = T>
			struct is_static_castable : std::false_type
			{
			};

			template<class F, class T>
			struct is_static_castable<F, T, decltype(static_cast<T>(std::declval<F>()))>
				: std::true_type
			{
			};

			template<class>
			struct result_of;
			template<class F, class... ArgTypes>
			struct result_of<F(ArgTypes...)>
			{
				using type = decltype(detail::INVOKE(std::declval<F>(),
													 std::declval<ArgTypes>()...));
			};
		} // namespace BaseType
	}	  // namespace Core
} // namespace DGE
