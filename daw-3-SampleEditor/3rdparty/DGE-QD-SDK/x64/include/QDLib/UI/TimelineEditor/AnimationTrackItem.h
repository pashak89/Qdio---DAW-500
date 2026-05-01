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

#include <Qt/QEngineDef.h>

#include <Core/MemoryManager/UndefMemoryMacros.h>

#include <UI/BaseUIItem.h>
#include <UI/TimelineEditor/KeyFrameItem.h>
#include <optional>
#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtQml/QQmlEngine>
#else
#endif
#include <Core/MemoryManager/RedefMemoryMacros.h>

#include <Core/MemoryManager/MemoryManager.h>
#include <Core/System/DObject.h>
#include <Core/Utility/Singleton/Singleton.h>
#include <Scene/Components/AnimationComponent.h>
#include <Scene/DEntity.h>
#include <Scene/SceneHelper.h>

class QD_API AnimationTrackItem
	: public QObject
	, public DGE::Core::System::DObject
{
	Q_OBJECT
	QML_ELEMENT

	D_OBJECT(AnimationTrackItem)

	D_QT_PROPERTY(DGE::Scene::AnimationComponentRef, animationController, AnimationController)
	D_QT_READONLY_PROPERTY_BINDING(quint64, uint64, currentTime, CurrentTime, animationController,
								   (uint64))
	D_QT_PROPERTY_BINDING(qreal, float, speed, Speed, animationController, (float) )

	Q_PROPERTY(QList<KeyFrameItem *> keyframeItemsList READ keyframeItemsList WRITE
				   setKeyframeItemsList NOTIFY keyframeItemsListChanged FINAL)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
	Q_PROPERTY(bool isSelected READ isSelected WRITE setIsSelected NOTIFY isSelectedChanged FINAL)
	Q_PROPERTY(quint64 id READ id NOTIFY idChanged FINAL)
public:
	explicit AnimationTrackItem(QObject *parent = nullptr);
	explicit AnimationTrackItem(quint64 id, QString name, QObject *parent = nullptr);

	QList<KeyFrameItem *> keyframeItemsList() const;

	QString name() const;

	bool isSelected() const;

	quint64 id() const;
	void	setId(quint64 newId);

	Q_INVOKABLE bool addKeyFrame(quint64 time);
	Q_INVOKABLE bool addKeyFrame(quint64 time, KeyFrameItem::TransitionType transitionType);
	Q_INVOKABLE bool addKeyFrame(uint64								  time,
								 DGE::Core::Math::Vector3D			  position,
								 DGE::Core::Animation::TransitionType transition);
	Q_INVOKABLE bool addKeyFrame(uint64 time, DGE::Core::Math::Vector3D position,
								 DGE::Core::Animation::TransitionType transition,
								 DGE::Core::Math::Vector3D			  beginControlValue,
								 DGE::Core::Math::Vector3D			  endControlValue);

	/// \brief Remove the key frame at specified time if existed
	Q_INVOKABLE void removeKeyFrame(quint64 time);

	/// \brief Remove all key frames
	Q_INVOKABLE void removeAllKeyFrames();

	/// \brief Export the animation to the a specified file
	Q_INVOKABLE void exportAnimation(QString fullPath);

	/// \brief Import the animation from the a specified file
	Q_INVOKABLE void importAnimation(QString fullPath);

	/// \brief Automatically save the animation.
	Q_INVOKABLE void autoSaveAnimation();

	/// \brief Automatically load the animation.
	Q_INVOKABLE void autoLoadAnimation();

	/// \brief Play animation of this track item.
	void play();

	/// \brief Play animation of this track item in reverse direction.
	void reversePlay();

	/// \brief Stop playing animation of this track item.
	void stop();

	/// \brief Pause playing animation of this track item. When call play() function, playing
	/// animation continues.
	void pause();

	DGE::Scene::DEntityWRef entity() const;

protected:
	/// \brief return value of previous key-frame at specified time
	std::optional<DGE::Scene::KeyFrameVector3D> getMappedKeyFrame(uint64 time);
	DGE::Core::Math::Vector3D					previousKeyFrameValue(uint64 time);
	DGE::Scene::PathControllerRef				createPathController(
					  DGE::Core::Math::Vector3D begin, DGE::Core::Math::Vector3D end,
					  DGE::Scene::KeyEntity beginKeyEntity, DGE::Scene::KeyEntity endKeyEntity,
					  DGE::Core::Animation::TransitionType	 transition,
					  DGE::Core::Animation::TransformerRef<> transformer,
					  DGE::Core::Math::Vector3D beginControlValue, DGE::Core::Math::Vector3D endControlValue);

	void swapKeys(uint64 key1, uint64 key2);
	/// \brief Update an ID (=time) of key-frame in the lists
	void updateKeyInLists(uint64 oldID, uint64 newID);
	/// \brief Update animation controller with resetting time in the animation controller object to
	/// specified time value.
	void updateAnimation(uint64 time = 0, std::optional<uint64> newTime = {});

public slots:
	void setKeyframeItemsList(const QList<KeyFrameItem *> &newKeyframeItemsList);

	void setName(const QString &newName);

	void setIsSelected(bool newIsSelected);

	void setEntity(const DGE::Scene::DEntityWRef &newEntity);

	bool isPlayable();
	bool isPlaying();
	bool canStop();
	bool containsKeyFrame(quint64 frameTime);
	bool isValidTime(quint64 frameTime);
	bool setFrameTime(quint64 frameTime);

	void onTimeOfKeyFrameChanged(quint64);

signals:

	void keyframeItemsListChanged(QList<KeyFrameItem *> keyframeItemsList);

	void nameChanged(QString name);

	void isSelectedChanged(bool isSelected);

	void idChanged(quint64 id);

	void animationChanged();

protected:
	static DGE::Scene::ShaderComponentRef m_mainShader;

protected:
	quint64										 m_id;
	QList<KeyFrameItem *>						 m_keyframeItemsList;
	std::map<uint64, KeyFrameItem *>			 m_keyFrameTimesMap;
	unordered_map<uint64, DGE::Scene::KeyEntity> m_keyEntityList;
	QString										 m_name;

	bool m_isSelected;

	DGE::Scene::DEntityWRef				  m_entity;
	DGE::GFX::Resource::ShaderResourceRef m_shaderResource;

	int64 m_lastKeyFrameTime = 0;
	int64 m_lastKeyFrameID	 = 0;

	DGE::Scene::KeyEntity m_previousKeyEntity;
	KeyFrameItem		 *m_previousKeyFrame;

	DByteArray m_lastAutoSaveData;
};
