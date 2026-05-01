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

#include <QtCore/QObject>

#include <QElapsedTimer>

class CustomTimer : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int elapsed MEMBER m_elapsed NOTIFY elapsedChanged)
	Q_PROPERTY(bool running MEMBER m_running NOTIFY runningChanged)

public:
	CustomTimer()
	{
		this->m_elapsed = 0;
		this->m_running = true;
		m_timer.start();
		emit runningChanged();
	}

public slots:
	void start()
	{
		this->m_elapsed = 0;
		this->m_running = true;

		m_timer.start();
		emit runningChanged();
	}

	void updateTimer()
	{
		this->m_elapsed = m_timer.elapsed();
		this->m_running = false;

		emit elapsedChanged();
		emit runningChanged();
	}

signals:
	void runningChanged();
	void elapsedChanged();

signals:

private:
	QElapsedTimer m_timer;
	int			  m_elapsed;
	bool		  m_running;
};
