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

#include "DObjectMacrosDef.h"

#include <any>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			class DMetaCoreObject;
			/// \brief A Basic class for the engine object that is providing some necessary methods
			class DGE_CoreAPI DCoreObject
			{
				D_TypeInfo(DCoreObject)
				{
					auto&						 typeInfo = typeid(DCoreObject);
					DGE::Core::System::DTypeInfo resultInfo;
					resultInfo.typeInfoRef.reset(&typeInfo, [](const std::type_info*) {});
					resultInfo.instanceName = DST("DObject") + this->m_idDCoreObject;
					return resultInfo;
				}
				virtual inline DGE::Core::System::DTypeInfo parentTypeInfo()
				{
					DGE::Core::System::DTypeInfo resultInfo;
					resultInfo.instanceName = DST("None");
					return resultInfo;
				}
				virtual DString typeName() const { return "DCoreObject"; }

			protected:
				DCoreObject(std::shared_ptr<DMetaCoreObject> metaObject);

				// template<typename Type>
				// DGE::Core::System::DProperty<Type> makeProperty()
				// {
				// }

			public:
				DCoreObject();
				virtual ~DCoreObject();

				std::shared_ptr<DMetaCoreObject> metaCoreObject() const;
				static std::shared_ptr<DMetaCoreObject> staticMetaCoreObject();

				std::any property(DString name) const;

				bool setProperty(DString name, std::any value);

			protected:
				D_OBJECT_DESTROYING_FUNC_DEBUG()

				/// \brief Generate a text in output log with given message with Information label.
				///
				/// \param msg It's main message of log
				void logInfo(const char* msg);

				/// \brief Generate a text in output log with given message with Warning label.
				///
				/// \param msg It's main message of log
				void logWarning(const char* msg);

				/// \brief Generate a text in output log with given message with Error label.
				///
				/// \param msg It's main message of log
				void logError(const char* msg);

				/// \brief Generate a text in output log with given message with FatalError label.
				///
				/// \param msg It's main message of log
				void logFatalError(const char* msg);

				/// \brief Generate a text in output log with given message with Debug label.
				///
				/// \param msg It's main message of log
				void logDebug(const char* msg);

				/// \brief Generate a text in output log with given message with Success label.
				///
				/// \param msg It's main message of log
				void logSuccess(const char* msg);

				/// \brief Generate a text in output log with given message with Information label.
				///
				/// \param msg It's main message of log
				void logInfo(const DString& msg);

				/// \brief Generate a text in output log with given message with Warning label.
				///
				/// \param msg It's main message of log
				void logWarning(const DString& msg);

				/// \brief Generate a text in output log with given message with Error label.
				///
				/// \param msg It's main message of log
				void logError(const DString& msg);

				/// \brief Generate a text in output log with given message with FatalError label.
				///
				/// \param msg It's main message of log
				void logFatalError(const DString& msg);

				/// \brief Generate a text in output log with given message with Debug label.
				///
				/// \param msg It's main message of log
				void logDebug(const DString& msg);

				/// \brief Generate a text in output log with given message with Success label.
				///
				/// \param msg It's main message of log
				void logSuccess(const DString& msg);

			protected:
				std::shared_ptr<DMetaCoreObject> m_metaObject;
				static std::shared_ptr<DMetaCoreObject> s_staticMetaObject;
			};

			using DCoreObjectRef  = std::shared_ptr<System::DCoreObject>;
			using DCoreObjectWRef = std::weak_ptr<System::DCoreObject>;

			using DCoreObjectPtr	  = DCoreObject*;
			using DCoreObjectRefArray = std::vector<DCoreObjectRef>;
			using DCoreObjectPtrArray = std::vector<DCoreObject*>;

		} // namespace System
	}	  // namespace Core
} // namespace DGE
