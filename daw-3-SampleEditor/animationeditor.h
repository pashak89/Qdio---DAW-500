#pragma once

#include <UI/TimelineEditor/AnimationTrackItem.h>

#include <Scene/DEntity.h>

class AnimationEditor
    : public QObject,
      public DGE::Core::Utility::Singleton<AnimationEditor, std::shared_ptr<AnimationEditor>,
          DGE::Core::Utility::Deleter<AnimationEditor>::emptyDeleter> {
    Q_OBJECT
    D_SINGLETON(AnimationEditor, std::shared_ptr<AnimationEditor>,
        DGE::Core::Utility::Deleter<AnimationEditor>::emptyDeleter)
    explicit AnimationEditor(QObject* parent = nullptr);

public:
    D_Destructor(AnimationEditor);

    Q_INVOKABLE void play();

    /// \brief Added new track
    Q_INVOKABLE void addTrack();

    /// \brief Added key-frame into last created track
    Q_INVOKABLE void addKeyFrame(quint64 time);

protected:
    void onSelectedEntityChanged(DGE::Scene::DEntityRef selectedEntity);

signals:
    void modelChanged(QList<KeyFrameItem*> model);

protected:
    /// \brief Current selected entity in the scene
    DGE::Scene::DEntityWRef m_selectedEntity;

    /// \brief List of the entity that has AnimationComponent in the scene.
    std::map<DGE::Scene::DEntityRef, AnimationTrackItem*> m_entityToTrackItemsMap;

    /// \brief List of created track item.
    QList<AnimationTrackItem*> m_trackItemsList;
    AnimationTrackItem* m_lastSelectedTrack = nullptr;
};
