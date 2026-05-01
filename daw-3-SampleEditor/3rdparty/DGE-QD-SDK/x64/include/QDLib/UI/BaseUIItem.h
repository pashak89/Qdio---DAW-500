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
// #include <Core/BaseTypes/BaseTypes.h>
#include <memory>
#include <QDCore/QDConfig.h>

class QQuickItem;

class QD_API BaseUIItem : public QObject
{
	Q_OBJECT
public:
	explicit BaseUIItem(QObject* parent = nullptr);
	explicit BaseUIItem(std::shared_ptr<QQuickItem> rootItem, QObject* parent = nullptr);

	~BaseUIItem();

	std::shared_ptr<QQuickItem> rootItem() const;
	void						setRootItem(std::shared_ptr<QQuickItem> rootItem);
	virtual void				startLoading();
	virtual void				stopLoading();

	std::shared_ptr<QQuickItem> getQMLItem(QString objectName);

signals:

protected:
	std::shared_ptr<QQuickItem> m_rootItem;
};
