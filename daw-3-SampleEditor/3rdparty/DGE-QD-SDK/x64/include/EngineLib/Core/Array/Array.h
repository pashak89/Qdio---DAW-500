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
 * Array class is using for storing objects in an array
 */
#pragma once
#include <Core/Config/EngineConfigurator.h>
#include <optional>
namespace DGE
{
	namespace Core
	{
		namespace Array
		{
#define PStlFind(P, T) ((P)->find(T) != (P)->end())
#define StlFind(P, T) ((P).find(T) != (P).end())
#define StlInsert(Array, Data) Array.pushback(Data);
			template<typename T, typename M>
			using _mapType = std::map<T, M, std::less<T>
#if defined(DGE_Memory_Allocator) && DGE_Memory_Allocator == DGE_Memory_NedMalloc
									  ,
									  nedalloc::nedallocator<T>
#endif
									  >; // Array of Pointer Ready For Use In Engine;
#pragma region DArray Implementation
			/// \brief This class is Optimized Dynamic Array that can used especial allocator for
			/// memory management.
			template<typename T>
			class DArray
				: public vector<T
#if defined(DGE_Memory_Allocator) && DGE_Memory_Allocator == DGE_Memory_NedMalloc
								,
								nedalloc::nedallocator<T>
#endif
								> // Array Ready For Use In Engine
			{
			public:
				/// \brief get last element data from array.
				std::optional<T> last()
				{
					if (this->empty())
						return std::nullopt;
					return *(--this->end());
				}

				/// \brief get first element data from array.
				std::optional<T> first()
				{
					if (this->empty())
						return std::nullopt;
					return *(this->begin());
				}

				/// \brief find data in stored in this class and return it.
				///
				/// \param data the item that want to find in this class.
				///
				/// \return if data found in the container return iterator to it else return invalid
				/// iterator.
				inline typename std::optional<T> find(T data)
				{
					for (auto ptr = this->cbegin(); ptr != this->cend(); ptr++)
						if (*ptr == data)
							return *ptr;
					return std::nullopt;
				}

				/// \brief destructor of DArray class.
				virtual ~DArray()
				{
					/*for (auto p : *this)
						SafeDelete(p);*/
				}
			};
#pragma endregion

			template<typename T, typename M>
			class HashTable
				: public unordered_map<T, M, hash<T>, equal_to<T>
#if defined(DGE_Memory_Allocator) && DGE_Memory_Allocator == DGE_Memory_NedMalloc
									   ,
									   nedalloc::nedallocator<T*>
#endif
									   > // Array of Pointer Ready For Use In Engine
			{
			public:
				/// \brief get last element data from array.
				std::optional<std::pair<T, M>> last()
				{
					if (this->empty())
						return std::nullopt;

					auto it = std::next(this->begin(), this->size() - 1);

					return *it;
				}

				/// \brief get first element data from array.
				std::optional<std::pair<T, M>> first()
				{
					if (this->empty())
						return std::nullopt;
					return *(this->begin());
				}

				/// \brief find data in stored in this class and return it.
				///
				/// \param data the item that want to find in this class.
				///
				/// \return if data found in the container return iterator to it else return invalid
				/// iterator.
				typename unordered_map<T, M, hash<T>, equal_to<T>>::const_iterator find(T data)
				{
					auto it = unordered_map<T, M, hash<T>, equal_to<T>>::find(data);

					return it;
				}

				/// \brief destructor of HashTable that remove all stored data in this class
				virtual ~HashTable() {}
			};

			template<typename T, typename M>
			class Map : public _mapType<T, M> // Array of Pointer Ready For Use In Engine
			{
			public:
				// get last element data from array.
				std::optional<std::pair<T, M>> last()
				{
					if (this->empty())
						return std::nullopt;
					return *(--this->end());
				}

				// get first element data from array.
				std::optional<std::pair<T, M>> first()
				{
					if (this->empty())
						return std::nullopt;
					return *(this->begin());
				}

				virtual ~Map() { this->clear(); }

				std::optional<M> get(T&& key)
				{
					if (_mapType<T, M>::contains(key))
						return _mapType<T, M>::operator[](key);
					return std::nullopt;
				}

				std::optional<M> get(T& key)
				{
					if (_mapType<T, M>::contains(key))
						return _mapType<T, M>::operator[](key);
					return std::nullopt;
				}
			};
		} // namespace Array
	}	  // namespace Core
} // namespace DGE
