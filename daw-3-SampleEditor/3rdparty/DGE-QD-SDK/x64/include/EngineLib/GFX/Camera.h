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

#include "Core/Math/Bound.h"
#include <Core/Math/Math.h>
#include <Core/System/DObjectMacrosDef.h>
#include <GFX/CameraLens.h>
#include <GFX/GCore/Drawable.h>
#include <GFX/GCore/Renderer.h>
#include <glm/gtc/quaternion.hpp>

namespace DGE
{
    namespace GFX
    {
        /// \brief Base class for working with camera
        class DGE_GFXAPI Camera : public GFX::GCore::Drawable
        {
            D_OBJECT(Camera)
        public:
            Camera(
                DGE::Core::System::ContextRef context = Core::System::Context::getInstance());
            Camera(
                glm::vec3 position, glm::vec3 lookat,
                DGE::Core::System::ContextRef context = Core::System::Context::getInstance());
            D_Destructor(Camera);

            // IRenderPipeline interface
            /// \brief Call when object must be initialized.
            using GFX::GCore::Drawable::initialize;

            // IInitialize interface
            virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

            // IUpdate interface
            virtual bool update(GCore::InputParameters& input) override;

            // IPreProcessRender interface
            virtual bool preProcessRender(GCore::InputParameters& input) override;

            // IRender interface
            virtual bool render(GCore::InputParameters& input) override;

            // IPostProcessRender interface
            virtual bool postProcessRender(GCore::InputParameters& input) override;

            glm::mat4 projectionMatrix() const;

            glm::mat4 viewMatrix() const;

            CameraLensType cameraLensType();

            void setRotation(glm::vec3 direction, float distance);

            void setGeometryLens(Core::Math::Bound geometry);

            virtual Core::Math::Vector3D mouseToSpace(Core::Math::Vector2D	mousePos,
                                                      Core::Math::Vector3D& position);

        protected:
            virtual void initializedVectors();
            virtual void updateProjectionMatrix();

        protected:
            const glm::vec3 c_xAxis = glm::vec3(1, 0, 0);
            const glm::vec3 c_yAxis = glm::vec3(0, 1, 0);
            const glm::vec3 c_zAxis = glm::vec3(0, 0, 1);

            /// \brief Store the position of camera
            D_PROPERTY(glm::vec3, position, Position, glm::vec3(1, 1, 1))

            /// \brief Store the orientation of camera in 3 axis's
            glm::quat m_quaternion = glm::quat(0, 0, 0, -1);
            /// \brief Store the point that camera is looking to it
            D_PROPERTY(glm::vec3, lookatPoint, LookatPoint, c_yAxis);

            /// \brief Store the look at vector that camera is looking to it
            D_READONLY_PROPERTY(glm::vec3, lookatVector,LookatVector, glm::vec3, c_yAxis)

            /// \brief Store up vector of camera
            D_READONLY_PROPERTY(glm::vec3, upVector,UpVector, glm::vec3, c_yAxis);
            /// \brief Store right vector of camera
            D_READONLY_PROPERTY(glm::vec3, rightVector,RightVector, glm::vec3, c_xAxis);

            /// \brief Indicate whether this camera is using look-at-point as pivot point of rotation
            D_PROPERTY(bool, isUsingPivot, IsUsingPivot, false);

            /// \brief The value of rotation of camera in x-axis
            D_PROPERTY(float, roll, Roll, 0)
            /// \brief The value of rotation of camera in y-axis
            D_PROPERTY(float, yaw, Yaw, 0)
            /// \brief The value of rotation of camera in z-axis
            D_PROPERTY(float, pitch, Pitch, 0)

            /// \brief Indicate whether rotation of camera in x-axis is locked
            D_PROPERTY(bool, isLockedRotationX, IsLockedRotationX, false)
            /// \brief Indicate whether rotation of camera in y-axis is locked
            D_PROPERTY(bool, isLockedRotationY, IsLockedRotationY, false)
            /// \brief Indicate whether rotation of camera in z-axis is locked
            D_PROPERTY(bool, isLockedRotationZ, IsLockedRotationZ, false)

            D_PROPERTY(GFX::CameraLensRef, cameraLens, CameraLens);

            float m_lastYaw	  = 0;
            float m_lastPitch = 0;

            std::atomic_bool m_isLookAtChanged;
            std::atomic_bool m_isChanged;

            /// \brief Represent projection matrix for the camera
            glm::mat4 m_projectionMatrix;

            /// \brief Represent view matrix for the camera
            glm::mat4 m_viewMatrix;
        };

        using CameraRef = std::shared_ptr<GFX::Camera>;
    } // namespace GFX
} // namespace DGE
