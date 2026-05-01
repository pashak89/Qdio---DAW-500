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
#include <Core/System/DObject.h>
#include <Core/System/DSubsystem.h>
#include <type_traits>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			/// \brief Abstract class for object factory
			class DGE_CoreAPI AbstractDObjectFactory : public Core::System::DObject
			{
				D_OBJECT(AbstractDObjectFactory)
			public:
				AbstractDObjectFactory(DGE::Core::System::ContextRef context);

				/// \brief Abstract method that Create an object that this class is implemented for
				/// it. It's factory method
				virtual DObjectRef create();

				/// \brief Get type information of object that this class create it
				const TypeInfoRef& objectTypeInfo() const;

			protected:
				/// \brief store type information of object that this class create it
				TypeInfoRef m_objectTypeInfo;
			};

			/// \brief Template class for create object factory in Engine
			template<class T, class Enable = void>
			class DObjectFactory;

			/// \brief Template class for create object factory in Engine
			template<class T>
			class DObjectFactory<T, typename std::enable_if<std::is_convertible_v<T*, DObject*>>::type>
				: public AbstractDObjectFactory
			{
			public:
				// template<typename U = T,
				// 		 typename	= std::enable_if_t<std::is_convertible_v<U*, DObject*>>>
				DObjectFactory(DGE::Core::System::ContextRef context
							   = DGE::Core::System::Context::getInstance())

					: AbstractDObjectFactory(context)
				{
					auto& typeInfo = typeid(T);
					m_objectTypeInfo.reset(&typeInfo, [](const std::type_info* p) { UNUSED(p); });
				}

				// template<typename U = T,
				// 		 typename	= std::enable_if_t<!std::is_convertible_v<U*, DObject*>>>
				// DObjectFactory(DGE::Core::System::ContextRef context)

				// 	: AbstractDObjectFactory(context)
				// {
				// 	auto& typeInfo = typeid(U);
				// 	m_objectTypeInfo.reset(&typeInfo, [](const std::type_info* p) { UNUSED(p); });
				// }

				// AbstractDObjectFactory interface
				/// \brief Get a shared pointer to created object that this class create it
				///
				/// \return a shared pointer to created object that this class create it
				template<typename... Args>
				auto createObject(Args... args)
				{
					auto sharedPtr = std::make_shared<T>(args..., m_context);
					return sharedPtr;
				}

				virtual DObjectRef create() override
				{
					return std::static_pointer_cast<DObject>(createObject());
				}
			};

			/// \brief Template class for create object factory in Engine
			template<class T>
			class DObjectFactory<T,
								 typename std::enable_if<!std::is_convertible_v<T*, DObject*>>::type>
				: public AbstractDObjectFactory
			{
				D_OBJECT(DObjectFactory)
			public:
				// template<typename U = T,
				// 		 typename	= std::enable_if_t<std::is_convertible_v<U*, DObject*>>>
				DObjectFactory(DGE::Core::System::ContextRef context
							   = DGE::Core::System::Context::getInstance())

					: AbstractDObjectFactory(context)
				{
					auto& typeInfo = typeid(T);
					m_objectTypeInfo.reset(&typeInfo, [](const std::type_info* p) { UNUSED(p); });
				}

				// template<typename U = T,
				// 		 typename	= std::enable_if_t<!std::is_convertible_v<U*, DObject*>>>
				// DObjectFactory(DGE::Core::System::ContextRef context)

				// 	: AbstractDObjectFactory(context)
				// {
				// 	auto& typeInfo = typeid(U);
				// 	m_objectTypeInfo.reset(&typeInfo, [](const std::type_info* p) { UNUSED(p); });
				// }

				// AbstractDObjectFactory interface
				/// \brief Get a shared pointer to created object that this class create it
				///
				/// \return a shared pointer to created object that this class create it
				template<typename... Args>
				auto createObject(Args... args)
				{
					auto sharedPtr = std::make_shared<T>(args...);
					return sharedPtr;
				}

				virtual DObjectRef create() override { return nullptr; }
			};

			template<class T>
			using DObjectFactoryRef = std::shared_ptr<DObjectFactory<T>>;
		} // namespace System
	}	  // namespace Core
} // namespace DGE
