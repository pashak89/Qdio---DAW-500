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
#include "EngineModuleDef.h"
#include <Core/ResourceManager/Loaders/ILoad.h>
#include <Core/ResourceManager/ResourceCache.h>
#include <Core/ResourceManager/Savers/ISave.h>
#include <Core/ResourceManager/Savers/SaversManager.h>
#include <Core/System/Context.h>
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <memory>

namespace DGE
{
	namespace Managers
	{
		/// \brief This class manages all assets in the engine
		class DGE_EngineAPI AssetsManager
			: public DGE::Core::System::DSubsystem
			, public Core::Utility::Singleton<AssetsManager>
			, public Core::ResourceManager::Loaders::ILoad
			, public ResourceManager::Savers::ISave
		{
			D_OBJECT_SINGLETON(AssetsManager)
			AssetsManager(DGE::Core::System::ContextRef context);

		public:
			D_Destructor(AssetsManager);

			// IInitialize interface
			using Core::System::DSubsystem::initialize;
			/// \brief Initialize itself with getting the SystemCache subsystem and register
			/// default resource loaders of the engine
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual bool release() override;

			template<class Resource>
			std::shared_ptr<Resource> loadAsset(DPath						  pathName,
												const std::optional<DOption>& option = {})
			{
				if (m_resourceCache)
					return m_resourceCache->loadResource<Resource>(pathName, option);
				return nullptr;
			}

			template<class Resource>
			bool saveAsset(const DPath&					 path,
						   System::DCoreObjectRef		 object,
						   const std::optional<DOption>& option = {})
			{
				if (m_saversManager)
				{
					auto saver = m_saversManager->getSaver<Resource>();
					if (saver)
					{
						return saver->save(path, object, option);
					}
				}
				return false;
			}

			template<typename Resource>
			static std::shared_ptr<Resource> load(const DPath&					pathName,
												  const std::optional<DOption>& option = {})
			{
				auto astManager = AssetsManager::getInstance(Core::System::Context::getInstance());
				if (astManager)
					return astManager->loadAsset<Resource>(pathName, option);
				return nullptr;
			}

			template<typename Resource>
			static std::shared_ptr<Resource> load(DPath&&						pathName,
												  const std::optional<DOption>& option = {})
			{
				return AssetsManager::load<Resource>(pathName, option);
			}

			template<typename Resource>
			static bool save(const DPath&				   pathName,
							 System::DCoreObjectRef		   object,
							 const std::optional<DOption>& option = {})
			{
				auto astManager = AssetsManager::getInstance(Core::System::Context::getInstance());
				if (astManager)
					return astManager->saveAsset<Resource>(pathName, object, option);
				return false;
			}

			template<typename Resource>
			static bool save(DPath&&				  pathName,
							 System::DCoreObjectRef	  object,
							 std::optional<DOption>&& option)
			{
				return AssetsManager::save<Resource>(pathName, object, option);
			}

			template<typename Resource>
			static bool save(const DPath&			  pathName,
							 System::DCoreObjectRef	  object,
							 std::optional<DOption>&& option)
			{
				return AssetsManager::save<Resource>(pathName, object, option);
			}

			template<typename Resource>
			static bool save(DPath&&					   pathName,
							 System::DCoreObjectRef		   object,
							 const std::optional<DOption>& option = {})
			{
				return AssetsManager::save<Resource>(pathName, object, option);
			}

			static DString defaultAssetsPath();
			static void	   setDefaultAssetsPath(DString defaultAssetsPath);

			template<typename ResourceType, typename ResourceLoaderType, typename ResourceSaverType>
			bool registerResourceAssets(DEnV env)
			{
				auto loaderManager	  = m_resourceCache->loadersManager();
				auto defaultRLManager = m_resourceCache->resourceLoadersManager();
				if (defaultRLManager && loaderManager
					&& defaultRLManager->registerResourceLoader<ResourceType, ResourceLoaderType>())
				{
					auto loader = loaderManager->getLoader<ResourceType>();
					if (loader)
						loader->setEnvironmentVariable(env);
					else
					{
						LogWarning("Can't get loader to set environment variable!");
						return false;
					}
					LogDebug(DST "Register resource loader '" + ResourceLoaderType::staticTypeName()
							 + "' for resource type of " + ResourceType::staticTypeName());
					if (m_saversManager->registerSaver<ResourceType, ResourceSaverType>())
					{
						auto saver = m_saversManager->getSaver<ResourceType>();
						if (saver)
							saver->setEnvironmentVariable(env);
						else
						{
							LogWarning("Can't get saver to set environment variable!");
							return false;
						}
						LogDebug(DST "Register resource saver '"
								 + ResourceSaverType::staticTypeName() + "' for resource type of "
								 + ResourceType::staticTypeName());
					}
					return true;
				}
				return false;
			}
			template<typename ResourceType, typename ResourceLoaderType>
			bool registerResourceAssets(DEnV env)
			{
				auto loaderManager	  = m_resourceCache->loadersManager();
				auto defaultRLManager = m_resourceCache->resourceLoadersManager();
				if (defaultRLManager && loaderManager
					&& defaultRLManager->registerResourceLoader<ResourceType, ResourceLoaderType>())
				{
					auto loader = loaderManager->getLoader<ResourceType>();
					if (loader)
						loader->setEnvironmentVariable(env);
					else
					{
						LogWarning("Can't get loader to set environment variable!");
						return false;
					}
					LogDebug(DST "Register resource loader '" + ResourceLoaderType::staticTypeName()
							 + "' for resource type of " + ResourceType::staticTypeName());
					return true;
				}
				return false;
			}

		protected:
			// DSubsystem interface
			virtual bool registerAll(
				Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual System::DCoreObjectRef load(const DPath&				  path,
												const std::optional<DOption>& option = {}) override;
			virtual bool				   save(const DPath&				  path,
												System::DCoreObjectRef		  object,
												const std::optional<DOption>& option = {}) override;

		public:
			/// \brief Default assets folder name
			static const DString s_defaultAssetsFolderName;

		protected:
			static DString									s_defaultAssetsPath;
			Core::ResourceManager::ResourceCacheRef			m_resourceCache;
			Core::ResourceManager::Savers::SaversManagerRef m_saversManager;
		};

		using AssetsManagerRef	= std::shared_ptr<DGE::Managers::AssetsManager>;
		using AssetsManagerWRef = std::weak_ptr<DGE::Managers::AssetsManager>;
	} // namespace Managers
} // namespace DGE
