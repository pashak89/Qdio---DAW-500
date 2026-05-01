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

#include <Core/Animation/Transformer.h>
#include <Core/Math/Vector3D.h>
#include <Core/System/Functor.h>
#include <GFX/Primitives/Segments.h>
#include <Scene/Components/AnimationComponent.h>
#include <Scene/Components/ShaderComponent.h>
#include <Scene/SceneHelper.h>
#include <Scene/SceneModuleDef.h>
#include <memory>

namespace DGE
{
	using namespace Core::Math;
	namespace Scene
	{
		using InterpolatorVector3D = Core::System::FunctionRef<std::vector<Vector3D>, Vector3D,
															   Vector3D, Vector3D, Vector3D>;
		using KeyEntity = DGE::Scene::GoemetryEntityTuple<DGE::GFX::Primitives::Geometry>;

		/// \brief Control creating a path
		class DGE_SceneAPI PathController
		{
		public:
			PathController();
			PathController(Core::Math::Vector3D beginPoint, Core::Math::Vector3D endPoint,
						   GFX::Resource::ShaderResourceRef shaderResource);

			void create(Vector3D controlBegin, Vector3D controlEnd);
			void create();

			Core::Animation::TransformerRef<> transformer() const;
			void setTransformer(const Core::Animation::TransformerRef<>& newTransformer);

			Core::Math::Vector3D end() const;
			void				 setEnd(const Core::Math::Vector3D& newEnd);

			Core::Math::Vector3D begin() const;
			void				 setBegin(const Core::Math::Vector3D& newBegin);
			void				 setBeginPosition(const Core::Math::Vector3D& newBegin);

			InterpolatorVector3D interpolator() const;
			void				 setInterpolator(const InterpolatorVector3D& newInterpolator);

			Core::System::Event<void>& pathChanged() const;

			KeyEntity beginKeyEntity() const;
			void	  setBeginKeyEntity(const KeyEntity& newBeginKeyEntity);

			KeyEntity endKeyEntity() const;
			void	  setEndKeyEntity(const KeyEntity& newEndKeyEntity);

			Scene::AnimationComponentRef animationController() const;
			void setAnimationController(const Scene::AnimationComponentRef& newAnimationController);

			uint64 endKeyFrameID() const;
			void   setEndKeyFrameID(uint64 newEndKeyFrameID);

			uint64 beginKeyFrameID() const;
			void   setBeginKeyFrameID(uint64 newBeginKeyFrameID);
			void   destroy();

			Vector3D controlBegin() const;

			Vector3D controlEnd() const;

		protected:
			KeyEntity createControlPoint(DString name);

			void updateBeginPoint();
			void updateEndPoint();
			void updateControlPoints(Vector3D controlBegin, Vector3D controlEnd);
			void updateBeginControlPoint(Vector3D controlBegin);
			void updateEndControlPoint(Vector3D controlEnd);

			void updatePathEntity(Vector3D controlBegin, Vector3D controlEnd);
			void moveBeginControlPoint(glm::vec3 move);
			void moveEndControlPoint(glm::vec3 move);
			void moveBeginKey(glm::vec3 move);
			void moveEndKey(glm::vec3 move);

		protected:
			/// \brief begin point of path
			Core::Math::Vector3D m_begin;
			/// \brief end point of path
			Core::Math::Vector3D m_end;

			uint64 m_beginKeyFrameID;
			uint64 m_endKeyFrameID;

			Vector3D m_controlBegin;
			Vector3D m_controlEnd;

			InterpolatorVector3D m_interpolator;

			Core::Animation::TransformerRef<> m_transformer;
			Scene::AnimationComponentRef	  m_animationController;

			GFX::Resource::ShaderResourceRef m_shaderResource;

			DGE::Scene::ShaderComponentRef m_mainShader;

			GoemetryEntityTuple<GFX::Primitives::Segments> m_pathEntity;

			KeyEntity m_cubeBeginControlEntity;
			KeyEntity m_cubeEndControlEntity;

			KeyEntity m_beginKeyEntity;
			KeyEntity m_endKeyEntity;

			mutable Core::System::Event<void> m_pathChanged;

			bool m_hasControlPoints;
			bool m_isDestroied;
		};

		D_DefRefType(PathController);

	} // namespace Scene
} // namespace DGE
