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

#include <Core/IO/LogSystem/LogMacrosDef.h>
#include <Core/ResourceManager/Loaders/ILoad.h>
#include <Core/ResourceManager/Loaders/LoadersManager.h>
#include <Core/ResourceManager/ResourceLoadersManager.h>
#include <Core/System/Context.h>
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace ResourceManager
		{
			/// \brief Resources cache subsystems that is manage dedicated resource loaders
			class DGE_CoreAPI ResourceCache
				: public DGE::Core::System::DSubsystem
				, public Core::Utility::Singleton<ResourceCache>
				, public Loaders::ILoad
			{
				D_OBJECT_SINGLETON(ResourceCache)
				ResourceCache(DGE::Core::System::ContextRef context);

			public:
				D_Destructor(ResourcesManager);

				// IInitialize interface
				using Core::System::DSubsystem::initialize;

				/// \brief Initialize itself with getting current Renderer subsystem
				virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

				virtual bool release() override;

				template<typename ResourceType>
				System::DCoreObjectRef load(const DPath& path, const std::optional<DOption>& option)
				{
					auto resourceLoader = m_resourceLoadersManager->getResourceLoader<ResourceType>();
					if (resourceLoader)
						return loadResource(path, resourceLoader, option);
					return nullptr;
				}

				virtual System::DCoreObjectRef load(
					const DPath& path, const std::optional<DOption>& option = {}) override;

				/// \brief load a resource and cache it for future usage
				template<typename ResourceType>
				std::shared_ptr<ResourceType> loadResource(DPath pathName,
														   const std::optional<DOption>& option = {})
				{
					return std::dynamic_pointer_cast<ResourceType>(
						load<ResourceType>(pathName, option));
				}

				Core::ResourceManager::ObjectResourceRef loadResource(
					DPath pathName,
					std::shared_ptr<DGE::Core::ResourceManager::BaseResourceLoader>
												  resourceLoader,
					const std::optional<DOption>& option = {});

				Core::ResourceManager::ResourceLoadersManagerRef resourceLoadersManager() const;

				Core::ResourceManager::Loaders::LoadersManagerRef loadersManager() const;

			protected:
				// DSubsystem interface
				virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;

			protected:
				Core::ResourceManager::ResourceLoadersManagerRef  m_resourceLoadersManager;
				Core::ResourceManager::Loaders::LoadersManagerRef m_loadersManager;
				std::map<DString, Core::ResourceManager::ObjectResourceRef> m_resourceCacheObject;
			};

			using ResourceCacheRef	= std::shared_ptr<DGE::Core::ResourceManager::ResourceCache>;
			using ResourceCacheWRef = std::weak_ptr<DGE::Core::ResourceManager::ResourceCache>;
		} // namespace Assets
	}	  // namespace Core
} // namespace DGE
