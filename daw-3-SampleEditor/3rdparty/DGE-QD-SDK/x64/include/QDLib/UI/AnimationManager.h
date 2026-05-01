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

#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtQml/QQmlEngine>
#else
#endif
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QtCore/QObject>
#include <QtGui>

#include <Core/MemoryManager/RedefMemoryMacros.h>
#include <Qt/QEngineDef.h>

#include <UI/TimelineEditor/AnimationTrackItem.h>
#include <UI/TimelineEditor/KeyFrameItem.h>

#include <Scene/Components/AnimationComponent.h>
#include <Scene/Components/AnimationGroup.h>
#include <Scene/DEntity.h>

#include <Core/MemoryManager/MemoryManager.h>
#include <Core/Utility/Singleton/Singleton.h>

Q_DECLARE_METATYPE(QList<AnimationTrackItem *>)
Q_DECLARE_METATYPE(QList<KeyFrameItem *>)

class QD_API AnimationManager
	: public BaseUIItem
	, public DGE::Core::System::DObject
	, public DGE::Core::Utility::Singleton<AnimationManager, std::shared_ptr<AnimationManager>,
										   DGE::Core::Utility::Deleter<AnimationManager>::emptyDeleter>
{
	Q_OBJECT
	QML_ELEMENT

	D_SINGLETON(AnimationManager, std::shared_ptr<AnimationManager>,
				DGE::Core::Utility::Deleter<AnimationManager>::emptyDeleter)
	Q_PROPERTY(QList<AnimationTrackItem *> trackItemsList READ trackItemsList WRITE
				   setTrackItemsList NOTIFY trackItemsListChanged FINAL)

	D_QT_PROPERTY_QPROPERTY(AnimationTrackItem *, lastSelectedItem, LastSelectedItem, nullptr)
	D_QT_PROPERTY_BINDING(qreal, float, speed, Speed, animationGroup, (float) )

	Q_PROPERTY(
		quint64 currentTime READ currentTime WRITE setCurrentTime NOTIFY currentTimeChanged FINAL)
	Q_PROPERTY(bool canAddTrack READ canAddTrack NOTIFY canAddTrackChanged FINAL)
	Q_PROPERTY(bool canAddKeyFrame READ canAddKeyFrame NOTIFY canAddKeyFrameChanged FINAL)
	Q_PROPERTY(bool canRemoveKeyFrame READ canRemoveKeyFrame NOTIFY canRemoveKeyFrameChanged FINAL)
	Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged FINAL)
	Q_PROPERTY(bool canPlay READ canPlay NOTIFY canPlayChanged FINAL)
	Q_PROPERTY(bool canStop READ canStop NOTIFY canStopChanged FINAL)

	explicit AnimationManager(QObject *parent = nullptr);

public:
	D_Destructor(AnimationManager);

	QList<AnimationTrackItem *> trackItemsList() const;

	Q_INVOKABLE void updateFlags();
	void			 updateCanAddTrack();
	void			 updateCanAddKeyFrame();
	void			 updateCanRemoveKeyFrame();
	void			 updateIsPlaying();
	void			 updateCanPlay();
	void			 updateCanStop();

	/// \brief Added new track
	Q_INVOKABLE void addTrack();
	Q_INVOKABLE void addTrack(bool mustBeSelected);
	/// \brief Added new Key-frame to current selected track
	Q_INVOKABLE void addKeyFrame(quint64 time);
	/// \brief Remove selected key-frame
	Q_INVOKABLE void removeKeyFrame();

	Q_INVOKABLE void play();
	Q_INVOKABLE void reversePlay();
	Q_INVOKABLE void stop();
	Q_INVOKABLE void pause();

	Q_INVOKABLE void selectTrackItem(AnimationTrackItem *, bool applyToEntity = true);

	Q_INVOKABLE void moveTimeTo(quint64 frameTime);

	Q_INVOKABLE void setCurrentKeyFrameType(int keyFrameType);
	Q_INVOKABLE void setSelectedKeyFrame(KeyFrameItem *keyFrame);

	Q_INVOKABLE void undo();
	Q_INVOKABLE void redo();

	bool canAddTrack() const;

	bool isPlaying();

	bool canPlay();

	bool canStop();

	bool canAddKeyFrame();
	bool canRemoveKeyFrame() const;

	void onSelectedEntityChanged(DGE::Scene::DEntityRef selectedEntity);
	void onLastSelectedTrackItemChanged(AnimationTrackItem *);

	quint64 currentTime() const;

public slots:
	void setTrackItemsList(const QList<AnimationTrackItem *> &newTrackItemsList);

	void setCurrentTime(quint64 newCurrentTime);
	void onCurrentTimeChanged(quint64 time);
	void onAnimationTrackChanged();

private slots:
	void setCanAddTrack(bool newValue);
	void setCanAddKeyFrame(bool newValue);
	void setCanRemoveKeyFrame(bool newValue);
	void setIsPlaying(bool newValue);
	void setCanPlay(bool newValue);
	void setCanStop(bool newValue);

signals:

	void trackItemsListChanged(QList<AnimationTrackItem *> trackItemsList);

	void canAddTrackChanged();

	void isPlayingChanged();

	void canPlayChanged();

	void canStopChanged();

	void canAddKeyFrameChanged();

	void canRemoveKeyFrameChanged();

	void currentTimeChanged(quint64 currentTime);

protected:
	DGE::Scene::AnimationGroupRef						   m_animationGroup;
	QList<AnimationTrackItem *>							   m_trackItemsList;
	std::map<DGE::Scene::DEntityRef, AnimationTrackItem *> m_entityToTrackItemsMap;
	DGE::Scene::DEntityWRef								   m_selectedEntity;
	KeyFrameItem::TransitionType						   m_keyFrameType;
	KeyFrameItem										  *m_lastSelectedKeyFrame;
	bool												   m_canAddKeyFrame;
	bool												   m_canAddTrack;
	bool												   m_canRemoveKeyFrame;
	bool												   m_isPlaying;
	bool												   m_canPlay;
	bool												   m_canStop;
	float												   m_speed;
};
