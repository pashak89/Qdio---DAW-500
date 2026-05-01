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

#include "DCoreObject.h"

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			/// \brief A class for manage tags
			class DGE_CoreAPI Tags : public DGE::Core::System::DCoreObject
			{
			public:
				Tags();
				Tags(const DString &tagName);
				Tags(std::initializer_list<DString> &&tagsName);

				Tags &operator=(std::initializer_list<DString> tags);
				Tags &operator=(const std::set<DString> tags);

				Tags operator+(const DString &tagName);
				Tags operator-(const DString &tagName);

				Tags &operator+=(const DString &tagName);
				Tags &operator-=(const DString &tagName);

				bool operator!=(const DString &tagName) const;
				bool operator!=(const Tags &tags) const;
				bool operator!=(const std::set<DString> &tags) const;

				bool operator==(const DString &tagName) const;
				bool operator==(const Tags &tags) const;
				bool operator==(const std::set<DString> &tags) const;

				/// \brief indicate whether this class contains tags values
				bool contains(const Tags &tags) const;
				bool contains(const std::set<DString> &tags) const;

				/// \brief indicate whether this class contains one of tags values
				bool containsOneOf(const Tags &tags) const;
				bool containsOneOf(const std::set<DString> &tags) const;

				/// \brief Assign a tag to current view
				///
				/// \param tagName Name of tag for assigning into current view
				///
				/// \return Return true if name of tag is assigned to current view successfully else
				/// return false.
				bool assign(DString tagName);

				/// \brief Determine is assign a tag to current view or not
				///
				/// \param tagName Name of tag for checking existence the tag name in current view
				///
				/// \return Return true if name of tag is found in current view else return false.
				bool isAssigned(DString tagName) const;

				/// \brief Remove a tag from current view
				///
				/// \param tagName Name of tag for removing into current view
				///
				/// \return Return true if removing the tag name from current view is successful
				/// else return false.
				bool remove(DString tagName);

				/// \brief Get set of all tags that is assigned to current view
				///
				/// \return Return set of all tags that is assigned to current view
				const std::set<DString> &tags();

				/// \brief Set list of all tags that is assigned to current view
				///
				/// \param newTagList List of tag that is assigned to current view
				void setTags(const std::set<DString> &newTagList);

				uint64 count() const;

				bool isEmpty() const;

			protected:
				/// \brief Set of tags for grouping items in the views and apply special policy on them
				std::set<DString> m_tags;
			};

		} // namespace System
	}	  // namespace Core
} // namespace DGE
