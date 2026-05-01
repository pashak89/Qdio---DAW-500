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
#include <Core/BaseTypes/DOption.h>
#include <Core/BaseTypes/DPath.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/System/DCoreObject.h>
#include <optional>

namespace DGE
{
	namespace Core
	{
		namespace ResourceManager
		{
			namespace Loaders
			{
				class ILoad
				{
				public:
					virtual System::DCoreObjectRef load(const DPath&				  path,
														const std::optional<DOption>& option = {})
						= 0;
					System::DCoreObjectRef load(DPath&& path, std::optional<DOption>&& option = {})
					{
						return load(path, option);
					}
					/// \brief Try to load and convert to specified type
					template<typename ObjectType>
					std::shared_ptr<ObjectType> loadByType(const DPath& path)
					{
						return Memory::reinterpret_pointer_cast<ObjectType>(load(path));
					}
				};
			} // namespace Loaders
		}	  // namespace ResourceManager
	}		  // namespace Core
} // namespace DGE
