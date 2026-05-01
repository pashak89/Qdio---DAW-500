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

#include <Core/ResourceManager/Savers/Saver.h>
#include <Core/Serializers/JsonSerializer.h>
#include <Core/Utility/Interfaces/ISerializer.h>
#include <GFX/GFXModuleDef.h>

namespace DGE
{
	namespace GFX
	{
		namespace Assets
		{
			namespace Saver
			{

				class DGE_GFXAPI MaterialSaver
					: public DGE::Core::ResourceManager::Savers::Saver
					, public Core::Utility::Interfaces::ISerializer
				{
				public:
					MaterialSaver();

					virtual bool save(const DPath& path, Core::System::DCoreObjectRef object,
									  const std::optional<DOption>& option = {}) override;

				protected:
					// ISerializer interface
					virtual DByteArray serialize(Core::System::DCoreObjectRef object) const override;

				protected:
					Core::Serializers::JsonSerializer m_serializer;
				};
				D_DefRefType(MaterialSaver)
			} // namespace Saver
		}	  // namespace Assets
	}		  // namespace Core
} // namespace DGE
