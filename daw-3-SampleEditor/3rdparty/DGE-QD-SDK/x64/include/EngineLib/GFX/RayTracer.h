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

#include <Scene/Components/PredefinedComponents.h>
#include <Scene/DEntity.h>

namespace DGE
{
	namespace GFX
	{
		// The ray tracer class is using for
		class DGE_GFXAPI RayTracer : public Core::System::DCoreObject
		{
		public:
			RayTracer();

			Scene::DEntityRef		 doRayTrace(GFX::GCore::InputParameters& input);
			Scene::DEntityRef doRayTrace(GFX::GCore::InputParameters& input, Core::Math::Ray& ray);
			static Scene::DEntityRef doRayTrace(
				Scene::CameraComponentRef							camera,
				const std::vector<std::shared_ptr<Scene::DEntity>>& entitiesList,
				Core::Math::Vector2D mousePosition, Core::Math::Ray& ray,
				bool checkOnlyHitable = true);

		protected:
			static Core::Math::AABB s_calcBoundary;
		};

	} // namespace GFX
} // namespace DGE
