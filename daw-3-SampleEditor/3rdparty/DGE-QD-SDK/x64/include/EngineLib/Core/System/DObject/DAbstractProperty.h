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

#include "Core/CoreModuleDef.h"
#include "Core/System/DCoreObject.h"
#include "Core/System/DObjectMacrosDef.h"
#include <Core/System/Event.h>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			using DPropertyKey = std::pair<TypeInfoRef, DString>;
			class DMetaCoreObject;
			using DMetaCoreObjectRef = std::shared_ptr<DMetaCoreObject>;
			/// \brief The Abstract class for storng information about every property of DCoreObject
			class DGE_CoreAPI DAbstractProperty
			{
			public:
				DAbstractProperty(std::any addressValue);
				DAbstractProperty(std::any						   addressValue,
								  std::shared_ptr<DMetaCoreObject> metaObject,
								  DGE::Core::System::DTypeInfo	   typeinfo,
								  const DString &propertyName, bool isReadonly = false,
								  bool isStatic = false);
				DAbstractProperty &operator=(const DAbstractProperty &propertry);
				virtual ~DAbstractProperty();
				virtual void	 setValueProperty(std::any value);
				virtual std::any valueProperty();

				const Core::System::Event<void, std::any> &anyValueChanged() const;

				/// \brief Indicate whether two properties are equal and they belong to a unique object.
				bool isEqualProp(DAbstractProperty *property);

				DPropertyKey key();
				DString		 name() const;

				bool isReadonly() const;

				bool isStatic() const;

			protected:
				DString										m_propertyName;
				DGE::Core::System::DTypeInfo				m_typeinfo;
				std::any									m_anyValue;
				bool										m_isValueUpdated;
				bool										m_isReadonly;
				bool										m_isStatic;
				mutable Core::System::Event<void, std::any> m_anyValueChanged;
			};
			D_DefRefType(DAbstractProperty);

		} // namespace System
	}	  // namespace Core
} // namespace DGE
