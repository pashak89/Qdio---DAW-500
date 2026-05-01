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
#include "Core/Utility/Singleton/SingletonMacrosDef.h"
#include <Qt/QMLScene/QEntity3D.h>
#include <Qt/QtModuleDef.h>
#include <optional>
#if defined(DGE_Track_Memory) || defined(DGE_UseCRTDebugMemory)
#undef new
#undef delete
#endif
#if defined(DGE_USE_QT)
#include <QObject>
#include <QQuickWindow>
#include <QtCore/QtCore>

#include <Qt/QtDGFX/QuickFBRenderer.h>
#include <Qt/QtModule.h>
#endif

#ifdef DGE_Track_Memory
#define new(param, ...) DBG_NEW(param, DGE_FileCodeInfo, DGE_LineCodeInfo)
#define delete(param, ...) DBG_Delete(param, DGE_FunctionInfo, DGE_FileCodeInfo, DGE_LineCodeInfo)
#elif defined(DGE_UseCRTDebugMemory)
#define _New DBG_NEW
#define new DBG_NEW
#endif
#include <Core/System/DObject.h>
#include <Core/System/DSubsystem.h>
#include <EngineCore/Engine.h>
#include <memory>

#if defined(DGE_USE_QT)
#define QSTRING(x) QString::fromWCharArray(x)
Q_DECLARE_METATYPE(int8)
Q_DECLARE_METATYPE(int16)
Q_DECLARE_METATYPE(int32)
Q_DECLARE_METATYPE(int64)
Q_DECLARE_METATYPE(uint8)
Q_DECLARE_METATYPE(uint16)
Q_DECLARE_METATYPE(uint32)
Q_DECLARE_METATYPE(DGE::Core::BaseType::uint64)
Q_DECLARE_METATYPE(DGE::Core::Utility::Time)

#include <Qt/QEngineDef.h>

namespace DGE
{
	namespace QtWrapper
	{
		class QEngine;
	}
} // namespace DGE
/// \brief Initialize QEngine resources for using with Qt framework
extern DGE_QtExtern void initQEngineResources(bool isRedirectQtLog = true);

/// \brief Process input arguments of the application
///  *
/// * Some parameters is detect from input of the app that they are :
/// * 1) Default assets path: -assets <the path>
extern DGE_QtExtern void processInputArgs(QStringList args);
#endif

namespace DGE
{
	namespace QtWrapper
	{
#if defined(DGE_USE_QT)
		/// \brief Qt Wrapper for Engine that load and initialized it
		class DGE_QtAPI QEngine
			: public QObject
			, public DSubsystem
			, protected Singleton<QEngine, std::shared_ptr<QEngine>,
								  DGE::Core::Utility::Deleter<QEngine>::emptyDeleter>
		{
			Q_OBJECT
			D_OBJECT_SINGLETON(QEngine, std::shared_ptr<QEngine>,
							   DGE::Core::Utility::Deleter<QEngine>::emptyDeleter)

			friend class DGE::QtWrapper::QMLEngine;
			friend std::shared_ptr<DGE::QtWrapper::QEngine> getQEngine();
			friend bool										releaseQEngine();

			explicit QEngine(std::optional<DString> defaultAssetsPath, QObject* parent = nullptr);

		public:
			D_Destructor(QEngine);
			// IInitialize interface
			using IInitialize::IInitialize::initialize;
			virtual bool initialize(Core::Utility::Interfaces::InitializeParams& parameters) override;

			virtual bool release() override;

			/// \brief true when engine is initialized and it's true when engine is initialized
			virtual bool isInitialized() override;

			/// \brief Run one frame
			bool runFrame(GFX::GCore::InputParameters& input);

			/// \brief Dump profiling data into string
			QString dumpProfilingData();

			DString defaultVertexShader();
			DString defaultFragmentShader();

			/// \brief set enable flag for creating debug item for ray tracing
			Q_INVOKABLE void setIsEnabledDebugRayTracing(bool isEnabled);
			Q_INVOKABLE bool isEnabledDebugRayTracing();

			DString defaultAssetsPath() const;

		protected:
			DString		 readQtResources(DString fileName);
			virtual bool registerAll(Core::Utility::Interfaces::InitializeParams& parameters) override;
			bool		 onUpdated(GFX::GCore::InputParameters input);
			void		 onSelectedEntityChanged(Scene::DEntityRef selectedEntity);

		public slots:
			/// \brief Call when must initialize the engine
			void onInitialize(QQuickWindow* window);

			/// \brief Call when render pipeline is finished
			void onFinializeRenderPipeline();

			void setDefaultAssetsPath(const DString& newDefaultAssetsPath);

		protected:
		signals:
			/// \brief emit when initialize of engine finished
			void initialized();

			/// \brief emit when engine ready for running it main loop for rendering and
			/// updating process.
			void startedEngine();

			/// \brief emit when engine is updated.
			void updated();

			/// \brief emit when selected entity is changed.
			void selectedEntityChanged(QMLScene::QEntity3D*);

		protected:
			std::shared_ptr<DGE::Engine> m_engine;
			InitializeParams	 m_initializedParameters;
			QtWrapper::QtModuleRef		 m_qtModule;
			DString						 m_defaultAssetsPath;
		};

		using QEngineRef = std::shared_ptr<QEngine>;

		extern DGE_QtExtern std::shared_ptr<DGE::QtWrapper::QEngine> getQEngine();
		extern DGE_QtExtern bool									 releaseQEngine();
#endif
	} // namespace QtWrapper
} // namespace DGE
