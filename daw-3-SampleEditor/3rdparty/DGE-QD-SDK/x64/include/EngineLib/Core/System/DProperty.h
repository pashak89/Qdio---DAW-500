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

#include <Core/Config/DCoreConfig.h>
#include <Core/IO/LogSystem/LogMacrosDef.h>
#include <Core/System/DObject/DAbstractProperty.h>
#include <Core/System/Functor.h>
#include <any>
namespace DGE
{
	namespace Core
	{
		namespace System
		{

			template<typename Type, typename EventType = Type>
			class DPropertyBase : public DAbstractProperty
			{
			public:
				DPropertyBase()
					: DAbstractProperty(&m_value)
				{
					m_innerValueChanged += D_PFUNC(this,
												   &DPropertyBase<Type, EventType>::onValueChanged);
					m_anyValueChanged += D_PFUNC(this,
												 &DPropertyBase<Type, EventType>::onAnyValueChanged);
				}
				DPropertyBase(const Type &value)
					: DPropertyBase()
				{
					m_value	   = value;
				}
				DPropertyBase(Type &&value)
					: DPropertyBase(value)
				{
				}

				DPropertyBase(std::shared_ptr<DMetaCoreObject> metaObject,
							  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName,
							  const Type &value, bool isReadonly = false)
					: DPropertyBase(metaObject, typeinfo, propertyName, value, isReadonly, false)
				{
				}

				DPropertyBase(std::shared_ptr<DMetaCoreObject> metaObject,
							  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName,
							  const EventType &value, bool isReadonly, bool isStatic)
					: DAbstractProperty(&m_value, metaObject, typeinfo, propertyName, isReadonly,
										isStatic)
				{
					m_value = value;
					m_innerValueChanged += D_PFUNC(this,
												   &DPropertyBase<Type, EventType>::onValueChanged);
					m_anyValueChanged += D_PFUNC(this,
												 &DPropertyBase<Type, EventType>::onAnyValueChanged);
				}

				virtual ~DPropertyBase()
				{
					m_innerValueChanged.clear();
					m_valueChanged.clear();
					m_anyValueChanged.clear();
					if (DCoreConfig::s_debugContext)
						LogConsole("Destroyed DPropertyBase");
				}

				/// \brief Retrieve value of this DProperty
				Type value() const { return m_value; }
				Type &valueRef() { return m_value; }
				/// \brief Set value of this DProperty
				void setValue(EventType &value)
				{
					if (m_value != value)
					{
						setValueDirect(value);
						m_innerValueChanged(m_value);
						m_valueChanged(m_value);
					}
				}

				/// \brief Set value of this DProperty without additional activity such as emit
				/// signals and checking value
				inline void setValueDirect(EventType &value) { this->m_value = value; }

				operator Type() const { return m_value; }

				DPropertyBase<Type, EventType> &operator=(const DPropertyBase<Type, EventType> &rhs)
				{
					m_value = rhs;
					m_innerValueChanged(m_value);
					m_valueChanged = rhs.m_valueChanged;
					m_valueChanged(m_value);
					// D_EMIT(m_valueChanged, rhs);
					return *this;
				}

				Type &operator=(const Type &rhs)
				{
					m_value = rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					// D_EMIT(m_valueChanged, rhs);
					return m_value;
				}
				template<typename TypeOperator = Type>
					requires(std::is_pointer_v<TypeOperator>)
				Type &operator=(TypeOperator rhs)
				{
					m_value = (Type) rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					// D_EMIT(m_valueChanged, rhs);
					return m_value;
				}

				Type &operator=(Type &&rhs)
				{
					m_value = rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return m_value;
				}

				DPropertyBase<Type> &operator++()
				{
					m_value += 1;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				Type operator++(int)
				{
					Type retVal = m_value;
					m_value += 1;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return retVal;
				}

				DPropertyBase<Type> &operator--()
				{
					m_value -= 1;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				Type operator-() { return -m_value; }
				template<typename T>
				auto operator-(const T &rhs)
					-> std::enable_if_t<!std::is_same_v<DPropertyBase<Type>, T>, Type>
				{
					return m_value - rhs;
				}

				Type operator-(const DPropertyBase<Type> &rhs) { return m_value - rhs.m_value; }

				Type operator+() { return m_value; }
				template<typename T>
				auto operator+(const T &rhs)
					-> std::enable_if_t<!std::is_same_v<DPropertyBase<Type>, T>, Type>
				{
					return m_value + rhs;
				}

				Type operator+(const DPropertyBase<Type> &rhs) { return m_value + rhs.m_value; }

				template<typename T>
				auto operator*(const T &rhs)
					-> std::enable_if_t<!std::is_same_v<DPropertyBase<Type>, T>, Type>
				{
					return m_value * rhs;
				}

				Type operator*(const DPropertyBase<Type> &rhs) { return m_value * rhs.m_value; }

				template<typename T>
				auto operator/(const T &rhs)
					-> std::enable_if_t<!std::is_same_v<DPropertyBase<Type>, T>, Type>
				{
					return m_value / rhs;
				}

				Type operator/(const DPropertyBase<Type> &rhs) { return m_value / rhs.m_value; }

				Type operator--(int)
				{
					Type retVal = m_value;
					m_value -= 1;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return retVal;
				}

				template<typename T>
				DPropertyBase<Type> &operator+=(const T &rhs)
				{
					m_value += rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				DPropertyBase<Type> &operator-=(const T &rhs)
				{
					m_value -= rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				DPropertyBase<Type> &operator*=(const T &rhs)
				{
					m_value *= rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				DPropertyBase<Type> &operator/=(const T &rhs)
				{
					m_value /= rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				DPropertyBase<Type> &operator%=(const T &rhs)
				{
					m_value %= rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				DPropertyBase<Type> &operator&=(const T &rhs)
				{
					m_value &= rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				DPropertyBase<Type> &operator|=(const T &rhs)
				{
					m_value |= rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				DPropertyBase<Type> &operator^=(const T &rhs)
				{
					m_value ^= rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				DPropertyBase<Type> &operator<<=(const T &rhs)
				{
					m_value <<= rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				DPropertyBase<Type> &operator>>=(const T &rhs)
				{
					m_value >>= rhs;
					m_innerValueChanged(m_value);
					m_valueChanged(m_value);
					return *this;
				}

				template<typename T>
				bool operator!=(const T &rhs)
				{
					return m_value != rhs;
				}
				bool operator!=(const Type &rhs) { return m_value != rhs; }
				bool operator==(const Type &rhs) { return m_value == rhs; }
				bool operator>(const Type &rhs) { return m_value > rhs; }
				bool operator>=(const Type &rhs) { return m_value >= rhs; }
				bool operator<(const Type &rhs) { return m_value < rhs; }
				bool operator<=(const Type &rhs) { return m_value <= rhs; }
				bool operator!=(Type &&rhs) { return m_value != rhs; }
				bool operator==(Type &&rhs) { return m_value == rhs; }
				bool operator>(Type &&rhs) { return m_value > rhs; }
				bool operator>=(Type &&rhs) { return m_value >= rhs; }
				bool operator<(Type &&rhs) { return m_value < rhs; }
				bool operator<=(Type &&rhs) { return m_value <= rhs; }
				Type *operator->() { return &m_value; }

				Core::System::Event<void, EventType> &valueChanged() { return m_valueChanged; }

			protected:
				void onValueChanged(EventType value)
				{
					if (!m_isValueUpdated)
					{
						m_isValueUpdated = true;
						this->setValueProperty(value);
					}
					else
					{
						m_isValueUpdated = false;
					}
				}

				void onAnyValueChanged(std::any value)
				{
					if (!m_isValueUpdated)
					{
						m_isValueUpdated = true;
						try
						{
							auto newValue = std::any_cast<EventType>(value);
							this->setValue(newValue);
						}
						catch (std::bad_any_cast error)
						{
							LogError(DST "Can't Convert into " + m_typeinfo.typeInfoRef->name());
						}
					}
					else
					{
						m_isValueUpdated = false;
					}
				}

			protected:
				/// \brief Store value of this DProperty
				Type										  m_value;
				/// \brief Used for notifying changes in internal routines.
				mutable Core::System::Event<void, EventType> m_innerValueChanged;
				/// \brief Used for notifying changes in external objects
				mutable Core::System::Event<void, EventType>  m_valueChanged;
				/// \brief the name of this property that is set by defining the property in the class.
				DString										  m_propertyName;
				/// \brief the type of this property that is set by defining the property in the class.
				DGE::Core::System::DTypeInfo				  m_typeinfo;
			};

			template<typename Type>
			class DProperty : public DPropertyBase<Type>
			{
			public:
				typedef Type type;

				DProperty()
					: DPropertyBase<Type>()
				{
				}

				explicit DProperty(Type &&value)
					: DPropertyBase<Type>()
				{
					this->m_value = value;
				}

				// explicit DProperty(const Type &value)
				// 	: DPropertyBase<Type>(value)
				// {
				// }

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName,
						  const Type &value, bool isReadonly = false)
					: DPropertyBase<Type>(metaObject, typeinfo, propertyName, value, isReadonly)
				{
				}
				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName,
						  const Type &value, bool isReadonly, bool isStatic)
					: DPropertyBase<Type>(metaObject, typeinfo, propertyName, value, isReadonly,
										  isStatic)
				{
				}

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo	   typeinfo,
						  const DString					  &propertyName)
					: DPropertyBase<Type>(metaObject, typeinfo, propertyName)
				{
				}
				using DPropertyBase<Type>::operator=;

				using DPropertyBase<Type>::operator Type;
			};

			template<typename Type>
				requires std::is_pointer_v<Type>
			class DProperty<Type> : public DPropertyBase<Type>
			{
			public:
				typedef Type *type;
				DProperty()
					: DPropertyBase<Type>(nullptr)
				{
				}

				DProperty(Type const value)
					: DPropertyBase<Type>()
				{
					this->m_value = value;
				}

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName,
						  Type const value, bool isReadonly = false)
					: DPropertyBase<Type>(metaObject, typeinfo, propertyName, value, isReadonly,
										  false)
				{
					this->m_value = value;
				}

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName)
					: DPropertyBase<Type>(metaObject, typeinfo, propertyName)
				{
				}

				using DPropertyBase<Type>::operator=;

				Type operator->() { return this->m_value; }

				Type &operator*() { return *(this->m_value); }
			};

			template<typename Type>
			class DProperty<std::atomic<Type>> : public DPropertyBase<std::atomic<Type>, Type>
			{
			public:
				typedef Type *type;

				DProperty()
					: DPropertyBase<std::atomic<Type>, Type>()
				{
				}

				DProperty(Type value)
					: DPropertyBase<std::atomic<Type>, Type>(value)
				{
				}

				DProperty(std::atomic<Type>){};

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName,
						  Type value, bool isReadonly = false)
					: DPropertyBase<std::atomic<Type>, Type>(metaObject, typeinfo, propertyName,
															 value, isReadonly)
				{
				}

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName,
						  std::atomic<Type>)
					: DPropertyBase<std::atomic<Type>, Type>(metaObject, typeinfo, propertyName)
				{
				}

				DProperty(DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName)
					: DPropertyBase<std::atomic<Type>, Type>(typeinfo, propertyName)
				{
				}

				operator Type() const { return this->m_value.load(); }

				/// \brief Retrieve value of this DProperty
				Type getValue() { return this->m_value; }
				/// \brief Set value of this DProperty
				void setValue(Type &value)
				{
					if (this->m_value != value)
					{
						this->setValueDirect(value);
						this->m_innerValueChanged(this->m_value);
						this->m_valueChanged(this->m_value);
					}
				}

				Type operator=(std::atomic<Type> rhs)
				{
					this->m_value = rhs;
					this->m_innerValueChanged(this->m_value);
					this->m_valueChanged(this->m_value);
					return rhs.load();
				}
				Type operator=(std::atomic<Type> &rhs)
				{
					this->m_value = rhs;
					this->m_innerValueChanged(this->m_value);
					this->m_valueChanged(this->m_value);
					return rhs.load();
				}
				Type operator=(Type rhs)
				{
					this->m_value = rhs;
					this->m_innerValueChanged(this->m_value);
					this->m_valueChanged(this->m_value);
					return this->m_value;
				}

				bool operator!=(const Type &rhs) { return this->m_value != rhs; }
				bool operator==(const Type &rhs) { return this->m_value == rhs; }
				bool operator>(const Type &rhs) { return this->m_value > rhs; }
				bool operator>=(const Type &rhs) { return this->m_value >= rhs; }
				bool operator<(const Type &rhs) { return this->m_value < rhs; }
				bool operator<=(const Type &rhs) { return this->m_value <= rhs; }
			};

			template<typename Type>
			class DProperty<std::shared_ptr<Type>> : public DPropertyBase<std::shared_ptr<Type>>
			{
			public:
				typedef Type *type;

				DProperty()
					: DPropertyBase<std::shared_ptr<Type>>()
				{
				}

				DProperty(std::shared_ptr<Type> value)
					: DPropertyBase<std::shared_ptr<Type>>(value)
				{
				}

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName,
						  std::shared_ptr<Type> value, bool isReadonly = false)
					: DPropertyBase<std::shared_ptr<Type>>(metaObject, typeinfo, propertyName,
														   value, isReadonly)
				{
				}

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName)
					: DPropertyBase<std::shared_ptr<Type>>(metaObject, typeinfo, propertyName)
				{
				}

				//				template<typename class_type, typename get_type, typename set_type>
				//				DProperty(class_type *obj, get_type get, set_type set)
				//					: DPropertyBase<Type *>(obj, get, set)
				//				{
				//				}

				using DPropertyBase<std::shared_ptr<Type>>::operator=;
				std::shared_ptr<Type> &operator=(std::shared_ptr<Type> &rhs)
				{
					this->m_value = rhs;
					this->m_innerValueChanged(this->m_value);
					this->m_valueChanged(this->m_value);
					// D_EMIT(m_valueChanged, rhs);
					return this->m_value;
				}

				operator std::shared_ptr<Type>() const { return this->m_value; }
				operator bool() { return this->m_value.operator bool(); }

				Type *operator->() { return this->m_value.get(); }
				Type &operator*() { return *this->m_value; }
			};

			template<template<typename... ElementType> class Container, typename... Type>
				requires requires(Container<Type...> t) {
					std::begin(t);
					std::end(t);
				}
			class DProperty<Container<Type...>> : public DPropertyBase<Container<Type...>>
			{
			public:
				DProperty()
					: DPropertyBase<Container<Type...>>()
				{
				}

				explicit DProperty(const Container<Type...> &value)
					: DPropertyBase<Container<Type...>>(value)
				{
				}

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName,
						  const Container<Type...> &value, bool isReadonly = false)
					: DPropertyBase<Container<Type...>>(metaObject, typeinfo, propertyName, value,
														isReadonly)
				{
				}

				DProperty(std::shared_ptr<DMetaCoreObject> metaObject,
						  DGE::Core::System::DTypeInfo typeinfo, const DString &propertyName)
					: DPropertyBase<Container<Type...>>(metaObject, typeinfo, propertyName)
				{
				}

				void insert(const typename Container<Type...>::value_type &value)
				{
					this->m_value.insert(value);
					this->m_innerValueChanged(this->m_value);
					this->m_valueChanged(this->m_value);
				}

				template<typename TContainer = Container<Type...>>
					requires requires(TContainer t) {
						typename TContainer::mapped_type;
						typename TContainer::key_type;
					}
				bool insert(const typename TContainer::key_type &key,
							const typename TContainer::mapped_type &value)
				{
					const auto [it_hinata, success] = this->m_value.insert({key, value});
					this->m_innerValueChanged(this->m_value);
					this->m_valueChanged(this->m_value);
					return success;
				}

				void clear() { this->m_value.clear(); }

				auto begin() { return this->m_value.begin(); }
				auto end() { return this->m_value.end(); }
				auto cbegin() const { return this->m_value.cbegin(); }
				auto cend() const { return this->m_value.cend(); }

				using DPropertyBase<Container<Type...>>::operator=;
			};

			template<typename Type>
			using DPropertyRefWrap = reference_wrapper<System::DProperty<Type>>;

			template<typename T>
			T abs(const DProperty<T> &a, const DProperty<T> &b)
			{
				return a > b ? a - b : b - a;
			}

		} // namespace System
	}	  // namespace Core
} // namespace DGE
