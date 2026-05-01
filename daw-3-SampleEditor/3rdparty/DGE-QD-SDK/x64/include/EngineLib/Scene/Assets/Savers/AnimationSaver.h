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
#include <Core/ResourceManager/Savers/Saver.h>
#include <Core/Serializers/JsonSerializer.h>
#include <Core/Utility/Interfaces/ISerializer.h>
#include <Scene/Components/AnimationComponent.h>

namespace DGE
{
	namespace Scene
	{
		namespace Assets
		{
			namespace Savers
			{

				class AnimationSaver
					: public DGE::Core::ResourceManager::Savers::Saver
					, public Core::Utility::Interfaces::ISerializer
				{
					D_CORE_OBJECT(AnimationSaver)
				public:
					AnimationSaver();
					virtual ~AnimationSaver();

					virtual bool save(const DPath& path, Core::System::DCoreObjectRef object,
									  const std::optional<DOption>& option = {}) override;

				protected:
					DByteArray keyFrameListSerialize(KeyFrameVector3DList keyFrameList) const;
					// ISerializer interface
					virtual DByteArray serialize(
						std::shared_ptr<Core::System::DCoreObject> object) const override;

				public:
					/// \brief The option for this saver class that is specified that will be export
					/// all animation points or not.
					///
					/// The type of this option is boolean or 'true'/'false'
					static inline const DString s_optionIsExportPoints = "isExportPoints";

					/// \brief The option for this saver class that is specified that what number of
					/// steps is used for exporting all animation points.
					///
					/// The type of this option is integer and in 'milliseconds'
					static inline const DString s_optionNumStepsPoints = "numStepsPoints";

				protected:
					Core::Serializers::JsonSerializer m_serializer;
				};

				D_DefRefType(AnimationSaver);

			} // namespace Savers
		}	  // namespace Assets
	}		  // namespace Scene
} // namespace DGE
