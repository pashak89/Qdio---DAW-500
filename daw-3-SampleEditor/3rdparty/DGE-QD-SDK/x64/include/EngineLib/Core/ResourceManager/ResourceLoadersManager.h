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
#include "Core/Array/Array.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/ResourceManager/Loaders/LoadersManager.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <Core/Utility/Utility.h>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace ResourceManager
		{
			class DGE_CoreAPI ResourceLoadersManager
				: public Core::System::DObject
				, public Singleton<ResourceLoadersManager>
			{
				D_OBJECT_SINGLETON(ResourceLoadersManager)
			public:
				ResourceLoadersManager(
					Core::ResourceManager::Loaders::LoadersManagerRef = nullptr,
					DGE::Core::System::ContextRef context = Core::System::Context::getInstance());
				ResourceLoadersManager(
					DString						  name, Loaders::LoadersManagerRef = nullptr,
					DGE::Core::System::ContextRef context = Core::System::Context::getInstance());
				D_Destructor(ResourceLoadersManager);

				template<typename ResourceType, typename ResourceLoaderType
												= Core::ResourceManager::ResourceLoader<ResourceType>>
				std::shared_ptr<ResourceLoaderType> getResourceLoader()
				{
					auto								typeInfo = ResourceType::staticTypeInfo();
					DString								typeName = typeInfo.typeInfoRef->name();
					auto								it		 = m_resourceLoaders.find(typeName);
					std::shared_ptr<ResourceLoaderType> pResourceManager;
					if (it != m_resourceLoaders.end())
					{
						pResourceManager = std::dynamic_pointer_cast<ResourceLoaderType>(it->second);
					}

					return pResourceManager;
				}

				template<typename ResourceType, typename ResourceLoaderType
												= Core::ResourceManager::ResourceLoader<ResourceType>>
				std::shared_ptr<ResourceLoaderType> registerResourceLoader(
					Loaders::LoaderRef defaultLoader)
				{
					auto typeInfo = ResourceType::staticTypeInfo();
					DString								typeName = typeInfo.typeInfoRef->name();
					auto it		  = m_resourceLoaders.find(typeName);
					std::shared_ptr<ResourceLoaderType> pResourceManager;
					if (it != m_resourceLoaders.end())
					{
						pResourceManager = std::dynamic_pointer_cast<ResourceLoaderType>(it->second);
					}
					else
					{
						pResourceManager = std::make_shared<ResourceLoaderType>();
						m_resourceLoaders.insert(
							std::pair<DString, std::shared_ptr<ResourceManager::BaseResourceLoader>>(
								typeName,
								std::static_pointer_cast<ResourceManager::BaseResourceLoader>(
									pResourceManager)));
					}
					if (pResourceManager && defaultLoader)
						pResourceManager->setLoader(defaultLoader);

					return pResourceManager;
				}

				template<typename ResourceType, typename LoaderType,
						 typename ResourceLoaderType
						 = Core::ResourceManager::ResourceLoader<ResourceType>>
				std::shared_ptr<ResourceLoaderType> registerResourceLoader()
				{
					Loaders::LoaderRef defaultLoader = nullptr;
					if (m_loadersManager)
					{
						if (!m_loadersManager->isRegisteredLoader<ResourceType>())
							DGE_AssertLog((!m_loadersManager
												->registerLoader<ResourceType, LoaderType>()),
										  "Failed to register Resource Loader Type", DoNothing);
						defaultLoader = m_loadersManager->getLoader<ResourceType>();
					}
					return registerResourceLoader<ResourceType>(defaultLoader);
				}
				template<typename ResourceType>
				bool unregisterResourceLoader()
				{
					auto   typeInfo = ResourceType::staticTypeInfo();
					DString typeName = typeInfo.typeInfoRef->name();
					return unregisterResourceLoader(typeName);
				}

				bool unregisterResourceLoader(const DString& name);

				uint64 numberRegisteredResourceLoader();

			protected:
				Core::Array::Map<DString, std::shared_ptr<ResourceManager::BaseResourceLoader>>
																  m_resourceLoaders;
				DString m_name;
				Core::ResourceManager::Loaders::LoadersManagerRef m_loadersManager;
			};

			using ResourceLoadersManagerRef
				= std::shared_ptr<Core::ResourceManager::ResourceLoadersManager>;
			using ResourceLoadersManagerWRef
				= std::weak_ptr<Core::ResourceManager::ResourceLoadersManager>;
		} // namespace Resource
	}	  // namespace Core
} // namespace DGE
