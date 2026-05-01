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

#include <QDCore/QDConfig.h>
#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include "private/qqmlnullablevalue_p.h"
#include <QBasicTimer>
#include <QQuickItem>

class QQuickMouseEvent;
#else
#endif

class QD_API HighResMouseArea : public QQuickItem
{
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(double mouseX READ mouseX NOTIFY mouseXChanged)
	Q_PROPERTY(double mouseY READ mouseY NOTIFY mouseYChanged)
	Q_PROPERTY(bool hovered READ hovered NOTIFY hoveredChanged)
	Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
	Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
	Q_PROPERTY(Qt::MouseButtons pressedButtons READ pressedButtons NOTIFY pressedButtonsChanged)
	Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons NOTIFY
				   acceptedButtonsChanged)
	Q_PROPERTY(bool containsPress READ containsPress NOTIFY containsPressChanged)
	Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
	Q_PROPERTY(int pressAndHoldInterval READ pressAndHoldInterval WRITE setPressAndHoldInterval
				   NOTIFY pressAndHoldIntervalChanged)
	Q_PROPERTY(bool isClippedMouse READ isClippedMouse WRITE setIsClippedMouse NOTIFY
				   isClippedMouseChanged FINAL)

	QML_NAMED_ELEMENT(HighResMouseArea)

public:
	enum PropagateType
	{
		Click,
		DoubleClick,
		PressAndHold
	};
	HighResMouseArea(QQuickItem* parent = nullptr);
	bool pressed() const;

	int	 pressAndHoldInterval() const;
	void setPressAndHoldInterval(int newPressAndHoldInterval);

	bool isClippedMouse() const;
	void setIsClippedMouse(bool newIsClippedMouse);

public slots:
	double mouseX() const;
	double mouseY() const;

	bool hovered() const;

	bool enabled() const;
	void setEnabled(bool newEnabled);

	Qt::MouseButtons pressedButtons() const;

	Qt::MouseButtons acceptedButtons() const;
	void			 setAcceptedButtons(const Qt::MouseButtons& newAcceptedButtons);

	bool hoverEnabled() const;
	void setHoverEnabled(bool newHoverEnabled);

	bool containsPress() const;

protected:
	void setHovered(bool isHovered);
	bool setPressed(Qt::MouseButton button, bool p, Qt::MouseEventSource source);
	void propagate(QQuickMouseEvent* event, PropagateType);
	void saveEvent(QMouseEvent* event);

	bool isClickConnected();
	bool isPressAndHoldConnected();

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
	// virtual void wheelEvent(QWheelEvent* event) override;
	virtual void touchEvent(QTouchEvent* event) override;

signals:
	void mouseXChanged(QQuickMouseEvent* mouse);
	void mouseYChanged(QQuickMouseEvent* mouse);
	void hoveredChanged();
	void pressedChanged();
	void enabledChanged();
	void pressedButtonsChanged();
	void acceptedButtonsChanged();
	void hoverEnabledChanged();

	/// \brief Emit this signal when a mouse moved with delta x and y
	void mouseMoved(double dX, double dY);

	void pressed(QQuickMouseEvent* mouse);
	void pressAndHold(QQuickMouseEvent* mouse);
	void released(QQuickMouseEvent* mouse);
	void clicked(QQuickMouseEvent* mouse);
	void doubleClicked(QQuickMouseEvent* mouse);
	// void wheel(QQuickWheelEvent* wheel);
	void entered();
	void exited();
	void canceled();

	void containsPressChanged();

	void pressAndHoldIntervalChanged();

	void isClippedMouseChanged();

protected:
	int				 m_pressAndHoldInterval;
	bool			 m_enabled;
	Qt::MouseButtons m_pressed;
	Qt::MouseButtons m_acceptedButtons;
	bool			 m_hoverEnabled;
	bool			 m_containsPress;
	/// \brief Indicate whether limit mouse in geometry of this object
	bool m_isClippedMouse;

	bool m_hovered : 1;
	bool m_longPress : 1;
	bool m_moved : 1;
	bool m_stealMouse : 1;
	bool m_doubleClick : 1;
	bool m_preventStealing : 1;
	bool m_propagateComposedEvents : 1;

	QPointF					   m_startScene;
	QPointF					   m_targetStartPos;
	QPointF					   m_lastPos;
	QQmlNullableValue<QPointF> m_lastScenePos;
	Qt::MouseButton			   m_lastButton;
	Qt::MouseButtons		   m_lastButtons;
	Qt::KeyboardModifiers	   m_lastModifiers;
	QBasicTimer				   m_pressAndHoldTimer;
	Qt::MouseEventFlags		   m_lastFlags;
};
