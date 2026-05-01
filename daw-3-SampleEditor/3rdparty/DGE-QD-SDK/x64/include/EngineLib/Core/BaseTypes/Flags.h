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
 * Flags class is using for adding bit operators on enum in C/C++
 */
#pragma once

#include <bitset>
#include <initializer_list>
#include <numeric>
#include <string>
#include <utility>

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{
			template<class E, class Parent = std::false_type, class Enabler = void>
			struct IsFlags : public Parent
			{
			};

			template<class E, class Validator>
			class Flags;

			template<class E, class Validator>
			class FlagsIterator
			{
			public:
				using Flags_type		= Flags<E, Validator>;
				using difference_type	= std::ptrdiff_t;
				using value_type		= E;
				using pointer			= value_type *;
				using reference			= const value_type;
				using iterator_category = std::forward_iterator_tag;

				constexpr FlagsIterator() noexcept
					: uvalue_(0)
					, mask_(0)
				{
				}

				constexpr FlagsIterator(const FlagsIterator &other) noexcept
					: uvalue_(other.uvalue_)
					, mask_(other.mask_)
				{
				}

				FlagsIterator &operator++() noexcept
				{
					nextMask();
					return *this;
				}
				FlagsIterator operator++(int) noexcept
				{
					auto copy = *this;
					++(*this);
					return copy;
				}

				constexpr reference operator*() const noexcept
				{
					return static_cast<value_type>(mask_);
				}

				friend inline constexpr bool operator==(const FlagsIterator &i,
														const FlagsIterator &j) noexcept
				{
					return i.mask_ == j.mask_;
				}

				friend inline constexpr bool operator!=(const FlagsIterator &i,
														const FlagsIterator &j) noexcept
				{
					return i.mask_ != j.mask_;
				}

			private:
				template<class E_, typename Validator_>
				friend class Flags;

				using impl_type = typename Flags_type::impl_type;

				explicit FlagsIterator(impl_type uv) noexcept
					: uvalue_(uv)
					, mask_(1)
				{
					if (!(mask_ & uvalue_))
					{
						nextMask();
					}
				}

				constexpr FlagsIterator(impl_type uv, E e) noexcept
					: uvalue_(uv)
					, mask_(static_cast<impl_type>(static_cast<impl_type>(e) & uv))
				{
				}

				void nextMask() noexcept
				{
					do
					{
						mask_ <<= 1;
					} while (mask_ && !(mask_ & uvalue_));
				}

				impl_type uvalue_;
				impl_type mask_;
			};

			constexpr struct empty_t
			{
				constexpr empty_t() noexcept = default;
			} empty;

			template<class E, class Validator = IsFlags<E, std::false_type>>
			class Flags
			{
			public:
				static_assert(Validator::value,
							  "Flags::Flags is disallowed for this type; "
							  "use D_EnumFlags macro.");

				using enum_type		  = typename std::decay<E>::type;
				using underlying_type = typename std::underlying_type<enum_type>::type;
				using impl_type		  = typename std::make_unsigned<underlying_type>::type;

				using iterator		  = FlagsIterator<enum_type, Validator>;
				using const_iterator  = iterator;
				using value_type	  = typename iterator::value_type;
				using reference		  = typename iterator::reference;
				using const_reference = typename iterator::reference;
				using pointer		  = enum_type *;
				using const_pointer	  = const enum_type *;
				using size_type		  = std::size_t;
				using difference_type = typename iterator::difference_type;

				constexpr static std::size_t bit_size() { return sizeof(impl_type) * 8; }

			private:
				template<class T, class Res = std::nullptr_t>
				using convertible = std::enable_if<std::is_convertible<T, enum_type>::value, Res>;

			public:
				Flags() noexcept						   = default;
				Flags(const Flags &fl) noexcept			   = default;
				Flags &operator=(const Flags &fl) noexcept = default;
				Flags(Flags &&fl) noexcept				   = default;
				Flags &operator=(Flags &&fl) noexcept	   = default;

				explicit constexpr Flags(empty_t) noexcept
					: val_(0)
				{
				}

#ifdef ENUM_CLASS_FLAGS_FORBID_IMPLICT_CONVERSION
				explicit
#endif
					constexpr Flags(enum_type e) noexcept
					: val_(static_cast<impl_type>(e))
				{
				}

				Flags &operator=(enum_type e) noexcept
				{
					val_ = static_cast<impl_type>(e);
					return *this;
				}

				Flags(std::initializer_list<enum_type> il) noexcept
					: val_(0)
				{
					insert(il);
				}

				Flags &operator=(std::initializer_list<enum_type> il) noexcept
				{
					clear();
					insert(il);
					return *this;
				}

				template<class... Args>
				Flags(enum_type e, Args... args) noexcept
					: Flags{e, args...}
				{
				}

				template<class FwIter>
				Flags(FwIter b, FwIter e,
					  typename convertible<decltype(*std::declval<FwIter>())>::type
					  = nullptr) noexcept(noexcept(std::declval<Flags>()
													   .insert(std::declval<FwIter>(),
															   std::declval<FwIter>())))
					: val_(0)
				{
					insert(b, e);
				}

				constexpr explicit operator bool() const noexcept { return val_ != 0; }

				constexpr bool operator!() const noexcept { return !val_; }

				friend constexpr bool operator==(Flags fl1, Flags fl2)
				{
					return fl1.val_ == fl2.val_;
				}

				friend constexpr bool operator!=(Flags fl1, Flags fl2)
				{
					return fl1.val_ != fl2.val_;
				}

				constexpr Flags operator~() const noexcept { return Flags(~val_); }

				Flags &operator|=(const Flags &fl) noexcept
				{
					val_ |= fl.val_;
					return *this;
				}

				Flags &operator&=(const Flags &fl) noexcept
				{
					val_ &= fl.val_;
					return *this;
				}

				Flags &operator^=(const Flags &fl) noexcept
				{
					val_ ^= fl.val_;
					return *this;
				}

				Flags &operator|=(enum_type e) noexcept
				{
					val_ |= static_cast<impl_type>(e);
					return *this;
				}

				Flags &operator&=(enum_type e) noexcept
				{
					val_ &= static_cast<impl_type>(e);
					return *this;
				}

				Flags &operator^=(enum_type e) noexcept
				{
					val_ ^= static_cast<impl_type>(e);
					return *this;
				}

				friend constexpr Flags operator|(Flags f1, Flags f2) noexcept
				{
					return Flags{static_cast<impl_type>(f1.val_ | f2.val_)};
				}

				friend constexpr Flags operator&(Flags f1, Flags f2) noexcept
				{
					return Flags{static_cast<impl_type>(f1.val_ & f2.val_)};
				}

				friend constexpr Flags operator^(Flags f1, Flags f2) noexcept
				{
					return Flags{static_cast<impl_type>(f1.val_ ^ f2.val_)};
				}

				void swap(Flags &fl) noexcept { std::swap(val_, fl.val_); }

				constexpr underlying_type underlying_value() const noexcept
				{
					return static_cast<underlying_type>(val_);
				}

				void set_underlying_value(underlying_type newval) noexcept
				{
					val_ = static_cast<impl_type>(newval);
				}

				constexpr explicit operator std::bitset<Flags<E, Validator>::bit_size()>()
					const noexcept
				{
					return to_bitset();
				}

				constexpr std::bitset<Flags<E, Validator>::bit_size()> to_bitset() const noexcept
				{
					return {val_};
				}

				constexpr bool empty() const noexcept { return !val_; }

				size_type size() const noexcept
				{
					return std::distance(this->begin(), this->end());
				}

				constexpr size_type max_size() const noexcept { return bit_size(); }

				iterator begin() const noexcept { return cbegin(); }
				iterator cbegin() const noexcept { return iterator{val_}; }

				constexpr iterator end() const noexcept { return cend(); }
				constexpr iterator cend() const noexcept { return {}; }

				constexpr iterator find(enum_type e) const noexcept { return {val_, e}; }

				constexpr size_type count(enum_type e) const noexcept
				{
					return find(e) != end() ? 1 : 0;
				}

				std::pair<iterator, iterator> equal_range(enum_type e) const noexcept
				{
					auto i = find(e);
					auto j = i;
					return {i, ++j};
				}

				template<class... Args>
				std::pair<iterator, bool> emplace(Args &&...args) noexcept
				{
					return insert(enum_type{args...});
				}

				template<class... Args>
				iterator emplace_hint(iterator, Args &&...args) noexcept
				{
					return emplace(args...).first;
				}

				std::pair<iterator, bool> insert(enum_type e) noexcept
				{
					auto i = find(e);
					if (i == end())
					{
						i.mask_ = static_cast<impl_type>(e);
						val_ |= i.mask_;
						update_uvalue(i);
						return {i, true};
					}
					return {i, false};
				}

				std::pair<iterator, bool> insert(iterator, enum_type e) noexcept
				{
					return insert(e);
				}

				template<class FwIter>
				auto insert(FwIter i1,
							FwIter i2) noexcept(noexcept(++i1) && noexcept(*i1) && noexcept(i1
																							== i2))
					-> typename convertible<decltype(*i1), void>::type
				{
					val_ |= std::accumulate(i1, i2, impl_type{0},
											[](impl_type i, enum_type e)
											{ return i | static_cast<impl_type>(e); });
				}

				template<class Container>
				auto insert(const Container &ctn) noexcept
					-> decltype(std::begin(ctn), std::end(ctn), void())
				{
					insert(std::begin(ctn), std::end(ctn));
				}

				iterator erase(iterator i) noexcept
				{
					val_ ^= i.mask_;
					update_uvalue(i);
					return ++i;
				}

				size_type erase(enum_type e) noexcept
				{
					auto e_count = count(e);
					val_ &= ~static_cast<impl_type>(e);
					return e_count;
				}

				iterator erase(iterator i1, iterator i2) noexcept
				{
					val_ ^= Flags(i1, i2).val_;
					update_uvalue(i2);
					return i2;
				}

				void clear() noexcept { val_ = 0; }

			private:
				constexpr explicit Flags(impl_type val) noexcept
					: val_(val)
				{
				}

				void update_uvalue(iterator &it) const noexcept { it.uvalue_ = val_; }

				impl_type val_;
			};

			template<class E, class Validator>
			void swap(Flags<E, Validator> &fl1, Flags<E, Validator> &fl2) noexcept
			{
				fl1.swap(fl2);
			}

			template<class E, class Validator>
			constexpr auto operator|(E e1, E e2) noexcept ->
				typename std::enable_if<IsFlags<E, Validator>::value, Flags<E, Validator>>::type
			{
				return Flags<E, Validator>(e1) | e2;
			}

			template<class E, class Validator>
			constexpr auto operator&(E e1, E e2) noexcept ->
				typename std::enable_if<IsFlags<E, Validator>::value, Flags<E, Validator>>::type
			{
				return Flags<E, Validator>(e1) & e2;
			}

			template<class E, class Validator>
			constexpr auto operator^(E e1, E e2) noexcept ->
				typename std::enable_if<IsFlags<E, Validator>::value, Flags<E, Validator>>::type
			{
				return Flags<E, Validator>(e1) ^ e2;
			}

#define D_EnumFlags(name)                                \
	using IsFlags##name = IsFlags<name, std::true_type>; \
	using name##s		= Flags<name, IsFlags##name>;

		} // namespace BaseType
	}	  // namespace Core
} // namespace DGE
