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

#include <Core/MemoryManager/MemoryManager.h>
#include <Core/System/DCoreObject.h>
#include <Core/System/DObjectMacrosDef.h>
#include <Core/Utility/Interfaces/IInitialize.h>
#include <Core/Utility/Singleton/Singleton.h>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			/// TODO: Clear message of registering and initializing subsystems.
#define D_RegisterSubsystem(contextObject, subsystemObject, subsystemName) \
	__D_RegisterSubsystem(contextObject, subsystemObject, #subsystemName)

#define __D_RegisterSubsystemInternal(contextObject, subsystemObject, subsystemName, loggerInfo, \
									  loggerError)                                               \
	if (contextObject && subsystemObject && contextObject->registerSubsystem(subsystemObject))   \
	{                                                                                            \
		loggerInfo(DST("The subsystem \"") + DST(subsystemName)                                  \
				   + "\" is registered successfully.");                                          \
	}                                                                                            \
	else                                                                                         \
	{                                                                                            \
		loggerError(DST("The registration of subsystem \"") + DST(subsystemName)                 \
					+ "\" is failed.");                                                          \
	}

#define __D_RegisterSubsystem(contextObject, subsystemObject, subsystemName) \
	__D_RegisterSubsystemInternal(contextObject, subsystemObject, subsystemName, LogInfo, LogError)

/// \brief Helper macro for simplify and unifying initialize process and log
///
/// \param object It's the object that want to initialize it.
/// \param contextObject The contect object that is used for initializing the object
/// \param initParam It's input initialize parameter
/// \param objectName It's name of object that is showing in log message
#define InitializeAndRegister(object, contextObject, initParam, objectName)      \
	if (object->initialize(initParam))                                           \
	{                                                                            \
		if (contextObject->registerSubsystem(object))                            \
		{                                                                        \
			LogInfo(DST("The \"" #objectName "\" is registered successfully.")); \
		}                                                                        \
		else                                                                     \
		{                                                                        \
			LogError(DST("*The registration \"" #objectName "\" is failed."));   \
		}                                                                        \
	}                                                                            \
	else                                                                         \
	{                                                                            \
		LogError("*The initialization \"" #objectName "\" is failed.");          \
	}

/// \brief Helper macro for simplify and unifying initialize process and log
///
/// \param object It's the object that want to initialize it.
/// \param contextObject The contect object that is used for initializing the object
/// \param objectName It's name of object that is showing in log message
#define D_ReleaseSubsystem(object, contextObject)                   \
	if (!contextObject->releaseSubsystem(object))                   \
	{                                                               \
		LogError(DST("*The releasing \"" #object "\" is failed.")); \
		if (object)                                                 \
		{                                                           \
			object->release();                                      \
			object.reset();                                         \
		}                                                           \
	}                                                               \
	else                                                            \
	{                                                               \
		object->release();                                          \
		object.reset();                                             \
	}

			class DObject;
			class DSubsystem;
			class BaseEvent;

			/// \brief Runtime engine context. Contain subsystems and global objects and etc.
			///
			/// This class is using for establish connection between subsystems
			/// and engine and simplify it. It's created after creating App object.
			class DGE_CoreAPI Context
				: public DGE::Core::System::DCoreObject
				, public Utility::Singleton<Context>
				, public DGE::Core::Utility::Interfaces::IInitialize
			{
				D_SINGLETON(Context)

				Context();

			public:
				D_Destructor(Context);
				/// IInitialize interface
				///
				/// \brief Use default implementation of
				/// initialize(std::initializer_list<std::pair<const String, std::any>>&&)
				/// function from IInitialize interface.
				using Utility::Interfaces::IInitialize::initialize;
				///
				/// \details Call this function with these parameters
				///
				/// \param parameters Input parameters for initialize this class.
				/// must set by below example:
				///
				/// \return return true if initialization is successfully otherwise return false
				///
				///	\code
				/// #include <iostream>
				/// #include <Core/Application/Application.h>
				/// int main(int argc, char* argv[])
				/// {
				///		// contain other codes
				///		DGE::Platforms::BackendInit backendInitRequest;
				///  	auto context= Context::getInstance();
				///		if(context.initialize(
				///			{
				///			{Window::s_nativeWindowKey,quickWindow},
				/// 		{DGE::Platforms::Platform::s_backendInitKey, backendInitRequest},
				///			}
				///		))
				///			std::cout<<"Context is initialized."<<std::endl;
				///		else
				///			std::cout<<"Context initialization is failed."<<std::endl;
				///
				///		return 0;
				/// }
				/// \endcode
				virtual bool initialize(
					Core::Utility::Interfaces::InitializeParams& parameters) override;

				/// Register Subsystems in engine context and it will use in future.
				///
				/// \note the type of object must inherit from DSubsystem
				///
				/// \param object is shared pointer to subsystem that want to
				/// register in the Context
				///
				/// \return return true if this subsystem is registered successfully
				///  otherwise return false. In almost situation, When duplicate
				///  subsystem requested to register that cause to failed registration.
				///
				/// \code
				///
				/// 	auto context = Context::getInstance();
				///		auto renderer = std::make_shared<GL::RendererGL>(context);
				///		context->registerSubsystem(renderer);
				///
				/// \endcode
				template<typename T>
				bool registerSubsystem(
					std::shared_ptr<T> object,
					typename std::enable_if<std::is_base_of_v<DSubsystem, T>, bool>::type = true);

				/// Register Subsystems in engine context and it will use in future.
				///
				/// \param object is shared pointer to subsystem that want to
				/// register in the Context
				///
				/// \return return true if this subsystem is registered successfully
				///  otherwise return false. In almost situation, When duplicate
				///  subsystem requested to register that cause to failed registration.
				///
				/// \code
				///
				/// 	auto context = Context::getInstance();
				///		auto renderer = std::make_shared<GL::RendererGL>(context);
				///		context->registerSubsystem(renderer);
				///
				/// \endcode
				template<typename T>
				bool registerSubsystem(
					std::shared_ptr<T> object,
					typename std::enable_if<!std::is_base_of_v<DSubsystem, T>, bool>::type = true);

				/// Register Subsystems in engine context and it will use in future.
				///
				/// \param object is shared pointer to subsystem that want to
				/// register in the Context
				///
				/// \return return true if this subsystem is registered successfully
				///  otherwise return false. In almost situation, When duplicate
				///  subsystem requested to register that cause to failed registration.
				///
				/// \code
				///
				/// 	auto context = Context::getInstance();
				///		auto renderer = std::make_shared<GL::RendererGL>(context);
				///		context->registerSubsystem(renderer.get());
				///
				/// \endcode
				template<class T>
#ifdef requires
					requires requires(T* t) { t->typeInfo(); }
#endif
				bool registerSubsystem(
					T* object,
					typename std::enable_if<std::is_base_of_v<DSubsystem, T>, bool>::type = true);

				/// Register Subsystems in engine context and it will use in future.
				///
				/// \param object is shared pointer to subsystem that want to
				/// register in the Context
				///
				/// \return return true if this subsystem is registered successfully
				///  otherwise return false. In almost situation, When duplicate
				///  subsystem requested to register that cause to failed registration.
				///
				/// \code
				///
				/// 	auto context = Context::getInstance();
				///		auto renderer = std::make_shared<GL::RendererGL>(context);
				///		context->registerSubsystem(renderer.get());
				///
				/// \endcode
				template<class T>
#ifdef requires
					requires requires(T* t) { t->typeInfo(); }
#endif
				bool registerSubsystem(
					T* object,
					typename std::enable_if<!std::is_base_of_v<DSubsystem, T>, bool>::type = true);

				/// Can get subsystem very simply
				///
				/// \return
				/// return a shared pointer to requested subsystem with type DObject
				///
				/// \code
				///
				/// auto context = Context::getInstance();
				/// auto renderer = context->getRegisteredSubsystem<GL::RendererGL>();
				///
				/// \endcode
				template<typename T>
				std::weak_ptr<T> getRegisteredSubsystem(
					typename std::enable_if<std::is_base_of_v<DSubsystem, T>, bool>::type = true);

				/// Can get subsystem very simply
				///
				/// \return
				/// return a shared pointer to requested subsystem with type DObject
				///
				/// \code
				///
				/// auto context = Context::getInstance();
				/// auto renderer = context->getRegisteredSubsystem<GL::RendererGL>();
				///
				/// \endcode
				template<typename T>
				std::weak_ptr<T> getRegisteredSubsystem(
					typename std::enable_if<!std::is_base_of_v<DSubsystem, T>, bool>::type = true);
				/// Release registered subsystem
				///
				/// \return
				/// If releasing register subsystem is successful return true else return false
				///
				/// \code
				///
				/// auto context = Context::getInstance();
				/// auto renderer = context->releaseSubsystem<GL::RendererGL>();
				///
				/// \endcode
				template<typename T>
				bool releaseSubsystem();

				/// Release registered subsystem
				///
				/// \return
				/// If releasing register subsystem is successful return true else return false
				///
				/// \code
				///
				/// auto context = Context::getInstance();
				/// GL::RendererGL renderer;
				/// auto renderer = context->releaseSubsystem(renderer.get());
				///
				/// \endcode
				template<typename T>
				bool releaseSubsystem(T*);

				/// Release registered subsystem
				///
				/// \return
				/// If releasing register subsystem is successful return true else return false
				///
				/// \code
				///
				/// auto context = Context::getInstance();
				/// GL::RendererGL renderer;
				/// auto renderer = context->releaseSubsystem(renderer);
				///
				/// \endcode
				template<typename T>
				bool releaseSubsystem(std::shared_ptr<T>);

				/// \brief register event for use later
				bool registerEvent(const DTypeInfo& typeinfo, const DString eventName,
								   BaseEvent* eventObject);

				/// \brief register object for use later
				bool registerObject(uint64 id, DObject* objectPtr);

				/// Release registered object
				///
				/// \return
				/// If releasing register object is successful return true else return false
				///
				/// \code
				///
				/// auto context = Context::getInstance();
				/// auto renderer = context->releaseObject<GL::RendererGL>();
				///
				/// \endcode
				bool releaseObject(uint64 id);

				/// \brief get object by using id of it
				template<typename ObjectType>
				ObjectType* object(uint64 id);

				/// \brief Release all objects that is registered in the object list of context;
				void clearObjectList();

				/// \brief Release all resources that is registered in the context;
				bool releaseAllResources(bool printRemainResources = false);

				bool isEnableVerboseLog() const;
				void setIsEnableVerboseLog(bool newIsEnableVerboseLog);

			protected:
				std::map<DString, std::any>						 m_subsystems;
				std::mutex										 m_subsystemsLock;
				std::map<DString, std::map<DString, BaseEvent*>> m_events;
				std::mutex										 m_eventsLock;
				std::unordered_map<uint64, void*>				 m_objects;
				std::mutex										 m_objectsLock;
				std::atomic_bool								 m_isEnableVerboseLog;
			};

			D_DefRefType(Context);
		} // namespace System

	} // namespace Core
} // namespace DGE
