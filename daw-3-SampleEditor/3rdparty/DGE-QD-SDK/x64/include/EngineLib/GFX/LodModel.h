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

#include "AbstractModel.h"
#include "Core/System/DObjectMacrosDef.h"

namespace DGE
{
	namespace GFX
	{
		/// \brief A Wrapper class for using Drawable object as a model that can be a loaded object
		/// file or built-in Geometry classes. It control level of details of the model based on the
		/// level that can be set.
		class DGE_GFXAPI LodModel : public DGE::GFX::AbstractModel
		{
			D_OBJECT(LodModel)
		public:
			LodModel(

				GFX::GCore::Interfaces::IGObjectDrawableRef geometryMesh = nullptr,
				uint16										 totalLevels  = 10,
				DGE::Core::System::ContextRef context = Core::System::Context::getInstance());

			// IInitialize interface
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams &parameters) override;

			// IUpdate interface
			virtual bool update(GCore::InputParameters &input) override;

			// IPreProcessRender interface
			virtual bool preProcessRender(GCore::InputParameters &input) override;

			// IRender interface
			virtual bool render(GCore::InputParameters &input) override;

			// IPostProcessRender interface
			virtual bool postProcessRender(GCore::InputParameters &input) override;

			GFX::GCore::Interfaces::IGObjectDrawableRef mesh() const;
			void setMesh(const GFX::GCore::Interfaces::IGObjectDrawableRef &newMesh);

			uint16 currentLOD() const;
			void   setCurrentLOD(uint16 newCurrentLOD);

		protected:
			/// \brief Map of level of details and related model
			std::map<uint16, GCore::Interfaces::IGObjectDrawableRef> m_lodMeshes;
			uint16													  m_currentLOD;
			uint16													  m_totalLevels;
		};

		using LodModelRef = std::shared_ptr<DGE::GFX::LodModel>;
	} // namespace GFX
} // namespace DGE
