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

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/CoreModuleDef.h>
#include <Core/System/DProperty.h>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			/// \brief Store meta data about runtime object and its properties
			class DGE_CoreAPI DMetaCoreObject
			{
			public:
				DMetaCoreObject();
				virtual ~DMetaCoreObject();

				TypeInfoRef type() const;
				void		setType(const TypeInfoRef& newType);

				bool									registerProperty(DAbstractPropertyRef ref);
				bool									registerProperty(DAbstractProperty* ref);
				DAbstractPropertyRef					property(DString name) const;
				std::any								propertyValue(DString name) const;
				bool setProperty(DString name, std::any, bool force = false);
				std::map<DString, DAbstractPropertyRef> properties() const;

			protected:
				TypeInfoRef								m_type;
				std::map<DString, DAbstractPropertyRef> m_properties;
			};

		} // namespace System
	}	  // namespace Core
} // namespace DGE
