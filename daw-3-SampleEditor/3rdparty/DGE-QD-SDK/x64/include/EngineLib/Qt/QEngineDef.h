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
#include <QObject>
#include <Qt/QtModuleDef.h>

#include <Core/System/DProperty.h>

namespace DGE
{
    namespace QtWrapper
    {
/// \part D_QT_PROPERTY
/// \brief D_QT_PROPERTY definition section
#define __D_QT_NO_CONVERTER(value) value
#define __D_QT_BINDING_CONVERTER(value) value

#define D_DEFAULT_CONVERTER(value) value

#define __D_QT_PROPERTY_DEFINE(Type, PropertyName, AccessType) \
    __D_PROPERTY_DEFINE(Type, PropertyName, AccessType)

#define __D_QT_PROPERTY_GETTER(Type, PropertyName, getName, Converter) \
    Type getName() const                                               \
    {                                                                  \
        return Converter(this->m_##PropertyName.value());              \
    }

#define __D_QT_PROPERTY_SETTER(Type, PropertyName, capitalName, Converter)          \
    void set##capitalName(                                                          \
        std::conditional_t<std::is_pointer<Type>::value, Type, const Type &> value) \
    {                                                                               \
        if (this->m_##PropertyName != value)                                        \
        {                                                                           \
            this->m_##PropertyName = Converter(value);                              \
            emit PropertyName##Changed(this->m_##PropertyName);                     \
        }                                                                           \
    }

#define __D_QT_PROPERTY_EVENT(Type, PropertyName, EventName) \
signals:                                                     \
    Q_SIGNAL void PropertyName##EventName(Type);

#define __D_QT_PROPERTY_10(Type, PropertyName, GetterSetterType, getName, capitalName,          \
                           defaultValue, VariableAccessType, GetterSetterAccessType, Converter, \
                           EventType)                                                           \
    __D_QT_PROPERTY_DEFINE(Type, PropertyName, VariableAccessType), defaultValue                \
    }                                                                                           \
    ;                                                                                           \
    GetterSetterAccessType:                                                                     \
    __D_QT_PROPERTY_GETTER(GetterSetterType, PropertyName, getName, Converter)                  \
    __D_QT_PROPERTY_SETTER(GetterSetterType, PropertyName, capitalName, Converter)              \
    __D_QT_PROPERTY_EVENT(EventType, PropertyName, Changed)                                     \
    __D_PROPERTY_EVENT(EventType, PropertyName, Changed, GetterSetterAccessType)

#define __D_QT_PROPERTY_9(Type, PropertyName, GetterSetterType, getName, capitalName,            \
                          defaultValue, VariableAccessType, GetterSetterAccessType, Converter)   \
    __D_QT_PROPERTY_10(Type, PropertyName, GetterSetterType, getName, capitalName, defaultValue, \
                       VariableAccessType, GetterSetterAccessType, Converter, Type)

#define __D_QT_PROPERTY_8(Type, PropertyName, GetterSetterType, getName, capitalName,           \
                          defaultValue, VariableAccessType, GetterSetterAccessType)             \
    __D_QT_PROPERTY_9(Type, PropertyName, GetterSetterType, getName, capitalName, defaultValue, \
                      VariableAccessType, GetterSetterAccessType, __D_QT_NO_CONVERTER)

#define __D_QT_PROPERTY_7(Type, PropertyName, getName, capitalName, defaultValue,   \
                          VariableAccessType, GetterSetterAccessType)               \
    __D_QT_PROPERTY_8(Type, PropertyName, Type, getName, capitalName, defaultValue, \
                      VariableAccessType, GetterSetterAccessType)

#define __D_QT_PROPERTY_6(Type, PropertyName, getName, capitalName, defaultValue,                 \
                          VariableAccessType)                                                     \
    __D_QT_PROPERTY_7(Type, PropertyName, getName, capitalName, defaultValue, VariableAccessType, \
                      public)

#define __D_QT_PROPERTY_5(Type, PropertyName, getName, capitalName, defaultValue) \
    __D_QT_PROPERTY_6(Type, PropertyName, getName, capitalName, defaultValue, protected)

#define __D_QT_PROPERTY_4(Type, PropertyName, capitalName, defaultValue) \
    __D_QT_PROPERTY_5(Type, PropertyName, PropertyName, capitalName, defaultValue)

#define __D_QT_PROPERTY_3(Type, PropertyName, capitalName) \
    __D_QT_PROPERTY_4(Type, PropertyName, capitalName, Type())

#define __D_QT_PROPERTY_2(Type, PropertyName) __D_QT_PROPERTY_3(Type, PropertyName, PropertyName)

/// \brief Add Qt property to current class is using of Qt framework same as D_QT_PROPERTY.
///
/// It's useful when need base type of TYPE that is real type of this property. base type will using
/// in getter/setter
#define D_QT_PROPERTY_BASE_TYPE(Type, PropertyName, capitalName, GetterSetterType)       \
    D_QT_PROPERTY_BASE_TYPE_CONVERTER(Type, PropertyName, capitalName, GetterSetterType, \
                                      __D_QT_NO_CONVERTER)

#define D_QT_PROPERTY_BASE_TYPE_CONVERTER(Type, PropertyName, capitalName, GetterSetterType,    \
                                          Converter)                                            \
    __D_QT_PROPERTY_10(Type, PropertyName, GetterSetterType, PropertyName, capitalName, Type(), \
                       protected, public, Converter, Type)

#define __D_QT_PROPERTY_CHOOSE_FROM_ARG_COUNT(...)                                               \
	FUNC_RECOMPOSER_15((__VA_ARGS__, __D_QT_PROPERTY_14, __D_QT_PROPERTY_13, __D_QT_PROPERTY_12, \
						__D_QT_PROPERTY_11, __D_QT_PROPERTY_10, __D_QT_PROPERTY_9,               \
						__D_QT_PROPERTY_8, __D_QT_PROPERTY_7, __D_QT_PROPERTY_6,                 \
						__D_QT_PROPERTY_5, __D_QT_PROPERTY_4, __D_QT_PROPERTY_3,                 \
						__D_QT_PROPERTY_2, __D_QT_PROPERTY_1, ))

#define __D_QT_PROPERTY_NO_ARG_EXPANDER() , , , , , , , , , , __D_QT_PROPERTY_0
#define __D_QT_PROPERTY_MACRO_CHOOSER(...) \
    __D_QT_PROPERTY_CHOOSE_FROM_ARG_COUNT(__D_QT_PROPERTY_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief Add Qt property to current class is using of Qt framework
#define D_QT_PROPERTY(...) __D_QT_PROPERTY_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
/// \endpart D_QT_PROPERTY
/// TODO remove Initialize() function with setting equal function from constructor of DProperty
/// \part D_QT_PROPERTY_QPROPERTY
#define __D_QT_PROPERTY_QPROPERTY_10(Type, PropertyName, GetterSetterType, getName, capitalName, \
									 defaultValue, VariableAccessType, GetterSetterAccessType,   \
									 Converter, EventType)                                       \
	Q_PROPERTY(                                                                                  \
		Type PropertyName READ PropertyName WRITE set##capitalName NOTIFY PropertyName##Changed) \
	__D_QT_PROPERTY_DEFINE(Type, PropertyName, VariableAccessType), defaultValue                 \
	}                                                                                            \
	;                                                                                            \
	GetterSetterAccessType:                                                                      \
	__D_QT_PROPERTY_GETTER(GetterSetterType, PropertyName, getName, Converter)                   \
	__D_QT_PROPERTY_SETTER(GetterSetterType, PropertyName, capitalName, Converter)               \
	__D_QT_PROPERTY_EVENT(EventType, PropertyName, Changed)                                      \
	__D_PROPERTY_EVENT(EventType, PropertyName, Changed, GetterSetterAccessType)                 \
GetterSetterAccessType slots : Q_SLOT void on##capitalName##Changed(Type value)                  \
	{                                                                                            \
		UNUSED(value);                                                                           \
		emit this->PropertyName##Changed(value);                                                 \
		/*LogDebug(DST "Property " + #PropertyName + " Changed.");*/                             \
	}                                                                                            \
	void PropertyName##Initialize()                                                              \
	{                                                                                            \
		PropertyName##Changed() += D_PFUNC(                                                      \
			[this]() { this->on##capitalName##Changed(Converter(PropertyName())); });            \
	}                                                                                            \
	__D_PROPERTY_GETTER_REF_PROPERTY(Type, PropertyName, capitalName)

#define __D_QT_PROPERTY_QPROPERTY_9(Type, PropertyName, GetterSetterType, getName, capitalName, \
									defaultValue, VariableAccessType, GetterSetterAccessType,   \
									Converter)                                                  \
	__D_QT_PROPERTY_QPROPERTY_10(Type, PropertyName, GetterSetterType, getName, capitalName,    \
								 defaultValue, VariableAccessType, GetterSetterAccessType,      \
								 Converter, Type)

#define __D_QT_PROPERTY_QPROPERTY_8(Type, PropertyName, GetterSetterType, getName, capitalName, \
									defaultValue, VariableAccessType, GetterSetterAccessType)   \
	__D_QT_PROPERTY_QPROPERTY_9(Type, PropertyName, GetterSetterType, getName, capitalName,     \
								defaultValue, VariableAccessType, GetterSetterAccessType,       \
								__D_QT_NO_CONVERTER)

#define __D_QT_PROPERTY_QPROPERTY_7(Type, PropertyName, getName, capitalName, defaultValue,   \
									VariableAccessType, GetterSetterAccessType)               \
	__D_QT_PROPERTY_QPROPERTY_8(Type, PropertyName, Type, getName, capitalName, defaultValue, \
								VariableAccessType, GetterSetterAccessType)

#define __D_QT_PROPERTY_QPROPERTY_6(Type, PropertyName, getName, capitalName, defaultValue, \
									VariableAccessType)                                     \
	__D_QT_PROPERTY_QPROPERTY_7(Type, PropertyName, getName, capitalName, defaultValue,     \
								VariableAccessType, public)

#define __D_QT_PROPERTY_QPROPERTY_5(Type, PropertyName, getName, capitalName, defaultValue) \
	__D_QT_PROPERTY_QPROPERTY_6(Type, PropertyName, getName, capitalName, defaultValue, protected)

#define __D_QT_PROPERTY_QPROPERTY_4(Type, PropertyName, capitalName, defaultValue) \
	__D_QT_PROPERTY_QPROPERTY_5(Type, PropertyName, PropertyName, capitalName, defaultValue)

#define __D_QT_PROPERTY_QPROPERTY_3(Type, PropertyName, capitalName) \
	__D_QT_PROPERTY_QPROPERTY_4(Type, PropertyName, capitalName, Type())

#define __D_QT_PROPERTY_QPROPERTY_2(Type, PropertyName) \
	__D_QT_PROPERTY_QPROPERTY_3(Type, PropertyName, PropertyName)

/// \brief Add Qt property to current class is using of Qt framework same as D_QT_PROPERTY and
/// define a QProperty to use in QML side as getter/setter.
///
/// It's useful when need base type of TYPE that is real type of this property. base type will using
/// in getter/setter
#define D_QT_PROPERTY_QPROPERTY_BASE_TYPE(Type, PropertyName, capitalName, GetterSetterType)       \
	D_QT_PROPERTY_QPROPERTY_BASE_TYPE_CONVERTER(Type, PropertyName, capitalName, GetterSetterType, \
												__D_QT_NO_CONVERTER)

#define D_QT_PROPERTY_QPROPERTY_BASE_TYPE_CONVERTER(Type, PropertyName, capitalName,              \
													GetterSetterType, Converter)                  \
	__D_QT_PROPERTY_QPROPERTY_10(Type, PropertyName, GetterSetterType, PropertyName, capitalName, \
								 Type(), protected, public, Converter, Type)

#define __D_QT_PROPERTY_QPROPERTY_CHOOSE_FROM_ARG_COUNT(...)                                       \
	FUNC_RECOMPOSER_15(                                                                            \
		(__VA_ARGS__, __D_QT_PROPERTY_QPROPERTY_14, __D_QT_PROPERTY_QPROPERTY_13,                  \
		 __D_QT_PROPERTY_QPROPERTY_12, __D_QT_PROPERTY_QPROPERTY_11, __D_QT_PROPERTY_QPROPERTY_10, \
		 __D_QT_PROPERTY_QPROPERTY_9, __D_QT_PROPERTY_QPROPERTY_8, __D_QT_PROPERTY_QPROPERTY_7,    \
		 __D_QT_PROPERTY_QPROPERTY_6, __D_QT_PROPERTY_QPROPERTY_5, __D_QT_PROPERTY_QPROPERTY_4,    \
		 __D_QT_PROPERTY_QPROPERTY_3, __D_QT_PROPERTY_QPROPERTY_2, __D_QT_PROPERTY_QPROPERTY_1, ))

#define __D_QT_PROPERTY_QPROPERTY_NO_ARG_EXPANDER() , , , , , , , , , , __D_QT_PROPERTY_QPROPERTY_0
#define __D_QT_PROPERTY_QPROPERTY_MACRO_CHOOSER(...) \
	__D_QT_PROPERTY_QPROPERTY_CHOOSE_FROM_ARG_COUNT( \
		__D_QT_PROPERTY_QPROPERTY_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief Add Qt property to current class is using of Qt framework
#define D_QT_PROPERTY_QPROPERTY(...) \
	__D_QT_PROPERTY_QPROPERTY_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
/// \endpart D_QT_PROPERTY_QPROPERTY

/// \part D_QT_PROPERTY_BINDING
/// \brief D_QT_PROPERTY_BINDING definition section
#define __D_QT_PROPERTY_BINDING_DEFINE_PROPERTY(Type, name, capitalName, AccessType) \
    AccessType:                                                                      \
    std::optional<Type> m_##name##InitValue;                                         \
    Q_PROPERTY(Type name READ name WRITE set##capitalName NOTIFY name##Changed)
		//__D_QT_PROPERTY_DEFINE(Type, name, AccessType)};

#define __D_QT_PROPERTY_BINDING_SIGNAL_SLOT(Type, name, capitalName, SlotAccessType) \
signals:                                                                             \
    Q_SIGNAL void name##Changed(Type);                                               \
SlotAccessType slots : Q_SLOT void on##capitalName##Changed(Type value)              \
    {                                                                                \
        UNUSED(value);                                                               \
        emit this->name##Changed(value);                                             \
        /*LogDebug(DST "Property " + #name + " Changed.");*/                         \
    }

#define __D_QT_PROPERTY_BINDING_SETTER(Type, name, capitalName, DInstance, AccessType, Converter) \
    AccessType:                                                                                   \
    void set##capitalName(                                                                        \
        std::conditional_t<std::is_pointer<Type>::value, Type, const Type&> value)                \
    {                                                                                             \
        m_##name##InitValue = value;                                                              \
        if (m_##DInstance)                                                                        \
        {                                                                                         \
            auto convertedValue = Converter(value);                                               \
            if (m_##DInstance->name() != convertedValue)                                          \
            {                                                                                     \
                m_##DInstance->set##capitalName(convertedValue);                                  \
                emit name##Changed(value);                                                        \
            }                                                                                     \
        }                                                                                         \
    }

#define __D_QT_PROPERTY_GETTER_REF_PROPERTY(Type, PropertyName, getName, DInstance) \
	std::optional<DGE::Core::System::DPropertyRefWrap<Type>> property##getName()    \
	{                                                                               \
		if (m_##DInstance)                                                          \
		{                                                                           \
			auto&& propertyRef = this -> m_##DInstance->property##getName();        \
			return propertyRef;                                                     \
		}                                                                           \
		return std::nullopt;                                                        \
	}

#define __D_QT_PROPERTY_BINDING_GETTER(Type, name, DInstance, AccessType, Converter) \
    AccessType:                                                                      \
    template<class TemplateType = Type, bool isEnabled = std::is_pointer_v<Type>>    \
    auto name()->std::enable_if_t<!isEnabled, Type>                                  \
    {                                                                                \
        if (m_##DInstance)                                                           \
        {                                                                            \
            return Converter(m_##DInstance->name());                                 \
        }                                                                            \
        return (TemplateType{});                                                     \
    }                                                                                \
    template<class TemplateType = Type, bool isEnabled = std::is_pointer_v<Type>>    \
    auto name()->std::enable_if_t<isEnabled, Type>                                   \
    {                                                                                \
        if (m_##DInstance)                                                           \
        {                                                                            \
            return Converter(m_##DInstance->name());                                 \
        }                                                                            \
        return nullptr;                                                              \
    }

#define __D_QT_PROPERTY_BINDING_INITIALIZER(Type, name, capitalName, DInstance, AccessType, \
											Converter)                                      \
	void name##Initialize()                                                                 \
	{                                                                                       \
		if (m_##DInstance)                                                                  \
		{                                                                                   \
			auto parentChangedLambda = [this]()                                             \
			{                                                                               \
				if (!m_##DInstance || !this)                                                \
				{                                                                           \
					LogWarning(DST "Object is empty " #DInstance);                          \
					return;                                                                 \
				}                                                                           \
				this->on##capitalName##Changed(Converter(m_##DInstance->name()));           \
			};                                                                              \
			m_##DInstance->name##Changed() += D_PFUNC(parentChangedLambda);                 \
		}                                                                                   \
	}                                                                                       \
	auto& name##Changed()                                                                   \
	{                                                                                       \
		return m_##DInstance->name##Changed();                                              \
	}

#define __D_QT_PROPERTY_BINDING_GETTER_INITIALIZER_WITH_CONVERTER(Type, name, capitalName, \
                                                                  DInstance, AccessType,   \
                                                                  Converter)               \
    __D_QT_PROPERTY_BINDING_GETTER(Type, name, DInstance, AccessType, Converter)           \
    __D_QT_PROPERTY_BINDING_INITIALIZER(Type, name, capitalName, DInstance, AccessType, Converter)

#define __D_QT_PROPERTY_BINDING_GETTER_INITIALIZER(Type, name, capitalName, DInstance, AccessType) \
    __D_QT_PROPERTY_BINDING_GETTER_INITIALIZER_WITH_CONVERTER(Type, name, capitalName, DInstance,  \
                                                              AccessType,                          \
                                                              __D_QT_BINDING_CONVERTER)

#define __D_QT_PROPERTY_BINDING_GETTER_SETTER_INITIALIZER_WITH_CONVERTER(Type, name, capitalName, \
                                                                         DInstance, AccessType,   \
                                                                         Converter)               \
    __D_QT_PROPERTY_BINDING_SETTER(Type, name, capitalName, DInstance, AccessType, Converter)     \
    __D_QT_PROPERTY_BINDING_GETTER_INITIALIZER_WITH_CONVERTER(Type, name, capitalName, DInstance, \
                                                              AccessType, Converter)

#define __D_QT_PROPERTY_BINDING_GETTER_SETTER_INITIALIZER(Type, name, capitalName, DInstance, \
                                                          AccessType)                         \
    __D_QT_PROPERTY_BINDING_GETTER_SETTER_INITIALIZER_WITH_CONVERTER(Type, name, capitalName, \
                                                                     DInstance, AccessType,   \
                                                                     __D_QT_BINDING_CONVERTER)

#define __D_QT_PROPERTY_BINDING_6(Type, RealType, name, capitalName, DInstance,               \
								  MemberConvertFunction)                                      \
	__D_QT_PROPERTY_BINDING_DEFINE_PROPERTY(Type, name, capitalName, protected)               \
	__D_QT_PROPERTY_BINDING_GETTER_SETTER_INITIALIZER_WITH_CONVERTER(Type, name, capitalName, \
																	 DInstance, public,       \
																	 MemberConvertFunction)   \
	__D_QT_PROPERTY_GETTER_REF_PROPERTY(RealType, name, capitalName, DInstance)               \
	__D_QT_PROPERTY_BINDING_SIGNAL_SLOT(Type, name, capitalName, public)

#define __D_QT_PROPERTY_BINDING_5(Type, name, capitalName, DInstance, MemberConvertFunction) \
	__D_QT_PROPERTY_BINDING_6(Type, Type, name, capitalName, DInstance, __D_QT_BINDING_CONVERTER)

#define __D_QT_PROPERTY_BINDING_4(Type, name, capitalName, DInstance) \
    __D_QT_PROPERTY_BINDING_5(Type, name, capitalName, DInstance, __D_QT_BINDING_CONVERTER)

#define __D_QT_PROPERTY_BINDING_CHOOSE_FROM_ARG_COUNT(...)                                \
    FUNC_RECOMPOSER_9((__VA_ARGS__, __D_QT_PROPERTY_BINDING_8, __D_QT_PROPERTY_BINDING_7, \
                       __D_QT_PROPERTY_BINDING_6, __D_QT_PROPERTY_BINDING_5,              \
                       __D_QT_PROPERTY_BINDING_4, __D_QT_PROPERTY_BINDING_3,              \
                       __D_QT_PROPERTY_BINDING_2, __D_QT_PROPERTY_BINDING_1, ))

#define __D_QT_PROPERTY_BINDING_NO_ARG_EXPANDER() , , , , , , , , __D_QT_PROPERTY_BINDING_0
#define __D_QT_PROPERTY_BINDING_MACRO_CHOOSER(...) \
    __D_QT_PROPERTY_BINDING_CHOOSE_FROM_ARG_COUNT( \
        __D_QT_PROPERTY_BINDING_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief Bind Qt property with Engine property to sync them
#define D_QT_PROPERTY_BINDING(...) __D_QT_PROPERTY_BINDING_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
		/// \endpart D_QT_PROPERTY_BINDING

/// \part D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF
#define __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_5(Type, name, capitalName, DInstance, MemberConvertFunction)  \
    __D_QT_PROPERTY_BINDING_DEFINE_PROPERTY(Type, name, capitalName, protected)               \
    __D_QT_PROPERTY_BINDING_GETTER_SETTER_INITIALIZER_WITH_CONVERTER(Type, name, capitalName, \
                                                                     DInstance, public,       \
                                                                     MemberConvertFunction)   \
    __D_QT_PROPERTY_BINDING_SIGNAL_SLOT(Type, name, capitalName, public)

#define __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_4(Type, name, capitalName, DInstance) \
    __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_5(Type, name, capitalName, DInstance, __D_QT_BINDING_CONVERTER)

#define __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_CHOOSE_FROM_ARG_COUNT(...)                                \
    FUNC_RECOMPOSER_9((__VA_ARGS__, __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_8, __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_7, \
                       __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_6, __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_5,              \
                       __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_4, __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_3,              \
                       __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_2, __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_1, ))

#define __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_NO_ARG_EXPANDER() , , , , , , , , __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_0
#define __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_MACRO_CHOOSER(...) \
    __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_CHOOSE_FROM_ARG_COUNT( \
        __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief Bind Qt property with Engine property to sync them without defining property reference function
#define D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF(...) __D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
/// \endpart D_QT_PROPERTY_BINDING_WITHOUT_PROPERTY_REF

/// \part D_QT_READONLY_PROPERTY_BINDING
/// \brief D_QT_READONLY_PROPERTY_BINDING definition section
#define __D_QT_READONLY_PROPERTY_BINDING_DEFINE_PROPERTY(Type, name, AccessType) \
	AccessType:                                                                  \
	Q_PROPERTY(Type name READ name NOTIFY name##Changed)

#define __D_QT_READONLY_PROPERTY_BINDING_6(Type, RealType, name, capitalName, DInstance,          \
										   MemberConvertFunction)                                 \
	__D_QT_READONLY_PROPERTY_BINDING_DEFINE_PROPERTY(Type, name, protected)                       \
	__D_QT_PROPERTY_BINDING_GETTER_INITIALIZER_WITH_CONVERTER(Type, name, capitalName, DInstance, \
															  public, MemberConvertFunction)      \
	__D_QT_PROPERTY_GETTER_REF_PROPERTY(RealType, name, capitalName, DInstance)                   \
	__D_QT_PROPERTY_BINDING_SIGNAL_SLOT(Type, name, capitalName, public)

#define __D_QT_READONLY_PROPERTY_BINDING_5(Type, name, capitalName, DInstance,   \
										   MemberConvertFunction)                \
	__D_QT_READONLY_PROPERTY_BINDING_6(Type, Type, name, capitalName, DInstance, \
									   __D_QT_BINDING_CONVERTER)

#define __D_QT_READONLY_PROPERTY_BINDING_4(Type, name, capitalName, DInstance) \
    __D_QT_READONLY_PROPERTY_BINDING_5(Type, name, capitalName, DInstance, __D_QT_BINDING_CONVERTER)

#define __D_QT_READONLY_PROPERTY_BINDING_CHOOSE_FROM_ARG_COUNT(...)                            \
    FUNC_RECOMPOSER_9((__VA_ARGS__, __D_QT_READONLY_PROPERTY_BINDING_8,                        \
                       __D_QT_READONLY_PROPERTY_BINDING_7, __D_QT_READONLY_PROPERTY_BINDING_6, \
                       __D_QT_READONLY_PROPERTY_BINDING_5, __D_QT_READONLY_PROPERTY_BINDING_4, \
                       __D_QT_READONLY_PROPERTY_BINDING_3, __D_QT_READONLY_PROPERTY_BINDING_2, \
                       __D_QT_READONLY_PROPERTY_BINDING_1, ))

#define __D_QT_READONLY_PROPERTY_BINDING_NO_ARG_EXPANDER() \
    , , , , , , , , __D_QT_READONLY_PROPERTY_BINDING_0
#define __D_QT_READONLY_PROPERTY_BINDING_MACRO_CHOOSER(...) \
    __D_QT_READONLY_PROPERTY_BINDING_CHOOSE_FROM_ARG_COUNT( \
        __D_QT_READONLY_PROPERTY_BINDING_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief Bind read-only Qt property with read-only Engine property to sync them
#define D_QT_READONLY_PROPERTY_BINDING(...) \
    __D_QT_READONLY_PROPERTY_BINDING_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
		/// \endpart D_QT_READONLY_PROPERTY_BINDING

#define RegisterQmlType(Type, ImportName, Major, Minor, QMLComponentName, ...)                \
    GenericRegisterQmlType(qmlRegisterType, Type, ImportName, Major, Minor, QMLComponentName, \
                           ##__VA_ARGS__)

#define RegisterQmlSingletonType(Type, ImportName, Major, Minor, QMLComponentName, ...) \
    GenericRegisterQmlType(qmlRegisterSingletonType, Type, ImportName, Major, Minor,    \
                           QMLComponentName, ##__VA_ARGS__)

#define GenericRegisterQmlType(QmlFunctionType, Type, ImportName, Major, Minor, QMLComponentName,   \
                               ...)                                                                 \
    {                                                                                               \
        auto id = QmlFunctionType<Type>(ImportName, Major, Minor, QMLComponentName, ##__VA_ARGS__); \
        if (id >= 0)                                                                                \
            qDebug() << QMLComponentName << " register in Qt with id: " << id;                      \
        else                                                                                        \
            qDebug() << QMLComponentName << " registeration in Qt is failed";                       \
    }
    } // namespace QtWrapper
} // namespace DGE
