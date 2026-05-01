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
 * Tree class is using for storing objects in a tree
 */
#pragma once
#include <Core/Array/Array.h>

namespace DGE
{
	namespace Core
	{
		using namespace DGE::Core::Array;
		namespace Tree
		{
			/// \brief An element of tree that can have children and data
			template<class T>
			class NodeTree
			{
			public:
				/// \brief create a empty NodeTree
				NodeTree() {}

				/// \brief create a NodeTree that is stored data in it and parent of this NodeTree
				/// sets to given parent input parameter or doesn't have parent
				///
				/// \param data is stored data in this NodeTree
				/// \param parent is parent of this NodeTree or parent is null
				NodeTree(T data, NodeTree<T>* parent = nullptr)
				{
					m_data	 = data;
					m_parent = parent;
				}

				/// \brief add a new child to this NodeTree that stored data in it
				///
				/// \param data is stored data in new child NodeTree of this class
				///
				/// \return return created child NodeTree.
				NodeTree<T>* addChild(T data)
				{
					m_child.push_back(new NodeTree<T>(data, this));
					return m_child[m_child.size() - 1];
				}

				/// \brief add given NodeTree to list of children and set parent of it to this class
				///
				/// \param NodeTree is new child NodeTree that add to list of children of this class
				///
				void addChild(NodeTree<T>* NodeTree)
				{
					m_child.push_back(NodeTree);
					NodeTree->m_parent = this;
				}

				/// \brief add a new NodeTree and set parent of this NodeTree to created NodeTree
				/// that stored
				///
				/// \param data is stored data in new NodeTree that it's parent of  this class
				///
				/// \return return created NodeTree.
				NodeTree<T>* addParent(T data)
				{
					auto newParent = new NodeTree<T>(data, nullptr);
					m_parent	   = newParent;
					m_parent->AddChild(this);
					return newParent;
				}

				/// \brief get next child of this NodeTree that stored in a list
				NodeTree<T>* nextSibling()
				{
					m_indexCurrent++;
					if (m_indexCurrent > (m_child.size() - 1))
						return nullptr;
					return &m_child[m_indexCurrent];
				}
				/// \brief get current child of this NodeTree that stored in a list
				NodeTree<T>* currentSibling()
				{
					if (m_indexCurrent > (m_child.size() - 1))
						return nullptr;
					return &m_child[m_indexCurrent];
				}

				/// \brief get previous child of this NodeTree that stored in a list
				NodeTree<T>* prevSibling()
				{
					m_indexCurrent--;
					if (m_indexCurrent < 0)
						return nullptr;
					return &m_child[m_indexCurrent];
				}

				/// \brief get parent of this NodeTree
				NodeTree<T>* parent() { return m_parent; }

				/// \brief get data that is stored in this NodeTree
				const T& getData() const { return m_data; }
				/// \brief set data that is stored in this NodeTree
				void setData(const T& newData) { m_data = newData; }

			protected:
				DArray<NodeTree<T>*> m_child;
				NodeTree<T>*		 m_parent		= nullptr;
				int				 m_indexCurrent = 0;
				T				 m_data;
			};

			/// \brief This class use for create a Tree store data in children and itself.
			template<class T>
			class Tree : public NodeTree<T>
			{
			public:
				Tree() {}

				Tree(T data, NodeTree<T>* newParent = nullptr)
					: NodeTree<T>(data, newParent)
				{
				}
			};
		} // namespace Tree
	}
}
