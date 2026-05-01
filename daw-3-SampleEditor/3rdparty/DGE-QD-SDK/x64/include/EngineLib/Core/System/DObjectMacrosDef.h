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
 * Base Object type for all the engine class
 */
#pragma once

#include <Core/BaseTypes/DMacro.h>
#include <Core/Utility/Singleton/SingletonMacrosDef.h>

namespace DGE
{
    namespace Core
    {
        namespace System
        {

            struct DTypeInfo
            {
                TypeInfoRef typeInfoRef;
                DString		instanceName;

                DString toString() const { return DST(typeInfoRef->name()) + "_" + instanceName; }
				bool	operator==(const DString &input) const { return this->toString() == input; }
				bool   operator==(const DTypeInfo &input) const
                {
					return (*typeInfoRef.get()) == (*input.typeInfoRef.get());
				}

                bool typeEqual(TypeInfoRef typeRef) const
                {
                    return (*typeInfoRef.get()) == (*typeRef.get());
                }
            };
            using DTypeInfoRef = std::shared_ptr<DGE::Core::System::DTypeInfo>;

            template<typename T>
            T &getParentObject(DGE::Core::System::DTypeInfo (T::*)());

/// D_EMIT definition section
#define __D_EMIT(event) event()

#define __D_DO_NOTHING()

#define __D_PROPERTY_DEFINE(Type, PropertyName, AccessType)                        \
	AccessType:                                                                    \
	DGE::Core::System::DProperty<Type> m_##PropertyName = {this->metaCoreObject(), \
														   this->typeInfo(), #PropertyName

#define __D_PROPERTY_GETTER(Type, PropertyName, getName) \
    Type getName() const                                 \
    {                                                    \
        return this->m_##PropertyName;                   \
    }

#define __D_PROPERTY_GETTER_REF_PROPERTY(Type, PropertyName, capitalName) \
    DGE::Core::System::DProperty<Type> &property##capitalName()           \
    {                                                                     \
        return this->m_##PropertyName;                                    \
    }

#define __D_PROPERTY_SETTER(Type, PropertyName, capitalName) \
    /* */                                                    \
    void set##capitalName(const Type &value)                 \
    {                                                        \
        if (this->m_##PropertyName != value)                 \
        {                                                    \
            this->m_##PropertyName = value;                  \
        }                                                    \
    }

#define __D_PROPERTY_EVENT_GETTER(Type, PropertyName, EventName)    \
	DGE::Core::System::Event<void, Type> &PropertyName##EventName() \
	{                                                               \
		return this->m_##PropertyName.value##EventName();           \
	}

#define __D_PROPERTY_EVENT(Type, PropertyName, EventName, AccessType) \
                                                                      \
    AccessType:                                                       \
    __D_DO_NOTHING()                                                  \
    /* The engine event*/                                             \
    __D_PROPERTY_EVENT_GETTER(Type, PropertyName, EventName)

#define __D_PROPERTY_GETTER_CHANGED_EVENT(Type, PropertyName, getName, AccessType) \
    __D_PROPERTY_EVENT(Type, PropertyName, Changed, AccessType)                    \
    __D_PROPERTY_GETTER(Type, PropertyName, getName)

#define __D_PROPERTY_GETTER_SETTER_CHANGED_EVENT(RealType, CompatibleType, PropertyName, getName, \
												 capitalName, AccessType)                         \
	__D_PROPERTY_GETTER_CHANGED_EVENT(CompatibleType, PropertyName, getName, AccessType)          \
	__D_PROPERTY_GETTER_REF_PROPERTY(RealType, PropertyName, capitalName)                         \
	__D_PROPERTY_SETTER(CompatibleType, PropertyName, capitalName)

#define __D_PROPERTY_8(Type, PropertyName, GetterSetterType, getName, capitalName, defaultValue, \
					   VariableAccessType, GetterSetterAccessType)                               \
	__D_PROPERTY_DEFINE(Type, PropertyName, VariableAccessType), defaultValue                    \
	}                                                                                            \
	;                                                                                            \
	__D_PROPERTY_GETTER_SETTER_CHANGED_EVENT(Type, GetterSetterType, PropertyName, getName,      \
											 capitalName, GetterSetterAccessType)

#define __D_PROPERTY_7(Type, PropertyName, getName, capitalName, defaultValue, VariableAccessType, \
                       GetterSetterAccessType)                                                     \
    __D_PROPERTY_8(Type, PropertyName, Type, getName, capitalName, defaultValue,                   \
                   VariableAccessType, GetterSetterAccessType)

#define __D_PROPERTY_6(Type, PropertyName, getName, capitalName, defaultValue, VariableAccessType) \
    __D_PROPERTY_7(Type, PropertyName, getName, capitalName, defaultValue, VariableAccessType,     \
                   public)

#define __D_PROPERTY_5(Type, PropertyName, getName, capitalName, defaultValue) \
    __D_PROPERTY_6(Type, PropertyName, getName, capitalName, defaultValue, protected)

#define __D_PROPERTY_4(Type, PropertyName, capitalName, defaultValue) \
    __D_PROPERTY_5(Type, PropertyName, PropertyName, capitalName, defaultValue)

#define __D_PROPERTY_3(Type, PropertyName, capitalName) \
    __D_PROPERTY_4(Type, PropertyName, capitalName, Type())

#define __D_PROPERTY_2(Type, PropertyName) __D_PROPERTY_3(Type, PropertyName, PropertyName)

#define __D_PROPERTY_1(Type) D_ERROR_MACRO("Must At-least give 2 parameters")

#define __D_PROPERTY_0() __D_PROPERTY_1(void)

/// \brief D_PROPERTY definition section
#define __D_PROPERTY_CHOOSE_FROM_ARG_COUNT(...)                                                     \
    FUNC_RECOMPOSER_9((__VA_ARGS__, __D_PROPERTY_8, __D_PROPERTY_7, __D_PROPERTY_6, __D_PROPERTY_5, \
                       __D_PROPERTY_4, __D_PROPERTY_3, __D_PROPERTY_2, __D_PROPERTY_1, ))

#define __D_PROPERTY_NO_ARG_EXPANDER() , , , , , , , , __D_PROPERTY_0
#define __D_PROPERTY_MACRO_CHOOSER(...) \
    __D_PROPERTY_CHOOSE_FROM_ARG_COUNT(__D_PROPERTY_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief Define the property for DObject class
#define D_PROPERTY(...) __D_PROPERTY_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

/// \brief Define the read-only property macro for DObject class
#define __D_READONLY_PROPERTY_8(Type, PropertyName, getName, capitalName, GetterSetterType, \
								defaultValue, VariableAccessType, GetterSetterAccessType)   \
	__D_PROPERTY_DEFINE(Type, PropertyName, VariableAccessType), defaultValue, true         \
	}                                                                                       \
	;                                                                                       \
	__D_PROPERTY_GETTER_CHANGED_EVENT(GetterSetterType, PropertyName, getName,              \
									  GetterSetterAccessType)                               \
	__D_PROPERTY_GETTER_REF_PROPERTY(Type, PropertyName, capitalName)                       \
protected:                                                                                  \
	__D_PROPERTY_SETTER(GetterSetterType, getName, capitalName)

#define __D_READONLY_PROPERTY_7(Type, PropertyName, capitalName, GetterSetterType, defaultValue,      \
                                VariableAccessType, GetterSetterAccessType)                       \
    __D_READONLY_PROPERTY_8(Type, PropertyName, PropertyName, capitalName, GetterSetterType, defaultValue, \
                            VariableAccessType, GetterSetterAccessType)

#define __D_READONLY_PROPERTY_6(Type, PropertyName, capitalName, GetterSetterType, defaultValue, \
                                VariableAccessType)                                          \
    __D_READONLY_PROPERTY_7(Type, PropertyName, capitalName, GetterSetterType, defaultValue,     \
                            VariableAccessType, public)

#define __D_READONLY_PROPERTY_5(Type, PropertyName, capitalName, GetterSetterType, defaultValue) \
    __D_READONLY_PROPERTY_6(Type, PropertyName, capitalName, GetterSetterType, defaultValue, protected)

#define __D_READONLY_PROPERTY_4(Type, PropertyName, GetterSetterType, defaultValue) \
    __D_READONLY_PROPERTY_5(Type, PropertyName, PropertyName, GetterSetterType, defaultValue)

#define __D_READONLY_PROPERTY_3(Type, PropertyName, GetterSetterType) \
    __D_READONLY_PROPERTY_4(Type, PropertyName, GetterSetterType, Type())

#define __D_READONLY_PROPERTY_2(Type, PropertyName) \
    __D_READONLY_PROPERTY_3(Type, PropertyName, Type)

#define __D_READONLY_PROPERTY_CHOOSE_FROM_ARG_COUNT(...)                                          \
    FUNC_RECOMPOSER_9((__VA_ARGS__, __D_READONLY_PROPERTY_8, __D_READONLY_PROPERTY_7,             \
                       __D_READONLY_PROPERTY_6, __D_READONLY_PROPERTY_5, __D_READONLY_PROPERTY_4, \
                       __D_READONLY_PROPERTY_3, __D_READONLY_PROPERTY_2,                          \
                       __D_READONLY_PROPERTY_1, ))

#define __D_READONLY_PROPERTY_NO_ARG_EXPANDER() , , , , , , , , __D_READONLY_PROPERTY_0
#define __D_READONLY_PROPERTY_MACRO_CHOOSER(...) \
    __D_READONLY_PROPERTY_CHOOSE_FROM_ARG_COUNT(__D_READONLY_PROPERTY_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief Define the read-only property for DObject class
#define D_READONLY_PROPERTY(...) __D_READONLY_PROPERTY_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

/// \brief Define the static property macro for DObject class
#define __D_STATIC_READONLY_PROPERTY_DEFINE(Type, PropertyName, AccessType)                      \
	AccessType:                                                                                  \
	static inline DGE::Core::System::DProperty<Type> s_##PropertyName = {staticMetaCoreObject(), \
																		 staticTypeInfo(),       \
																		 #PropertyName

#define __D_STATIC_PROPERTY_EVENT_GETTER(Type, PropertyName, EventName)          \
    static const DGE::Core::System::Event<void, Type> &PropertyName##EventName() \
    {                                                                            \
        return std::move(s_##PropertyName.value##EventName());                   \
    }

#define __D_STATIC_PROPERTY_GETTER(Type, PropertyName, getName) \
    static Type getName()                                       \
    {                                                           \
        return s_##PropertyName;                                \
    }

#define __D_STATIC_PROPERTY_SETTER(Type, PropertyName, capitalName) \
    /* */                                                           \
    static void set##capitalName(const Type &value)                 \
    {                                                               \
        if (s_##PropertyName != value)                              \
        {                                                           \
            s_##PropertyName = value;                               \
        }                                                           \
    }

#define __D_STATIC_PROPERTY_GETTER_REF_PROPERTY(Type, PropertyName, capitalName) \
    static DGE::Core::System::DProperty<Type> &property##capitalName()           \
    {                                                                            \
        return s_##PropertyName;                                                 \
    }

#define __D_STATIC_PROPERTY_EVENT(Type, PropertyName, EventName, AccessType) \
                                                                             \
    AccessType:                                                              \
    __D_DO_NOTHING()                                                         \
    /* The engine event*/                                                    \
    __D_STATIC_PROPERTY_EVENT_GETTER(Type, PropertyName, EventName)

#define __D_STATIC_READONLY_PROPERTY_GETTER_CHANGED_EVENT(Type, PropertyName, getName, AccessType) \
    __D_STATIC_PROPERTY_EVENT(Type, PropertyName, Changed, AccessType)                             \
    __D_STATIC_PROPERTY_GETTER(Type, PropertyName, getName)

#define __D_STATIC_READONLY_PROPERTY_8(Type, PropertyName, getName, capitalName, GetterSetterType, \
									   defaultValue, VariableAccessType, GetterSetterAccessType)   \
	__D_STATIC_READONLY_PROPERTY_DEFINE(Type, PropertyName, VariableAccessType)                    \
	, defaultValue, true, true                                                                     \
	}                                                                                              \
	;                                                                                              \
	__D_STATIC_READONLY_PROPERTY_GETTER_CHANGED_EVENT(GetterSetterType, PropertyName, getName,     \
													  GetterSetterAccessType)                      \
	__D_STATIC_PROPERTY_GETTER_REF_PROPERTY(Type, PropertyName, capitalName)                       \
protected:                                                                                         \
	__D_STATIC_PROPERTY_SETTER(GetterSetterType, getName, capitalName)

#define __D_STATIC_READONLY_PROPERTY_7(Type, PropertyName, getName, GetterSetterType,            \
                                       defaultValue, VariableAccessType, GetterSetterAccessType) \
    __D_STATIC_READONLY_PROPERTY_8(Type, PropertyName, getName, getName, GetterSetterType,       \
                                   defaultValue, VariableAccessType, GetterSetterAccessType)

#define __D_STATIC_READONLY_PROPERTY_6(Type, PropertyName, getName, GetterSetterType,           \
                                       defaultValue, VariableAccessType)                        \
    __D_STATIC_READONLY_PROPERTY_7(Type, PropertyName, getName, GetterSetterType, defaultValue, \
                                   VariableAccessType, public)

#define __D_STATIC_READONLY_PROPERTY_5(Type, PropertyName, getName, GetterSetterType, defaultValue) \
    __D_STATIC_READONLY_PROPERTY_6(Type, PropertyName, getName, GetterSetterType, defaultValue,     \
                                   protected)

#define __D_STATIC_READONLY_PROPERTY_4(Type, PropertyName, GetterSetterType, defaultValue) \
    __D_STATIC_READONLY_PROPERTY_5(Type, PropertyName, PropertyName, GetterSetterType, defaultValue)

#define __D_STATIC_READONLY_PROPERTY_3(Type, PropertyName, GetterSetterType) \
    __D_STATIC_READONLY_PROPERTY_4(Type, PropertyName, GetterSetterType, Type())

#define __D_STATIC_READONLY_PROPERTY_2(Type, PropertyName) \
    __D_STATIC_READONLY_PROPERTY_3(Type, PropertyName, Type)

#define __D_STATIC_READONLY_PROPERTY_CHOOSE_FROM_ARG_COUNT(...)                                     \
    FUNC_RECOMPOSER_9((__VA_ARGS__, __D_STATIC_READONLY_PROPERTY_8, __D_STATIC_READONLY_PROPERTY_7, \
                       __D_STATIC_READONLY_PROPERTY_6, __D_STATIC_READONLY_PROPERTY_5,              \
                       __D_STATIC_READONLY_PROPERTY_4, __D_STATIC_READONLY_PROPERTY_3,              \
                       __D_STATIC_READONLY_PROPERTY_2, __D_STATIC_READONLY_PROPERTY_1, ))

#define __D_STATIC_READONLY_PROPERTY_NO_ARG_EXPANDER() \
    , , , , , , , , __D_STATIC_READONLY_PROPERTY_0
#define __D_STATIC_READONLY_PROPERTY_MACRO_CHOOSER(...) \
    __D_STATIC_READONLY_PROPERTY_CHOOSE_FROM_ARG_COUNT( \
        __D_STATIC_READONLY_PROPERTY_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief Define the static property for DObject class
#define D_STATIC_READONLY_PROPERTY(...) \
    __D_STATIC_READONLY_PROPERTY_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define __D_STATIC_PROPERTY_8(Type, PropertyName, getName, capitalName, GetterSetterType,      \
							  defaultValue, VariableAccessType, GetterSetterAccessType)        \
	__D_STATIC_READONLY_PROPERTY_DEFINE(Type, PropertyName, VariableAccessType), defaultValue, \
		false, true                                                                            \
	}                                                                                          \
	;                                                                                          \
	__D_STATIC_READONLY_PROPERTY_GETTER_CHANGED_EVENT(GetterSetterType, PropertyName, getName, \
													  GetterSetterAccessType)                  \
	__D_STATIC_PROPERTY_GETTER_REF_PROPERTY(Type, PropertyName, capitalName)                   \
public:                                                                                        \
	__D_STATIC_PROPERTY_SETTER(GetterSetterType, getName, capitalName)

#define __D_STATIC_PROPERTY_7(Type, PropertyName, getName, GetterSetterType, defaultValue,      \
                              VariableAccessType, GetterSetterAccessType)                       \
    __D_STATIC_PROPERTY_8(Type, PropertyName, getName, getName, GetterSetterType, defaultValue, \
                          VariableAccessType, GetterSetterAccessType)

#define __D_STATIC_PROPERTY_6(Type, PropertyName, getName, GetterSetterType, defaultValue, \
                              VariableAccessType)                                          \
    __D_STATIC_PROPERTY_7(Type, PropertyName, getName, GetterSetterType, defaultValue,     \
                          VariableAccessType, public)

#define __D_STATIC_PROPERTY_5(Type, PropertyName, getName, GetterSetterType, defaultValue) \
	__D_STATIC_PROPERTY_6(Type, PropertyName, getName, GetterSetterType, defaultValue, protected)

#define __D_STATIC_PROPERTY_4(Type, PropertyName, GetterSetterType, defaultValue) \
	__D_STATIC_PROPERTY_5(Type, PropertyName, PropertyName, GetterSetterType, defaultValue)

#define __D_STATIC_PROPERTY_3(Type, PropertyName, GetterSetterType) \
	__D_STATIC_PROPERTY_4(Type, PropertyName, GetterSetterType, Type())

#define __D_STATIC_PROPERTY_2(Type, PropertyName) __D_STATIC_PROPERTY_3(Type, PropertyName, Type)

#define __D_STATIC_PROPERTY_CHOOSE_FROM_ARG_COUNT(...)                                      \
    FUNC_RECOMPOSER_9((__VA_ARGS__, __D_STATIC_PROPERTY_8, __D_STATIC_PROPERTY_7,           \
                       __D_STATIC_PROPERTY_6, __D_STATIC_PROPERTY_5, __D_STATIC_PROPERTY_4, \
                       __D_STATIC_PROPERTY_3, __D_STATIC_PROPERTY_2, __D_STATIC_PROPERTY_1, ))

#define __D_STATIC_PROPERTY_NO_ARG_EXPANDER() , , , , , , , , __D_STATIC_PROPERTY_0

#define __D_STATIC_PROPERTY_MACRO_CHOOSER(...) \
    __D_STATIC_PROPERTY_CHOOSE_FROM_ARG_COUNT(__D_STATIC_PROPERTY_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief Define the static property for DObject class
#define D_STATIC_PROPERTY(...) __D_STATIC_PROPERTY_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

/// \brief Define emit event
#define D_EMIT(event, ...) __D_EMIT(event)(__VA_ARGS__)

/// \brief Define D_TypeInfoFunction macro for typeInfo() function signature
#define D_TypeInfoFunction virtual DGE::Core::System::DTypeInfo typeInfo()

#define D_TypeInfo(ClassName)                                                   \
protected:                                                                      \
	static inline uint64 s_instance##ClassName = 1;                             \
	const uint64		 m_id##ClassName	   = s_instance##ClassName++;       \
                                                                                \
public:                                                                         \
	static inline DGE::Core::System::DTypeInfo staticTypeInfo()                 \
	{                                                                           \
		static auto					&typeInfo = typeid(ClassName);              \
		DGE::Core::System::DTypeInfo resultInfo;                                \
		resultInfo.typeInfoRef.reset(&typeInfo, [](const std::type_info *) {}); \
		resultInfo.instanceName = "";                                           \
		return resultInfo;                                                      \
	}                                                                           \
	D_TypeInfoFunction

#define D_OBJECT_BASE_TYPE(ClassName, Parent)                                                \
	using Base = typename std::remove_reference<decltype(DGE::Core::System::getParentObject( \
		&Parent::typeInfo))>::type;

#define D_OBJECT_REGISTER_IN_CONTEXT(ClassName, Parent)                                           \
                                                                                                  \
protected:                                                                                        \
	virtual bool registerInContext(DGE::Core::System::ContextRef &context) override               \
	{                                                                                             \
		if (*(ClassName::parentTypeInfo().typeInfoRef.get())                                      \
				== *(DGE::Core::System::DSubsystem::staticTypeInfo().typeInfoRef.get())           \
			|| this->hasTagType(DGE::Core::System::DObjectTagType::DSubsystem))                   \
                                                                                                  \
		{                                                                                         \
			__D_RegisterSubsystemInternal(context, this, typeInfo().typeInfoRef->name(), logInfo, \
										  logError);                                              \
		}                                                                                         \
		return true;                                                                              \
	}                                                                                             \
	virtual bool unregisterFromContext(DGE::Core::System::ContextRef &context) override           \
	{                                                                                             \
		if (*(ClassName::parentTypeInfo().typeInfoRef.get())                                      \
				== *(DGE::Core::System::DSubsystem::staticTypeInfo().typeInfoRef.get())           \
			|| this->hasTagType(DGE::Core::System::DObjectTagType::DSubsystem))                   \
                                                                                                  \
		{                                                                                         \
			if (context)                                                                          \
				context->releaseSubsystem<ClassName>();                                           \
		}                                                                                         \
		return true;                                                                              \
	}

#define __D_CORE_OBJECT_2(ClassName, Parent)                                                 \
	D_OBJECT_BASE_TYPE(ClassName, Parent)                                                    \
	virtual inline DGE::Core::System::DTypeInfo parentTypeInfo() override                    \
	{                                                                                        \
		static auto					&typeInfo = typeid(ClassName::Base);                     \
		DGE::Core::System::DTypeInfo resultInfo;                                             \
		resultInfo.typeInfoRef.reset(&typeInfo, [](const std::type_info *p) { UNUSED(p); }); \
		return resultInfo;                                                                   \
	}                                                                                        \
	D_TypeInfo(ClassName) override                                                           \
	{                                                                                        \
		static auto					&typeInfo = typeid(ClassName);                           \
		DGE::Core::System::DTypeInfo resultInfo;                                             \
		resultInfo.typeInfoRef.reset(&typeInfo, [](const std::type_info *p) { UNUSED(p); }); \
		resultInfo.instanceName = DST(#ClassName) + this->m_id##ClassName;                   \
		return resultInfo;                                                                   \
	}                                                                                        \
	virtual DString typeName() const override                                                \
	{                                                                                        \
		return #ClassName;                                                                   \
	}                                                                                        \
	static inline DString staticTypeName()                                                   \
	{                                                                                        \
		return #ClassName;                                                                   \
	}
#define __D_CORE_OBJECT_1(ClassName) __D_CORE_OBJECT_2(ClassName, ClassName)
#define __D_CORE_OBJECT_CHOOSE_FROM_ARG_COUNT(...) \
	FUNC_RECOMPOSER_4((__VA_ARGS__, __D_CORE_OBJECT_3, __D_CORE_OBJECT_2, __D_CORE_OBJECT_1, ))

#define __D_CORE_OBJECT_NO_ARG_EXPANDER() , , , __D_CORE_OBJECT_0

#define __D_CORE_OBJECT_MACRO_CHOOSER(...) \
	__D_CORE_OBJECT_CHOOSE_FROM_ARG_COUNT(__D_CORE_OBJECT_NO_ARG_EXPANDER __VA_ARGS__())

/// \brief
#define D_CORE_OBJECT(...) __D_CORE_OBJECT_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define D_OBJECT_COMPACT(ClassName, Parent) \
	D_CORE_OBJECT(ClassName, Parent)        \
	D_OBJECT_REGISTER_IN_CONTEXT(ClassName, Parent)

/// \brief Define the necessary properties and methods for DObject class
#define D_OBJECT(ClassName)                \
	D_OBJECT_COMPACT(ClassName, ClassName) \
	D_DESTRUCTOR_DEBUG(ClassName)

#define D_OBJECT_TEMPLATE(ClassName, TemplateClassName, Parent) \
	D_OBJECT_COMPACT(ClassName, Parent)                         \
                                                                \
	D_DESTRUCTOR_DEBUG(TemplateClassName)

#define D_OBJECT_SINGLETON(type, ...) \
    D_OBJECT(type)                    \
    D_SINGLETON_INTERNAL(type, ##__VA_ARGS__)

#define D_DefRefType(ClassName)                         \
	using ClassName##URef = std::unique_ptr<ClassName>; \
	using ClassName##Ref  = std::shared_ptr<ClassName>; \
	using ClassName##WRef = std::weak_ptr<ClassName>;

#define D_DefRefTypeTemplate(ClassName)                       \
	template<typename Type>                                   \
	using ClassName##URef = std::unique_ptr<ClassName<Type>>; \
	template<typename Type>                                   \
	using ClassName##Ref = std::shared_ptr<ClassName<Type>>;  \
	template<typename Type>                                   \
	using ClassName##WRef = std::weak_ptr<ClassName<Type>>;
		} // namespace System
	}	  // namespace Core
} // namespace DGE
