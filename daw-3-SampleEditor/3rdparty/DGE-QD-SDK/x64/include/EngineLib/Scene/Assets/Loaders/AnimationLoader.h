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

#include <Core/ResourceManager/Loaders/Loader.h>
#include <Core/Serializers/JsonSerializer.h>

namespace DGE
{
	namespace Scene
	{
		namespace Assets
		{
			namespace Loaders
			{
				/// \brief Load animation resource into the scene
				class AnimationLoader
					: public DGE::Core::ResourceManager::Loaders::Loader
				{
					D_CORE_OBJECT(AnimationLoader)
				public:
					AnimationLoader();
					virtual ~AnimationLoader();
					// Loader interface
					Core::System::DCoreObjectRef load(
						const DPath& path, const std::optional<DOption>& option = {}) override;

				protected:
					Core::Serializers::JsonSerializer m_serializer;
				};

				D_DefRefType(AnimationLoader)

			} // namespace Loaders
		}	  // namespace Assets
	}		  // namespace Scene
} // namespace DGE
