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
#include <Core/BaseTypes/DString.h>
#include <Core/Math/Bound.h>
#include <atomic>
#include <memory>
#include <Qt/QMLScene/Camera/QCameraController.h>
#include <Qt/QMLScene/QINode3D.h>
#include <Qt/QtDManagers/QEnvironment3DManager.h>

#include <Qt/QMLScene/Camera/QCameraComponent.h>
#include <Qt/QMLScene/QScene3D.h>
#include <Qt/QtModuleDef.h>

#include <Core/MemoryManager/UndefMemoryMacros.h>

#include <Qt/QEngine.h>
#include <Qt/QtDCore/IO/InputHandler.h>

#include <Core/MemoryManager/RedefMemoryMacros.h>

#include <Core/Windows/QtWindow.h>
#include <Scene/ViewportManager.h>

namespace DGE
{
	namespace QtWrapper
	{

		/// \brief Wrapper for QD Engine and control and sync it on QML
		class DGE_QtAPI ViewPort3D
			: public QQuickFramebufferObject
			, public DObject
		{
			D_OBJECT(ViewPort3D)

			Q_OBJECT
			QML_ELEMENT

			Q_PROPERTY(bool isFullScreen READ isFullScreen WRITE setIsFullScreen RESET
						   resetIsFullScreen NOTIFY isFullScreenChanged)
			Q_PROPERTY(float fps READ fps NOTIFY fpsChanged)
			Q_PROPERTY(
				uint32 sceneNodeID READ sceneNodeID WRITE setSceneNodeID NOTIFY sceneNodeIDChanged)
			Q_PROPERTY(uint64 totalNumberVertices READ totalNumberVertices NOTIFY
						   totalNumberVerticesChanged)
			Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
			Q_PROPERTY(int id READ id NOTIFY idChanged)
			Q_PROPERTY(DGE::QtWrapper::QMLScene::QCameraComponent* camera MEMBER m_camera WRITE
						   setCamera NOTIFY cameraChanged USER true)
			Q_PROPERTY(DGE::QtWrapper::QMLScene::QNode3D* scene3D READ scene3D WRITE setScene3D
						   NOTIFY scene3DChanged)
			Q_PROPERTY(DGE::QtWrapper::QMLScene::QCameraController* cameraController READ
						   cameraController WRITE setCameraController NOTIFY cameraControllerChanged)
			Q_PROPERTY(bool isActivatedInput READ isActivatedInput WRITE setIsActivatedInput NOTIFY
						   isActivatedInputChanged)
			Q_PROPERTY(uint8 priorityView READ priorityView WRITE setPriorityView NOTIFY
						   priorityViewChanged)
		public:
			ViewPort3D(QQuickItem* parent = nullptr);

			// QQuickFramebufferObject interface
			virtual QQuickFramebufferObject::Renderer* createRenderer() const override;

			bool isFullScreen() const;

			/// \brief Return current FPS of the Engine pipeline
			float fps() const;

			uint32 sceneNodeID() const;

			QColor color() const;

			int id() const;

			QMLScene::QCameraComponent* camera() const;
			void						setCamera(QMLScene::QCameraComponent* newCamera);
			void						resetCamera();

			QMLScene::QNode3D* scene3D() const;
			void			   setScene3D(DGE::QtWrapper::QMLScene::QNode3D* newScene3D);

			DGE::QtWrapper::QtDCore::IO::InputHandlerRef inputHandler() const;

			DGE::QtWrapper::QMLScene::QCameraController* cameraController() const;

			ViewPort3D* referenceView3D() const;
			void		setReferenceView3D(ViewPort3D* newReferenceView3D);

			bool isActivatedInput() const;
			void setIsActivatedInput(bool newIsActivatedInput);

			uint8 priorityView() const;

			Qt::MouseButtons mouseButtons() const;

		public slots:
			/// \brief call when must initialize view-port
			void onInitialize();

			/// \brief call every time when QQuickWindow::beforeSynchronizing emit
			void onSync();

			/// \brief call when QML want to close the window
			void onCleanup();

			/// \brief call when QtRender emit QtRender::paint() signal.
			void onRender();

			void onClosing();

			/// \brief Set full screen state of the engine
			void setIsFullScreen(bool newIsFullScreen);
			void resetIsFullScreen();

			void setSceneNodeID(DGE::Core::BaseType::uint32 newSceneNodeID);

			void setColor(const QColor& newBackgroundColor);

			void setCameraController(
				DGE::QtWrapper::QMLScene::QCameraController* newCameraController);

			uint64 totalNumberVertices();

			void setPriorityView(uint8 newPriorityView);

			void setMouseButtons(const Qt::MouseButtons& newMouseButtons);

		protected:
			void setFPS(float currentFPS);

			/// \brief Call when selected entity changed in the engine.
			void onSelectedEntityChanged(Scene::DEntityRef entity);

			void timerEvent(QTimerEvent* event) override;

			// QQuickItem interface
			virtual void hoverEnterEvent(QHoverEvent* event) override;
			virtual void hoverMoveEvent(QHoverEvent* event) override;
			virtual void hoverLeaveEvent(QHoverEvent* event) override;
			virtual void keyPressEvent(QKeyEvent* event) override;
			virtual void keyReleaseEvent(QKeyEvent* event) override;
			virtual void mousePressEvent(QMouseEvent* event) override;
			virtual void mouseMoveEvent(QMouseEvent* event) override;
			virtual void mouseReleaseEvent(QMouseEvent* event) override;
			virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
			virtual void mouseUngrabEvent() override;
			virtual void touchUngrabEvent() override;
			virtual void wheelEvent(QWheelEvent* event) override;
			virtual void touchEvent(QTouchEvent* event) override;

		private slots:
			/// \brief update properties that related to QML window
			void handleWindowChanged(QQuickWindow* win);

			void decreaseFPS();

			void onEnabledChange();

			/// \brief process event of click on this viewport 3d
			void onClicked(QMouseEvent* mouseEvent);

			/// \brief process event of moving mouse on this viewport 3d
			void onMouseHoverMoved(QHoverEvent* mouseEvent);

			void onNodesListUpdated();

			void onUpdated();
		signals:
			/// \brief emit before start initializing of current view-port
			void beforeInitalizing(QQuickWindow* window);

			/// \brief emit when initializing of view-port finished
			void initialized();

			/// \brief Emit when isFullScreen changed
			void isFullScreenChanged(bool isFullScreen);

			/// \brief Emit dump profiling data at end of every frame
			void dumpProfilingDataReady(QString dumpProfilingData);

			/// \brief Emit when fps changed
			void fpsChanged(float fps);

			void sceneNodeIDChanged(uint32 sceneNodeID);

			void colorChanged(QColor backgroundColor);
			void idChanged(int id);

			void cameraChanged();

			void scene3DChanged();

			void cameraControllerChanged(
				DGE::QtWrapper::QMLScene::QCameraController* cameraController);

			void referenceView3DChanged();

			void isActivatedInputChanged();

			void totalNumberVerticesChanged();

			void selectedEntityChanged(Scene::DEntityRef);

			void priorityViewChanged();

			void mouseButtonsChanged(Qt::MouseButtons mouseButtons);

		private:
			/// \brief release resources that get when startup engine.
			void releaseResources() override;

			void updateCamera();
			void updateViewport();
			void updateTimer(uint16 updateInterval);

			Q_PROPERTY(Qt::MouseButtons mouseButtons READ mouseButtons WRITE setMouseButtons NOTIFY
						   mouseButtonsChanged FINAL)

		protected:
			/// \brief
			static uint16					 s_updateInterval;
			static uint16					 s_minimumUpdateInterval;
			static uint16					 s_decreaseIntervalStep;
			static uint16					 s_increaseIntervalStep;
			static std::chrono::milliseconds s_updateProfilingInterval;

			mutable std::shared_ptr<QuickFBRenderer> m_quickFBRenderer;
			InitializeParams						 m_initializedParameters;
			GFX::GCore::InputParameters				 m_inputRenderParameters;
			atomic_bool								 m_isFullScreen;
			/// \brief Indicate whether	processing the input event of this view is activated
			atomic_bool								 m_isActivatedInput;
			atomic_bool								 m_isFirstFrame;
			/// \brief Indicate whether the mouse is hovered on this view.
			atomic_bool m_isMouseHovered;
			/// \brief Specify priority of this view-port in the render pipeline from enumerator
			/// PriorityView
			///
			/// \value 1 Background
			/// \value 2 Low
			/// \value 3 Normal
			/// \value 4 High
			atomic_uint8_t m_priorityView;
			/// \brief Indicate whether the parent window of this view-port is going to close
			atomic_bool m_isClosing;

			std::shared_ptr<QEngine>			  m_engine;
			std::shared_ptr<GFX::GCore::Renderer> m_renderer;
			QtWindowWRef						  m_window;
			QtDManagers::QEnvironment3DManager	  m_qEnvironment3DManager;

			Scene::ViewportManagerRef			m_viewportManager;
			GFX::ViewportRef					m_viewport;
			Core::Math::Bound					m_geometry;
			std::vector<ViewInfo>				m_viewsInfo;
			QColor								m_color;
			bool								m_isMouseClicked;
			bool								m_isMousePressed;
			Core::Math::Vector2D				m_mousePosition;

			/// \brief Total number rendered vertices in latest frame for this view
			atomic_uint64_t m_totalNumberVertices;

			DGE::QtWrapper::QMLScene::QScene3DRef		 m_rootScene;
			DGE::QtWrapper::QMLScene::QCameraComponent*	 m_camera			= nullptr;
			DGE::QtWrapper::QMLScene::QCameraController* m_cameraController = nullptr;
			DGE::QtWrapper::QMLScene::QNode3D*			 m_sceneNode3D		= nullptr;
			Scene::DEntityWRef							 m_lastSelectedEntity;
			QtDCore::IO::InputHandlerRef				 m_inputHandler;
			/// \brief Mouse buttons are used for controlling the camera
			Qt::MouseButtons m_mouseButtons;

			/// \brief ID of node in scene graph that is assign to this view-port
			uint32 m_sceneNodeID;

			/// \brief No. number of current frame
			uint64 m_frameNumber = 0;

			int32							m_timerId;
			Core::PreciseTimer				m_profilerTimer;
			Core::PreciseTimer				m_frameTimer;
			std::shared_ptr<Profiling::FPS> m_fps;
		};

	} // namespace QtWrapper
} // namespace DGE
