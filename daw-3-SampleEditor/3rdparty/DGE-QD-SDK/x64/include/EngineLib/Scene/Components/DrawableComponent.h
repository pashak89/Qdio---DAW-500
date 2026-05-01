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

#include <Core/Profiling/Profiler.h>
#include <Core/System/DSubsystem.h>
#include <Scene/Component.h>
#include <memory>

namespace DGE
{
	namespace Scene
	{
#define JOIN(x, y) JOIN_AGAIN(x, y)
#define JOIN_AGAIN(x, y) x##y

		/// \brief Template base class for create a component for a class that inherited from
		/// Drawable class
		template<class DrawableType, class ParentNodeType = Scene::Component, typename... Args>
		class DrawableComponent
			: public ParentNodeType
			, public virtual DrawableType
		{
			using ThisType = DrawableComponent<DrawableType, ParentNodeType, Args...>;
			D_OBJECT_TEMPLATE(ThisType, DrawableComponent, ParentNodeType)
		public:
			DrawableComponent(DGE::Core::System::ContextRef context, Args... arg)
				: ParentNodeType(context, arg...)
			{
			}
			D_Destructor(DrawableComponent) {}

			// IRenderPipeline interface
			/// \brief Call when object must be initialized.
			using Scene::Component::initialize;

			// IInitialize interface
			bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override
			{
				DGE_ProfileVarNamed(_Initialize, this->m_name);
				bool initializeResult = DrawableType::initialize(parameters);
				initializeResult &= ParentNodeType::initialize(parameters);

				return initializeResult;
			}

			// IUpdate interface
			bool update(GFX::GCore::InputParameters& input) override
			{
				DGE_ProfileVarNamed(_Update, this->m_name);
				bool updateResult = DrawableType::update(input);
				updateResult &= ParentNodeType::update(input);

				return updateResult;
			}

			// // IPreProcessRender interface
			// bool preProcessRender(GFX::GCore::InputParameters& input) override
			// {
			// 	DGE_ProfileVarNamed(_PreRender, this->m_name);
			// 	auto preProcessResult = DrawableType::preProcessRender(input);
			// 	preProcessResult &= ParentNodeType::preProcessRender(input);

			// 	return preProcessResult;
			// }

			// // IRender interface
			// bool render(GFX::GCore::InputParameters& input) override
			// {
			// 	DGE_ProfileVarNamed(_Render, this->m_name);
			// 	auto renderResult = DrawableType::render(input);
			// 	renderResult &= ParentNodeType::render(input);

			// 	return renderResult;
			// }

			// // IPostProcessRender interface
			// bool postProcessRender(GFX::GCore::InputParameters& input) override
			// {
			// 	DGE_ProfileVarNamed(_PostRender, this->m_name);
			// 	auto postProcessResult = DrawableType::postProcessRender(input);
			// 	postProcessResult	   = ParentNodeType::postProcessRender(input);
			// 	return postProcessResult;
			// }

			// IInitialize interface
			virtual bool isInitialized() override { return ParentNodeType::m_isInitialized; }

		protected:
			virtual void setIsInitialized(bool isInitialized) override
			{
				ParentNodeType::m_isInitialized = isInitialized;
			}

			/// \brief Generate a text in output log with given message with Information label.
			///
			/// \param msg It's main message of log
			void logInfo(const char* msg) { ParentNodeType::logInfo(msg); }

			/// \brief Generate a text in output log with given message with Warning label.
			///
			/// \param msg It's main message of log
			void logWarning(const char* msg) { ParentNodeType::logWarning(msg); }

			/// \brief Generate a text in output log with given message with Error label.
			///
			/// \param msg It's main message of log
			void logError(const char* msg) { ParentNodeType::logError(msg); }

			/// \brief Generate a text in output log with given message with FatalError label.
			///
			/// \param msg It's main message of log
			void logFatalError(const char* msg) { ParentNodeType::logFatalError(msg); }

			/// \brief Generate a text in output log with given message with Debug label.
			///
			/// \param msg It's main message of log
			void logDebug(const char* msg) { ParentNodeType::logDebug(msg); }

			/// \brief Generate a text in output log with given message with Success label.
			///
			/// \param msg It's main message of log
			void logSuccess(const char* msg) { ParentNodeType::logSuccess(msg); }

			/// \brief Generate a text in output log with given message with Information label.
			///
			/// \param msg It's main message of log
			void logInfo(const DString& msg) { ParentNodeType::logInfo(msg); }

			/// \brief Generate a text in output log with given message with Warning label.
			///
			/// \param msg It's main message of log
			void logWarning(const DString& msg) { ParentNodeType::logWarning(msg); }

			/// \brief Generate a text in output log with given message with Error label.
			///
			/// \param msg It's main message of log
			void logError(const DString& msg) { ParentNodeType::logError(msg); }

			/// \brief Generate a text in output log with given message with FatalError label.
			///
			/// \param msg It's main message of log
			void logFatalError(const DString& msg) { ParentNodeType::logFatalError(msg); }

			/// \brief Generate a text in output log with given message with Debug label.
			///
			/// \param msg It's main message of log
			void logDebug(const DString& msg) { ParentNodeType::logDebug(msg); }

			/// \brief Generate a text in output log with given message with Success label.
			///
			/// \param msg It's main message of log
			void logSuccess(const DString& msg) { ParentNodeType::logSuccess(msg); }

			/// \brief Indicate whether tagType exist in tagtypeList of this object
			///
			/// \param tagType tagType for searching in tagtypeList
			bool hasTagType(DGE::Core::System::DObjectTagType tagType)
			{
				return ParentNodeType::hasTagType(tagType);
			}
		};

#define DrawableComponentDefine(ComponentName, DrawableType, ...)                            \
	using ComponentName = DGE::Scene::DrawableComponent<DrawableType, ##__VA_ARGS__>; \
	using JOIN(ComponentName, Ref) = std::shared_ptr<DGE::Scene::ComponentName>;
	} // namespace Scene
} // namespace DGE
