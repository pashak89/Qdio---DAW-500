
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

#include <typeindex>

namespace std
{

	/// \brief Specialization of functor std::hash for std::type_info references.
	template<>
	struct hash<std::reference_wrapper<const std::type_info>>
	{
		/// \brief Invokes std::type_info::hash_code on the wrapped type.
		/// @param typeinfo  The wrapped type information.
		/// @return The hash value
		inline std::size_t operator()(std::reference_wrapper<const std::type_info> typeinfo) const
		{
			return typeinfo.get().hash_code();
		}
	};

	/// \brief Specialization of functor std::equal_to for std::type_info references.
	template<>
	struct equal_to<std::reference_wrapper<const std::type_info>>
	{
		/// \brief Invokes operator==  on the std::type_info of \p{lhs} and \p{rhs}.
		/// \param lhs  The left-hand side value.
		/// \param rhs  The right-hand side value.
		/// \return true if the objects represent the same type, false otherwise.
		inline bool operator()(std::reference_wrapper<const std::type_info> lhs,
							   std::reference_wrapper<const std::type_info>
								   rhs) const
		{
			return lhs.get() == rhs.get();
		}
	};

	/// \brief Specialization of functor std::less for std::type_info references.
	template<>
	struct less<std::reference_wrapper<const std::type_info>>
	{
		/// \brief Invokes operator<  on the std::type_index of \p{lhs} and \p{rhs}.
		/// \param lhs  The left-hand side value.
		/// \param rhs  The right-hand side value.
		/// \return true if the objects represent the same type, false otherwise.
		inline bool operator()(std::reference_wrapper<const std::type_info> lhs,
							   std::reference_wrapper<const std::type_info>
								   rhs) const
		{
			return std::type_index(lhs.get()) < std::type_index(rhs.get());
		}
	};

} // namespace std
namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			/// \brief Type definition for a std::set that uses key objects of type
			/// std::reference_wrapper<const std::type_info>.
			using TypeSet = std::set<std::reference_wrapper<const std::type_info>>;

			/// \brief Templated type definition for a std::unordered_map that uses key objects of
			/// type std::reference_wrapper<const std::type_info>.
			///
			/// This map is used internally, for example to store pointers to objects to
			/// singletons of the provided type.
			///
			/// \tparam T The type of the objects to map.
			template<typename T>
			using TypeMap = std::unordered_map<std::reference_wrapper<const std::type_info>, T>;

		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
