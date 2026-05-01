#include "effectchain.h"
#include "Effect.h"
#include "EffectChain.h"
#include "MixHelpers.h"
#include "audioengine.h"
#include "dummyeffect.h"
#include "engine.h"
#include "mixhelpers.h"
#include "song.h"
#include <eq/multieq.h>
QMutex mutex_1;
EffectChain::EffectChain(int trackIndex, QObject* _parent)
    : QAbstractListModel(_parent)
    , m_enabledModel(false)
    , m_trackIndex(trackIndex)
{

    m_roleNames[ROLE_Index] = "index";
    m_roleNames[ROLE_Name] = "name";
    m_roleNames[ROLE_Type] = "type";
    m_roleNames[ROLE_Enabled] = "enabled";
    m_roleNames[ROLE_TrackIndex] = "TrackIndex";
    m_roleNames[ROLE_ControlVisible] = "controlVisible";
}

EffectChain::~EffectChain()
{
    qDebug() << "~EffectChain";
    clear();
}

int EffectChain::appendEffect(QSharedPointer<Effect> _effect)
{

    beginInsertRows(QModelIndex(), m_effects.size(), m_effects.size());

    AudioManager::audioEngine()->requestChangeInModel();
    m_effects.push_back(_effect);

    effect_names.insert(_effect->name(), m_effects.size() - 1);
    AudioManager::audioEngine()->doneChangeInModel();

    m_enabledModel = true;

    endInsertRows();

    Q_EMIT countChanged();
    return m_effects.size() - 1;
}

void EffectChain::removeEffect(int index)
{

    if (index >= 0 && index < m_effects.size()) {
        beginRemoveRows(QModelIndex(), index, index);

        AudioManager::audioEngine()->requestChangeInModel();

        effect_names.remove(m_effects[index]->name());

        QMap<QString, int>::iterator it = effect_names.begin();
        while (it != effect_names.end()) {
            if (it.value() > index) {
                // Decrease the index by one if it is greater than the removed row
                it.value() = it.value() - 1;
            }
            it++;
        }

        m_effects[index].reset();
        m_effects.remove(index);

        AudioManager::audioEngine()->doneChangeInModel();

        if (m_effects.isEmpty()) {
            m_enabledModel = false;
        }

        Q_EMIT countChanged();

        endRemoveRows();
    }
}

bool EffectChain::processAudioBuffer(sampleFrame* _buf, const fpp_t _frames, bool hasInputNoise)
{

    if (m_enabledModel == false) {
        return false;
    }

    bool moreEffects = false;
    QMutexLocker locker(&mutex_1);
    for (int i = 0; i < m_effects.size(); i++) {
        if ((hasInputNoise || m_effects[i]->isRunning())) {

            if (m_effects[i]->isEnabled()) {
                moreEffects |= m_effects[i]->processAudioBuffer(_buf);
                // MixHelpers::sanitize(_buf, _frames);
            }
        }
    }

    return moreEffects;
}

void EffectChain::startRunning()
{
    if (m_enabledModel == false) {
        return;
    }

    for (const auto& effect : m_effects) {
        effect->startRunning();
    }
}

void EffectChain::clear()
{
    emit aboutToClear();

    AudioManager::audioEngine()->requestChangeInModel();

    qDebug() << "CLOSED EffectChain";
    while (m_effects.count()) {
        m_effects.takeAt(0);
    }

    AudioManager::audioEngine()->doneChangeInModel();

    m_enabledModel = false;
}

int EffectChain::size()
{
    return m_effects.size();
}

QSharedPointer<Effect> EffectChain::effect(int index)
{
    QMutexLocker locker(&mutex_1);
    if (index >= 0 && index < m_effects.size()) {
        return m_effects[index];
    }
    return nullptr;
}

Effect* EffectChain::getEffect(QString name)
{
    QMutexLocker locker(&mutex_1);

    if (effect_names.contains(name)) {

        Effect* item = m_effects[effect_names[name]].data();
        QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        return item;
    }
    return nullptr;
}

void EffectChain::up(int index)
{
    QMutexLocker locker(&mutex_1);
    if (index >= 1 && index < m_effects.size()) {

        // AudioManager::audioEngine()->requestChangeInModel();
        auto name = m_effects[index]->name();
        auto pre_name = m_effects[index - 1]->name();
        m_effects.move(index, index - 1);

        effect_names[name] = index - 1;
        effect_names[pre_name] = index;

        Q_EMIT dataChanged(QAbstractListModel::index(index - 1, 0), QAbstractListModel::index(index, 0), QVector<int>() << ROLE_Index);
        Q_EMIT dataChanged(QAbstractListModel::index(index - 1, 0), QAbstractListModel::index(index, 0), QVector<int>() << ROLE_Name);
        Q_EMIT dataChanged(QAbstractListModel::index(index - 1, 0), QAbstractListModel::index(index, 0), QVector<int>() << ROLE_Enabled);
        Q_EMIT dataChanged(QAbstractListModel::index(index - 1, 0), QAbstractListModel::index(index, 0), QVector<int>() << ROLE_TrackIndex);
        Q_EMIT dataChanged(QAbstractListModel::index(index - 1, 0), QAbstractListModel::index(index, 0), QVector<int>() << ROLE_ControlVisible);
        // AudioManager::audioEngine()->doneChangeInModel();
    }
}

void EffectChain::down(int index)
{
    QMutexLocker locker(&mutex_1);
    if (index >= 0 && index < m_effects.size() - 1) {

        // AudioManager::audioEngine()->requestChangeInModel();
        auto name = m_effects[index]->name();
        auto next_name = m_effects[index + 1]->name();
        m_effects.move(index, index + 1);

        effect_names[name] = index + 1;
        effect_names[next_name] = index;

        Q_EMIT dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index + 1, 0), QVector<int>() << ROLE_Index);
        Q_EMIT dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index + 1, 0), QVector<int>() << ROLE_Name);
        Q_EMIT dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index + 1, 0), QVector<int>() << ROLE_Enabled);
        Q_EMIT dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index + 1, 0), QVector<int>() << ROLE_TrackIndex);
        Q_EMIT dataChanged(QAbstractListModel::index(index, 0), QAbstractListModel::index(index + 1, 0), QVector<int>() << ROLE_ControlVisible);

        // AudioManager::audioEngine()->doneChangeInModel();
    }
}

bool EffectChain::isAutomated(QString menu, QString submenu)
{

    QMutexLocker locker(&mutex_1);
    if (effect_names.contains(menu)) {
        int index = effect_names[menu];
        if (m_effects[index]->automation(submenu))
            return m_effects[index]->automation(submenu)->isAutomated();
    }
    return false;
}

bool EffectChain::isAutomationEnabled(QString menu, QString submenu)
{
    QMutexLocker locker(&mutex_1);
    if (effect_names.contains(menu)) {
        int index = effect_names[menu];
        if (m_effects[index]->automation(submenu))
            return m_effects[index]->automation(submenu)->enabled();
    }
    return false;
}

bool EffectChain::isEnabled(QString menu)
{
    QMutexLocker locker(&mutex_1);
    if (effect_names.contains(menu)) {
        int index_ = effect_names[menu];
        return m_effects[index_]->isEnabled();
    }
    return false;
}

void EffectChain::enabledChanged(QString menu)
{
    QMutexLocker locker(&mutex_1);
    if (effect_names.contains(menu)) {
        int index_ = effect_names[menu];

        Q_EMIT dataChanged(index(index_, 0), index(index_, 0), QVector<int>() << ROLE_Enabled);
    }
}

void EffectChain::controlVisibleChanged(QString menu)
{
    QMutexLocker locker(&mutex_1);
    if (effect_names.contains(menu)) {
        int index_ = effect_names[menu];
        Q_EMIT dataChanged(index(index_, 0), index(index_, 0), QVector<int>() << ROLE_ControlVisible);
    }
}

QMap<QString, int> EffectChain::getEffectList()
{
    return effect_names;
}

QVector<QSharedPointer<Effect>> EffectChain::effects() const
{
    return m_effects;
}

int EffectChain::rowCount(const QModelIndex& parent) const
{
    return m_effects.size();
}

QVariant EffectChain::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (m_effects.size() == 0 || index.row() < 0)
        return QVariant();

    auto value = m_effects[index.row()];

    switch (role) {
    case ROLE_Name:
        return value->name();
    case ROLE_Type:
        return value->type();
    case ROLE_Index:
        return index.row();
    case ROLE_Enabled: {
        // qDebug() << "IENALED" << value->isEnabled();
        return value->isEnabled();
    }
    case ROLE_TrackIndex:
        return m_trackIndex;
    case ROLE_ControlVisible:
        return value->controlVisible();
    }

    return QVariant();
}
bool EffectChain::setData(const QModelIndex& index, const QVariant& value, int role)
{

    if (m_effects.size() == 0 || index.row() < 0 || index.row() > m_effects.size())
        return false;

    if (index.isValid() && role == ROLE_Enabled) {
        // qDebug() << "setData" << value.toBool();
        m_effects[index.row()]->setEnabled(value.toBool());
        Q_EMIT dataChanged(index, index, QVector<int>() << ROLE_Enabled);
        return true;
    }

    return false;
}
