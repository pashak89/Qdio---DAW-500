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

#include <UI/BaseUIItem.h>

#include <Core/MemoryManager/UndefMemoryMacros.h>

#include <QPointF>
#include <QVariant>
#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtQml/QQmlEngine>
#else
#endif
#include <Core/MemoryManager/RedefMemoryMacros.h>

#include <Core/Animation/BaseKeyframe.h>
#include <Core/Math/Vector3D.h>
#include <Core/MemoryManager/MemoryManager.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <Scene/PathController.h>

class AnimationTrackItem;

class QD_API KeyFrameItem : public QObject
{
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(quint64 time READ time WRITE setTime NOTIFY timeChanged FINAL)
	Q_PROPERTY(bool isEnabled READ isEnabled WRITE setIsEnabled NOTIFY isEnabledChanged FINAL)
	Q_PROPERTY(bool isSelected READ isSelected WRITE setIsSelected NOTIFY isSelectedChanged FINAL)
	Q_PROPERTY(TransitionType transitionType READ transitionType WRITE setTransitionType NOTIFY
				   transitionTypeChanged FINAL)
	Q_PROPERTY(quint64 id READ id WRITE setId NOTIFY idChanged FINAL)
public:
	enum TransitionType
	{
		Linear = (uint8) DGE::Core::Animation::TransitionType::Linear,
		Stepped,
		QuadraticIn,
		QuadraticOut,
		QuadraticInOut,
		CubicIn,
		CubicOut,
		CubicInOut,
		QuarticIn,
		QuarticOut,
		QuarticInOut,
		QuinticIn,
		QuinticOut,
		QuinticInOut,
		SinusoidalIn,
		SinusoidalOut,
		SinusoidalInOut,
		ExponentialIn,
		ExponentialOut,
		ExponentialInOut,
		CircularIn,
		CircularOut,
		CircularInOut,
		BounceIn,
		BounceOut,
		BounceInOut,
		ElasticIn,
		ElasticOut,
		ElasticInOut,
		BackIn,
		BackOut,
		BackInOut = (uint8) DGE::Core::Animation::TransitionType::BackInOut,
	};
	Q_ENUM(TransitionType);
	explicit KeyFrameItem(quint64 time = 0, QObject* parent = nullptr);

	bool isEnabled() const;

	quint64 time() const;

	TransitionType transitionType() const;
	void		   setTransitionType(const DGE::Core::Animation::TransitionType& newTransitionType);

	DGE::Core::Math::Vector3D value() const;

	quint64 id() const;

	DGE::Core::Math::Vector3D beginTangent() const;

	DGE::Core::Math::Vector3D endTangent() const;

	DGE::Scene::PathControllerRef beginPathController() const;

	DGE::Scene::PathControllerRef endPathController() const;

	bool isSelected() const;

	AnimationTrackItem* trackParent() const;

public slots:

	void setIsEnabled(bool newIsEnabled);

	void setTime(quint64 newTime);

	void setTransitionType(const TransitionType& newTransitionType);

	void setValue(const DGE::Core::Math::Vector3D& newValue);

	void setId(quint64 newId);

	// void setBeginTangent(const DGE::Core::Math::Vector3D& newBeginTangent);

	// void setEndTangent(const DGE::Core::Math::Vector3D& newEndTangent);

	void setBeginPathController(const DGE::Scene::PathControllerRef& newPathController);

	void setEndPathController(const DGE::Scene::PathControllerRef& newEndPathController);

	void setIsSelected(bool newIsSelected);

	void setTrackParent(AnimationTrackItem* newTrackParent);

signals:

	void isEnabledChanged(bool isEnabled);

	void timeChanged(quint64 time);

	void transitionTypeChanged(TransitionType transitionType);

	void idChanged(quint64 id);

	void isSelectedChanged(bool isSelected);

protected:
	quint64						  m_time;
	bool						  m_isEnabled;
	bool						  m_isSelected;
	TransitionType				  m_transitionType;
	DGE::Core::Math::Vector3D	  m_value;
	quint64						  m_id;
	DGE::Scene::PathControllerRef m_beginPathController;
	DGE::Scene::PathControllerRef m_endPathController;
	AnimationTrackItem*			  m_trackParent;

private:
};
