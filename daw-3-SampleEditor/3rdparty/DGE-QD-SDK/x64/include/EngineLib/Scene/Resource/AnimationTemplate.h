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

#include <Core/Animation/AnimationEngine.h>
#include <Core/Animation/BaseKeyframe.h>
#include <Core/Math/Vector3D.h>
#include <Core/ResourceManager/ResourceLoader.h>
#include <Core/Serializers/JsonSerializerImp.hpp>
#include <Core/Serializers/Serialize.h>
#include <Core/System/DCoreObject.h>
#include <Core/System/DObjectMacrosDef.h>
#include <memory>
#include <ranges>
#include <vector>

namespace DGE
{
	namespace Scene
	{
		namespace Resource
		{
			template<typename Type>
			using Keys = Serializers::DMapVariants;

			template<typename Type>
			class AnimationTemplate : public DGE::Core::ResourceManager::ObjectResource
			{
				D_OBJECT(AnimationTemplate)
			public:
				AnimationTemplate(const DString name, const DString path = DL("./"))
					: DGE::Core::ResourceManager::ObjectResource(name, path)
				{
				}
				D_Destructor(AnimationTemplate) {}
				virtual void unloadResource() override { m_keys.clear(); }
				virtual bool reloadResource(const DString& Name, const DString& Path) override
				{
					unloadResource();

					setFullPath(Name, Path);
					return true;
				}

				bool reloadData()
				{
					auto keys = m_keys;
					m_keys.clear();
					for (auto [time, key] : keys)
					{
						auto baseKey = std::make_shared<Core::Animation::BaseKeyframe<Type>>();
						if (key.typeName.contains(Serializers::s_DCoreObjectRefTypeName))
						{
							auto keyRef		= std::get<System::DCoreObjectRef>(key.value);
							auto meta		= keyRef->metaCoreObject();
							auto properties = meta->properties();
							for (auto property : properties)
								baseKey->setProperty(property.first,
													 property.second->valueProperty());
							DGE::Core::Serializers::DVariant keyFrame(
								std::static_pointer_cast<System::DCoreObject>(baseKey));
							m_keys.insert(time, keyFrame);
						}
						else
						{
							LogError("DVariant must be contain type of DCoreObjectRef");
							return false;
						}
					}
					return true;
				}

				bool addKey(Core::Animation::BaseKeyframeRef<Type> key)
				{
					return m_keys.insert(key->frameTime(), key);
				}

				bool addKeys(const std::vector<Core::Animation::BaseKeyframeRef<Type>>& keys)
				{
					bool result = true;
					for (auto key : keys)
					{
						DGE::Core::Serializers::DVariant time = DGE::Core::Serializers::DVariant(
							key->frameTime());
						DGE::Core::Serializers::DVariant keyFrame(
							std::static_pointer_cast<System::DCoreObject>(key));
						result &= m_keys.insert(time, keyFrame);
					}

					return result;
				}

				bool addPoint(uint64 time, Type value)
				{
					return m_points.insert(DGE::Core::Serializers::DVariant(time),
										   DGE::Core::Serializers::DVariant(value));
				}

				Core::Animation::KeyFrameList<Type> keyFrames()
				{
					std::map<uint64, Core::Animation::BaseKeyframeRef<Type>> mapKeys;
					for (auto key : m_keys)
					{
						if (key.first.typeName.contains(Serializers::s_AccurateTimeTypeName)
							&& key.second.typeName.contains(Serializers::s_DCoreObjectRefTypeName))
						{
							auto baseKey = Memory::reinterpret_pointer_cast<
								Core::Animation::BaseKeyframe<Core::Math::Vector3D>>(
								std::get<System::DCoreObjectRef>(key.second.value));
							mapKeys.insert({baseKey->frameTime().toMilliSecond(), baseKey});
						}
					}

					auto values = std::views::values(mapKeys);

					return {values.begin(), values.end()};
				}

			protected:
				D_PROPERTY(Keys<Type>, keys, Keys);
				D_PROPERTY(Serializers::DMapVariants, points, Points);
			};
			D_DefRefTypeTemplate(AnimationTemplate)

		} // namespace Resource
	}	  // namespace Scene
} // namespace DGE
