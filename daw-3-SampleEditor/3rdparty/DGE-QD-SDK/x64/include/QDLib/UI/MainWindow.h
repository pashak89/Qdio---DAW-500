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
#include "BaseUIItem.h"
#include <Core/Config.h>

class QQuickItem;
class SettingsWindow;
class CustomScene3D;
class MovableModel;

class MainWindow : public BaseUIItem
{
	Q_OBJECT
public:
	explicit MainWindow(std::shared_ptr<QQuickItem> rootItem, QObject* parent = nullptr);

	const std::shared_ptr<MovableModel>& head3D() const;

	const std::shared_ptr<MovableModel>& audioObject() const;

	const std::shared_ptr<MovableModel>& room() const;

	const std::shared_ptr<MovableModel>& carInterior() const;

	const std::shared_ptr<MovableModel>& sphere() const;

private:
public slots:
	void onThemeChanged(int index);
	void onModel3dListUpdated(const QList<QObject*> model);

signals:

protected:
	std::shared_ptr<SettingsWindow> m_settingsWindow;
	std::shared_ptr<CustomScene3D>	m_customScene3D;
};
