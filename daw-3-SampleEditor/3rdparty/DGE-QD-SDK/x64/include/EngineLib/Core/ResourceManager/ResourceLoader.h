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

#include <Core/Array/Array.h>
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/BaseTypes/DPath.h>
#include <Core/IO/LogSystem/LogMacrosDef.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/Loaders/Loader.h>
#include <Core/System/DObjectMacrosDef.h>
#include <Core/System/DProperty.h>
#include <Core/Utility/Utility.h>

namespace DGE
{
	using namespace Core;
	using namespace Core::Utility;
	using namespace Array;
	namespace Core
	{
		namespace ResourceManager
		{
			/// \brief Base class for resource file that must be loaded
			class DGE_CoreAPI ObjectResource : public System::DObject
			{
				D_OBJECT(ObjectResource)

				template<class type>
				friend class ResourceLoader;

			public:
				ObjectResource(DGE::Core::System::ContextRef context
							   = DGE::Core::System::Context::getInstance());
				ObjectResource(const DString& Name,
							   const DString& Path = Core::BaseType::DPath::s_sepratorPath,
							   DGE::Core::System::ContextRef context
							   = DGE::Core::System::Context::getInstance());

				D_Destructor(ObjectResource);

				/// \brief Return name of ObjectResource.
				const DString name();
				void		  setName(const DString& newName);

				/// \brief Return path of ObjectResource.
				const DString path();
				void		  setPath(const DString& newPath);

				/// \brief Return full path of ObjectResource.
				const DString fullPath();
				void		  setFullPath(const DString& name, const DString& path);

				/// \brief Return reference count of resource.
				uint64 refCount();

				bool isLoaded();

				bool isLoadable();

				bool hasError();

				uint32 index();

			protected:
				virtual bool reloadResource(
					const DString& Name, const DString& Path = Core::BaseType::DPath::s_sepratorPath);
				virtual void unloadResource();

				/// \brief Increase reference count of resource.
				void increaseRef();

				/// \brief Decrease reference count of resource.
				void decreaseRef();

				void setIsLoaded(bool newIsLoaded);

				void setIsLoadable(bool newIsLoadable);

			protected:
				/// \brief Name of resource file.
				DString m_name = "";
				/// \brief Path to resource file.
				DString m_path = "";
				/// \brief Reference Count
				uint64 m_refCount = 0;
				/// \brief Index of this resource
				uint32 m_index = 0;
				/// \brief It Indicate that, is this resource loaded?
				bool m_isLoaded = false;
				/// \brief It Indicate that, is this resource loadable?
				bool m_isLoadable = false;
				/// \brief Error flag that it's true when error occurred.
				bool m_errorFlag = false;
				/// \brief Indicate whether this resource using template path (similar to relative
				/// path) to prevent from getting error when moving the resources files
				D_PROPERTY(bool, isUsingTemplatePath, IsUsingTemplatePath, false);
			};

			class DGE_CoreAPI BaseResourceLoader
			{
			protected:
				uint32 m_objectCount = 0;

			public:
				virtual ~BaseResourceLoader();
				virtual std::shared_ptr<ObjectResource> create(
					const DString& name, const DString& path = Core::BaseType::DPath::s_sepratorPath)
					= 0;
				virtual std::shared_ptr<ObjectResource> create(
					const DChar* name,
					const DChar* path = (char*) Core::BaseType::DPath::s_sepratorPath)
				{
					return create(DString(name), DString(path));
				}

				virtual bool contains(const DString& name,
									  const DString& path = Core::BaseType::DPath::s_sepratorPath)
					= 0;
				virtual std::shared_ptr<ObjectResource> get(
					const DString& name, const DString& path = Core::BaseType::DPath::s_sepratorPath)
					= 0;
				virtual std::shared_ptr<ObjectResource> getByIndex(uint32 index)			  = 0;

				virtual bool remove(std::shared_ptr<ObjectResource> resource) = 0;
			};

			template<class Type>
			class ResourceLoader : public BaseResourceLoader
			{
			public:
				ResourceLoader(Loaders::LoaderRef loader = nullptr) { m_loader = loader; }

				virtual ~ResourceLoader()
				{
					//				for (auto& p : m_listResources)
					//					SafeDelete(p);
					// SafeDelete(m_List);
				}

				virtual std::shared_ptr<ObjectResource> create(
					const DString& name, const DString& path = Core::BaseType::DPath::s_sepratorPath)
				{
					if (name.isEmpty() || path.isEmpty())
						return nullptr;
					auto element = getTypeOf(DString(name), DString(path));
					if (element)
					{
						element->increaseRef();
						return element;
					}

					if (m_loader)
						element = m_loader->loadByType<Type>(DPath(name, path));
					else
						element.reset(new Type((DChar*) name, (DChar*) path));
					if (element == nullptr)
					{
						LogWarning("Can't load resource in " + path + ":" + name);
						return element;
					}
					element->m_index = m_objectCount;
					m_objectCount++;
					m_listResources.insert({element->fullPath(), element});
					return element;
				}

				virtual std::shared_ptr<Type> createTypeOf(const DChar* name,
														   const DChar* path = (char*)
															   Core::BaseType::DPath::s_sepratorPath)
				{
					return std::dynamic_pointer_cast<Type>(create(name, path));
				}

				virtual std::shared_ptr<Type> createTypeOf(
					const DString& name, const DString& path = Core::BaseType::DPath::s_sepratorPath)
				{
					return std::dynamic_pointer_cast<Type>(create(name, path));
				}

				virtual std::shared_ptr<ObjectResource> getTypeOf(
					const DChar* name,
					const DChar* path = (char*) Core::BaseType::DPath::s_sepratorPath)
				{
					auto ptr = getTypeOf(DString(name), DString(path));
					if (ptr)
						return std::dynamic_pointer_cast<ObjectResource>(ptr);
					return nullptr;
				}

				virtual std::shared_ptr<Type> getTypeOf(
					const DString& name, const DPath& path = Core::BaseType::DPath::s_sepratorPath)
				{
					if (m_listResources.size() == 0)
						return nullptr;
					DString fullPath = DPath::toFullPathFileName(name, path);
					if (m_listResources.contains(fullPath))
					{
						auto resource = m_listResources.at(fullPath);
						if (resource)
							return resource;
					}

					return nullptr;
				}

				virtual std::shared_ptr<ObjectResource> get(
					const DString& name,
					const DString& path = (char*) Core::BaseType::DPath::s_sepratorPath)
				{
					return getTypeOf(name, path);
				}

				virtual std::shared_ptr<ObjectResource> getByIndex(uint32 index)
				{
					for (auto resource : m_listResources)
						if (resource.second->index() == index)
							return std::dynamic_pointer_cast<ObjectResource>(resource.second);
					return nullptr;
				}

				virtual bool remove(std::shared_ptr<ObjectResource> resource)
				{
					if (!resource)
						return false;
					resource->decreaseRef();

					if (resource->refCount() == 0)
					{
						m_listResources.erase(resource->fullPath());
					}
					return true;
				}

				virtual bool contains(const DString& name,
									  const DString& path = (char*)
										  Core::BaseType::DPath::s_sepratorPath)
				{
					if (m_listResources.size() == 0)
						return false;
					for (auto var : m_listResources)
					{
						if (var.second->name() == name && var.second->path() == path)
							return true;
					}
					return false;
				}

				const Map<DString, std::shared_ptr<Type>> getArrayReference()
				{
					return m_listResources;
				}

				uint64 numberCreatedResource() { return m_listResources.size(); }

				Loaders::LoaderRef loader() { return m_loader; }
				void			   setLoader(const Loaders::LoaderRef& newLoader)
				{
					if (m_loader != newLoader)
						m_loader = newLoader;
				}

			protected:
				/// \brief List of resource
				Map<DString, std::shared_ptr<Type>> m_listResources;

				/// \brief Function loader for this resource type.
				/// TODO: must use a more flexible way for loader of resources that support type of
				/// resources and type of accessing to resource
				Loaders::LoaderRef m_loader;
			};

			using ObjectResourceRef	 = std::shared_ptr<DGE::Core::ResourceManager::ObjectResource>;
			using ObjectResourceWRef = std::weak_ptr<DGE::Core::ResourceManager::ObjectResource>;

			using BaseResourceLoaderRef
				= std::shared_ptr<DGE::Core::ResourceManager::BaseResourceLoader>;
			using BaseResourceLoaderWRef
				= std::weak_ptr<DGE::Core::ResourceManager::BaseResourceLoader>;
		} // namespace Resource
	}	  // namespace Core
} // namespace DGE
