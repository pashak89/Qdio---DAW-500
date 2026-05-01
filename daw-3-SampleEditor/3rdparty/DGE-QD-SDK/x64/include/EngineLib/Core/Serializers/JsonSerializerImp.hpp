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
#include <Core/Animation/BaseKeyframe.h>
#include <Core/Animation/TransformerGenerator.h>
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/BaseTypes/DString.h>
#include <Core/IO/LogSystem/Log.h>
#include <Core/IO/LogSystem/LogMacrosDef.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector3D.h>
#include <Core/Serializers/JsonSerializer.h>
#include <Core/Serializers/Serialize.h>
#include <Core/System/DObject/DMetaCoreObject.h>
#include <Core/Utility/Utility.h>

#include <Core/MemoryManager/UndefMemoryMacros.h>
#include <memory>
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/json.hpp>

#include <Core/MemoryManager/RedefMemoryMacros.h>
#include <string>
#include <variant>

namespace DGE
{
	namespace Core
	{
		namespace Serializers
		{
#define D_TypeName(Type) s_##Type##TypeName
#define D_DefineTypeNameString(Type) const DString D_TypeName(Type) = D_MakeString(Type);

			const DString s_typeNameString				= "typeName";
			const DString s_valueString					= "value";
			const DString s_propertyContainerTypeString = "propertyContainerType";
			const uint32  s_numIndentInDump				= 4;
			D_DefineTypeNameString(uint8);
			D_DefineTypeNameString(uint16);
			D_DefineTypeNameString(uint32);
			D_DefineTypeNameString(uint64);
			D_DefineTypeNameString(bool);
			D_DefineTypeNameString(int8);
			D_DefineTypeNameString(int16);
			D_DefineTypeNameString(int32);
			D_DefineTypeNameString(int64);
			D_DefineTypeNameString(float);
			D_DefineTypeNameString(double);
			D_DefineTypeNameString(DPropertiesMap);
			D_DefineTypeNameString(DArrayBool);
			D_DefineTypeNameString(DArrayInt8);
			D_DefineTypeNameString(DArrayInt16);
			D_DefineTypeNameString(DArrayInt32);
			D_DefineTypeNameString(DArrayInt64);
			D_DefineTypeNameString(DArrayFloat);
			D_DefineTypeNameString(DArrayDouble);
			D_DefineTypeNameString(DArrayString);

			D_DefineTypeNameString(DString);
			D_DefineTypeNameString(DPath);
			D_DefineTypeNameString(Color);
			D_DefineTypeNameString(Vector2D);
			D_DefineTypeNameString(Vector3D);
			D_DefineTypeNameString(Vector4D);

			D_DefineTypeNameString(TransitionType);
			D_DefineTypeNameString(AccurateTime);

			D_DefineTypeNameString(DCoreObjectPtr);
			D_DefineTypeNameString(DCoreObjectRef);

			D_DefineTypeNameString(DCoreObjectRefArray);
			D_DefineTypeNameString(DCoreObjectPtrArray);
			D_DefineTypeNameString(StandardTransformerRef);
			D_DefineTypeNameString(BaseKeyframe);

			/// \brief String of container type name
			D_DefineTypeNameString(DVariant);
			D_DefineTypeNameString(DArrayVariant);
			D_DefineTypeNameString(DMapVariants);
			D_DefineTypeNameString(DMapStringVariant);
			D_DefineTypeNameString(DUndefinedContainerType);

			extern DGE_CoreExtern DString dcontainerIndexToDString(uint64 index);

			template<typename SerializeType, typename VariantType>
				requires(std::same_as<VariantType, DGE::Core::Serializers::DVariant>)
			void variantToType(nlohmann::json& jsonObject,
							   VariantType	   variantObj,
							   DString		   serializeTypeName = D_MakeString(SerializeType))
			{
				try
				{
					jsonObject[s_typeNameString] = variantObj.typeName;
					nlohmann::json jsonObjectValue;
					nlohmann::adl_serializer<SerializeType>::to_json(jsonObjectValue,
																	 std::get<SerializeType>(
																		 variantObj.value));
					jsonObject[s_valueString] = jsonObjectValue;
				}
				catch (const std::bad_variant_access& ex)
				{
					LogError(DST "Error in try convert into " + serializeTypeName + ex.what());
				}
			}

			template<typename SerializeType, typename VariantType>
				requires(std::same_as<VariantType, DGE::Core::Serializers::DPropertySerializerType>)
			void variantToType(nlohmann::json& jsonObject,
							   VariantType	   variantObj,
							   DString		   serializeTypeName = D_MakeString(SerializeType))
			{
				try
				{
					std::map<DString, SerializeType> mapData;
					nlohmann::adl_serializer<SerializeType>::to_json(jsonObject,
																	 std::get<SerializeType>(
																		 variantObj.value));
					jsonObject[s_propertyContainerTypeString] = serializeTypeName;
				}
				catch (const std::bad_variant_access& ex)
				{
					LogError(DST "Error in try convert into " + serializeTypeName + ex.what());
				}
			}

			template<typename SerializeType, typename VariantType>
				requires(std::same_as<VariantType, DGE::Core::Serializers::DMapStringVariant>
						 || std::same_as<VariantType, DGE::Core::Serializers::DMapVariants>)
			void variantToType(nlohmann::json& jsonObject,
							   VariantType	   variantObj,
							   DString		   serializeTypeName = D_MakeString(SerializeType))
			{
				try
				{
					jsonObject[s_propertyContainerTypeString] = serializeTypeName;
					jsonObject[s_typeNameString]			  = serializeTypeName;
					nlohmann::json jsonObjectValue;
					nlohmann::adl_serializer<SerializeType>::to_json(jsonObjectValue, variantObj);
					jsonObject[s_valueString] = jsonObjectValue;
				}
				catch (const std::bad_variant_access& ex)
				{
					LogError(DST "Error in try convert into " + serializeTypeName + ex.what());
				}
			}

			/// \brief Deserialize json to SerializeType
			template<typename SerializeType, typename VariantType>
			void DeserializeVariantTo(const nlohmann::json& jsonObject,
									  VariantType&			variantObj,
									  DString				deserializeTypeName)
			{
				SerializeType result;
				nlohmann::adl_serializer<SerializeType>::from_json(jsonObject[s_valueString],
																   result);
				variantObj.typeName = deserializeTypeName;
				variantObj.value	= result;
			}

			/// \brief Deserialize json to SerializeType
			template<typename SerializeType, typename VariantType>
			void DeserializeVariantTo(const nlohmann::json& jsonObject, VariantType& variantObj,
									  DString deserializeTypeName, const SerializeType& result)
			{
				nlohmann::adl_serializer<SerializeType>::from_json(jsonObject[s_valueString],
																   result);
				variantObj.typeName = deserializeTypeName;
				variantObj.value	= result;
			}

#define SerializeVariantToType(Type, variantObj, jsonObject) \
	variantToType<Type>(jsonObject, variantObj, D_MakeString(Type))

#define DeserializeVariantToType(typeNameValue, Type, variantObj, jsonObject, ...)        \
	if (typeNameValue.contains(D_TypeName(Type)))                                         \
	{                                                                                     \
		DeserializeVariantTo<Type>(jsonObject, variantObj, D_TypeName(Type) __VA_ARGS__); \
		return;                                                                           \
	}
		} // namespace Serializers
	}	  // namespace Core
} // namespace DGE

namespace nlohmann
{
	using namespace DGE::Core::Serializers;
	using namespace DGE::Core::Math;

	template<>
	struct adl_serializer<DGE::Core::BaseType::DString>
	{
		static void to_json(json& j, const DGE::Core::BaseType::DString& opt)
		{
			j = opt.toStdString();
		}

		static void from_json(const json& j, DGE::Core::BaseType::DString& opt)
		{
			if (j.is_null())
			{
				opt = "";
			}
			else
			{
				opt = j.template get<std::string>(); // same as above, but with
													 // adl_serializer<T>::from_json
			}
		}
	};
	template<>
	struct adl_serializer<DGE::Core::BaseType::DPath>
	{
		static void to_json(json& j, const DPath& opt) { j = opt.toString(); }

		static void from_json(const json& j, DPath& opt)
		{
			if (j.is_null())
			{
				opt = DST "";
			}
			else
			{
				opt = j.template get<DString>(); // same as above, but with
												 // adl_serializer<T>::from_json
			}
		}
	};
	template<>
	struct adl_serializer<DGE::Core::BaseType::Color>
	{
		static void to_json(json& j, const Color& opt) { j = opt.toString(); }

		static void from_json(const json& j, Color& opt)
		{
			if (j.is_null())
			{
				opt = DST "";
			}
			else
			{
				opt = j.template get<DString>(); // same as above, but with
												 // adl_serializer<T>::from_json
			}
		}
	};
	template<>
	struct adl_serializer<DGE::Core::Math::Vector2D>
	{
		static void to_json(json& j, const Vector2D& opt)
		{
			j[0] = opt.x();
			j[1] = opt.y();
		}

		static void from_json(const json& j, Vector2D& opt)
		{
			if (j.is_null())
			{
				return;
			}
			else
			{
				if (j.contains("x") && j.contains("y"))
				{
					opt.setX(j["x"].template get<float>());
					opt.setY(j["y"].template get<float>());
				}
				else if (j.is_array())
				{
					opt.setX(j[0].template get<float>());
					opt.setY(j[1].template get<float>());
				}
			}
		}
	};
	template<>
	struct adl_serializer<DGE::Core::Math::Vector3D>
	{
		static void to_json(json& j, const Vector3D& opt)
		{
			j[0] = opt.x();
			j[1] = opt.y();
			j[2] = opt.z();
		}

		static void from_json(const json& j, Vector3D& opt)
		{
			if (j.is_null())
			{
				return;
			}
			else
			{
				if (j.contains("x") && j.contains("y") && j.contains("z"))
				{
					opt.setX(j["x"].template get<float>());
					opt.setY(j["y"].template get<float>());
					opt.setZ(j["z"].template get<float>());
				}
				if (j.is_array())
				{
					opt.setX(j[0].template get<float>());
					opt.setY(j[1].template get<float>());
					opt.setZ(j[2].template get<float>());
				}
			}
		}
	};
	template<>
	struct adl_serializer<DGE::Core::Math::Vector4D>
	{
		static void to_json(json& j, const Vector4D& opt)
		{
			j["x"] = opt.x();
			j["y"] = opt.y();
			j["z"] = opt.z();
			j["w"] = opt.w();
		}

		static void from_json(const json& j, Vector4D& opt)
		{
			if (j.is_null())
			{
				return;
			}
			else
			{
				opt.setX(j["x"].template get<float>());
				opt.setY(j["y"].template get<float>());
				opt.setZ(j["z"].template get<float>());
				opt.setW(j["w"].template get<float>());
			}
		}
	};
	template<>
	struct adl_serializer<DGE::Core::Utility::AccurateTime>
	{
		static void to_json(json& j, const DGE::Core::Utility::AccurateTime& opt)
		{
			j = opt.toMilliSecond();
		}

		static void from_json(const json& j, DGE::Core::Utility::AccurateTime& opt)
		{
			if (j.is_null())
			{
				return;
			}
			else
			{
				opt = DGE::Core::Utility::AcTime::fromMilliSeconds(j.template get<uint64>());
			}
		}
	};
	template<>
	struct adl_serializer<DGE::Core::Animation::StandardTransformerRef>
	{
		static void to_json(json& j, const DGE::Core::Animation::StandardTransformerRef& opt)
		{
			if (opt)
			{
				j[s_typeNameString] = opt->timeTransiation();
				j[s_valueString]	= opt->toString();
			}
			else
			{
				j[s_typeNameString] = DGE::Core::Animation::TransitionType::Linear;
				j[s_valueString]	= vector<vector<DString>>();
			}
		}

		static void from_json(const json& j, DGE::Core::Animation::StandardTransformerRef& opt)
		{
			if (j.is_null())
			{
				return;
			}
			else if (j.contains(s_typeNameString) && j.contains(s_valueString))
			{
				auto type = j[s_typeNameString].template get<DGE::Core::Animation::TransitionType>();
				auto  controlPointsVec = j[s_valueString].template get<vector<vector<DString>>>();
				float controlPoints[6] = {0};
				if (type != DGE::Core::Animation::TransitionType::Linear)
				{
					if (controlPointsVec.size() != 3)
					{
						LogError(
							"'StandardTransformer' need exactly 3 pair of control values but it's "
							"not.");
						return;
					}
					uint8 counter = 0;
					for (auto controlPoint : controlPointsVec)
					{
						if (controlPoint.size() != 2)
						{
							LogError("every pair of control point for 'StandardTransformer' need "
									 "exactly 2 values of string but it's not.");
							return;
						}
						controlPoints[counter++] = controlPoint[0].toDouble();
						controlPoints[counter++] = controlPoint[1].toDouble();
					}
				}

				opt = DGE::Core::Animation::TransformerGenerator::generateVector3D(type,
																				   controlPoints[0],
																				   controlPoints[1],
																				   controlPoints[2],
																				   controlPoints[3],
																				   controlPoints[4],
																				   controlPoints[5]);
			}
		}
	};

	template<>
	struct adl_serializer<DGE::Core::Serializers::DVariant>
	{
		static void to_json(json& j, const DGE::Core::Serializers::DVariant& variantObj)
		{
			if (variantObj.value.valueless_by_exception())
				j = "";
			else
			{
				switch (variantObj.value.index())
				{
					case 0: // bool
						SerializeVariantToType(bool, variantObj, j);
						break;
					case 1: // int8
						SerializeVariantToType(int8, variantObj, j);
						break;
					case 2: // int16
						SerializeVariantToType(int16, variantObj, j);
						break;
					case 3: // int32
						SerializeVariantToType(int32, variantObj, j);
						break;
					case 4: // int64
						SerializeVariantToType(int64, variantObj, j);
						break;
					case 5: // uint8
						SerializeVariantToType(uint8, variantObj, j);
						break;
					case 6: // uint16
						SerializeVariantToType(uint16, variantObj, j);
						break;
					case 7: // uint32
						SerializeVariantToType(uint32, variantObj, j);
						break;
					case 8: // uint64
						SerializeVariantToType(uint64, variantObj, j);
						break;
					case 9: // float
						SerializeVariantToType(float, variantObj, j);
						break;
					case 10: // double
						SerializeVariantToType(double, variantObj, j);
						break;
					case 11: // DString
						SerializeVariantToType(DString, variantObj, j);
						break;
					case 12: // DPath
						SerializeVariantToType(DPath, variantObj, j);
						break;
					case 13: // DPropertiesMap
						SerializeVariantToType(DGE::Core::Serializers::DPropertiesMap,
											   variantObj,
											   j);
						break;
					case 14: // DArrayBool
						SerializeVariantToType(DGE::Core::Serializers::DArrayBool, variantObj, j);
						break;
					case 15: // DArrayInt8
						SerializeVariantToType(DGE::Core::Serializers::DArrayInt8, variantObj, j);
						break;
					case 16: // DArrayInt16
						SerializeVariantToType(DGE::Core::Serializers::DArrayInt16, variantObj, j);
						break;
					case 17: // DArrayInt32
						SerializeVariantToType(DGE::Core::Serializers::DArrayInt32, variantObj, j);
						break;
					case 18: // DArrayInt64
						SerializeVariantToType(DGE::Core::Serializers::DArrayInt64, variantObj, j);
						break;
					case 19: // DArrayFloat
						SerializeVariantToType(DGE::Core::Serializers::DArrayFloat, variantObj, j);
						break;
					case 20: // DArrayDouble
						SerializeVariantToType(DGE::Core::Serializers::DArrayDouble, variantObj, j);
						break;
					case 21: // DArrayString
						SerializeVariantToType(DGE::Core::Serializers::DArrayString, variantObj, j);
						break;
					case 22: // Color
						SerializeVariantToType(Color, variantObj, j);
						break;
					case 23: // Vector2D
						SerializeVariantToType(Vector2D, variantObj, j);
						break;
					case 24: // Vector3D
						SerializeVariantToType(Vector3D, variantObj, j);
						break;
					case 25: // Vector3D
						SerializeVariantToType(Vector4D, variantObj, j);
						break;
					// case 26: // Math::Mat4x4
					// 	SerializeVariantToType(DGE::Core::Math::Mat4x4, variantObj, j);
					// 	break;
					case 27: // DArrayVector2D
						SerializeVariantToType(DArrayVector2D, variantObj, j);
						break;
					case 28: // DArrayVector3D
						SerializeVariantToType(DArrayVector3D, variantObj, j);
						break;
					case 29: // DArrayVector4D
						SerializeVariantToType(DArrayVector4D, variantObj, j);
						break;
					// case 30: // DArrayMatrix
					// 	SerializeVariantToType(DArrayMatrix, variantObj, j);
					// 	break;
					case 31: // Utility::AccurateTime
						SerializeVariantToType(DGE::Core::Utility::AccurateTime, variantObj, j);
						break;
					case 32: // Animation::TransitionType
						SerializeVariantToType(DGE::Core::Animation::TransitionType, variantObj, j);
						break;
					case 33: // Animation::TransitionType
						SerializeVariantToType(DGE::Core::System::DCoreObjectRef, variantObj, j);
						break;
					case 34: // Animation::TransitionType
						SerializeVariantToType(DGE::Core::System::DCoreObjectRefArray,
											   variantObj,
											   j);
						break;
					case 35: // Animation::TransitionType
						SerializeVariantToType(DGE::Core::System::DCoreObjectPtrArray,
											   variantObj,
											   j);
						break;
					case 36: // Animation::TransitionType
						SerializeVariantToType(DGE::Core::Animation::StandardTransformerRef,
											   variantObj,
											   j);
						break;
					default:
						j = "";
						LogError("Deserialize type is undefined: " + variantObj.typeName);
						break;
				}
			}
		}
		static void from_json(const json& j, DGE::Core::Serializers::DVariant& variantObj)
		{
			using namespace DGE::Core::Utility;
			using namespace DGE::Core::Animation;
			using namespace DGE::Core::System;
			if (j.empty())
				return;
			if (j.contains(s_typeNameString) && j.contains(s_valueString))
			{
				DString containerType = j[s_typeNameString].template get<std::string>();
				DeserializeVariantToType(containerType, bool, variantObj, j);
				DeserializeVariantToType(containerType, uint8, variantObj, j);
				DeserializeVariantToType(containerType, uint16, variantObj, j);
				DeserializeVariantToType(containerType, uint32, variantObj, j);
				DeserializeVariantToType(containerType, uint64, variantObj, j);
				DeserializeVariantToType(containerType, int8, variantObj, j);
				DeserializeVariantToType(containerType, int16, variantObj, j);
				DeserializeVariantToType(containerType, int32, variantObj, j);
				DeserializeVariantToType(containerType, int64, variantObj, j);
				DeserializeVariantToType(containerType, float, variantObj, j);
				DeserializeVariantToType(containerType, double, variantObj, j);
				DeserializeVariantToType(containerType, DString, variantObj, j);
				DeserializeVariantToType(containerType, DPath, variantObj, j);
				DeserializeVariantToType(containerType, Color, variantObj, j);
				DeserializeVariantToType(containerType, Vector2D, variantObj, j);
				DeserializeVariantToType(containerType, Vector3D, variantObj, j);
				DeserializeVariantToType(containerType, Vector4D, variantObj, j);
				DeserializeVariantToType(containerType, AccurateTime, variantObj, j);
				DeserializeVariantToType(containerType, TransitionType, variantObj, j);
				DeserializeVariantToType(containerType, DCoreObjectPtr, variantObj, j);
				DeserializeVariantToType(containerType, DCoreObjectRef, variantObj, j);

				DeserializeVariantToType(containerType, DCoreObjectRefArray, variantObj, j);

				DeserializeVariantToType(containerType, DCoreObjectPtrArray, variantObj, j);
				DeserializeVariantToType(containerType, StandardTransformerRef, variantObj, j);

				// DeserializeVariantToType(containerType, BaseKeyframe<Vector3D>, variantObj, j);
				LogError("Deserialize type is undefined: " + containerType);
			}
		}
	};

	template<>
	struct adl_serializer<DGE::Core::Serializers::DPropertySerializerType>
	{
		static void to_json(json&												   j,
							const DGE::Core::Serializers::DPropertySerializerType& variantObj)
		{
			if (variantObj.value.valueless_by_exception())
				j = "";
			else
			{
				switch (variantObj.value.index())
				{
					case 0: // DVariant
						SerializeVariantToType(DGE::Core::Serializers::DVariant, variantObj, j);
						break;
					// case 1: // DArrayVariant
					// 	SerializeVariantToType(DGE::Core::Serializers::DArrayVariant, variantObj,
					// j); 	break;
					case 2: // DMapStringVariant
					{
						SerializeVariantToType(DGE::Core::Serializers::DMapStringVariant,
											   std::get<DGE::Core::Serializers::DMapStringVariant>(
												   variantObj.value),
											   j);
					}
					break;
					case 3: // DMapVariants
					{
						SerializeVariantToType(DGE::Core::Serializers::DMapVariants,
											   std::get<DGE::Core::Serializers::DMapVariants>(
												   variantObj.value),
											   j);
					}
					break;
					default:
						j = "";
						break;
				}
			}
		}

		static void from_json(const json&									   j,
							  DGE::Core::Serializers::DPropertySerializerType& variantObj)
		{
			if (j.empty())
				return;
			if (j.contains(s_propertyContainerTypeString))
			{
				DString containerType = j[s_propertyContainerTypeString].template get<std::string>();
				if (containerType.contains(s_DVariantTypeName))
				{
					DGE::Core::Serializers::DVariant result;
					adl_serializer<DGE::Core::Serializers::DVariant>::from_json(j, result);
					variantObj.typeName = s_DVariantTypeName;
					variantObj.value	= result;
				}
				else if (containerType.contains(s_DArrayVariantTypeName))
				{
					DGE::Core::Serializers::DArrayVariant result;
					adl_serializer<DGE::Core::Serializers::DArrayVariant>::from_json(j, result);
					variantObj.typeName = s_DArrayVariantTypeName;
					variantObj.value	= result;
				}
				else if (containerType.contains(s_DMapStringVariantTypeName))
				{
					DGE::Core::Serializers::DMapStringVariant result;
					adl_serializer<DGE::Core::Serializers::DMapStringVariant>::from_json(j, result);
					variantObj.typeName = s_DMapStringVariantTypeName;
					variantObj.value	= result;
				}
				else if (containerType.contains(s_DMapVariantsTypeName))
				{
					// DGE::Core::Serializers::DVariant variantObj;
					DeserializeVariantToType(containerType, DMapVariants, variantObj, j, );
					// adl_serializer<DGE::Core::Serializers::DMapVariants>::from_json(j, result);
					// variantObj.typeName = s_DMapVariantsTypeName;
					// variantObj.value	= result;
				}
				else
				{
					LogConsole(DST "Undefined 'Container Type':" + containerType);
				}
			}
		}
	};

	template<>
	struct adl_serializer<DGE::Core::System::DCoreObjectPtr>
	{
		static void to_json(json& j, const DGE::Core::System::DCoreObjectPtr& opt)
		{
			auto									   metaObject = opt->metaCoreObject();
			auto									   properties = metaObject->properties();
			DGE::Core::Serializers::DSerializeProperty seriazlieProperties;
			for (auto property : properties)
			{
				DContainerVariantType valueStr;
				auto				  value	   = property.second->valueProperty();
				auto				  name	   = property.first;
				auto				  typeName = s_DUndefinedContainerTypeTypeName;
				if (value.has_value())
				{
					valueStr	   = anythingToDContainerVariant(value);
					auto typeIndex = valueStr.index();
					typeName	   = dcontainerIndexToDString(typeIndex);
				}
				seriazlieProperties.insert({name, {typeName, valueStr}});
			}
			j = seriazlieProperties;
		}

		static void from_json(const json& j, DGE::Core::System::DCoreObjectPtr& opt)
		{
			if (j.is_null())
			{
				return;
			}
			else
			{
				if (opt == nullptr)
					opt = new DGE::Core::System::DCoreObject();
				DSerializeProperty result;
				j.get_to(result);
				auto metaObject = opt->metaCoreObject();
				for (auto property : result)
					metaObject->setProperty(property.first,
											dcontainerVariantToAnything(property.second.value),
											true);
				// opt.setX(j["x"].template get<float>());
				// opt.setY(j["y"].template get<float>());
				// opt.setZ(j["z"].template get<float>());
				// opt.setW(j["w"].template get<float>());
			}
		}
	};

	template<>
	struct adl_serializer<DGE::Core::System::DCoreObjectRef>
	{
		static void to_json(json& j, const DGE::Core::System::DCoreObjectRef& opt)
		{
			adl_serializer<DGE::Core::System::DCoreObjectPtr>::to_json(j, opt.get());
		}

		static void from_json(const json& j, DGE::Core::System::DCoreObjectRef& opt)
		{
			if (!opt)
				opt = std::make_shared<DGE::Core::System::DCoreObject>();
			auto optPtr = opt.get();
			adl_serializer<DGE::Core::System::DCoreObjectPtr>::from_json(j, optPtr);
		}
	};

} // namespace nlohmann
