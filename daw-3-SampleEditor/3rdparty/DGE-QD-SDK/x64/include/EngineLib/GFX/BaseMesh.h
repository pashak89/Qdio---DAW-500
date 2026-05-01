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

#include "Material.h"
#include <Core/BaseTypes/BaseTypes.h>
#include <Core/Math/Math.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/ResourceManager/ResourceLoader.h>

namespace DGE
{
	using namespace DGE::ResourceManager;
	using namespace DGE::Core::Math;
	namespace GFX
	{
		/// \brief Base class for working with mesh object
		class DGE_GFXAPI BaseMesh
		{
		public:
			BaseMesh();
			virtual ~BaseMesh();

			/// \brief Set position of this Mesh
			virtual void SetPosition(Vector3D);
			virtual void SetColor(Core::BaseType::Color color);
			virtual void Update(float elapse);
			virtual void Render();
			virtual void AddForce(Vector3D force);

		protected:
			ObjectMesh*				  m_meshObject;
			std::shared_ptr<Material> m_material; // Array of materials used by the mesh container.
			Matrix					  m_matrixTransform;
			// DGE::Physic::Px_shape m_pxShape;
		};
	} // namespace GFX
} // namespace DGE
