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
#include "SingletonMacrosDef.h"
#include <Core/Config/DCoreConfig.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/Sync/Mutex.h>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include <Core/Utility/lib/typedemangler.hpp>

#if DGE_Platform == DGE_Android_Platform \
	&& (!defined(DGE_Track_Memory) || !defined(DGE_UseCRTDebugMemory))
#include <memory>
namespace std
{
	template<class _Tp, class _Up>
	shared_ptr<_Tp> reinterpret_pointer_cast(const shared_ptr<_Up>& __r)
	{
		return shared_ptr<_Tp>(__r,
							   reinterpret_cast<typename shared_ptr<_Tp>::element_type*>(__r.get()));
	}
} // namespace std
#endif

#if DGE_SINGLETON_MAPPED
#include <Core/Utility/lib/typemap.hpp>
#endif

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
#if DGE_SINGLETON_MAPPED
			extern DGE_CoreAPI bool unlockSingleton();
			extern DGE_CoreAPI bool getSingleton(const std::type_info&	  type,
												 std::shared_ptr<MemPtr>& theSingleton,
												 bool					  unlockMutex = false);
			extern DGE_CoreAPI void storeSingleton(const std::type_info&	type,
												   std::shared_ptr<MemPtr>& theSingleton);
			extern DGE_CoreAPI void removeSingleton(const std::type_info& type);
#endif

			class DoNotCopy
			{
			protected:
				DoNotCopy(void)				= default;
				DoNotCopy(const DoNotCopy&) = delete;
				DoNotCopy& operator=(const DoNotCopy&) = delete;
			};

			class DoNotMove
			{
			protected:
				DoNotMove(void)		   = default;
				DoNotMove(DoNotMove&&) = delete;
				DoNotMove& operator=(DoNotMove&&) = delete;
			};

			class DoNotCopyMove
				: public DoNotCopy
				, public DoNotMove
			{
			protected:
				DoNotCopyMove(void) = default;
			};

			template<typename TDerivedClass>
			class Deleter
			{
			public:
				static void deleter(TDerivedClass* p) { SafeDelete(p); }
				static void emptyDeleter(TDerivedClass* p) { UNUSED(p); }
			};

			template<typename TDerivedClass>
			class Provider
			{
			public:
				template<typename... ContructorArgs>
				TDerivedClass* provide(ContructorArgs... args)
				{
					return new TDerivedClass(args...);
				}
			};

			/// \brief Template class for using singleton design pattern
			/// \note Be careful when inheriting from this class out side of this library because
			/// this class won't work. Instead of this class, use GlobalSingleton.
			template<typename TDerivedClass,
					 typename pointer		 = std::shared_ptr<TDerivedClass>,
					 auto deleter			 = Deleter<TDerivedClass>::deleter,
					 typename ProviderObject = Provider<TDerivedClass>,
					 typename allocator		 = std::allocator<char>>
			class Singleton : public DoNotCopyMove
			{
			protected:
				using SingletonType
					= Singleton<TDerivedClass, pointer, deleter, ProviderObject, allocator>;

			public:
				virtual ~Singleton()
				{
#if DGE_SINGLETON_MAPPED
					s_instance.reset();
					removeSingleton(typeid(TDerivedClass));
#endif
				}
				template<typename... ContructorArgs>
				static pointer getInstance(ContructorArgs... args)
				{
					std::unique_lock<Sync::StdMutex> lock(SingletonType::s_mutex);
					ProviderObject					 provider;
					if (!SingletonType::s_instance)
					{
						allocator alc;
#if DGE_SINGLETON_MAPPED
						std::shared_ptr<MemPtr> castedAsSingleton;
						getSingleton(typeid(TDerivedClass), castedAsSingleton);
						if (!castedAsSingleton)
						{
							auto newValue = provider.provide(args...);
							if (newValue == nullptr)
							{
								unlockSingleton();
								return nullptr;
							}
							SingletonType::s_instance.reset(newValue, deleter, alc);
							castedAsSingleton = std::reinterpret_pointer_cast<MemPtr>(
								SingletonType::s_instance);
							storeSingleton(typeid(TDerivedClass), castedAsSingleton);
						}
						else
							SingletonType::s_instance = std::reinterpret_pointer_cast<TDerivedClass>(
								castedAsSingleton);
#else
						auto newValue = provider.provide(args...);
						if (newValue == nullptr)
							return nullptr;
						SingletonType::s_instance.reset(newValue, deleter, alc);
#endif
						SingletonType::s_ownerThreadID = this_thread::get_id();
						SingletonType::s_isValid	   = true;
					}

					return SingletonType::s_instance;
				}

				static pointer getInstance(void (*_Ax)(TDerivedClass*))
				{
					std::unique_lock<Sync::StdMutex> lock(SingletonType::s_mutex);
					ProviderObject					 provider;
					if (!SingletonType::s_instance)
					{
						allocator alc;
#if DGE_SINGLETON_MAPPED
						std::shared_ptr<MemPtr> castedAsSingleton;
						if (!getSingleton(typeid(TDerivedClass), castedAsSingleton))
						{
							auto newValue = provider.provide();
							if (newValue == nullptr)
							{
								unlockSingleton();
								return nullptr;
							}
							SingletonType::s_instance.reset(newValue, _Ax, alc);
							castedAsSingleton = std::reinterpret_pointer_cast<MemPtr>(
								SingletonType::s_instance);
							storeSingleton(typeid(TDerivedClass), castedAsSingleton);
						}
						else
							SingletonType::s_instance = std::reinterpret_pointer_cast<TDerivedClass>(
								castedAsSingleton);
#else
						s_instance.reset(provider.provide(), _Ax, alc);
#endif
						s_ownerThreadID = this_thread::get_id();
						s_isValid		= true;
					}

					return SingletonType::s_instance;
				}

				template<typename ClassType = TDerivedClass, typename PointerType = pointer>
				static auto releaseInstance() ->
					typename std::enable_if<std::is_same_v<PointerType, std::shared_ptr<ClassType>>,
											bool>::type
				{
					if (!SingletonType::s_instance)
						SingletonType::s_instance = internalInstanceValue();
					if (SingletonType::s_instance)
					{
						removeSingleton(typeid(TDerivedClass));
						auto refCounter = SingletonType::s_instance.use_count();
						if (refCounter <= 1)
						{
							SingletonType::s_instance.reset();
							SingletonType::s_isValid = false;
							return true;
						}
						else
						{
							std::cout << "Can't release instance of "
									  << typeid(TDerivedClass).name()
									  << " because the reference counter isn't "
									  << "zero and is: " << refCounter << std::endl;
						}
					}
					return false;
				}

				template<typename ClassType = TDerivedClass, typename PointerType = pointer>
				static auto releaseInstance() ->
					typename std::enable_if<!std::is_same_v<PointerType, std::shared_ptr<ClassType>>,
											bool>::type
				{
					if (hasInstance())
					{
						SafeDelete(SingletonType::s_instance);
						SingletonType::s_isValid = false;
						return true;
					}
					return false;
				}

				/// \brief Use for getting internal instance for using in unit test only. Use
				/// getInstance for real cases
				static pointer internalInstanceValue()
				{
					if (!SingletonType::s_instance)
					{
#if DGE_SINGLETON_MAPPED
						std::shared_ptr<MemPtr> castedAsSingleton;
						if (getSingleton(typeid(TDerivedClass), castedAsSingleton, true))
						{
							SingletonType::s_instance = std::reinterpret_pointer_cast<TDerivedClass>(
								castedAsSingleton);
						}
#endif
					}
					return SingletonType::s_instance;
				}

				static std::thread::id ownerThreadID() { return SingletonType::s_ownerThreadID; }

				static bool isValid() { return SingletonType::s_isValid; }

				static bool canRelease()
				{
					return SingletonType::s_isValid && SingletonType::s_instance.use_count() < 3;
				}

			protected:
				static bool hasInstance()
				{
					if (!SingletonType::s_instance)
					{
#if DGE_SINGLETON_MAPPED
						std::shared_ptr<MemPtr> castedAsSingleton;
						if (getSingleton(typeid(TDerivedClass), castedAsSingleton, true))
						{
							SingletonType::s_instance = std::reinterpret_pointer_cast<TDerivedClass>(
								castedAsSingleton);
							return true;
						}
						return false;
#endif
					}
					return true;
				}

				static inline pointer s_instance;
				inline static Sync::StdMutex	  s_mutex;
				inline static std::thread::id	  s_ownerThreadID;
				inline static atomic_bool		  s_isValid = false;
			};
			/*#ifdef DGE_CoreDLLExport
			template std::shared_ptr<Core::IO::FileSystem::FileManager>
				Singleton<std::shared_ptr<Core::IO::FileSystem::FileManager>>::s_instance;
			#endif*/

		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
