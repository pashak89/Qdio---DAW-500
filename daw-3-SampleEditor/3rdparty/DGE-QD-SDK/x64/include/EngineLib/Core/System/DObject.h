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
 * Base Object type for all the engine class
 */
#pragma once

#include <Core/System/DCoreObject.h>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			template<typename>
			class DProperty;
			class Context;
			using ContextRef  = std::shared_ptr<Core::System::Context>;
			using ContextWRef = std::weak_ptr<Core::System::Context>;

			/// \brief The tags is used for specifying type of the engine object quickly and exactly
			enum class DObjectTagType
			{
				Core,
				DSubsystem,
				DSubmodule,
				DModule,
				ResourceLoader,
				Resource,
				GFX,
				Scene,
				Qt,
				Script,
				Engine,
				Custom
			};

			/// \brief Base class that must inherit it for using Engine features and easy access to
			/// subsystem of Engine
			class DGE_CoreAPI DObject : public DCoreObject
			{
				D_TypeInfo(DObject)
				{
					auto&								typeInfo = typeid(DObject);
					DGE::Core::System::DTypeInfo resultInfo;
					resultInfo.typeInfoRef.reset(&typeInfo, [](const std::type_info*) {});
					resultInfo.instanceName = DST("DObject") + this->m_idDObject;
					return resultInfo;
				}
				virtual inline DGE::Core::System::DTypeInfo parentTypeInfo()
				{
					return DCoreObject::typeInfo();
				}

				friend class Context;

				template<class, class>
				friend class DObjectFactory;

			public:
				DObject(DGE::Core::System::ContextRef context);
				virtual ~DObject();

				uint64 objectId() const;

				virtual bool isInitialized();

				DGE::Core::System::ContextWRef getContextInstance();

			protected:
				virtual void setIsInitialized(bool isInitialized);

				virtual bool registerInContext(DGE::Core::System::ContextRef&);
				virtual bool unregisterFromContext(DGE::Core::System::ContextRef&);

				/// \brief Add DObjectTagType value into tagtypeList of this object
				bool addTagType(DObjectTagType tagType);
				/// \brief Remove DObjectTagType value from tagtypeList of this object
				bool removeTagType(DObjectTagType tagType);
				/// \brief Indicate whether tagType exist in tagtypeList of this object
				bool hasTagType(DObjectTagType tagType);

			protected:
				/// \brief shared object of Context class
				DGE::Core::System::ContextRef m_context;

				/// \brief ID of this object
				uint64 m_objectID;

				/// \brief indicate whether this object is initialized
				std::atomic_bool m_isInitialized;

				/// \brief List of tag types for this object
				std::set<DObjectTagType> m_tagTypesList;
			};

			using DObjectRef  = std::shared_ptr<Core::System::DObject>;
			using DObjectWRef = std::weak_ptr<Core::System::DObject>;
		} // namespace System
	}	  // namespace Core
} // namespace DGE
