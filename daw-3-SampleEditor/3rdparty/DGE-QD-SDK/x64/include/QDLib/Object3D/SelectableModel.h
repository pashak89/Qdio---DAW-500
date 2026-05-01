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

#include <QColor>
#include <QVector3D>
#include <QtCore/QObject>

class SelectableModel : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString sourceMesh READ sourceMesh WRITE setSourceMesh NOTIFY sourceMeshChanged)
	Q_PROPERTY(QVector3D position READ position WRITE setPosition RESET resetPosition NOTIFY
				   positionChanged)
	Q_PROPERTY(QVector3D scale READ scale WRITE setScale RESET resetScale NOTIFY scaleChanged)
	Q_PROPERTY(float equalScale READ equalScale WRITE setEqualScale RESET resetEqualScale NOTIFY
				   equalScaleChanged)
	Q_PROPERTY(QColor defaultColor READ defaultColor WRITE setDefaultColor RESET resetDefaultColor
				   NOTIFY defaultColorChanged)
	Q_PROPERTY(
		bool isPicked READ isPicked WRITE setIsPicked RESET resetIsPicked NOTIFY isPickedChanged)
	Q_PROPERTY(bool isSelectedInTable READ isSelectedInTable WRITE setIsSelectedInTable RESET
				   resetIsSelectedInTable NOTIFY isSelectedInTableChanged)
	Q_PROPERTY(bool isVisible READ isVisible WRITE setIsVisible RESET resetIsVisible NOTIFY
				   isVisibleChanged)
	Q_PROPERTY(float opacity READ opacity WRITE setOpacity RESET resetOpacity NOTIFY opacityChanged)
	Q_PROPERTY(QVector3D eulerRotation READ eulerRotation WRITE setEulerRotation RESET
				   resetEulerRotation NOTIFY eulerRotationChanged)
public:
	explicit SelectableModel(QObject* parent = nullptr);

	const QString&	 name() const;
	const QString&	 sourceMesh() const;
	const QVector3D& position() const;
	const QVector3D& scale() const;
	const QColor&	 defaultColor() const;
	bool			 isPicked() const;
	bool			 isSelectedInTable() const;
	float			 equalScale() const;

	bool isVisible() const;

	float opacity() const;

	const QVector3D& eulerRotation() const;

public slots:

	void setName(const QString& newName);

	void setSourceMesh(const QString& newSourceMesh);

	void setPosition(const QVector3D& newPosition);
	void resetPosition();

	void setScale(const QVector3D& newScale);
	void resetScale();

	void setDefaultColor(const QColor& newDefaultColor);
	void resetDefaultColor();

	void setIsPicked(bool newIsPicked);
	void resetIsPicked();

	void setIsSelectedInTable(bool newIsSelectedInTable);
	void resetIsSelectedInTable();

	void setEqualScale(float newEqualScale);
	void resetEqualScale();

	void setIsVisible(bool newIsVisible);
	void resetIsVisible();

	void setOpacity(float newOpacity);
	void resetOpacity();

	void setEulerRotation(const QVector3D& newEulerRotation);
	void resetEulerRotation();

signals:
	void nameChanged(const QString& name);

	void sourceMeshChanged(const QString& sourceMesh);

	void positionChanged(const QVector3D& position);

	void scaleChanged(const QVector3D& scale);

	void defaultColorChanged(const QColor& defaultColor);

	void isPickedChanged(bool isPicked);

	void isSelectedInTableChanged(bool isSelectedInTable);

	void equalScaleChanged(float equalScale);

	void isVisibleChanged(bool isVisible);

	void opacityChanged(float opacity);

	void eulerRotationChanged(const QVector3D& eulerRotation);

private:
	QString	  m_name			  = "";
	QString	  m_sourceMesh		  = "";
	QVector3D m_position		  = QVector3D(0, 0, 0);
	QVector3D m_scale			  = QVector3D(1, 1, 1);
	float	  m_equalScale		  = 1;
	QColor	  m_defaultColor	  = "#ff999999";
	bool	  m_isPicked		  = false;
	bool	  m_isSelectedInTable = false;
	bool	  m_isVisible		  = true;
	float	  m_opacity			  = 1.0;
	QVector3D m_eulerRotation	  = QVector3D(0, 0, 0);
};
