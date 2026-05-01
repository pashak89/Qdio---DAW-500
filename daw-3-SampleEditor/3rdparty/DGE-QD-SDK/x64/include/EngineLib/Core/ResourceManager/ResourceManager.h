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

#include <Core/ResourceManager/Loaders/LoadersManager.h>
#include <Core/ResourceManager/ResourceLoadersManager.h>

namespace DGE
{
	using namespace Core;
	using namespace Core::Utility;
	using namespace Array;
	namespace Core
	{
		namespace ResourceManager
		{
			/// * \details ResourceLoader class needs to Loader object for customizing loading resources
			/// process.
			/// * On the other hand, ResourceLoadersManager class manages the ResourceLoader objects.
			/// * And LoadersManager class manages the Loader objects.
		}
	} // namespace Core
} // namespace DGE
