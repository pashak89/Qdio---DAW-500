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

#include <Core/CoreModuleDef.h>
#include <Core/ResourceManager/Loaders/Loader.h>
#include <Core/System/DObject.h>
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace ResourceManager
		{
			namespace Loaders
			{
				/// \brief Loaders Manager class is a singleton class to manage all loaders and
				/// register all of them in itself
				class DGE_CoreAPI LoadersManager
					: public Core::System::DObject
					, public Utility::Singleton<LoadersManager>
				{
					D_OBJECT_SINGLETON(LoadersManager)
					LoadersManager(System::ContextRef context);

				public:
					template<typename LoadedType>
					LoaderRef getLoader()
					{
						auto&  typeInfo = typeid(LoadedType);
						DString typeName = typeInfo.name();
						return getLoader(typeName);
					}

					template<typename LoadedType, typename LoaderType>
					bool registerLoader()
					{
						auto&  typeInfo = typeid(LoadedType);
						DString typeName = typeInfo.name();
						return registerLoader(typeName, std::make_shared<LoaderType>());
					}

					template<typename LoadedType>
					bool isRegisteredLoader()
					{
						auto&	typeInfo = typeid(LoadedType);
						DString typeName = typeInfo.name();
						return m_loaders.contains(typeName);
					}

					template<typename LoadedType, typename LoaderType>
					bool unregisterLoader()
					{
						auto&  typeInfo = typeid(LoadedType);
						DString typeName = typeInfo.name();
						return unregisterLoader(typeName);
					}

				protected:
					LoaderRef getLoader(const DString& loaderName);
					bool registerLoader(const DString& loaderName, std::shared_ptr<Loader> loader);
					bool unregisterLoader(const DString& name);

				protected:
					std::map<DString, LoaderRef> m_loaders;
				};

				using LoadersManagerRef
					= std::shared_ptr<Core::ResourceManager::Loaders::LoadersManager>;
				using LoadersManagerWRef
					= std::weak_ptr<Core::ResourceManager::Loaders::LoadersManager>;

			} // namespace Loaders
		}	  // namespace Resource
	}		  // namespace Core
} // namespace DGE
