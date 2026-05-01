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
 * API Configurator header file is using for detecting available features and libraries
 */
#pragma once

#include <Core/Math/Vector.h>
#include <GFX/GCore/Drawable.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		namespace GCore
		{
			namespace Interfaces
			{
				class IGObjectDrawable;
				using IGObjectDrawableRef = std::shared_ptr<IGObjectDrawable>;
				/// \brief An interface for drawable objects that contain basic Graphic Objects to
				/// be rendered.
				class DGE_GFXAPI IGObjectDrawable : public DGE::GFX::GCore::Drawable
				{
					D_OBJECT(IGObjectDrawable)
				public:
					using GFX::GCore::Drawable::Drawable;

					/// \brief create a clone of this geometry with specified LOD (level of details)
					virtual IGObjectDrawableRef clone(uint16 lod) const = 0;

					/// \brief Retrieve list of vertices for this drawable object
					virtual std::vector<DGE::Core::Math::Vector3D> vertices() const = 0;
				};
			} // namespace Interfaces
		}	  // namespace GCore
	}		  // namespace GFX
} // namespace DGE
