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
#include <QQuickItem>
#include <QtCore/QObject>

class MovableModel;

class CustomScene3D : public BaseUIItem
{
	Q_OBJECT
public:
	explicit CustomScene3D(std::shared_ptr<QQuickItem> rootItem, QObject* parent = nullptr);

	void updateModel3dList(QList<QObject*> model);

	const std::shared_ptr<MovableModel>& head3D() const;

	const std::shared_ptr<MovableModel>& audioObject() const;

	const std::shared_ptr<MovableModel>& room() const;

	const std::shared_ptr<MovableModel>& carInterior() const;

	const std::shared_ptr<MovableModel>& sphere() const;

public slots:
	void onUpdateVisibilityHead3D(bool isVisible);
	void onUpdateVisibilityAudioObject(bool isVisible);
	void onUpdateVisibilityRoom(bool isVisible);
	void onUpdateVisibilityCarInteriory(bool isVisible);
	void onUpdateVisibilitySphere(bool isVisible);

	void setHead3D(const std::shared_ptr<MovableModel>& newHead3D);

	void setAudioObject(const std::shared_ptr<MovableModel>& newAudioObject);

	void setRoom(const std::shared_ptr<MovableModel>& newRoom);

	void setCarInterior(const std::shared_ptr<MovableModel>& newCarInterior);

	void setSphere(const std::shared_ptr<MovableModel>& newSphere);

private:
	std::shared_ptr<QQuickItem> m_mainNodeEntityItem;

	std::shared_ptr<MovableModel> m_head3D;
	std::shared_ptr<MovableModel> m_audioObject;
	std::shared_ptr<MovableModel> m_room;
	std::shared_ptr<MovableModel> m_carInterior;
	std::shared_ptr<MovableModel> m_sphere;
};
