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

#include "Scene/Resource/AnimationTemplate.h"
#include <Core/Animation/AnimationController.h>
#include <Core/Math/Vector2D.h>
#include <Core/OS/Timer.h>
#include <Core/System/DProperty.h>
#include <Scene/Component.h>
#include <type_traits>

namespace DGE
{
	using namespace Core::Math;
	namespace Scene
	{
		class AnimationGroup;
		using KeyFrameVector3D	   = DGE::Core::Animation::BaseKeyframe<DGE::Core::Math::Vector3D>;
		using KeyFrameVector3DList = Core::Animation::KeyFrameList<Vector3D>;
		using MappedKey = DGE::Core::Animation::AnimationEngine<DGE::Math::Vector3D>::MappedKeys;
		class DGE_SceneAPI AnimationComponent : public DGE::Scene::Component
		{
			D_OBJECT(AnimationComponent)
			using MapRefProperty
				= std::map<Core::System::DPropertyKey, Core::System::DPropertyRefWrap<Vector3D>>;

			friend class AnimationGroup;

		public:
			AnimationComponent(DGE::Core::System::ContextRef context);
			D_Destructor(AnimationComponent);

			// IRenderPipeline interface
			/// \brief Call when object must be initialized.
			using Scene::Component::initialize;

			// IInitialize interface
			bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			// IUpdate interface
			bool update(GFX::GCore::InputParameters& input) override;

			/// \brief stop playing animation for now. Must call play() function to playing
			/// animation again
			void stop();

			/// \brief start playing animation. Must call stop() or pause() function to preventing
			/// of continuing playing animation
			///
			/// \param isReversePlaying if set this flag to true, animation play in reverse direction
			void play(bool isReversePlaying = false);

			/// \brief pause playing animation for now. Must call play() function to continue
			/// playing animation from current place of animation
			void pause();

			/// \brief Return `true` if the animation is stopped otherwise return `false`.
			bool isStopped() const;

			/// \brief Return `true` if the animation is paused otherwise return `false`.
			bool isPaused() const;

			/// \brief Return `true` if the animation is playing now otherwise return `false`.
			bool isPlaying() const;

			/// \brief Return `true` if the animation is playing in reverse order now otherwise
			/// return `false`.
			bool isReversePlaying() const;

			/// \brief Indicate whether animation reached to end of playing animation
			bool isFinished() const;

			uint64 addKeyFrame(
				const Core::Utility::AcTime& frameTime, const Core::Math::Vector3D& value,
				Core::Animation::TransitionType									 transitionType,
				Core::Animation::TransformerRef<Core::Animation::TransitionType> transformerRef
				= nullptr);
			uint64 addKeyFrame(
				uint64 frameTime, const Core::Math::Vector3D& value,
				Core::Animation::TransitionType									 transitionType,
				Core::Animation::TransformerRef<Core::Animation::TransitionType> transformerRef
				= nullptr);

			bool modifyKeyFrame(uint64 id, uint64 frameTime);
			bool modifyKeyFrame(uint64 id, Core::Utility::AcTime frameTime);
			bool modifyKeyFrame(uint64 id, Core::Math::Vector3D value);
			bool modifyKeyFrame(uint64 id, Core::Animation::TransitionType transitionType);
			bool modifyKeyFrame(uint64 id, Core::Utility::AcTime frameTime,
								Core::Math::Vector3D			value,
								Core::Animation::TransitionType transitionType);

			bool removeKeyFrame(uint64 id);

			void addProperty(System::DProperty<Core::Math::Vector3D>& prop);
			bool isContainProperty(System::DProperty<Core::Math::Vector3D>& prop);

			Core::Math::Vector3D currentValue() const;

			void setFrameTime(uint64 frameTime);

			uint64 size() const;

			// // IPreProcessRender interface
			// bool preProcessRender(GFX::GCore::InputParameters& input) override;

			// // IRender interface
			// bool render(GFX::GCore::InputParameters& input) override;

			// // IPostProcessRender interface
			// bool postProcessRender(GFX::GCore::InputParameters& input) override;

			Core::Animation::AnimationEngine<Vector3D>::MappedValues getMappedValues(
				uint64 frameTime);

			Core::Animation::AnimationEngine<Vector3D>::MappedKeys getMappedKeys(uint64 frameTime);

			/// \brief Return previous key frame data of specified frame time.
			std::optional<Core::Animation::BaseKeyframe<Vector3D>> getPreviousKeyFrame(
				uint64 frameTime);

			/// \brief Return next key frame data of specified frame time.
			std::optional<Core::Animation::BaseKeyframe<Vector3D>> getNextKeyFrame(uint64 frameTime);

			/// \brief Return key frame data at specified frame time.
			std::optional<Core::Animation::BaseKeyframe<Vector3D>> keyFrame(uint64 frameTime);

			/// \brief Return value of the animation in the specified frame time.
			Vector3D peek(uint64 frameTime) const;

			std::weak_ptr<Scene::AnimationGroup> parentGroup() const;

			/// \brief Return list of key-frames that is available in this animation engine.
			///
			/// \note This list isn't updated automatically and need to call this function to
			/// generate the updated list of key-frames.
			KeyFrameVector3DList keyFrames() const;

			/// \brief Convert the current animation component into the Animation Template type to
			/// store in the resources.
			///
			/// \note For loading an animation and sharing between multiple objects, you should use
			/// Animation Template resources
			///
			/// \param isExportAllPoints Indicate whether all points of the animation will be
			/// exported to this animation template. \note Note that exporting all points will be
			/// time-consume process.
			Scene::Resource::AnimationTemplateRef<Vector3D> toAnimationTemplate(
				bool isExportAllPoints = false, uint64 steps = 1) const;

			/// \brief Apply loaded animation template to this animation component to use the
			/// key-frames in the animation template
			bool applyAnimationTemplate(
				const Scene::Resource::AnimationTemplateRef<Vector3D>& animTemplate);

		protected:
			bool onValueChanged(Vector3D newValue);
			void setParentGroup(const std::weak_ptr<Scene::AnimationGroup>& newParentGroup);

		protected:
			Animation::AnimationControllerURef<Vector3D>	 m_animController;
			std::weak_ptr<Scene::AnimationGroup>			 m_parentGroup;

			MapRefProperty									 m_refProperties;
			uint64											 m_firstKeyFrameID;
			uint64											 m_lastKeyFrameID;
			bool											 m_isModifingKeyFrame;
			bool											 m_isReversePlaying;
			Core::OS::PreciseTimer							 m_timer;
			D_READONLY_PROPERTY(uint64, firstKeyFrameTime, FirstKeyFrameTime, uint64, 0);
			D_READONLY_PROPERTY(uint64, lastKeyFrameTime, LastKeyFrameTime, uint64, 0);
			D_READONLY_PROPERTY(uint64, currentTime, CurrentTime, uint64, 0)
			D_PROPERTY(float, speed, Speed, 1.0f)
		};
		D_DefRefType(AnimationComponent);
	} // namespace Scene
} // namespace DGE
