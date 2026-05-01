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

#include <Core/Animation/Transformer.h>
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/BaseTypes/Color.h>
#include <Core/BaseTypes/DMacro.h>
#include <Core/BaseTypes/DPath.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector3D.h>
#include <Core/System/DCoreObject.h>
#include <Core/Utility/AccurateTime.h>
#include <any>
#include <unordered_map>
#include <variant>
#include <vector>

namespace DGE
{
	namespace Core
	{
		namespace Serializers
		{

			using DPropertiesMap = std::map<DString, DString>;
			using DArrayBool	 = std::vector<bool>;
			using DArrayInt8	 = std::vector<int8>;
			using DArrayInt16	 = std::vector<int16>;
			using DArrayInt32	 = std::vector<int32>;
			using DArrayInt64	 = std::vector<int64>;
			using DArrayFloat	 = std::vector<float>;
			using DArrayDouble	 = std::vector<double>;
			using DArrayString	 = std::vector<DString>;
			using DArrayVector2D = std::vector<Math::Vector2D>;
			using DArrayVector3D = std::vector<Math::Vector3D>;
			using DArrayVector4D = std::vector<Math::Vector4D>;
			using DArrayMatrix	 = std::vector<Math::Mat4x4>;
		} // namespace Serializers
	}	  // namespace Core
} // namespace DGE

#define DefineStdHashFor(Type)                 \
	template<>                                 \
	struct std::hash<Type>                     \
	{                                          \
		size_t operator()(const Type& p) const \
		{                                      \
			return std::hash<Type>{}(p);       \
		}                                      \
	};

template<class T1, class T2>
struct std::hash<std::pair<T1, T2>>
{
	size_t operator()(const std::pair<T1, T2>& p) const
	{
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ (h2 << 1); // Combine the hashes
	}
};

template<>
struct std::hash<std::map<DString, DString>>
{
	size_t operator()(const std::map<DString, DString>& p) const
	{
		std::size_t seed = 0;
		for (const auto& pair : p)
		{
			seed ^= std::hash<std::pair<DString, DString>>{}(
				pair); // Combine the hashes of each key-value pair
		}
		return seed;
	}
};

DefineStdHashFor(std::vector<float>);
DefineStdHashFor(std::vector<double>);
DefineStdHashFor(std::vector<uint8>);
DefineStdHashFor(std::vector<uint16>);
DefineStdHashFor(std::vector<uint32>);
DefineStdHashFor(std::vector<uint64>);
DefineStdHashFor(std::vector<int8>);
DefineStdHashFor(std::vector<int16>);
DefineStdHashFor(std::vector<int32>);
DefineStdHashFor(std::vector<int64>);
DefineStdHashFor(std::vector<DString>);
DefineStdHashFor(Color);
DefineStdHashFor(DGE::Core::Math::Vector2D);
DefineStdHashFor(DGE::Core::Math::Vector3D);
DefineStdHashFor(DGE::Core::Math::Vector4D);
DefineStdHashFor(DGE::Core::Math::Mat4x4);
DefineStdHashFor(DGE::Core::Serializers::DArrayVector2D);
DefineStdHashFor(DGE::Core::Serializers::DArrayVector3D);
DefineStdHashFor(DGE::Core::Serializers::DArrayVector4D);
DefineStdHashFor(DGE::Core::Serializers::DArrayMatrix);
// DefineStdHashFor(DGE::Core::Utility::AccurateTime);
DefineStdHashFor(DGE::Core::Animation::TransitionType);
DefineStdHashFor(DGE::Core::System::DCoreObjectRef);
DefineStdHashFor(DGE::Core::System::DCoreObjectRefArray);
DefineStdHashFor(DGE::Core::System::DCoreObjectPtrArray);

template<>
struct std::hash<DPath>
{
	size_t operator()(const DPath& p) const { return std::hash<DString>()(p.toString()); }
};

template<>
struct std::hash<DGE::Core::Utility::AccurateTime>
{
	size_t operator()(const DGE::Core::Utility::AccurateTime& p) const
	{
		return std::hash<std::string>()(p.toString());
	}
};

namespace DGE
{
	namespace Core
	{
		namespace Serializers
		{

			extern DGE_CoreExtern std::vector<DString> splite(
				const char* _stringified_arguments);

#define D_DefineSupportType(...)                               \
	using DBaseTypesSerialization = std::variant<__VA_ARGS__>; \
	const std::vector<DString> s_supportTypeNames = {FOREACH_STRINGIFY(__VA_ARGS__, "UnsupportedType")}

			/// \brief Supported basic types for using in serialization-core
			D_DefineSupportType(bool, int8, int16, int32, int64, uint8, uint16, uint32, uint64,
								float, double, DString, DPath, DPropertiesMap, DArrayBool,
								DArrayInt8, DArrayInt16, DArrayInt32, DArrayInt64, DArrayFloat,
								DArrayDouble, DArrayString, Color, Math::Vector2D, Math::Vector3D,
								Math::Vector4D, Math::Mat4x4, DArrayVector2D, DArrayVector3D,
								DArrayVector4D, DArrayMatrix, Utility::AccurateTime,
								Animation::TransitionType, System::DCoreObjectRef,
								System::DCoreObjectRefArray, System::DCoreObjectPtrArray,
								DGE::Core::Animation::StandardTransformerRef);

			struct DGE_CoreAPI DVariant
			{
				DString typeName;
				DBaseTypesSerialization value;
				DVariant();
				DVariant(const System::DCoreObjectRef& obj);
				DVariant(DBaseTypesSerialization variantValue);
				DVariant(DString type, DBaseTypesSerialization variantValue);
				bool operator()(const DVariant& rhl, const DVariant& lh);
				bool operator<(const DVariant& data) const;
			};

			using testVariant = std::variant<DBaseTypesSerialization>;

			struct StringHash
			{
				size_t operator()(const DString& obj) const
				{
					// Use std::hash<std::string> to hash the string member of MyString
					return std::hash<std::string>()(obj.toStdString());
				}
			};

			struct DVariantHash
			{
				size_t operator()(const DVariant& obj) const
				{
					std::hash<DBaseTypesSerialization> a;
					return a(obj.value);
				}
			};

			using DArrayVariant = std::vector<DVariant>;
			using DMapStringVariant = std::map<DString, DVariant>;
			using DMapVariants		= std::unordered_map<DVariant, DVariant, DVariantHash>;

			// struct DArrayVariant
			// {
			// 	DString			  typeName;
			// 	DBaseArrayVariant value;
			// };
			// struct DMapVariant
			// {
			// 	DString			typeName;
			// 	DBaseMapVariant value;
			// };

			using DContainerVariantType
				= std::variant<DVariant, DArrayVariant, DMapStringVariant, DMapVariants>;

			struct DPropertySerializerType
			{
				DString											   typeName;
				DContainerVariantType							   value;
			};

			using DArrayProperty = std::map<DString, DPropertiesMap>;
			using DSerializeProperty = std::map<DString, DPropertySerializerType>;

			/// \brief Convert std::any into DVariant class
			extern DGE_CoreExtern DVariant anythingToDVariant(std::any value);
			extern DGE_CoreExtern std::any dvariantToAnything(const DVariant& value);
			extern DGE_CoreExtern DContainerVariantType anythingToDContainerVariant(std::any value);
			extern DGE_CoreExtern std::any dcontainerVariantToAnything(
				const DContainerVariantType& value);

			extern DGE_CoreExtern bool operator==(const DVariant& lhs, const DVariant& rhs);

		} // namespace Serializers
	}	  // namespace Core
} // namespace DGE
