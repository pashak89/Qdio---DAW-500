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
#include <Core/BaseTypes/BaseTypes.h>
#include <QQuickFramebufferObject>

#include <atomic>
#include <Qt/QtModuleDef.h>

namespace DGE
{
	namespace Core
	{
		namespace System
		{
			class Context;
		}
	} // namespace Core

	namespace GFX
	{
		namespace GCore
		{
			namespace QtWrapper
			{
				class QtRenderer;
			}
		} // namespace GCore
	}	  // namespace GFX
	namespace QtWrapper
	{
		class ViewPort3D;
		/// \brief The renderer for QML object that is used by ViewPort3D
		class DGE_QtAPI QuickFBRenderer : public QQuickFramebufferObject::Renderer
		{
		public:
			QuickFBRenderer(bool isFullscreen);
			~QuickFBRenderer();

			bool requestUpdate();

			void synchronize(QQuickFramebufferObject *item) Q_DECL_OVERRIDE;

			void render() Q_DECL_OVERRIDE;

			QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE;

		private:
			QQuickWindow							   *m_window;
			bool										m_isFullscreen;
			bool										m_isChangedRenderType;
			std::shared_ptr<GFX::GCore::QtWrapper::QtRenderer> m_renderer;
			std::shared_ptr<Core::System::Context>		m_context;
			std::shared_ptr<ViewPort3D>					m_viewport3D;
			QOpenGLFramebufferObject				   *m_lastFramebufferObject;
			std::atomic_bool							m_isValid;
		};

	} // namespace QtWrapper
} // namespace DGE
