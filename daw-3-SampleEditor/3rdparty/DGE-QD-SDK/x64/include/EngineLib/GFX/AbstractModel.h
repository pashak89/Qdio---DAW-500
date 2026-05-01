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
#include <GFX/GCore/Interfaces/IGObjectDrawable.h>
#include <memory>

namespace DGE
{
	namespace GFX
	{
		/// \brief An abstract class for rendering model object
		class DGE_GFXAPI AbstractModel : public DGE::GFX::GCore::Drawable
		{
			D_OBJECT(AbstractModel)
		public:
			AbstractModel(
				GFX::GCore::Interfaces::IGObjectDrawableRef geometryMesh = nullptr,
				DGE::Core::System::ContextRef context = Core::System::Context::getInstance());

			/// \brief Retrieve list of vertices for this geometry object
			virtual std::vector<DGE::Core::Math::Vector3D> vertices() const;

			virtual uint64_t totalNumberVertices() override;

		public:
			/// \brief The key name for stored current rendering model in input parameters
			static DString s_currentModel;

		protected:
			GFX::GCore::Interfaces::IGObjectDrawableRef m_mesh;
		};
		using AbstractModelRef = std::shared_ptr<DGE::GFX::AbstractModel>;
		using AbstractModelWRef = std::weak_ptr<DGE::GFX::AbstractModel>;
	} // namespace GFX
} // namespace DGE
