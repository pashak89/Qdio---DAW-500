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

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
			/// \brief Smart class for managing instances of different Inherited class of a Abstract class
			template<class BaseClass>
			class DSmartInheritanceObject
			{
			public:
				/// \brief Add an instance of inherited class with unique id
				///
				/// \param id Unique Id of the object that want to be added
				/// \param child The object that want to be added into child list of this class that
				/// will be used as one of instance of the base class with calling setDefaultObject
				/// and setting id of child object for the input parameter
				///
				/// \return Return true if adding object is successful
				bool addChild(DString id, std::shared_ptr<BaseClass> child)
				{
					if (!objectMap.count(id))
					{
						objectMap[id] = child;
						return true;
					}
					return false;
				}

				/// \brief Set default object for calling this class
				///
				/// \param id Unique Id of the object that want to used for default object
				void setDefaultObject(DString id)
				{
					if (objectMap.count(id))
					{
						defaultObject = objectMap[id];
					}
				}

				/// \brief Get the default object for calling this class
				///
				/// \return Shared pointer to the default object for this class
				std::shared_ptr<BaseClass> getDefaultObject() { return defaultObject; }

				/// \brief Get the object with given id
				///
				/// \param id Unique Id of the object that want to be given
				///
				/// \return Return Shared pointer of found object else return nullptr
				std::shared_ptr<BaseClass> getChild(DString id)
				{
					if (objectMap.count(id))
					{
						return objectMap[id];
					}
					return nullptr;
				}

				/// \brief Overloading operator -> to access to defaultObject
				std::shared_ptr<BaseClass> operator->() { return defaultObject; }

				/// \brief Determined whether this object is accessible
				///
				/// \return Return true if this object is accessible
				operator bool() const { return objectMap.empty(); }

				/// \brief Overloading operator[] to access inner objects
				///
				/// \param id Unique Id of the object that want to be given
				///
				/// \return Shared pointer of found object
				std::shared_ptr<BaseClass>& operator[](DString id) { return objectMap[id]; }

			protected:
				std::shared_ptr<BaseClass>					 defaultObject;
				std::map<DString, std::shared_ptr<BaseClass>> objectMap;
			};
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
