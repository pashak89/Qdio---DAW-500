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
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/Loaders/Loader.h>
#include <Core/System/DCoreObject.h>
#include <Core/System/DObjectMacrosDef.h>
#include <GFX/GFXModuleDef.h>

namespace DGE
{
	namespace GFX
	{
		namespace Assets
		{
			namespace Loaders
			{
				/// \brief The Loader of texture that load it
				class DGE_GFXAPI TextureLoader
					: public Core::ResourceManager::Loaders::Loader
				{
					D_CORE_OBJECT(TextureLoader);

				public:
					TextureLoader();
					virtual ~TextureLoader();
					// Loader interface
					Core::System::DCoreObjectRef load(
						const DPath& path, const std::optional<DOption>& option = {}) override;
				};
			} // namespace Loaders
		}	  // namespace Assets
	}		  // namespace Core
} // namespace DGE
