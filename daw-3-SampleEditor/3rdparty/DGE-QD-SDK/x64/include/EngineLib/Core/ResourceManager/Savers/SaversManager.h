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

#include "Core/System/DObjectMacrosDef.h"
#include <Core/ResourceManager/Savers/Saver.h>
#include <Core/System/DObject.h>
#include <Core/System/DSubsystem.h>
#include <Core/Utility/Singleton/Singleton.h>

namespace DGE
{
	namespace Core
	{
		namespace ResourceManager
		{
			namespace Savers
			{

				class DGE_CoreAPI SaversManager
					: public Core::System::DObject
					, public Utility::Singleton<SaversManager>
				{
					D_OBJECT_SINGLETON(SaversManager)
					SaversManager(System::ContextRef context);

				public:
					template<typename ResourceType>
					SaverRef getSaver()
					{
						auto&	typeInfo = typeid(ResourceType);
						DString typeName = typeInfo.name();
						return getSaver(typeName);
					}

					template<typename ResourceType, typename SaverType>
					bool registerSaver()
					{
						auto&	typeInfo = typeid(ResourceType);
						DString typeName = typeInfo.name();
						return registerSaver(typeName, std::make_shared<SaverType>());
					}

					template<typename ResourceType>
					bool isRegisteredSaver()
					{
						auto&	typeInfo = typeid(ResourceType);
						DString typeName = typeInfo.name();
						return m_savers.contains(typeName);
					}

					template<typename ResourceType, typename SaverType>
					bool unregisterSaver()
					{
						auto&	typeInfo = typeid(SaverType);
						DString typeName = typeInfo.name();
						return unregisterSaver(typeName);
					}

				protected:
					SaverRef getSaver(const DString& saverName);
					bool	 registerSaver(const DString& saverName, SaverRef saver);
					bool	 unregisterSaver(const DString& name);

				protected:
					std::map<DString, SaverRef> m_savers;
				};

				D_DefRefType(SaversManager)

			} // namespace Savers
		}	  // namespace ResourceManager
	}		  // namespace Core
} // namespace DGE
