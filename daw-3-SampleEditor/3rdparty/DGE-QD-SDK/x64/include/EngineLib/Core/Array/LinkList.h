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
 * LinkList class is using for storing objects in a LinkList
 */

#pragma once

#include <Core/BaseTypes/BaseTypes.h>

namespace DGE
{
	namespace Core
	{
		namespace Array
		{
			using namespace DGE;

			/// \brief A dynamic vector that implement a link list structor.
			template<class Type>
			class DGE_CoreAPI LinkList
			{
			public:
				/// \brief A internal class for store data in the node in link list.
				struct node
				{
					Type  data;
					node* next;
					node *prev;

					/// \brief create a node with specific data
					///
					/// \param data the data that set into this node.
					node(const Type &data)
					{
						this->data = data;
						next = prev = nullptr;
					}

					/// \brief the destructor of this class
					///
					/// \details this destructor set previous
					/// and next node of itself to next and previous node of itself
					~node()
					{
						if (next)
							next->prev = prev;
						if (prev)
							prev->next = next;
					}
				};

				LinkList()
				{
					first = last = iterate = temp = nullptr;
				}
				~LinkList() { clear(); }

				/// \brief add a data to link list.
				bool push_back(Type newData)
				{
					if (last == nullptr)
					{
						last	   = new node();
						last->prev = nullptr;
						last->next = nullptr;
						last->data = newData;
						first	   = last;
					}
					else
					{
						temp	   = last->prev;
						last	   = new node();
						last->prev = temp;
						last->next = nullptr;
						last->data = newData;
					}
					return true;
				}

				/// \brief Clear All Element in Link list & Destroy it data.
				void clear()
				{
					while (last != nullptr)
					{
						temp = last;
						last = last->prev;
						SafeDelete(temp);
					}
					first = last = iterate = temp = nullptr;
					count = 0;
				}
			private:
				node *first;
				node *last;
				node *iterate;
				node *temp;
				BaseType::uint32 count;
			};
		}
	}
}

