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
 * Context class is using for storing heat of the engine class
 */
#pragma once
#include <Core/System/Context.h>

#include <Core/IO/LogSystem/Log.h>
#include <Core/System/DSubsystem.h>
#include <any>
#include <memory>
#include <type_traits>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			template<typename T>
			bool Context::registerSubsystem(
				std::shared_ptr<T> object,
				typename std::enable_if<std::is_base_of_v<DSubsystem, T>, bool>::type)
			{
				if (!object)
					return false;
				auto name = typeid(T).name();
				{
					std::scoped_lock<std::mutex> lockSubsystem(m_subsystemsLock);
					if (m_subsystems.find(name) != m_subsystems.end())
					{
						LogError(DST("A duplicate subsystem is registered: \"") + name + "\"");
						return false;
					}
					m_subsystems.insert({name, object});
				}
				if (m_isEnableVerboseLog)
					LogDebug(DST("The new subsystem is registered: \"") + name + "\"");
				return true;
			}

			template<typename T>
			bool Context::registerSubsystem(
				std::shared_ptr<T> object,
				typename std::enable_if<!std::is_base_of_v<DSubsystem, T>, bool>::type)
			{
				if (!object)
					return false;
				auto name = typeid(T).name();
				{
					std::scoped_lock<std::mutex> lockSubsystem(m_subsystemsLock);
					if (m_subsystems.find(name) != m_subsystems.end())
					{
						LogError(DST("A duplicate subsystem is registered: \"") + name + "\"");
						return false;
					}
					m_subsystems.insert({name, object});
				}
				if (m_isEnableVerboseLog)
					LogDebug(DST("The new subsystem is registered: \"") + name + "\"");
				return true;
			}

			template<class T>
#ifdef requires
				requires requires(T* t) { t->typeInfo(); }
#endif
			bool Context::registerSubsystem(
				T* object, typename std::enable_if<std::is_base_of_v<DSubsystem, T>, bool>::type)
			{
				if (object == nullptr)
					return false;
				auto name = object->typeInfo().typeInfoRef->name();
				{
					std::scoped_lock<std::mutex> lockSubsystem(m_subsystemsLock);
					if (m_subsystems.find(name) != m_subsystems.end())
					{
						LogError(DST("A duplicate subsystem is registered: \"") + name + "\"");
						return false;
					}
					std::shared_ptr<T> sharedPtr;
					sharedPtr.reset(object, [](T*) {});
					m_subsystems.insert({name, sharedPtr});
				}
				if (m_isEnableVerboseLog)
					LogDebug(DST("The new subsystem is registered: \"") + name + "\"");
				return true;
			}

			template<class T>
#ifdef requires
				requires requires(T* t) { t->typeInfo(); }
#endif
			bool Context::registerSubsystem(
				T* object, typename std::enable_if<!std::is_base_of_v<DSubsystem, T>, bool>::type)
			{
				if (object == nullptr)
					return false;
				auto name = object->typeInfo().typeInfoRef->name();
				{
					std::scoped_lock<std::mutex> lockSubsystem(m_subsystemsLock);
					if (m_subsystems.find(name) != m_subsystems.end())
					{
						LogError(DST("A duplicate subsystem is registered: \"") + name + "\"");
						return false;
					}
					std::shared_ptr<T> sharedPtr;
					sharedPtr.reset(object, [](T*) {});
					m_subsystems.insert({name, sharedPtr});
				}
				if (m_isEnableVerboseLog)
					LogDebug(DST("The new subsystem is registered: \"") + name + "\"");
				return true;
			}

			template<typename T>
			std::weak_ptr<T> Context::getRegisteredSubsystem(
				typename std::enable_if<std::is_base_of_v<DSubsystem, T>, bool>::type)
			{
				std::scoped_lock<std::mutex> lockSubsystem(m_subsystemsLock);
				auto it = m_subsystems.find(typeid(T).name());
				if (it != m_subsystems.end())
				{
					try
					{
						return std::any_cast<std::shared_ptr<T>>(it->second);
					}
					catch (std::bad_any_cast& exceptionValue)
					{
						LogError(DST "Can't convert found subsystem into " + typeid(T).name() + "\n"
								 + exceptionValue.what());
						return std::weak_ptr<T>();
					}
				}
				else
					return std::weak_ptr<T>();
			}

			template<typename T>
			std::weak_ptr<T> Context::getRegisteredSubsystem(
				typename std::enable_if<!std::is_base_of_v<DSubsystem, T>, bool>::type)
			{
				std::scoped_lock<std::mutex> lockSubsystem(m_subsystemsLock);
				auto it = m_subsystems.find(typeid(T).name());
				if (it != m_subsystems.end())
				{
					try
					{
						return std::any_cast<std::shared_ptr<T>>(it->second);
					}
					catch (std::bad_any_cast&)
					{
						LogError(DST "Can't convert found subsystem into " + typeid(T).name());
						return std::weak_ptr<T>();
					}
				}
				else
					return std::weak_ptr<T>();
			}

			template<typename T>
			bool Context::releaseSubsystem()
			{
				std::map<DString, std::any>::node_type it;
				{
					std::scoped_lock<std::mutex> lockSubsystem(m_subsystemsLock);
					it = m_subsystems.extract(typeid(T).name());
				}

				return !it.empty();
			}

			template<typename T>
			bool Context::releaseSubsystem(T*)
			{
				std::map<DString, std::any>::node_type it;
				{
					std::scoped_lock<std::mutex> lockSubsystem(m_subsystemsLock);
					it = m_subsystems.extract(typeid(T).name());
				}
				return !it.empty();
			}

			template<typename T>
			bool Context::releaseSubsystem(std::shared_ptr<T>)
			{
				std::map<DString, std::any>::node_type it;
				{
					std::scoped_lock<std::mutex> lockSubsystem(m_subsystemsLock);
					it = m_subsystems.extract(typeid(T).name());
				}
				return !it.empty();
			}

			template<typename ObjectType>
			ObjectType* Context::object(uint64 id)
			{
				std::scoped_lock<std::mutex> lock(m_objectsLock);
				if (m_objects.count(id) > 0)
					return (ObjectType*) m_objects.at(id);
				return nullptr;
			}
		} // namespace System
	}	  // namespace Core
} // namespace DGE
