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

#include <Core/BaseTypes/BaseTypes.h>
#include <Core/Serializers/Serialize.h>
#include <Core/System/DCoreObject.h>
#include <Core/System/DObject/DMetaCoreObject.h>
#include <Core/Utility/Interfaces/IDeserializer.h>
#include <Core/Utility/Interfaces/ISerializer.h>
#include <any>
#include <concepts>
#include <memory>

namespace DGE
{
	namespace Core
	{
		namespace Serializers
		{
			/// \todo TODO: Implement automatic serialization for DOBject with using D_OBJECT Macro
			///
			/// \brief Serialize any DCoreObject type into JSON format
			class DGE_CoreAPI JsonSerializer
				: public DGE::Core::System::DCoreObject
				, public Core::Utility::Interfaces::ISerializer
				, public Core::Utility::Interfaces::IDeserializer
			{
			public:
				JsonSerializer();

				template<typename DCoreObjectType>
					requires(std::derived_from<DCoreObjectType, DCoreObject>)
				DByteArray serialize(const DString&						 name,
									 const std::vector<DCoreObjectType>& objects) const
				{
					System::DCoreObjectPtrArray result;
					for (auto& object : objects)
					{
						result.push_back((DCoreObject*) &object);
					}
					return serializeArrayCoreObject(result);
				}
				template<typename DCoreObjectType>
					requires(std::derived_from<DCoreObjectType, DCoreObject>)
				DByteArray serialize(
					const DString&										 name,
					const std::vector<std::shared_ptr<DCoreObjectType>>& objects) const
				{
					System::DCoreObjectPtrArray result;
					for (auto& object : objects)
					{
						result.push_back((DCoreObject*) object.get());
					}
					return serializeArrayCoreObject(result);
				}

				template<typename DCoreObjectType>
					requires(std::same_as<DCoreObjectType, DCoreObject*>)
				DByteArray serialize(const DString&						 name,
									 const std::vector<DCoreObjectType>& objects) const
				{
					System::DCoreObjectPtrArray result;
					for (auto& object : objects)
					{
						result.push_back((DCoreObject*) object);
					}
					return serializeArrayCoreObject(result);
				}

				// ISerializer interface
				virtual DByteArray serialize(System::DCoreObjectRef object) const override;
				// IDeserializer interface
				virtual System::DCoreObjectRef deserialize(
					const DByteArray& byteArray) const override;

				bool deserialize(System::DCoreObjectRef object, const DByteArray& byteArray) const;

				DByteArray serializeProperties(const Serializers::DArrayProperty& propertyies) const;
				Serializers::DArrayProperty deserializeProperties(const DByteArray& byteArray);

				DByteArray serializeArrayCoreObject(
					const System::DCoreObjectPtrArray& propertyies) const;
				System::DCoreObjectPtrArray deserializeArrayCoreObject(const DByteArray& byteArray);

			protected:
				DByteArray serializeInternal(
					const Serializers::DSerializeProperty& propertyies) const;

				Serializers::DSerializeProperty deserializeInternal(
					const DByteArray& byteArray) const;
			};

		} // namespace Serializers
	}	  // namespace Core
} // namespace DGE
