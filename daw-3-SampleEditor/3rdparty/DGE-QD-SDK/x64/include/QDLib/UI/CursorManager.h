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

#include "UI/BaseUIItem.h"

#include <QCursor>
#include <QDebug>
#include <QKeyEvent>
#include <QQuickItem>
#include <QScreen>
#include <QtCore/QObject>

class QD_API CursorManager : public QObject
{
	Q_OBJECT

public:
	explicit CursorManager(QObject *parent = nullptr);

	bool isDebugCursor() const;
	void setIsDebugCursor(bool newIsDebugCursor);

public slots:
	void saveCursorPos(float x, float y);
	void saveCursorPos();
	void moveCursor(int x, int y);
	void hideCursor();
	void showCursor();

	void onlyShowCursor();

protected:
signals:
	void deleteKeyPress();

	void isDebugCursorChanged();

private:
	bool	m_isDebugCursor;
	QCursor cursor;

	float mousePosX;
	float mousePosY;
	Q_PROPERTY(bool isDebugCursor READ isDebugCursor WRITE setIsDebugCursor NOTIFY
				   isDebugCursorChanged FINAL)
};
