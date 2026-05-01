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

#include <Object3D/MovableModel.h>
#include <QQuickItem>
#include <QtCore/QObject>

class ObjectsSettings : public BaseUIItem
{
	Q_OBJECT
public:
	ObjectsSettings(std::shared_ptr<QQuickItem> rootItem, QObject *parent = nullptr);

	std::shared_ptr<MovableModel> addMovableModel(QString meshSourcePath, bool isVisible = true);

public slots:
	void onObjectFileSelected(QString fileName);

signals:
	void model3dListUpdated(const QList<QObject *> model);

private:
	QList<QObject *> m_movableModels;
};
