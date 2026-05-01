#include "AnimationEditor.h"
#include <UI/TimelineEditor/AnimationTrackItem.h>
#include <UI/TimelineEditor/KeyFrameItem.h>

#include <GFX/Viewport.h>

AnimationEditor::AnimationEditor(QObject* parent)
    : QObject { parent }
{
    qRegisterMetaType<QList<AnimationTrackItem*>>();
    qRegisterMetaType<QList<KeyFrameItem*>>();


}

D_Destructor_Implementation(AnimationEditor) { }

void AnimationEditor::play()
{
    if (m_lastSelectedTrack != nullptr) {
        if (!m_lastSelectedTrack->isPlaying())
            m_lastSelectedTrack->play();
        else
            m_lastSelectedTrack->pause();
    }
}

void AnimationEditor::addTrack()
{
    if (!m_selectedEntity.expired()) {
        auto id = m_trackItemsList.size() + 1;
        auto entityRef = m_selectedEntity.lock();
        auto newTrack = new AnimationTrackItem(id,
            "Track " + QString(entityRef->name().toChar())
                + QString::number(id),
            this);
        auto animationComponent = entityRef->getComponent<DGE::Scene::AnimationComponent>();
        if (!animationComponent)
            animationComponent = entityRef->createComponent<DGE::Scene::AnimationComponent>();
        animationComponent->addProperty(entityRef->propertyPosition3D());

        m_entityToTrackItemsMap.insert({ entityRef, newTrack });
        newTrack->setAnimationController(animationComponent);
        newTrack->setEntity(m_selectedEntity);
        m_trackItemsList.append(newTrack);
        qDebug() << "Added new Track";
        m_lastSelectedTrack = newTrack;
    }
}

void AnimationEditor::addKeyFrame(quint64 time)
{

    if (m_lastSelectedTrack != nullptr) {
        if (!m_lastSelectedTrack->animationController() && !m_selectedEntity.expired()) {
            auto entityRef = m_selectedEntity.lock();
            auto animationComponent = entityRef->getComponent<DGE::Scene::AnimationComponent>();
            animationComponent->addProperty(entityRef->propertyPosition3D());
            m_lastSelectedTrack->setAnimationController(animationComponent);
        }
        m_lastSelectedTrack->addKeyFrame(time, KeyFrameItem::TransitionType::CubicInOut);
        emit modelChanged(m_lastSelectedTrack->keyframeItemsList());
    }
}

void AnimationEditor::onSelectedEntityChanged(DGE::Scene::DEntityRef selectedEntity)
{
    m_selectedEntity = selectedEntity;
    bool canAddTrack = false;
    if (selectedEntity) {
        canAddTrack = !selectedEntity->hasComponentType<DGE::Scene::AnimationComponent>();
        if (!canAddTrack) {
            auto it = m_entityToTrackItemsMap.find(selectedEntity);
            if (it == m_entityToTrackItemsMap.end()) {
                /// \note Call addTrack() with invokeMethod to manage thread synchronizing
                QMetaObject::invokeMethod(this, "addTrack", Qt::QueuedConnection, Q_ARG(bool, true));
            } else {
                if (it->second != nullptr && !it->second->isSelected()) {
                    m_lastSelectedTrack = it->second;
                }
            }
        }
    }
}
