#include "cliparea.h"

#include "audio/audioengine.h"
#include "audio/song.h"
#include "audio/track.h"
#include "plugins/vst3effect.h"
#include <QDir>
#include <QDrag>
#include <QJsonArray>
#include <QMimeData>
#include <QThread>
#include <audio/engine.h>
#include <core/screeninterface.h>
#include <qdatetime.h>

QMutex _mutex;

bool ClipArea::shiftPressed() const
{
    return _areaInfo->shiftkeyPressed();
}

void ClipArea::setZoomFactor(float factor)
{
    _areaInfo->setZoomFactor(factor);

    _tracksModel->updateTracksHeight();
    Q_EMIT zoomFactorChanged();
    _areaInfo->updateUI();
}

float ClipArea::zoomFactor()
{
    return _areaInfo->zoomFactor();
}

void ClipArea::setOverrideCursor(Qt::CursorShape cursorShape)
{
    QGuiApplication::setOverrideCursor(QCursor(cursorShape));
}

int ClipArea::findTrackResizing(int posY)
{
    for (int i = 0; i < _tracksModel->size(); i++) {

        if (qAbs(_tracksModel->trackItem(i)->rect().bottom() - posY) <= _areaInfo->titleHeight() / 4.0) {
            return i;
        }
    }
    return -1;
}

int ClipArea::minStepSize()
{
    return _areaInfo->titleHeight();
}

int ClipArea::seperatorHeight()
{
    return _areaInfo->seperatorHeight();
}

void ClipArea::addVisr(int trackIndex, bool enabled, bool reverbEnabled)
{
    if (trackIndex >= 0 && trackIndex < _tracksModel->size()) {

        Q_EMIT sigAddVisr(trackIndex, enabled, reverbEnabled);
    }
}

void ClipArea::removeVisr(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _tracksModel->size()) {

        auto sampleTrack = (SampleTrack*)_tracksModel->trackItem(trackIndex)->track();

        auto effect = sampleTrack->audioPort()->visrEfect();

        if (effect) {
            QString menu = effect->controls()->name();

            if (_tracksModel->trackItem(trackIndex)->getAutomationMenuTitle() == menu) {
                _tracksModel->setAutomationMenuTitle(trackIndex, "None", "None");
            }

            _tracksModel->trackItem(trackIndex)->automationLaneModel()->remove(menu);

            effect.reset();

            sampleTrack->audioPort()->removeVisr();

            _tracksModel->visrRemoved(trackIndex);
        }
    }
}

void ClipArea::removeMultiEqEffect(int trackIndex)
{

    if (trackIndex >= 0 && trackIndex < _tracksModel->size()) {

        auto sampleTrack = (SampleTrack*)_tracksModel->trackItem(trackIndex)->track();

        auto effect = sampleTrack->audioPort()->effects()->getEffect("EQ");

        auto effectIndex = sampleTrack->audioPort()->effects()->getEffectList()["EQ"];
        if (effect) {
            QString menu = effect->controls()->name();

            if (_tracksModel->trackItem(trackIndex)->getAutomationMenuTitle() == menu) {
                _tracksModel->setAutomationMenuTitle(trackIndex, "None", "None");
            }

            _tracksModel->trackItem(trackIndex)->automationLaneModel()->remove(menu);

            _tracksModel->effectRemoved(trackIndex, effectIndex);

            sampleTrack->audioPort()->effects()->removeEffect(effectIndex);

            Q_EMIT effectsChanged(trackIndex, effectIndex);
        }
    }
}

void ClipArea::saveAddEffectInUndo(int trackIndex, QString effect_name, int effect_index)
{
    QJsonArray jArray;
    QJsonObject jObject;
    jObject["track_index"] = trackIndex;
    jObject["effect"] = effect_name;
    jObject["effect_index"] = effect_index;
    jObject["operation"] = ActionStack::Operation_EffectAdded;
    jArray.append(jObject);
    saveInUndoItems(jArray);
}

void ClipArea::saveRemoveEffectInUndo(int trackIndex, QString effect_name, int effect_index)
{
    QJsonArray jArray;
    QJsonObject jObject;
    jObject["track_index"] = trackIndex;
    jObject["effect"] = effect_name;
    jObject["effect_index"] = effect_name;
    jObject["operation"] = ActionStack::Operation_EffectRemoved;
    jArray.append(jObject);
    saveInUndoItems(jArray);
}

void ClipArea::saveAsyncProject(QStringList objects, QString projectName)
{

    _savedObjects = objects;
    Q_EMIT sigProjectSaving();
    QtConcurrent::run([this, objects, projectName]() {
        _project_name = projectName;

        this->saveProject(objects, projectName);
        Q_EMIT sigProjectSaved();
    });
}

void ClipArea::loadAsyncProject(QString projectName)
{

    QUrl url(projectName);
    projectName = url.toLocalFile();

    Q_EMIT sigProjectLoading();
    _project_name = projectName;

    this->loadProject(projectName);
}

bool ClipArea::hasEffect(int trackIndex)
{

    if (trackIndex >= 0 && trackIndex < _tracksModel->size()) {

        auto sampleTrack = (SampleTrack*)_tracksModel->trackItem(trackIndex)->track();
        auto effects = sampleTrack->audioPort()->effects();

        return effects->size() > 0;
    }
    return false;
}

void ClipArea::addVst3Effect(int trackIndex, QString path, QString name, int index)
{

    if (trackIndex >= 0 && trackIndex < _tracksModel->size() && path != "") {

        uint pluginIndex = uint(index);

        auto trackItem = _tracksModel->trackItem(trackIndex);

        if (pluginIndex != 0) {
            auto vst = QSharedPointer<Vst3Effect>(new Vst3Effect(path, name, pluginIndex, _areaInfo, trackIndex, trackItem->inputChannelCount(), trackItem->outputChannelCount()));
            connect(vst.data(), &Effect::sigEffectLoaded, this, &ClipArea::sltEffectLoaded);
            connect(vst.data(), &Effect::sigEffectUpdated, this, &ClipArea::sltEffectUpdated);
            m_loadingEffects.insert(name, vst);
            Q_EMIT sigEffectLoading(true);
            vst->initialize(trackIndex, vst);
        }
    }
}

void ClipArea::removeVst3Effect(int trackIndex, int effectIndex, int index)
{

    if (trackIndex >= 0 && trackIndex < _tracksModel->size()) {

        auto sampleTrack = (SampleTrack*)_tracksModel->trackItem(trackIndex)->track();
        auto effect = sampleTrack->audioPort()->effects()->effect(effectIndex);

        if (effect) {
            QString menu = effect->controls()->name();

            if (_tracksModel->trackItem(trackIndex)->getAutomationMenuTitle() == menu) {
                _tracksModel->setAutomationMenuTitle(trackIndex, "None", "None");
            }

            _tracksModel->trackItem(trackIndex)->automationLaneModel()->remove(menu);

            _tracksModel->effectRemoved(trackIndex, effectIndex);

            sampleTrack->audioPort()->effects()->removeEffect(effectIndex);

            Q_EMIT effectsChanged(trackIndex, effectIndex);
        }
    }
}

void ClipArea::sltControlVisibleChanged(int trackIndex, QString menu)
{
    auto sampleTrack = (SampleTrack*)_tracksModel->trackItem(trackIndex)->track();
    sampleTrack->audioPort()->effects()->controlVisibleChanged(menu);
}

void ClipArea::sltEffectEnableChanged(int trackIndex, QString menu)
{

    auto sampleTrack = (SampleTrack*)_tracksModel->trackItem(trackIndex)->track();
    sampleTrack->audioPort()->effects()->enabledChanged(menu);
}

void ClipArea::sltEffectLoaded(int trackIndex)
{
    Vst3Effect* _sender = (Vst3Effect*)sender();

    if (!m_loadingEffects.contains(_sender->name())) {
        Q_EMIT sigEffectLoading(false);
        return;
    }

    auto vst = m_loadingEffects[_sender->name()];
    bool status = vst->initialPlugin(vst);

    if (status) {

        bool plugin_enabled = true;
        if (m_loadingEffectsAutomations.contains(_sender->name())) {
            auto obj = m_loadingEffectsAutomations[_sender->name()];
            auto automation_array = obj["automations"].toArray();

            auto automation_menu = obj["automation_menu"].toString();
            auto automation_sub_menu = obj["automation_sub_menu"].toString();
            plugin_enabled = obj["plugin_enabled"].toBool();
            if (obj.contains("configured_parameters")) {
                auto configured_parameters_obj = obj["configured_parameters"].toArray();

                for (int i = 0; i < configured_parameters_obj.size(); i++) {
                    _sender->addConfiguredParameter(configured_parameters_obj[i].toInt());
                }
            }

            // effect->setEnabled(plugin_enabled);

            m_loadingEffectsAutomations.remove(_sender->name());
            for (int i = 0; i < automation_array.size(); i++) {
                auto automation_obj = automation_array[i].toObject();
                bool enabled = automation_obj["enabled"].toBool();
                QString type = automation_obj["type"].toString();
                QJsonArray state = automation_obj["state"].toArray();
                int sub_track_index = automation_obj["sub_track_index"].toInt();
                int track_index = automation_obj["track_index"].toInt();
                QString sub_menu = automation_obj["sub_menu"].toString();
                QString menu = automation_obj["menu"].toString();
                double normalized = automation_obj["normalized"].toDouble();

                bool show_envelopes = automation_obj["show_envelopes"].toBool();

                if (automation_menu == menu && automation_sub_menu == sub_menu) {
                    _tracksModel->setAutomationMenuTitle(track_index, menu, sub_menu);
                }

                if (sub_track_index == -1) {
                    if (_tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)) {
                        _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setLinesState(state);
                        _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setEnabled(enabled);
                        _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setNormalized(normalized);

                        _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setShowEnvelopes(show_envelopes);
                    } else {
                        qDebug() << "BUG2" << sub_track_index << track_index << menu << sub_menu;
                    }
                } else {
                    if (_tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)) {

                        _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setLinesState(state);
                        _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setEnabled(enabled);
                        _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setNormalized(normalized);

                        _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setShowEnvelopes(show_envelopes);
                        _tracksModel->automationLaneModel(track_index)->insert(menu, sub_menu, sub_track_index, 3);
                    } else {
                        qDebug() << "BUG3" << sub_track_index << track_index << menu << sub_menu;
                    }
                }
            }
        }
        Q_EMIT sigAddEffect(trackIndex, _sender->name(), plugin_enabled);
    }

    Q_EMIT sigEffectLoading(false);
}

void ClipArea::sltEffectUpdated(int trackIndex, int effectIndex)
{
    _tracksModel->effectUpdated(trackIndex, effectIndex);
}

void ClipArea::setCoverBound(bool status)
{
    _tracksModel->setCoverBound(status);
    Q_EMIT sigCoverBoundChanged();
}

void ClipArea::setTrackHeight(int _index, int posY)
{

    if (_index >= 0 && _index < _tracksModel->size()) {

        auto trackItem = _tracksModel->trackItem(_index);

        auto trackRect = trackItem->rect();

        if (posY > 0) {

            trackItem->setRowsExpandedCount(trackItem->rowsExpandedCount() + 1);
            _tracksModel->updateTracksHeight(_index);

        } else if (posY < 0) {

            if (trackItem->rowsExpandedCount() > 1) {
                trackItem->setRowsExpandedCount(trackItem->rowsExpandedCount() - 1);
                _tracksModel->updateTracksHeight(_index);
            }
        }

        trackItem->setLastRowExpanded(trackItem->rowsExpandedCount());

        if (trackItem->rowsExpandedCount() == 1) {

            if (trackItem->automationLaneModel()->count() > 0) {
                if (trackItem->automationLaneModel()->totalRowCount() == 1) {
                    trackItem->setRowsExpanded(false);
                } else {
                    trackItem->setRowsExpanded(false);
                }
            } else {
                trackItem->setRowsExpanded(false);
            }

        } else {
            trackItem->setRowsExpanded(true);
        }
    }
}

int ClipArea::trackListStartHeight() const
{
    return _trackListHeight;
}

int ClipArea::trackListStartY() const
{
    return _trackListStartY;
}

TracksModel* ClipArea::tracksModel() const
{

    if (_tracksModel == Q_NULLPTR) {
        return Q_NULLPTR;
    }
    TracksModel* item = _tracksModel.data();
    QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
    return item;
}

bool ClipArea::automationEnabled() const
{
    return _automationEnabled;
}

void ClipArea::setAutomationEnabled(bool automationEnabled)
{

    _automationEnabled = automationEnabled;

    if (_automationEnabled == false) {
        _areaInfo->selectionArea()->isClipSelectedAtFirst = false;
        _areaInfo->selectionArea()->isClipSelectedAtLast = true;
    } else {
        _areaInfo->selectionArea()->isClipSelectedAtFirst = false;
        _areaInfo->selectionArea()->isClipSelectedAtLast = false;
    }

    _tracksModel->setAutomationEnabed(automationEnabled);

    Q_EMIT automationEnabledChanged();
    _areaInfo->updateUI();
}

double ClipArea::startPixel() const
{
    return _startPixel;
}

double ClipArea::endPixel() const
{
    return _endPixel;
}

double ClipArea::stepPlotDuration() const
{
    return _areaInfo->stepPlotDuration();
}

QVector<TrackItem::CopyItem> ClipArea::movingItems() const
{
    return _movingItems;
}

QSharedPointer<ClipItem> ClipArea::draggingItem() const
{
    return _draggingItem;
}

int ClipArea::beatPerBar() const
{
    return _areaInfo->beatPerBar();
}

void ClipArea::setBeatPerBar(int beatPerBar)
{
    _areaInfo->setBeatPerBar(beatPerBar);
    Q_EMIT beatPerBarChanged();
}

int ClipArea::lenghtOfBeat() const
{
    return _areaInfo->lenghtOfBeat();
}

void ClipArea::setLenghtOfBeat(int lenghtOfBeat)
{
    _areaInfo->setLenghtOfBeat(lenghtOfBeat);
    Q_EMIT lenghtOfBeatChanged();
}

float ClipArea::tempo() const
{
    return _areaInfo->tempo();
}

void ClipArea::setTempo(float tempo)
{
    _areaInfo->setTempo(tempo);

    Q_EMIT tempoChanged();
}

AreaInfo* ClipArea::areaInfo() const
{
    return _areaInfo;
}

QString ClipArea::defaultSavePath() const
{
    return _defaultSavePath;
}

void ClipArea::setDefaultSavePath(QString defaultSavePath)
{

    QUrl url(defaultSavePath);
    _defaultSavePath = url.toLocalFile();
    Q_EMIT defaultSavePathChanged();
}

void ClipArea::sltRowsExpandedChanged(int _index)
{

    int totalH = -_areaInfo->startContentY();
    if (_tracksModel->size() >= 1) {
        for (int i = 0; i < _tracksModel->size(); i++) {

            auto new_rect = _tracksModel->trackItem(i)->getTotalRect();

            if (_areaInfo->selectedTrackItem() == i) {
                auto last_rect = _tracksModel->trackItem(i)->getTotalRect();

                last_rect = QRect(last_rect.left(), totalH, last_rect.width(), last_rect.height());
                _areaInfo->setSelectedTrackItemRect(last_rect);
            }
            totalH += new_rect.height();
        }
    }

    auto offset = _areaInfo->startContentY();
    if (_tracksModel->size() > 0) {

        int sY = -offset;

        _tracksModel->trackItem(0)->setGlobalYPosition(sY);

        for (int i = 1; i < _tracksModel->size(); i++) {

            int sumH = _tracksModel->trackItem(i - 1)->getTotalRect().height();
            sY += sumH;
            _tracksModel->trackItem(i)->setGlobalYPosition(sY);
        }
    }

    for (int i = 0; i < _tracksModel->size(); i++) {

        auto _boundingRect = _tracksModel->trackItem(i)->rect();
        auto rect = _boundingRect;
        rect.setY(tracksModel()->trackItem(i)->globalYPosition());
        rect.setHeight(_boundingRect.height());

        _areaInfo->selectionArea()->setTracksRect(i, rect);
    }

    //_trackListHeight = _tracksModel->trackItem(_tracksModel->size() - 1)->getTotalRect().bottom() - _trackListStartY;
    Q_EMIT sigTrackHeightChanged();
    _areaInfo->updateUI();
}

void ClipArea::addSampleClip()
{

    if (AudioManager::getSong() == Q_NULLPTR) {
        return;
    }

    auto track = AudioManager::getSong()->addSampleTrack(_areaInfo, _tracksModel->size());
    auto _trackItem = QSharedPointer<TrackItem>(new TrackItem(_tracksModel->size(), track, _areaInfo));

    connect(_trackItem.get(), &TrackItem::sigFirstBufferFilled, this, &ClipArea::sltFirstBufferFilled);
    connect(_trackItem.get(), &TrackItem::sigSampleChanged, this, &ClipArea::sltSampleChanged);
    connect(_trackItem.get(), &TrackItem::sigUpdate, _areaInfo, &AreaInfo::sigUpdateUI);

    connect(_trackItem.get(), &TrackItem::rowsExpandedChanged, this, &ClipArea::sltRowsExpandedChanged);

    connect(_trackItem.get(), &TrackItem::sigSetCursor, [](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    _trackItem->setGlobalYPosition(_tracksModel->size() * (_areaInfo->expandedHeight() + _areaInfo->seperatorHeight()));

    _trackItem->setIndicatorEnable(TrackItem::IndicatorType_Track);

    _tracksModel->addTrackItem(_trackItem);

    QList<QRectF> _rects;
    for (int i = 0; i < _tracksModel->size(); i++) {
        _tracksModel->trackItem(i)->setIndicatorEnable(TrackItem::IndicatorType_Disable);
        _rects.append(_tracksModel->trackItem(i)->rect());
    }
    _areaInfo->selectionArea()->setTracksRect(_rects);

    for (int i = 0; i < _tracksModel->size(); i++) {

        auto _boundingRect = _tracksModel->trackItem(i)->rect();
        auto rect = _boundingRect;
        rect.setY(tracksModel()->trackItem(i)->globalYPosition());
        rect.setHeight(_boundingRect.height());

        _areaInfo->selectionArea()->setTracksRect(i, rect);
    }

    _trackListHeight = _trackItem->rect().bottom() - _trackListStartY;

    if (_tracksModel->size() == 1) {
        _areaInfo->setSelectedTrackItem(0);
    }

    Q_EMIT sigTrackHeightChanged();
    Q_EMIT sigTrackChanged();
    _areaInfo->updateUI();

    addMultiEqEffect(_trackItem->trackIndex());
}

void ClipArea::addMultiEqEffect(int trackIndex)
{
    if (trackIndex < 0 || trackIndex > _tracksModel->size())
        return;

    auto vst = QSharedPointer<MultiEqEffect>(new MultiEqEffect(trackIndex, _areaInfo));

    vst->initialize(trackIndex, vst);

    auto sampleTrack = (SampleTrack*)tracksModel()->trackItem(trackIndex)->track();
    int index = sampleTrack->audioPort()->effects()->appendEffect(vst);
    auto effect = sampleTrack->audioPort()->effects()->effect(index);

    effect->setEnabled(true);

    connect(effect.data(), &Effect::sigSetCursor, [](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(effect.data(), &Effect::sigCursorPosition, [this](QPoint pos) {
        if (!m_windowRootitem || !m_windowRootitem->window())
            return;

        QPointF scenePos = m_windowRootitem->mapToScene(QPointF(pos));
        QPoint globalPos = m_windowRootitem->window()->mapToGlobal(scenePos.toPoint());

        QCursor::setPos(globalPos);
    });

    connect(effect.data(), &Effect::sigUpdate, _areaInfo, &AreaInfo::sigUpdateUI, Qt::QueuedConnection);

    connect(effect.data(), &Effect::sigSaveInUndoItems, this, &ClipArea::saveInUndoItems, Qt::QueuedConnection);
    connect(effect.data(), &Effect::sigSaveInRedoItems, this, &ClipArea::saveInRedoItems, Qt::QueuedConnection);

    tracksModel()->effectAdded(trackIndex, index);

    Q_EMIT effectsChanged(trackIndex, index);
}
void ClipArea::removeSampleClip(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < _tracksModel->size()) {

        _tracksModel->removeTrackItem(trackIndex);

        auto lastIndex = _tracksModel->size() - 1;
        if (lastIndex >= 0) {
            _trackListHeight = _tracksModel->trackItem(lastIndex)->rect().bottom() - _trackListStartY;
            sltStartContentYChanged();
        }

        if (trackIndex - 1 >= 0 && trackIndex < _tracksModel->size()) {
            _areaInfo->setSelectedTrackItemRect(_tracksModel->trackItem(trackIndex)->rect());
        } else {
            _areaInfo->setSelectedTrackItemRect(QRectF());
        }

        Q_EMIT sigTrackHeightChanged();
        Q_EMIT sigTrackChanged();
        _areaInfo->updateUI();
    }
}

void ClipArea::setCurrentIndicator(qint64 time)
{
    for (int i = 0; i < _tracksModel->size(); i++) {
        _tracksModel->trackItem(i)->deSelecteAll();
        _tracksModel->trackItem(i)->setIndicator(time);
    }
}

int ClipArea::adaptiveGrid() const
{
    return _areaInfo->adaptiveGrid();
}

void ClipArea::setAdaptiveGrid(int adaptiveGrid)
{
    if (_areaInfo->adaptiveGrid() != adaptiveGrid) {

        _areaInfo->setAdaptiveGrid(adaptiveGrid);

        Q_EMIT adaptiveGridChanged();

        _areaInfo->updateUI();
    }
}

int ClipArea::fixedGrid() const
{
    return _areaInfo->fixedGrid();
}

void ClipArea::setFixedGrid(int fixedGrid)
{

    if (_areaInfo->fixedGrid() != fixedGrid) {

        _areaInfo->setFixedGrid(fixedGrid);

        Q_EMIT fixedGridChanged();

        _areaInfo->updateUI();
    }
}

void ClipArea::redoPerformOperation()
{

    if (!ActionStack::instance()->redoIsEmpty()) {
        auto actions = ActionStack::instance()->popFromRedo();

        QJsonArray jArray_all;

        for (int i = actions._actions.size() - 1; i >= 0; i--) {

            if (actions._actions[i].isArray()) {
                auto array = actions._actions[i].toArray();

                QJsonArray jArray;
                for (int j = array.size() - 1; j >= 0; j--) {

                    if (array[j].isArray()) {
                        QJsonArray inner_array = array[j].toArray();
                        QJsonArray arr;
                        for (int k = inner_array.size() - 1; k >= 0; k--) {
                            QJsonObject obj = (inner_array[k]).toObject();

                            auto result = performOperation(obj);
                            if (!result.empty())
                                arr.append(result);
                        }
                        if (arr.size() > 0)
                            jArray.append(arr);
                    } else {
                        QJsonObject obj = (array[j]).toObject();

                        auto result = performOperation(obj);
                        if (!result.empty())
                            jArray.append(result);
                    }
                }
                if (jArray.size() > 0)
                    jArray_all.append(jArray);
            } else {

                if (actions._actions[i].isArray()) {
                    QJsonArray inner_array = actions._actions[i].toArray();
                    QJsonArray arr;
                    for (int k = inner_array.size() - 1; k >= 0; k--) {
                        QJsonObject obj = (inner_array[k]).toObject();

                        auto result = performOperation(obj);
                        if (!result.empty())
                            arr.append(result);
                    }
                    if (arr.size() > 0)
                        jArray_all.append(arr);
                } else {
                    auto object = (actions._actions[i]).toObject();
                    QJsonObject obj = performOperation(object);
                    if (!obj.empty())
                        jArray_all.append(obj);
                }
            }
        }
        if (jArray_all.size() > 0)
            ActionStack::instance()->pushInUndo(jArray_all);
    }
}

void ClipArea::add3dObject(int trackIndex, bool enabled)
{
    Q_EMIT _areaInfo->sigAddObject(trackIndex, enabled);
}

void ClipArea::undoPerformOperation()
{
    if (!ActionStack::instance()->undoIsEmpty()) {
        auto actions = ActionStack::instance()->popFromUndo();

        QJsonArray jArray_all;

        for (int i = actions._actions.size() - 1; i >= 0; i--) {

            if (actions._actions[i].isArray()) {
                auto array = actions._actions[i].toArray();

                QJsonArray jArray;
                for (int j = array.size() - 1; j >= 0; j--) {

                    if (array[j].isArray()) {
                        QJsonArray inner_array = array[j].toArray();
                        QJsonArray arr;
                        for (int k = inner_array.size() - 1; k >= 0; k--) {
                            QJsonObject obj = (inner_array[k]).toObject();
                            auto result = performOperation(obj);
                            if (!result.empty())
                                arr.append(result);
                        }
                        if (arr.size() > 0)
                            jArray.append(arr);
                    } else {
                        QJsonObject obj = (array[j]).toObject();
                        auto result = performOperation(obj);
                        if (!result.empty())
                            jArray.append(result);
                    }
                }
                if (jArray.size() > 0)
                    jArray_all.append(jArray);
            } else {

                if (actions._actions[i].isArray()) {
                    QJsonArray inner_array = actions._actions[i].toArray();
                    QJsonArray arr;
                    for (int k = inner_array.size() - 1; k >= 0; k--) {
                        QJsonObject obj = (inner_array[k]).toObject();

                        auto result = performOperation(obj);
                        if (!result.empty())
                            arr.append(result);
                    }
                    if (arr.size() > 0)
                        jArray_all.append(arr);
                } else {
                    auto object = (actions._actions[i]).toObject();
                    QJsonObject obj = performOperation(object);
                    if (!obj.empty())
                        jArray_all.append(obj);
                }
            }
        }

        if (jArray_all.size() > 0)
            ActionStack::instance()->pushInRedo(jArray_all);
    }
}

void ClipArea::exportAudio(QString fileName, int samplerate, int bitDepth, bool mono)
{
    AudioEngine::qualitySettings qs = AudioEngine::qualitySettings(
        static_cast<AudioEngine::qualitySettings::Interpolation>(2),
        static_cast<AudioEngine::qualitySettings::Oversampling>(0));

    const auto bitrates = std::array { 64, 128, 160, 192, 256, 320 };

    OutputSettings::BitRateSettings bitRateSettings(bitrates[0], false);

    OutputSettings::StereoMode mode = OutputSettings::StereoMode_Stereo;
    if (mono) {
        mode = OutputSettings::StereoMode_Mono;
    }

    OutputSettings os = OutputSettings(
        samplerate,
        bitRateSettings,
        static_cast<OutputSettings::BitDepth>(bitDepth),
        mode);

    QString output_name = fileName;
    m_renderManager.reset(new AudioRenderManager(qs, os, ProjectRenderer::WaveFile, output_name));

    connect(m_renderManager.get(), &AudioRenderManager::progressChanged, this, &ClipArea::sltExportRenderProgress);
    connect(m_renderManager.get(), &AudioRenderManager::finished, this, &ClipArea::sltExportRenderFinished);
    AudioManager::getSong()->setExportLoop(true);

    _exporing = true;

    if (qAbs(_areaInfo->selectionArea()->area().left() - _areaInfo->selectionArea()->area().right()) < 1) {

        auto _playbackMarker = 0;
        double samples = _playbackMarker / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        AudioManager::getSong()->setToTime(samples / 256.0);
        AudioManager::getSong()->readyToPlay();
        _areaInfo->setArrangementStartTime(_playbackMarker);
        _areaInfo->setPlayheadMarker(_playbackMarker);

        AudioManager::getSong()->setRenderBetweenMarkers(false);

    } else {
        auto _playbackMarker = _areaInfo->selectionArea()->area().left();
        double samples = _playbackMarker / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
        AudioManager::getSong()->setToTime(samples / 256.0);
        AudioManager::getSong()->readyToPlay();
        _areaInfo->setArrangementStartTime(_playbackMarker);
        _areaInfo->setPlayheadMarker(_playbackMarker);

        AudioManager::getSong()->setExportSongBegin(_areaInfo->selectionArea()->area().left() / 1000.0 * AudioManager::audioEngine()->outputSampleRate() / 256.0);
        AudioManager::getSong()->setExportSongEnd(_areaInfo->selectionArea()->area().right() / 1000.0 * AudioManager::audioEngine()->outputSampleRate() / 256.0);
        AudioManager::getSong()->setRenderBetweenMarkers(_areaInfo->selectedAreaActive());
    }
}

void ClipArea::sltExportRenderFinished()
{
    m_renderManager.reset(nullptr);
    AudioManager::getSong()->setPause();
    Q_EMIT sigExportFinished();
}

int ClipArea::exportProgress()
{
    return _exportProgress;
}

void ClipArea::setCursorPosition(int x, int y)
{
    QCursor::setPos(x, y);
}

QPoint ClipArea::globalPos()
{
    return QCursor::pos();
}

double ClipArea::findSnap(double posX)
{

    return _areaInfo->findSnap(posX);
}

void ClipArea::clearMasterTrackSelection()
{

    _areaInfo->masterTrackSelectedArea()->setRight(0);
    _areaInfo->masterTrackSelectedArea()->setLeft(0);
    _areaInfo->masterTrackSelectedArea()->update();
    drawGrids();
}

void ClipArea::clearSelection()
{
    _areaInfo->selectionArea()->setRight(0);
    _areaInfo->selectionArea()->setLeft(0);
    _areaInfo->selectionArea()->update();
    drawGrids();
}

void ClipArea::handleHoverMoved(QPoint localPos, QPoint globalPos, int button)
{

    QPointF pos(localPos.x(), localPos.y());

    QHoverEvent event(QEvent::HoverMove, pos, _oldHoverPos, Qt::NoModifier);

    _oldHoverPos = pos;
    int trackIndex = -1;
    QString clipIndex = "";

    QPair<int, int> temp = findTrackIndexByPoint(globalPos);
    trackIndex = temp.first;

    _trackLineResizing = -1;

    _tracksModel->trackItem(trackIndex)->hoverMoveEvent(&event);
    clipIndex = _tracksModel->trackItem(trackIndex)->hoverClipIndex();

    if ((trackIndex == -1 || clipIndex == "") && _trackLineResizing == -1) {
        QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
    } else {

        if (trackIndex >= 0 && clipIndex >= 0) {
            auto action = _tracksModel->trackItem(trackIndex)->clipItems(clipIndex)->mouseAction();
            if (action == ClipItem::MouseAction_ResizeRight
                || action == ClipItem::MouseAction_ResizeLeft) {

                QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
            } else if (action == ClipItem::MouseAction_Move) {
                QGuiApplication::setOverrideCursor(QCursor(Qt::OpenHandCursor));
            }
        }
    }
}

void ClipArea::handleMouseReleased(QPoint localPos, QPoint globalPos, int button)
{
    QMouseEvent event(QEvent::MouseButtonPress,
        QPointF(localPos.x(), localPos.y()),
        Qt::MouseButton(button),
        Qt::MouseButton(button),
        Qt::NoModifier);

    _listViewPositionChanged = 0;

    _currentMouseX = -1;
    _currentMouseY = -1;
    _mouseMoveEvent = false;

    _selectingAreaCreating = false;
    _selectingAreaMoving = false;

    _trackLineResizing = -1;
    _trackLineResizingStartY = 0;

    _timerCursorPosition.stop();
    if (_selectingAreaResizing || _selectedAreaMoved) {

        if (_selectingAreaResizing) {
            _selectingAreaResizing = false;
            auto item = _tracksModel->trackItem(_startSelectedTrackIndex)->clipItems(_startSelectedClipIndex);
            item->mouseReleaseEvent(&event);

            if (_selectingStateBeforeResizing.size() > 0) {
                saveInUndoItems(_selectingStateBeforeResizing);
                _selectingStateBeforeResizing = QJsonArray();
            }

        } else {

            QJsonArray jArray;
            if (_selectedAreaMoved) {

                _selectedAreaMoved = false;

                if (_movingAutomationPoint.size() > 0) {

                    for (int i = 0; i < _tracksModel->size(); i++) {
                        _tracksModel->trackItem(i)->clearCopyList();
                    }

                    double final_xpos = 0;

                    final_xpos = _areaInfo->findSnap(globalPos.x());
                    auto time = _areaInfo->pixel2Time(final_xpos);

                    double newLeft = time - _areaInfo->selectionArea()->leftDiff;

                    double newleftPixel = _areaInfo->time2Pixel(newLeft);

                    double newTopPixel = globalPos.y() - _areaInfo->selectionArea()->topDiff;

                    auto tmp = findTrackIndexByPoint(QPointF(newleftPixel, newTopPixel));
                    int firstIndex = tmp.first;

                    if (firstIndex < 0) {
                        firstIndex = 0;
                    }
                    if (firstIndex >= _tracksModel->size()) {
                        firstIndex = _tracksModel->size() - 1;
                    }

                    int first_track_index_order = -1;
                    int first_subtrack_index_order = -1;
                    for (int i = 0; i < _movingAutomationPoint.size(); i++) {
                        if (i == 0) {
                            first_track_index_order = _movingAutomationPoint[i].trackIndex;
                            first_subtrack_index_order = _movingAutomationPoint[i].subtrackIndex;
                        }

                        int tIndex = _movingAutomationPoint[i].trackIndex;

                        _tracksModel->trackItem(tIndex)->deleteAllAutomationItemsInArea(_movingAutomationPoint[i].selectedArea, jArray, tIndex);

                        for (int j = 0; j < _tracksModel->trackItem(tIndex)->automationLaneModel()->count(); j++) {
                            auto laneItem = _tracksModel->trackItem(tIndex)->automationLaneModel()->getItem(j);
                            _tracksModel->trackItem(tIndex)->deleteCurrentAutomationItemInArea(_movingAutomationPoint[i].selectedArea, jArray, laneItem->automationLaneMenuTitle,
                                laneItem->automationLaneSubMenuTitle, tIndex, j);
                        }

                        _movingAutomationPoint[i].trackIndex = firstIndex + (_movingAutomationPoint[i].trackIndex - first_track_index_order);
                    }

                    for (int i = 0; i < _tracksModel->size(); i++) {
                        _tracksModel->trackItem(i)->clearAutomationItems();
                    }

                    for (int i = 0; i < _movingAutomationPoint.size(); i++) {

                        int trackIndex = _movingAutomationPoint[i].trackIndex;

                        if (trackIndex >= 0 && trackIndex < _tracksModel->size()) {

                            _tracksModel->trackItem(trackIndex)
                                ->setCopyList2(_movingAutomationPoint[i].aline,
                                    _movingAutomationPoint[i].type, _movingAutomationPoint[i].sub_type);
                        }
                    }

                    for (int i = 0; i < _tracksModel->size(); i++) {

                        _tracksModel->trackItem(i)->insertAllAutomationItems(jArray, i, false);
                        for (int j = 0; j < _tracksModel->trackItem(i)->automationLaneModel()->count(); j++) {

                            auto laneItem = _tracksModel->trackItem(i)->automationLaneModel()->getItem(j);

                            _tracksModel->trackItem(i)->insertCurrentAutomationItemInArea(jArray, laneItem->automationLaneMenuTitle,
                                laneItem->automationLaneSubMenuTitle, i, j);
                        }
                    }
                }

                _movingAutomationPoint.clear();

                for (int i = 0; i < _tracksModel->size(); i++) {

                    qDebug() << "*_areaInfo->lastSelectedArea()" << _areaInfo->lastSelectedArea()->area();
                    QJsonArray array = _tracksModel->trackItem(i)->deleteSelectedArea(*_areaInfo->lastSelectedArea());

                    if (array.size() > 0)
                        jArray.append(array);
                }

                double _maxEndTime = 0;
                for (int i = 0; i < _movingItems.size(); i++) {

                    if (_movingItems[i].clipItem == Q_NULLPTR) {
                        continue;
                    }

                    double startTime = _movingItems[i].clipItem->sampleClip()->startTime();
                    double endTime = _movingItems[i].clipItem->sampleClip()->endTime();

                    if (endTime > _maxEndTime) {
                        _maxEndTime = endTime;
                    }

                    double startOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetStartFrame();
                    double endOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetEndFrame();

                    int trackIndex = _movingItems[i].trackIndex;
                    auto clipIndex = _tracksModel->trackItem(trackIndex)->addSampleClip(startTime, endTime, startOffset, endOffset, _movingItems[i].clipItem->clone(_tracksModel->trackItem(trackIndex)->track()));

                    QJsonObject jObject;
                    jObject["clip_index"] = clipIndex;
                    jObject["operation"] = ActionStack::Operation_InsertClipItem;
                    jObject["track_index"] = trackIndex;
                    jArray.append(jObject);

                    if (clipIndex > 0) {

                        auto list = _tracksModel->trackItem(trackIndex)->findOverlayItems(_movingItems[i].clipItem->sampleClip()->startTime(), _movingItems[i].clipItem->sampleClip()->endTime());

                        for (int j = 0; j < list.size(); j++) {
                            if (list[j] != clipIndex) {
                                auto array = _tracksModel->trackItem(trackIndex)->cutClipItem(list[j], _movingItems[i].clipItem->sampleClip()->startTime(), _movingItems[i].clipItem->sampleClip()->endTime());
                                jArray.append(array);
                            }
                        }
                    }

                    jArray.append(_areaInfo->lastSelectedArea()->state());
                    playbackSampleClipPositionChanged(trackIndex, clipIndex);
                }

                if (_maxEndTime > _areaInfo->maxEndTime()) {
                    _areaInfo->setMaxEndTime(_maxEndTime);
                }

                saveInUndoItems(jArray);
                clearMovingItems();
            }
        }

    } else {

        clearMovingItems();

        if (_automationChanging || _keyFramesPointSelected || _objectPointXSelected || _objectPointYSelected || _objectPointZSelected) {
            for (int i = 0; i < _tracksModel->size(); i++) {
                _tracksModel->trackItem(i)->mouseReleaseEvent(&event);
            }
        }
    }

    _selectedAreaMoved = false;
    _selectingAreaCreating = false;
    _selectingAreaMoving = false;
}

void ClipArea::handleMouseMoved(QPoint localPos, QPoint globalPos, int button)
{

    _currentMouseX = globalPos.x();
    _currentMouseY = globalPos.y();
    _listViewPositionChanged = 0;
    _mouseMoveEvent = true;

    QMouseEvent event(QEvent::MouseButtonPress,
        QPointF(localPos.x(), localPos.y()),
        Qt::MouseButton(button),
        Qt::MouseButton(button),
        Qt::NoModifier);

    if (_automationChanging == false) {

        if (_selectingAreaCreating) {
            double final_xpos = 0;

            final_xpos = _areaInfo->findSnap(event.pos().x());

            int trackIndex = -1;
            int subTrackIndex = -1;
            auto tmp = findTrackIndexByPoint(QPoint(globalPos.x(), globalPos.y()));
            trackIndex = tmp.first;
            subTrackIndex = tmp.second;

            if (trackIndex < 0) {
                trackIndex = 0;
            }
            if (trackIndex >= _tracksModel->size()) {
                trackIndex = _tracksModel->size() - 1;
            }

            auto endSelectedTime = _areaInfo->pixel2Time(final_xpos);

            applySelectedAreaToTracks(endSelectedTime, globalPos, trackIndex, subTrackIndex);

            Q_EMIT sigSelectionAreaChanged();
        } else if (_selectingAreaResizing) {

            auto item = _tracksModel->trackItem(_startSelectedTrackIndex)->clipItems(_startSelectedClipIndex);

            _areaInfo->selectionArea()->setfirstTrackIndex(_startSelectedTrackIndex);
            _areaInfo->selectionArea()->setLastTrackIndex(_startSelectedTrackIndex);
            _areaInfo->selectionArea()->setLeft(item->sampleClip()->startTime());

            _areaInfo->selectionArea()->setRight(item->sampleClip()->endTime());
            _areaInfo->selectionArea()->update();
            //_areaInfo->selectionArea()->area().setTop(item->rect().top());
            //_areaInfo->selectionArea()->area().setBottom(item->rect().bottom());

            if (_selectingStateBeforeResizing.size() == 0) {
                auto state = item->sampleClip()->state();
                auto old_rect = item->rect();
                QJsonObject jObject;
                jObject["clip_index"] = _startSelectedClipIndex;
                jObject["operation"] = ActionStack::Operation_ResizeClipItem;
                jObject["state"] = state;
                jObject["rect"] = QString("%1, %2, %3, %4").arg(old_rect.x()).arg(old_rect.y()).arg(old_rect.width()).arg(old_rect.height());
                jObject["track_index"] = _startSelectedTrackIndex;

                _selectingStateBeforeResizing.append(jObject);

                _selectingStateBeforeResizing.append(_areaInfo->selectionArea()->state());
            }

            item->mouseMoveEvent(&event);
            Q_EMIT sigSelectionAreaChanged();

        } else if (_trackLineResizing >= 0 && _trackLineResizingStartY >= 0) {

            int diff = localPos.y() - _trackLineResizingStartY;

            _trackLineResizingStartY = localPos.y();

            auto rect = _tracksModel->trackItem(_trackLineResizing)->rect();
            rect.setBottom(rect.bottom() + diff);
            if (rect.height() > _areaInfo->titleHeight()) {
                _tracksModel->trackItem(_trackLineResizing)->setRect(rect);
                for (int i = _trackLineResizing + 1; i < _tracksModel->size(); i++) {
                    auto rect = _tracksModel->trackItem(i)->rect();
                    rect.setBottom(rect.bottom() + diff);
                    rect.setTop(rect.top() + diff);
                    _tracksModel->trackItem(i)->setRect(rect);
                }
            }
            _trackListHeight = _tracksModel->trackItem(_tracksModel->size() - 1)->rect().bottom() - _trackListStartY;
            Q_EMIT sigTrackChanged();
        }

        _areaInfo->selectionArea()->update();
        _areaInfo->updateUI();

    } else {

        // clearMovingItems();

        _selectingAreaResizing = false;
        _selectingAreaMoving = false;
        _selectingAreaCreating = false;

        if (_startSelectedTrackIndex >= 0 && _startSelectedTrackIndex < _tracksModel->size()) {
            _tracksModel->trackItem(_startSelectedTrackIndex)->mouseMoveEvent(&event);

            _areaInfo->selectionArea()->update();
        }
    }
}

void ClipArea::handleMasterTrackMousePressed(QPoint localPos, int button)
{
    if (_masterTrack == Q_NULLPTR) {
        return;
    }

    // qDebug() << "handleHoverMoved";
    //  qDebug() << "localPos" << localPos << localPos;
    QMouseEvent event(QEvent::MouseButtonPress,
        QPointF(localPos.x(), localPos.y()),
        Qt::MouseButton(button),
        Qt::MouseButton(button),
        Qt::NoModifier);

    auto pressed = _masterTrack->mousePressEvent(&event);

    double final_xpos = 0;

    final_xpos = _areaInfo->findSnap(event.pos().x());

    _startSelectedTime = _areaInfo->pixel2Time(final_xpos);

    // qDebug() << "_startSelectedTime" << _startSelectedTime << final_xpos;
    _areaInfo->masterTrackSelectedArea()->setLeft(0);
    _areaInfo->masterTrackSelectedArea()->setRight(0);
    _areaInfo->masterTrackSelectedArea()->update();

    drawGrids();
}

void ClipArea::handleMasterTrackMouseMoved(QPoint localPos, int button)
{

    QMouseEvent event(QEvent::MouseButtonPress,
        QPointF(localPos.x(), localPos.y()),
        Qt::MouseButton(button),
        Qt::MouseButton(button),
        Qt::NoModifier);

    double final_xpos = 0;

    final_xpos = _areaInfo->findSnap(event.pos().x());

    auto endSelectedTime = _areaInfo->pixel2Time(final_xpos);

    if (endSelectedTime < _startSelectedTime) {
        _areaInfo->masterTrackSelectedArea()->setLeft(endSelectedTime);
        _areaInfo->masterTrackSelectedArea()->setRight(_startSelectedTime);
    } else {
        _areaInfo->masterTrackSelectedArea()->setRight(endSelectedTime);
        _areaInfo->masterTrackSelectedArea()->setLeft(_startSelectedTime);
    }

    //  applySelectedAreaToTracks(endSelectedTime, localPos, 3, 0);

    // qDebug() << "_areaInfo.updateUI();" << _startSelectedTime << endSelectedTime;
    _areaInfo->masterTrackSelectedArea()->update();
    drawGrids();
}

bool ClipArea::handleMousePressed(QPoint localPos, QPoint globalPos, int button)
{

    QMouseEvent event(QEvent::MouseButtonPress,
        QPointF(localPos.x(), localPos.y()),
        Qt::MouseButton(button),
        Qt::MouseButton(button),
        Qt::NoModifier);

    if (_trackLineResizing >= 0) {
        _trackLineResizingStartY = localPos.y();
    }

    QRect trackArea;

    if (_tracksModel->size() >= 0) {
        trackArea = _boundingRect;
        trackArea.setY(0);
        trackArea.setHeight(_tracksModel->trackItem(_tracksModel->size() - 1)->getTotalRect().bottom());
    }

    if (_trackLineResizing >= 0) {
        _trackLineResizingStartY = globalPos.y();
    }

    _areaInfo->masterTrackSelectedArea()->setLeft(0);
    _areaInfo->masterTrackSelectedArea()->setRight(0);
    _areaInfo->masterTrackSelectedArea()->update();

    _selectingAreaResizing = false;
    _startSelectedClipIndex = "";
    _selectingAreaCreating = false;
    _automationChanging = false;
    _selectingAreaMoving = false;
    _keyFramesPointSelected = false;

    int _firstSubTrack = -1;

    bool _automationlineSelected = false;
    bool _trackSelected = false;

    bool _dontChangeSelectionArea = false;

    QPair<int, int> temp = findTrackIndexByPoint(globalPos);

    _startSelectedTrackIndex = temp.first;

    if (_startSelectedTrackIndex == -1)
        return false;

    _startSelectedSubTrackIndex = temp.second;

    _areaInfo->setLoopFocusActive(false);

    auto presed = _tracksModel->trackItem(_startSelectedTrackIndex)->mousePressEvent(&event);

    if (presed == TrackItem::TrackItemEvent_TrackSelect) {
        _trackSelected = true;
    }
    if (presed == TrackItem::TrackItemEvent_ClipSelect) {
        _startSelectedClipIndex = _tracksModel->trackItem(_startSelectedTrackIndex)->selectedClipIndex();
    }
    if (presed == TrackItem::TrackItemEvent_InnerSelect) {
        _automationChanging = true;
        _dontChangeSelectionArea = true;
    }
    if (presed == TrackItem::TrackItemEvent_OuterPointSelect) {
        _automationChanging = true;
        _dontChangeSelectionArea = true;
    }

    if (presed == TrackItem::TrackItemEvent_AutomationLineSelect) {
        _automationlineSelected = true;
    }

    if (presed == TrackItem::TrackItemEvent_KeyFramesSelect) {
        _automationChanging = true;
        _keyFramesPointSelected = true;
        _dontChangeSelectionArea = true;
    }

    double final_xpos = 0;

    final_xpos = _areaInfo->findSnap(event.pos().x());
    _currentSelectedTime = _areaInfo->pixel2Time(final_xpos);

    _areaInfo->setArrangementStartTime(_currentSelectedTime);

    if (paused() || !playing()) {
        _areaInfo->setPlayheadMarkerVisible(false);
    }

    // qDebug() << "final_xpos" << qSetRealNumberPrecision(100) << final_xpos << _currentSelectedTime << event.pos().x() << _areaInfo->stepPlotDuration();
    _selectingAreaCreating = false;
    _selectedAreaMoved = false;

    if (_startSelectedTrackIndex >= 0 && _startSelectedClipIndex == "" && _startSelectedTrackIndex < _tracksModel->size()) {

        if (_automationlineSelected || _trackSelected || _keyFramesPointSelected) {
            for (int i = 0; i < _tracksModel->size(); i++) {
                _tracksModel->trackItem(i)->deSelecteAll();
                _tracksModel->trackItem(i)->setIndicator(_currentSelectedTime);
                Q_EMIT _areaInfo->sigCurrentSelectedTime(_currentSelectedTime);
            }
        }
        if (_automationlineSelected) {
            _tracksModel->trackItem(_startSelectedTrackIndex)->setIndicatorEnable(TrackItem::IndicatorType_Lane);
        } else if (_trackSelected || _keyFramesPointSelected) {
            _tracksModel->trackItem(_startSelectedTrackIndex)->setIndicatorEnable(TrackItem::IndicatorType_Track);
        }
    }

    if (_startSelectedTrackIndex >= 0 && _trackLineResizing == -1 && _startSelectedTrackIndex < _tracksModel->size()) {

        if (_areaInfo->shiftkeyPressed()) {

            if (_startSelectedClipIndex == "") {

                if (_dontChangeSelectionArea == false) {
                    if (_areaInfo->selectionArea()->firstTrackIndex() < 0) {

                        _startSelectedTime = 0;
                        createSelectedArea(nullptr, 0, _startSelectedTrackIndex, 0, _currentSelectedTime, _tracksModel->trackItem(0)->rect().top(),
                            _tracksModel->trackItem(_startSelectedTrackIndex)->rect().bottom());

                    } else {

                        _startSelectedTime = _currentSelectedTime;
                        auto startTime = _areaInfo->selectionArea()->area().left();
                        auto endTime = _areaInfo->selectionArea()->area().right();
                        auto top = _areaInfo->selectionArea()->area().top();
                        auto bottom = _areaInfo->selectionArea()->area().bottom();

                        double midX = (startTime + endTime) / 2.0;
                        double midY = (top + bottom) / 2.0;
                        if (_currentSelectedTime <= midX) {
                            startTime = _currentSelectedTime;
                        } else {
                            endTime = _currentSelectedTime;
                        }

                        if (globalPos.y() <= midY) {
                            QPair<int, int> tmp = findTrackIndexByPoint(QPoint(0, globalPos.y()));
                            int index = tmp.first;
                            if (index < 0)
                                index = 0;
                            if (index >= _tracksModel->size())
                                index = _tracksModel->size() - 1;

                            top = _tracksModel->trackItem(index)->rect().top();
                        } else {
                            QPair<int, int> tmp = findTrackIndexByPoint(QPoint(0, globalPos.y()));
                            int index = tmp.first;

                            if (index < 0)
                                index = 0;
                            if (index >= _tracksModel->size())
                                index = _tracksModel->size() - 1;

                            bottom = _tracksModel->trackItem(index)->rect().bottom();
                        }

                        createSelectedArea(nullptr, _areaInfo->selectionArea()->firstTrackIndex(), _startSelectedTrackIndex, startTime, endTime, top, bottom);
                    }
                }

            } else {

                if (_areaInfo->selectionArea()->isClipSelectedAtFirst == false) {
                    auto startTime = _areaInfo->selectionArea()->area().left();
                    auto endTime = _areaInfo->selectionArea()->area().right();
                    auto top = _areaInfo->selectionArea()->area().top();
                    auto bottom = _areaInfo->selectionArea()->area().bottom();

                    if (_areaInfo->selectionArea()->firstTrackIndex() < 0) {
                        _areaInfo->selectionArea()->setfirstTrackIndex(0);
                        _areaInfo->selectionArea()->setLastTrackIndex(_startSelectedTrackIndex - 1);
                        _areaInfo->selectionArea()->update();
                        _areaInfo->selectionArea()->last_width = _areaInfo->selectionArea()->area().width();

                        auto item = _tracksModel->trackItem(_startSelectedTrackIndex)->clipItems(_startSelectedClipIndex);
                        createSelectedArea(item, 0, _areaInfo->selectionArea()->_lastTrackIndex, startTime, endTime,
                            _tracksModel->trackItem(_areaInfo->selectionArea()->_lastTrackIndex)->rect().top(), _tracksModel->trackItem(_areaInfo->selectionArea()->_lastTrackIndex)->rect().bottom());
                    } else {

                        _areaInfo->selectionArea()->setLastTrackIndex(_startSelectedTrackIndex - 1);
                        auto item = _tracksModel->trackItem(_startSelectedTrackIndex)->clipItems(_startSelectedClipIndex);
                        createSelectedArea(item, _areaInfo->selectionArea()->firstTrackIndex(), _areaInfo->selectionArea()->_lastTrackIndex, startTime, endTime, top, bottom);
                    }
                } else {

                    _areaInfo->selectionArea()->isClipSelectedAtFirst = true;
                    _areaInfo->selectionArea()->isClipSelectedAtLast = true;

                    auto startTime = _areaInfo->selectionArea()->area().left();
                    auto endTime = _areaInfo->selectionArea()->area().right();
                    auto top = _areaInfo->selectionArea()->area().top();
                    auto bottom = _areaInfo->selectionArea()->area().bottom();
                    if (_areaInfo->selectionArea()->firstTrackIndex() < 0) {
                        _areaInfo->selectionArea()->setfirstTrackIndex(0);
                        _areaInfo->selectionArea()->setLastTrackIndex(_startSelectedTrackIndex);
                        _areaInfo->selectionArea()->update();
                        _areaInfo->selectionArea()->last_width = _areaInfo->selectionArea()->area().width();
                        auto item = _tracksModel->trackItem(_startSelectedTrackIndex)->clipItems(_startSelectedClipIndex);
                        createSelectedArea(item, 0, _areaInfo->selectionArea()->_lastTrackIndex, startTime, endTime,
                            _tracksModel->trackItem(_areaInfo->selectionArea()->_lastTrackIndex)->rect().top(), _tracksModel->trackItem(_areaInfo->selectionArea()->_lastTrackIndex)->rect().bottom());
                    } else {

                        _areaInfo->selectionArea()->setLastTrackIndex(_startSelectedTrackIndex);
                        auto item = _tracksModel->trackItem(_startSelectedTrackIndex)->clipItems(_startSelectedClipIndex);
                        createSelectedArea(item, _areaInfo->selectionArea()->firstTrackIndex(), _areaInfo->selectionArea()->_lastTrackIndex, startTime, endTime, top, bottom);
                    }
                }
            }

        } else {
            if (_startSelectedClipIndex == "") {

                if (_dontChangeSelectionArea == false) {
                    _areaInfo->selectionArea()->_firstTrackIndex = _startSelectedTrackIndex;
                    _areaInfo->selectionArea()->_lastTrackIndex = _startSelectedTrackIndex;

                    _startSelectedTime = _currentSelectedTime;
                    createSelectedArea(nullptr, _areaInfo->selectionArea()->firstTrackIndex(), _areaInfo->selectionArea()->lastTrackIndex(), _currentSelectedTime, _currentSelectedTime,
                        _tracksModel->trackItem(_startSelectedTrackIndex)->rect().top(),
                        _tracksModel->trackItem(_startSelectedTrackIndex)->rect().bottom(),
                        _firstSubTrack, _firstSubTrack);
                    _selectingAreaCreating = true;
                    _areaInfo->selectionArea()->firstPoint = globalPos;

                    if (!_tracksModel->trackItem(_startSelectedTrackIndex)->automationLaneEnabled()) {
                        _areaInfo->selectionArea()->isClipSelectedAtFirst = true;
                    } else {

                        if (_tracksModel->trackItem(_startSelectedTrackIndex)->containTitle(localPos)) {
                            _areaInfo->selectionArea()->isClipSelectedAtFirst = true;

                        } else {
                            _areaInfo->selectionArea()->isClipSelectedAtFirst = false;
                        }
                    }
                }

            } else {

                _areaInfo->selectionArea()->isClipSelectedAtFirst = true;
                _areaInfo->selectionArea()->isClipSelectedAtLast = true;
                _areaInfo->selectionArea()->setfirstSubTrackIndex(-1);

                _areaInfo->selectionArea()
                    ->setLastSubTrackIndex(_tracksModel->trackItem(_startSelectedTrackIndex)->automationLaneModel()->count() - 1);

                auto item = _tracksModel->trackItem(_startSelectedTrackIndex)->clipItems(_startSelectedClipIndex);
                if (item->mouseAction() == ClipItem::MouseAction_ResizeLeft
                    || item->mouseAction() == ClipItem::MouseAction_ResizeRight) {
                    _selectingAreaResizing = true;

                    item->mousePressEvent(&event);

                } else {

                    if (_areaInfo->selectionArea()->area().contains(QPoint(_currentSelectedTime, globalPos.y()))) {

                        _areaInfo->selectionArea()->leftDiff = _currentSelectedTime - _areaInfo->selectionArea()->area().left();
                        _areaInfo->selectionArea()->leftDiff_pixel = _currentSelectedTime - _areaInfo->selectionArea()->area().left();
                        _areaInfo->selectionArea()->topDiff = (globalPos.y()) - _areaInfo->selectionArea()->area().top();

                        _selectingAreaMoving = true;

                        _lastMouseMovingPoint = event.pos();
                        _timerCursorPosition.start();

                        clearMovingItems();

                        bool added = false;
                        for (int i = 0; i < _tracksModel->size(); i++) {

                            auto list = _tracksModel->trackItem(i)->copySelectedArea(*_areaInfo->selectionArea());

                            if (list.size() > 0) {
                                _movingItems.append(list);

                                added = true;
                            }
                        }

                        if (added)
                            Q_EMIT sigClipItemMoving();

                        _movingAutomationPoint.clear();

                        for (int i = 0; i < _tracksModel->size(); i++) {

                            _tracksModel->trackItem(i)->movingAllAutomationItems(*_areaInfo->selectionArea(), _movingAutomationPoint, i);
                        }

                        for (int i = 0; i < _movingItems.size(); i++) {
                            _areaInfo->selectionArea()->setfirstTrackIndex(_movingItems[i].trackIndex);
                            _areaInfo->selectionArea()->update();
                            _areaInfo->selectionArea()->last_width = _areaInfo->selectionArea()->area().width();
                            break;
                        }

                        for (int i = _movingItems.size() - 1; i >= 0; i--) {
                            _areaInfo->selectionArea()->setLastTrackIndex(_movingItems[i].trackIndex);
                            break;
                        }

                        _areaInfo->lastSelectedArea()->_firstTrackIndex = _areaInfo->selectionArea()->_firstTrackIndex;
                        _areaInfo->lastSelectedArea()->_lastTrackIndex = _areaInfo->selectionArea()->_lastTrackIndex;
                        _areaInfo->lastSelectedArea()->setRect(_areaInfo->selectionArea()->area());

                        _areaInfo->lastSelectedArea()->leftDiff = _areaInfo->selectionArea()->leftDiff;
                        _areaInfo->lastSelectedArea()->topDiff = _areaInfo->selectionArea()->topDiff;

                        for (int i = 0; i < _movingItems.size(); i++) {

                            if (_movingItems[i].clipItem == Q_NULLPTR) {
                                continue;
                            }

                            if (_areaInfo->selectionArea()->area().left() > _movingItems[i].clipItem->sampleClip()->startTime()
                                && _areaInfo->selectionArea()->area().right() > _movingItems[i].clipItem->sampleClip()->endTime()) {

                                double startTime = _areaInfo->selectionArea()->area().left();
                                double endTime = _areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff + _movingItems[i].length;

                                auto diff = _movingItems[i].startTimeDiff;

                                double startOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetStartFrame() + time2Frame(diff);
                                double endOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetEndFrame();

                                _movingItems[i].clipItem->setParameters(startTime, endTime, startOffset, endOffset);

                                _movingItems[i].type = 0;

                            } else if (_areaInfo->selectionArea()->area().right() < _movingItems[i].clipItem->sampleClip()->endTime()
                                && _areaInfo->selectionArea()->area().left() < _movingItems[i].clipItem->sampleClip()->startTime()) {

                                double length = _movingItems[i].endTime - _movingItems[i].startTime;
                                double startTime = _areaInfo->selectionArea()->area().left() + length + _movingItems[i].endTimeDiff - _movingItems[i].length;
                                double endTime = _areaInfo->selectionArea()->area().left() + length;

                                double diff = -_movingItems[i].endTimeDiff;

                                double startOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetStartFrame();
                                double endOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetEndFrame() + time2Frame(diff);

                                _movingItems[i].clipItem->setParameters(startTime, endTime, startOffset, endOffset);

                                _movingItems[i].type = 1;

                            } else if (_areaInfo->selectionArea()->area().right() <= _movingItems[i].clipItem->sampleClip()->endTime()
                                && _areaInfo->selectionArea()->area().left() >= _movingItems[i].clipItem->sampleClip()->startTime()) {

                                double startTime = _areaInfo->selectionArea()->area().left();
                                double length = _movingItems[i].endTime - _movingItems[i].startTime;
                                double endTime = startTime + length;

                                double diff_start = _movingItems[i].startTimeDiff;
                                double diff_end = _movingItems[i].endTimeDiff;

                                double startOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetStartFrame() + time2Frame(diff_start);

                                double endOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetEndFrame() - time2Frame(diff_end);

                                _movingItems[i].clipItem->setParameters(startTime, endTime, startOffset, endOffset);

                                _movingItems[i].type = 2;

                            } else {

                                double startTime = _areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff;
                                double endTime = startTime + _movingItems[i].length;

                                double startOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetStartFrame();
                                double endOffset = _movingItems[i].clipItem->sampleClip()->sampleBuffer()->offsetEndFrame();

                                _movingItems[i].clipItem->setParameters(startTime, endTime, startOffset, endOffset);

                                _movingItems[i].type = 3;
                            }
                        }

                    } else {
                        _selectingAreaMoving = true;

                        _lastMouseMovingPoint = event.pos();
                        _timerCursorPosition.start();

                        auto item = _tracksModel->trackItem(_startSelectedTrackIndex)->clipItems(_startSelectedClipIndex);
                        auto startTime = item->sampleClip()->startTime();
                        auto endTime = item->sampleClip()->endTime();

                        _tracksModel->trackItem(_startSelectedTrackIndex)->setSelectedClipIndex(_startSelectedClipIndex, true);

                        _areaInfo->selectionArea()->setfirstTrackIndex(_startSelectedTrackIndex);
                        _areaInfo->selectionArea()->setLastTrackIndex(_startSelectedTrackIndex);
                        _areaInfo->selectionArea()->setLeft(startTime);
                        _areaInfo->selectionArea()->setRight(endTime);

                        _areaInfo->selectionArea()->update();

                        _areaInfo->selectionArea()->last_width = _areaInfo->selectionArea()->area().width();

                        //_areaInfo->selectionArea()->area().setTop(_tracksModel->trackItem(_startSelectedTrackIndex)->rect().top());
                        //_areaInfo->selectionArea()->area().setBottom(_tracksModel->trackItem(_startSelectedTrackIndex)->rect().bottom());

                        _areaInfo->selectionArea()->leftDiff = _currentSelectedTime - _areaInfo->selectionArea()->area().left();

                        _areaInfo->selectionArea()->topDiff = (globalPos.y()) - _areaInfo->selectionArea()->area().top();

                        _areaInfo->lastSelectedArea()->_firstTrackIndex = _areaInfo->selectionArea()->_firstTrackIndex;
                        _areaInfo->lastSelectedArea()->_lastTrackIndex = _areaInfo->selectionArea()->_lastTrackIndex;
                        _areaInfo->lastSelectedArea()->setRect(_areaInfo->selectionArea()->area());
                        _areaInfo->lastSelectedArea()->leftDiff = _areaInfo->selectionArea()->leftDiff;
                        _areaInfo->lastSelectedArea()->topDiff = _areaInfo->selectionArea()->topDiff;

                        clearMovingItems();

                        bool added = false;
                        for (int i = 0; i < _tracksModel->size(); i++) {

                            auto list = _tracksModel->trackItem(i)->copySelectedArea(*_areaInfo->selectionArea());
                            if (list.size() > 0) {
                                _movingItems.append(list);
                                added = true;
                            }
                        }

                        if (added)
                            Q_EMIT sigClipItemMoving();

                        _movingAutomationPoint.clear();

                        for (int i = 0; i < _tracksModel->size(); i++) {

                            _tracksModel->trackItem(i)->movingAllAutomationItems(*_areaInfo->selectionArea(), _movingAutomationPoint, i);
                        }

                        for (int i = 0; i < _movingItems.size(); i++) {
                            if (_movingItems[i].clipItem) {
                                _movingItems[i].type = 4;
                            }
                        }
                    }
                }
            }
        }
    }

    return _automationChanging;
}

void ClipArea::sltExportRenderProgress(int p)
{
    _exportProgress = p;
    Q_EMIT exportProgressChanged();
}

void ClipArea::sltKeyPressed(int modifier, int key)
{

    if (_lastKey == key)
        return;
    _lastKey = key;
    if (modifier == Qt::ControlModifier) {
        _areaInfo->setCtrlkeyPressed(true);
    }

    if (modifier == Qt::ShiftModifier) {
        _areaInfo->setShiftkeyPressed(true);
        Q_EMIT shiftPressedChanged();
    }

    if (modifier == Qt::AltModifier) {
        _areaInfo->setAltKeyPressed(true);
    }

    // _areaInfo->setKeyPressEvent(event);

    if (key == Qt::Key_Space) {

        auto _playbackMarker = _areaInfo->arrangementStartTime();
        if (AudioManager::getSong()->isPause() == false) {

            AudioManager::getSong()->setPause();
            _areaInfo->sigPlayChanged(false);

        } else {

            if (_areaInfo->loopFocusActive()) {

                _playbackMarker = _areaInfo->startBraceMarker();
            } else {

                _areaInfo->setArrangementStartTime(_playbackMarker);
                _areaInfo->setPlayheadMarker(_playbackMarker);
            }

            _areaInfo->setArrangementStartTime(_playbackMarker);
            _areaInfo->setPlayheadMarker(_playbackMarker);

            AudioManager::getSong()->readyToPlay();

            _areaInfo->sigPlayChanged(true);
        }

        _areaInfo->setPlayheadMarkerVisible(true);

        Q_EMIT pausedChanged();
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_Z) {

        undoPerformOperation();
    }
    if ((modifier == (Qt::ControlModifier | Qt::ShiftModifier)) && key == Qt::Key_Z) {

        redoPerformOperation();
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_O) {
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_E) {

        int trackIndex = 0;
        for (int i = 0; i < _tracksModel->size(); i++) {
            if (_tracksModel->trackItem(i)->currentIndicator() >= 0
                && _tracksModel->trackItem(i)->indicatorEnabled()) {
                trackIndex = i;
                break;
            }
        }

        auto currentTime = _areaInfo->selectionArea()->area().left();
        double spixel = (currentTime - _areaInfo->startTime()) * _areaInfo->timePerPixel();

        QJsonArray jArray;
        auto clipItem = _tracksModel->trackItem(trackIndex)->findClipItemPixel(spixel);
        if (clipItem > 0) {

            QJsonArray array = _tracksModel->trackItem(trackIndex)->splitClipItem(clipItem, currentTime);
            if (array.size() > 0)
                jArray.append(array);
        }
        saveInUndoItems(jArray);
    }
    if (key == Qt::Key_Delete) {

        QJsonArray jArray;

        for (int i = 0; i < _tracksModel->size(); i++) {

            if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true) {
                _tracksModel->trackItem(i)->deleteCurrentAutomationItemInArea(*_areaInfo->selectionArea(), jArray, i);

                for (int j = 0; j < _tracksModel->trackItem(i)->automationLaneModel()->count(); j++) {
                    auto laneItem = _tracksModel->trackItem(i)->automationLaneModel()->getItem(j);
                    _tracksModel->trackItem(i)->deleteCurrentAutomationItemInArea(*_areaInfo->selectionArea(), jArray,
                        laneItem->automationLaneMenuTitle,
                        laneItem->automationLaneSubMenuTitle, i, j);
                }
            } else {

                _tracksModel->trackItem(i)->deleteAllAutomationItemsInArea(*_areaInfo->selectionArea(), jArray, i);
            }
        }
        if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true) {

            for (int i = 0; i < _tracksModel->size(); i++) {
                _tracksModel->trackItem(i)->keyPressEvent(modifier, key);
            }

            for (int i = 0; i < _tracksModel->size(); i++) {

                QJsonArray array = _tracksModel->trackItem(i)->deleteSelectedArea(*_areaInfo->selectionArea());
                if (array.size() > 0)
                    jArray.append(array);
            }

            clearMovingItems();
        }

        saveInUndoItems(jArray);
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_A) {

        auto firstTrackItem = _tracksModel->trackItem(0);
        auto startItem = firstTrackItem->getFirstClipItem();
        auto endItem = firstTrackItem->getLastClipItem();

        double startSelectedTime = -1;
        double endSelectedTime = -1;
        double startSelectedTime_automation = -1;
        double endSelectedTime_automation = -1;
        for (int i = 0; i < _tracksModel->size(); i++) {
            if (startItem == Q_NULLPTR) {
                startItem = _tracksModel->trackItem(i)->getFirstClipItem();
                if (startItem != Q_NULLPTR) {
                    break;
                }
            }
        }
        for (int i = 0; i < _tracksModel->size(); i++) {
            if (endItem == Q_NULLPTR) {
                endItem = _tracksModel->trackItem(i)->getLastClipItem();
                if (endItem != Q_NULLPTR) {
                    break;
                }
            }
        }

        if (startItem != Q_NULLPTR) {

            for (int i = 0; i < _tracksModel->size(); i++) {

                auto midItemStart = _tracksModel->trackItem(i)->getFirstClipItem();
                auto midItemEnd = _tracksModel->trackItem(i)->getLastClipItem();
                if (midItemStart != Q_NULLPTR) {
                    if (startItem->sampleClip()->startTime() > midItemStart->sampleClip()->startTime()) {
                        startItem = _tracksModel->trackItem(i)->getFirstClipItem();
                    }
                }

                if (midItemEnd) {
                    if (endItem->sampleClip()->endTime() < midItemEnd->sampleClip()->endTime()) {
                        endItem = _tracksModel->trackItem(i)->getLastClipItem();
                    }
                }
            }

            startSelectedTime = startItem->sampleClip()->startTime();
            endSelectedTime = endItem->sampleClip()->endTime();
        }

        // Finding Start Time
        for (int i = 0; i < _tracksModel->size(); i++) {

            auto result = _tracksModel->trackItem(i)->getStartTimeSelectAllAutomation();
            if ((result > 0 && result < startSelectedTime_automation) || startSelectedTime_automation < 0) {
                startSelectedTime_automation = result;
            }

            for (int j = 0; j < _tracksModel->trackItem(i)->automationLaneModel()->count(); j++) {
                result = _tracksModel->trackItem(i)->getStartTimeSelectAllAutomation(
                    _tracksModel->trackItem(i)->automationLaneModel()->getItem(j)->automationLaneMenuTitle,
                    _tracksModel->trackItem(i)->automationLaneModel()->getItem(j)->automationLaneSubMenuTitle);

                if ((result > 0 && result < startSelectedTime_automation) || startSelectedTime_automation < 0) {
                    startSelectedTime_automation = result;
                }
            }
        }

        double final_xpos = 0;

        auto pixel = _areaInfo->time2Pixel(startSelectedTime_automation);
        final_xpos = _areaInfo->findSnapLeft(pixel);

        startSelectedTime_automation = _areaInfo->pixel2Time(final_xpos);

        // Finding End Time
        for (int i = 0; i < _tracksModel->size(); i++) {

            auto result = _tracksModel->trackItem(i)->getEndTimeSelectAllAutomation();
            if ((endSelectedTime_automation < result && result > 0) || endSelectedTime_automation < 0) {
                endSelectedTime_automation = result;
            }

            for (int j = 0; j < _tracksModel->trackItem(i)->automationLaneModel()->count(); j++) {
                result = _tracksModel->trackItem(i)->getEndTimeSelectAllAutomation(
                    _tracksModel->trackItem(i)->automationLaneModel()->getItem(j)->automationLaneMenuTitle,
                    _tracksModel->trackItem(i)->automationLaneModel()->getItem(j)->automationLaneSubMenuTitle);

                if ((result > 0 && result > endSelectedTime_automation) || endSelectedTime_automation < 0) {
                    endSelectedTime_automation = result;
                }
            }
        }

        pixel = _areaInfo->time2Pixel(endSelectedTime_automation);

        final_xpos = _areaInfo->findSnapRight(pixel);

        endSelectedTime_automation = _areaInfo->pixel2Time(final_xpos);

        _areaInfo->selectionArea()->setfirstTrackIndex(0);
        _areaInfo->selectionArea()->setfirstSubTrackIndex(-1);
        _areaInfo->selectionArea()->setLastTrackIndex(_tracksModel->size() - 1);

        _areaInfo->selectionArea()->setLastSubTrackIndex(_tracksModel->trackItem(_tracksModel->size() - 1)->automationLaneModel()->count() - 1);
        _areaInfo->selectionArea()->update();

        if (startSelectedTime_automation >= 0) {
            if (startSelectedTime >= 0) {
                if (startSelectedTime_automation < startSelectedTime)
                    _areaInfo->selectionArea()->setLeft(startSelectedTime_automation);
                else
                    _areaInfo->selectionArea()->setLeft(startSelectedTime);
            } else {
                _areaInfo->selectionArea()->setLeft(startSelectedTime_automation);
            }

        } else {
            _areaInfo->selectionArea()->setLeft(startSelectedTime);
        }

        if (endSelectedTime_automation >= 0) {
            if (endSelectedTime >= 0) {
                if (endSelectedTime_automation > endSelectedTime)
                    _areaInfo->selectionArea()->setRight(endSelectedTime_automation);
                else
                    _areaInfo->selectionArea()->setRight(endSelectedTime);
            } else {
                _areaInfo->selectionArea()->setRight(endSelectedTime_automation);
            }

        } else {
            _areaInfo->selectionArea()->setRight(endSelectedTime);
        }

        _areaInfo->selectionArea()->update();

        _areaInfo->selectionArea()->isClipSelectedAtFirst = true;
        _areaInfo->selectionArea()->isClipSelectedAtLast = true;
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_C) {

        _copiedAutomationPoint.clear();
        _copeiedKeyFramesItems.clear();
        clearCopiedItems();
        for (int i = 0; i < _tracksModel->size(); i++) {

            if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true) {

                _tracksModel->trackItem(i)->copyAllAutomationItemsInArea(*_areaInfo->selectionArea(), _copiedAutomationPoint, i);
                _tracksModel->trackItem(i)->copyKeyFramesInArea(*_areaInfo->selectionArea(), _copeiedKeyFramesItems, i);

            } else {
                bool _automation_empty = _tracksModel->trackItem(i)->copyCurrentAutomationItemInArea(*_areaInfo->selectionArea(), _copiedAutomationPoint, i);
                bool _keyframes_empty = _tracksModel->trackItem(i)->copyKeyFramesItemInArea(*_areaInfo->selectionArea(), _copeiedKeyFramesItems, i);
                if (_automation_empty == true && _keyframes_empty == false) {
                    _tracksModel->trackItem(i)->copyEmptyAutomationItem(*_areaInfo->selectionArea(), _copiedAutomationPoint, i);
                }
                for (int j = 0; j < _tracksModel->trackItem(i)->automationLaneModel()->count(); j++) {

                    auto laneItem = _tracksModel->trackItem(i)->automationLaneModel()->getItem(j);
                    _automation_empty = _tracksModel->trackItem(i)->copyCurrentAutomationItemInArea(*_areaInfo->selectionArea(),
                        _copiedAutomationPoint, laneItem->automationLaneMenuTitle, laneItem->automationLaneSubMenuTitle, i, j);

                    if (_automation_empty == true) {
                        _tracksModel->trackItem(i)->copyEmptyAutomationItem(*_areaInfo->selectionArea(),
                            _copiedAutomationPoint, laneItem->automationLaneMenuTitle, laneItem->automationLaneSubMenuTitle, i, j);
                    }
                }
            }
        }

        // this section is for clipitems
        if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true) {

            for (int i = 0; i < _tracksModel->size(); i++) {
                auto list = _tracksModel->trackItem(i)->copySelectedArea(*_areaInfo->selectionArea());

                if (list.size() > 0) {
                    _copiedItems.append(list);
                }
            }
        }
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_X) {

        QJsonArray jArray;

        clearCopiedItems();
        _copiedAutomationPoint.clear();

        for (int i = 0; i < _tracksModel->size(); i++) {

            bool _empty = true;
            if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true) {
                _empty = _tracksModel->trackItem(i)->cutAllAutomationItemsInArea(*_areaInfo->selectionArea(), _copiedAutomationPoint, jArray, i);
            } else {
                _empty = _tracksModel->trackItem(i)->cutCurrentAutomationItemInArea(*_areaInfo->selectionArea(), _copiedAutomationPoint, jArray, i);
            }
            if (_empty) {
                _tracksModel->trackItem(i)->cutEmptyAutomationItem(*_areaInfo->selectionArea(), _copiedAutomationPoint, i);
            }

            for (int j = 0; j < _tracksModel->trackItem(i)->automationLaneModel()->count(); j++) {

                auto laneItem = _tracksModel->trackItem(i)->automationLaneModel()->getItem(j);
                _tracksModel->trackItem(i)->cutCurrentAutomationItemInArea(*_areaInfo->selectionArea(),
                    _copiedAutomationPoint, jArray, laneItem->automationLaneMenuTitle, laneItem->automationLaneSubMenuTitle, i, j);
            }
        }

        // this section is for clipitems
        if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true) {

            clearCopiedItems();
            for (int i = 0; i < _tracksModel->size(); i++) {
                auto list = _tracksModel->trackItem(i)->copySelectedArea(*_areaInfo->selectionArea());

                if (list.size() > 0)
                    _copiedItems.append(list);
            }

            for (int i = 0; i < _tracksModel->size(); i++) {
                QJsonArray array = _tracksModel->trackItem(i)->deleteSelectedArea(*_areaInfo->selectionArea());

                if (array.size() > 0)
                    jArray.append(array);
            }
            clearMovingItems();
        }

        saveInUndoItems(jArray);
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_V) {

        QJsonArray jArray;

        jArray.append(_areaInfo->selectionArea()->state());

        if (_copiedItems.size() > 0) {

            int first_track_index = -1;
            int init_track_index = -1;
            int new_track_index = -1;
            int last_track_index = -1;
            int base_track_indicator = -1;

            for (int i = 0; i < _tracksModel->size(); i++) {
                if (_tracksModel->trackItem(i)->currentIndicator() >= 0 && _tracksModel->trackItem(i)->indicatorEnabled()) {
                    base_track_indicator = i;
                    break;
                }
            }

            if (_pre_base_track_indicator >= 0 && base_track_indicator >= 0 && base_track_indicator == _pre_base_track_indicator) {
                if (qAbs(_tracksModel->trackItem(base_track_indicator)->currentIndicator() - _pre_paste_indicator) < 0.0001) {
                    return;
                }
            }

            if (base_track_indicator >= 0) {
                _pre_paste_indicator = _tracksModel->trackItem(base_track_indicator)->currentIndicator();
                _pre_base_track_indicator = base_track_indicator;
                new_track_index = base_track_indicator;
                QVector<QPair<int, QString>> _track_clips;
                QMap<int, QVector<QString>> _clip_list;

                for (int i = 0; i < _copiedItems.size(); i++) {

                    if (init_track_index == -1) {

                        init_track_index = _copiedItems[i].trackIndex;

                        _areaInfo->selectionArea()->setLeft(_tracksModel->trackItem(base_track_indicator)->currentIndicator());
                        _areaInfo->selectionArea()->setWidth(_areaInfo->selectionArea()->last_width);
                    }

                    if (_copiedItems[i].clipItem == Q_NULLPTR) {
                        continue;
                    }

                    new_track_index = base_track_indicator + (_copiedItems[i].clipItem->trackIndex() - init_track_index);

                    if (new_track_index >= _tracksModel->size() || new_track_index < 0)
                        continue;

                    if (first_track_index == -1) {
                        first_track_index = new_track_index;
                    }
                    last_track_index = new_track_index;

                    auto newItem = _copiedItems[i].clipItem->clone(_tracksModel->trackItem(new_track_index)->track());

                    if (_copiedItems[i].startTime > _copiedItems[i].clipItem->sampleClip()->startTime()
                        && _copiedItems[i].endTime > _copiedItems[i].clipItem->sampleClip()->endTime()) {

                        double startTime = _tracksModel->trackItem(base_track_indicator)->currentIndicator();
                        double endTime = _tracksModel->trackItem(base_track_indicator)->currentIndicator() - _copiedItems[i].startTimeDiff + _copiedItems[i].length;

                        auto diff = _copiedItems[i].startTimeDiff;

                        double startOffset = newItem->sampleClip()->sampleBuffer()->offsetStartFrame() + diff / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        double endOffset = newItem->sampleClip()->sampleBuffer()->offsetEndFrame();
                        auto clipIndex = _tracksModel->trackItem(new_track_index)->addSampleClip(startTime, endTime, startOffset, endOffset, newItem);

                        QJsonObject jObject;
                        jObject["clip_index"] = clipIndex;
                        jObject["operation"] = ActionStack::Operation_InsertClipItem;
                        jObject["track_index"] = new_track_index;
                        jArray.append(jObject);

                        _track_clips.append(QPair<int, QString>(new_track_index, clipIndex));
                        _clip_list[new_track_index].append(clipIndex);

                    } else if (_copiedItems[i].endTime < _copiedItems[i].clipItem->sampleClip()->endTime()
                        && _copiedItems[i].startTime < _copiedItems[i].clipItem->sampleClip()->startTime()) {

                        double length = _copiedItems[i].endTime - _copiedItems[i].startTime;
                        double startTime = _tracksModel->trackItem(base_track_indicator)->currentIndicator() + length + _copiedItems[i].endTimeDiff - _copiedItems[i].length;
                        double endTime = _tracksModel->trackItem(base_track_indicator)->currentIndicator() + length;

                        double diff = -_copiedItems[i].endTimeDiff;

                        double startOffset = newItem->sampleClip()->sampleBuffer()->offsetStartFrame();
                        double endOffset = newItem->sampleClip()->sampleBuffer()->offsetEndFrame() + diff / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        auto clipIndex = _tracksModel->trackItem(new_track_index)->addSampleClip(startTime, endTime, startOffset, endOffset, newItem);

                        QJsonObject jObject;
                        jObject["clip_index"] = clipIndex;
                        jObject["operation"] = ActionStack::Operation_InsertClipItem;
                        jObject["track_index"] = new_track_index;
                        jArray.append(jObject);

                        _track_clips.append(QPair<int, QString>(new_track_index, clipIndex));
                        _clip_list[new_track_index].append(clipIndex);

                    } else if (_copiedItems[i].endTime <= _copiedItems[i].clipItem->sampleClip()->endTime()
                        && _copiedItems[i].startTime >= _copiedItems[i].clipItem->sampleClip()->startTime()) {

                        double startTime = _tracksModel->trackItem(base_track_indicator)->currentIndicator();
                        double length = _copiedItems[i].endTime - _copiedItems[i].startTime;
                        double endTime = startTime + length;

                        double diff_start = _copiedItems[i].startTimeDiff;
                        double diff_end = _copiedItems[i].endTimeDiff;

                        double startOffset = newItem->sampleClip()->sampleBuffer()->offsetStartFrame()
                            + diff_start / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        double endOffset = newItem->sampleClip()->sampleBuffer()->offsetEndFrame()
                            - diff_end / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        auto clipIndex = _tracksModel->trackItem(new_track_index)->addSampleClip(startTime, endTime, startOffset, endOffset, newItem);

                        QJsonObject jObject;
                        jObject["clip_index"] = clipIndex;
                        jObject["operation"] = ActionStack::Operation_InsertClipItem;
                        jObject["track_index"] = new_track_index;
                        jArray.append(jObject);

                        _track_clips.append(QPair<int, QString>(new_track_index, clipIndex));
                        _clip_list[new_track_index].append(clipIndex);

                    } else {

                        double startTime = _tracksModel->trackItem(base_track_indicator)->currentIndicator() - _copiedItems[i].startTimeDiff;
                        double endTime = startTime + _copiedItems[i].length;

                        double startOffset = newItem->sampleClip()->sampleBuffer()->offsetStartFrame();
                        double endOffset = newItem->sampleClip()->sampleBuffer()->offsetEndFrame();

                        auto clipIndex = _tracksModel->trackItem(new_track_index)->addSampleClip(startTime, endTime, startOffset, endOffset, newItem);

                        QJsonObject jObject;
                        jObject["clip_index"] = clipIndex;
                        jObject["operation"] = ActionStack::Operation_InsertClipItem;
                        jObject["track_index"] = new_track_index;
                        jArray.append(jObject);

                        _track_clips.append(QPair<int, QString>(new_track_index, clipIndex));
                        _clip_list[new_track_index].append(clipIndex);
                    }
                }

                QMapIterator<int, QVector<QString>> itr(_clip_list);
                while (itr.hasNext()) {
                    itr.next();
                    auto clist = itr.value();
                    int trackIndex = itr.key();

                    for (int i = 0; i < clist.size(); i++) {

                        double start = _tracksModel->trackItem(trackIndex)->clipItems(clist[i])->sampleClip()->startTime();
                        double end = _tracksModel->trackItem(trackIndex)->clipItems(clist[i])->sampleClip()->endTime();
                        auto remove_list = _tracksModel->trackItem(trackIndex)->findOverlayItems(start, end);

                        for (int j = 0; j < remove_list.size(); j++) {
                            if (!clist.contains(remove_list[j])) {
                                auto array = _tracksModel->trackItem(trackIndex)->cutClipItem(remove_list[j], start, end);
                                jArray.append(array);
                            }
                        }
                    }
                }
                jArray.append(_areaInfo->selectionArea()->state());

                for (int i = 0; i < _track_clips.size(); i++) {
                    auto trackIndex = _track_clips[i].first;
                    auto clipIndex = _track_clips[i].second;
                    playbackSampleClipPositionChanged(trackIndex, clipIndex);
                }
            }

            if (_areaInfo->selectionArea()->area().right() > _areaInfo->clipDuration()) {
                if (_areaInfo->selectionArea()->area().right() > _areaInfo->maxEndTime()) {
                    _areaInfo->setMaxEndTime(_areaInfo->selectionArea()->area().right());
                }
            }

            _areaInfo->selectionArea()->isClipSelectedAtFirst = true;
            _areaInfo->selectionArea()->isClipSelectedAtLast = true;

            if (first_track_index <= last_track_index) {
                _areaInfo->selectionArea()->setfirstTrackIndex(first_track_index);
                _areaInfo->selectionArea()->setLastTrackIndex(last_track_index);
            } else {
                _areaInfo->selectionArea()->setfirstTrackIndex(last_track_index);
                _areaInfo->selectionArea()->setLastTrackIndex(first_track_index);
            }

            _areaInfo->selectionArea()->setfirstSubTrackIndex(-1);

            _areaInfo->selectionArea()->setLastSubTrackIndex(-1);

            _areaInfo->selectionArea()->update();
        }

        double selectedWidth;

        if (_copeiedKeyFramesItems.size() > 0) {
            qDebug() << "_copeiedKeyFramesItems" << _copeiedKeyFramesItems.size();
        }
        if (_copiedAutomationPoint.size() > 0) {
            int base_track_indicator = -1;
            int base_subtrack_indicator = -1;
            int first_track_index = -1;
            int first_subtrack_index = -1;
            int new_track_index = -1;
            int new_subtrack_index = -1;
            for (int i = 0; i < _tracksModel->size(); i++) {
                if (_tracksModel->trackItem(i)->currentIndicator() >= 0) {

                    if (_tracksModel->trackItem(i)->indicatorEnabled() == TrackItem::IndicatorType_Track) {
                        base_track_indicator = i;
                        base_subtrack_indicator = -1;

                        break;
                    }
                    if (_tracksModel->trackItem(i)->indicatorEnabled() == TrackItem::IndicatorType_Lane) {
                        base_track_indicator = i;
                        base_subtrack_indicator = _tracksModel->trackItem(i)->automationLaneModel()->currentSelected();

                        selectedWidth = _tracksModel->trackItem(base_track_indicator)->volumeAutomationItem()->automation()->getEndMarker()
                            - _tracksModel->trackItem(base_track_indicator)->volumeAutomationItem()->automation()->getStartMarker();

                        break;
                    }
                }
            }

            for (int i = 0; i < _tracksModel->size(); i++) {
                _tracksModel->trackItem(i)->clearCopyList();
            }

            int last_sub_track = -1;
            bool dont_copy = true;

            for (int i = 0; i < _copiedAutomationPoint.size(); i++) {
                if (first_track_index == -1) {

                    first_track_index = _copiedAutomationPoint[i].trackIndex;
                    first_subtrack_index = _copiedAutomationPoint[i].subtrackIndex;
                }
                auto type = _copiedAutomationPoint[i].type;
                auto subType = _copiedAutomationPoint[i].sub_type;

                if (_tracksModel->trackItem(first_track_index)->automationItem(type, subType))
                    selectedWidth = _tracksModel->trackItem(first_track_index)->automationItem(type, subType)->automation()->getEndMarker()
                        - _tracksModel->trackItem(first_track_index)->automationItem(type, subType)->automation()->getStartMarker();

                new_track_index = base_track_indicator + (_copiedAutomationPoint[i].trackIndex - first_track_index);
                new_subtrack_index = base_subtrack_indicator + (_copiedAutomationPoint[i].subtrackIndex - first_subtrack_index);

                if (new_track_index >= 0 && new_track_index < _tracksModel->size()) {

                    if (new_subtrack_index == -1) {
                        if (_tracksModel->trackItem(new_track_index)->automationSubMenuTitle() == "None"
                            && _copiedAutomationPoint[i].type != "None") {
                            break;
                        }
                    }
                    dont_copy = false;

                    if (_areaInfo->selectionArea()->isClipSelectedAtFirst && _areaInfo->selectionArea()->isClipSelectedAtLast) {

                        _tracksModel->trackItem(new_track_index)
                            ->setCopyList2(_copiedAutomationPoint[i].aline, _copiedAutomationPoint[i].type, _copiedAutomationPoint[i].sub_type);

                        last_sub_track = _tracksModel->trackItem(new_track_index)->automationLaneModel()->getIndex(_copiedAutomationPoint[i].type, _copiedAutomationPoint[i].sub_type);

                    } else {
                        if (new_subtrack_index >= -1 && new_subtrack_index < _tracksModel->trackItem(new_track_index)->automationLaneModel()->count()) {
                            if (_tracksModel->trackItem(new_track_index)->automationLaneEnabled()) {

                                _tracksModel->trackItem(new_track_index)->setCopyList(_copiedAutomationPoint[i].aline, new_subtrack_index);
                                last_sub_track = new_subtrack_index;
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < _tracksModel->size(); i++) {

                if (_areaInfo->selectionArea()->isClipSelectedAtFirst && _areaInfo->selectionArea()->isClipSelectedAtLast) {
                    _tracksModel->trackItem(i)->insertAllAutomationItems(jArray, i, dont_copy);
                } else {
                    _tracksModel->trackItem(i)->insertCurrentAutomationItem(jArray, i, dont_copy);
                }

                for (int j = 0; j < _tracksModel->trackItem(i)->automationLaneModel()->count(); j++) {

                    auto laneItem = _tracksModel->trackItem(i)->automationLaneModel()->getItem(j);

                    _tracksModel->trackItem(i)->insertCurrentAutomationItemInArea(jArray, laneItem->automationLaneMenuTitle,
                        laneItem->automationLaneSubMenuTitle, i, j);
                }
            }

            if (dont_copy == false) {
                _areaInfo->selectionArea()->setLeft(_tracksModel->trackItem(base_track_indicator)->currentIndicator());
                _areaInfo->selectionArea()->setWidth(selectedWidth);

                _areaInfo->selectionArea()->setfirstTrackIndex(base_track_indicator);
                _areaInfo->selectionArea()->setfirstSubTrackIndex(base_subtrack_indicator);

                _areaInfo->selectionArea()->setLastTrackIndex(base_track_indicator + _copiedAutomationPoint.last().trackIndex - _copiedAutomationPoint.first().trackIndex);

                _areaInfo->selectionArea()->setLastSubTrackIndex(last_sub_track);

                _areaInfo->selectionArea()->update();
            }

            if (_areaInfo->selectionArea()->area().right() > _areaInfo->clipDuration()) {
                if (_areaInfo->selectionArea()->area().right() > _areaInfo->maxEndTime()) {
                    _areaInfo->setMaxEndTime(_areaInfo->selectionArea()->area().right());
                }
            }
        }

        saveInUndoItems(jArray);
    }

    if (key == Qt::Key_D) {

        QJsonArray jArray;
        _duplicatedAutomationPoint.clear();
        jArray.append(_areaInfo->selectionArea()->state());
        for (int i = 0; i < _tracksModel->size(); i++) {

            if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true)
                _tracksModel->trackItem(i)->duplicateAllAutomationItems(*_areaInfo->selectionArea(), _duplicatedAutomationPoint, jArray, i, -1);
            else
                _tracksModel->trackItem(i)->duplicateCurrentAutomationItem(*_areaInfo->selectionArea(), _duplicatedAutomationPoint, jArray, i, -1);

            for (int j = 0; j < _tracksModel->trackItem(i)->automationLaneModel()->count(); j++) {

                auto laneItem = _tracksModel->trackItem(i)->automationLaneModel()->getItem(j);
                _tracksModel->trackItem(i)->duplicateCurrentAutomationItemInArea(*_areaInfo->selectionArea(), _duplicatedAutomationPoint,
                    jArray, i, j, laneItem->automationLaneMenuTitle, laneItem->automationLaneSubMenuTitle);
            }
        }

        clearDuplicatedItems();
        for (int i = 0; i < _tracksModel->size(); i++) {

            auto blist = _tracksModel->trackItem(i)->copySelectedArea(*_areaInfo->selectionArea());
            if (blist.size() > 0)
                _duplicatedItems.append(blist);
        }

        if (_duplicatedAutomationPoint.size() > 0) {

            for (int i = 0; i < _tracksModel->size(); i++) {
                _tracksModel->trackItem(i)->clearCopyList();
            }

            double currentIndicator = -1;
            double endIndicator = -1;
            bool first = true;
            for (int i = 0; i < _duplicatedAutomationPoint.size(); i++) {
                if (first) {
                    first = false;
                    currentIndicator = _areaInfo->selectionArea()->area().right();
                    endIndicator = currentIndicator + (_areaInfo->selectionArea()->area().right() - _areaInfo->selectionArea()->area().left());
                }
                int trackIndex = _duplicatedAutomationPoint[i].trackIndex;
                int subTrackIndex = _duplicatedAutomationPoint[i].subtrackIndex;

                if (trackIndex >= 0 && trackIndex < _tracksModel->size()) {
                    _tracksModel->trackItem(trackIndex)->setCopyList(_duplicatedAutomationPoint[i].aline, subTrackIndex);
                }
            }

            _areaInfo->selectionArea()->setLeft(currentIndicator);
            _areaInfo->selectionArea()->setRight(endIndicator);

            for (int i = 0; i < _tracksModel->size(); i++) {

                _tracksModel->trackItem(i)->insertAllAutomationItems(jArray, i, false);
                for (int j = 0; j < _tracksModel->trackItem(i)->automationLaneModel()->count(); j++) {

                    auto laneItem = _tracksModel->trackItem(i)->automationLaneModel()->getItem(j);

                    _tracksModel->trackItem(i)->insertCurrentAutomationItemInArea(jArray, laneItem->automationLaneMenuTitle,
                        laneItem->automationLaneSubMenuTitle, i, j);
                }
            }

            if (endIndicator - _areaInfo->startTime() > _areaInfo->clipDuration()) {

                double offset = _areaInfo->stepPlotDuration() * 4;

                _areaInfo->setClipProperties(_areaInfo->endTime() - _areaInfo->clipDuration(), endIndicator + offset, _areaInfo->clipDuration());

                if (endIndicator > _areaInfo->maxEndTime()) {
                    _areaInfo->setMaxEndTime(endIndicator);
                }
            }
        }

        if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true) {

            if (_duplicatedItems.size() > 0) {

                bool first = true;

                double currentIndicator = -1;
                double endIndicator = -1;
                QVector<QPair<int, QString>> _track_clips;
                QMap<int, QVector<QString>> _clip_list;

                for (int i = 0; i < _duplicatedItems.size(); i++) {

                    if (_duplicatedItems[i].clipItem) {
                        continue;
                    }
                    if (first) {
                        first = false;
                        currentIndicator = _duplicatedItems[i].endTime;
                        endIndicator = currentIndicator + (_duplicatedItems[i].endTime - _duplicatedItems[i].startTime);
                    }

                    int trackIndex = _duplicatedItems[i].clipItem->trackIndex();

                    auto newItem = _duplicatedItems[i].clipItem->clone(_tracksModel->trackItem(trackIndex)->track());

                    if (_duplicatedItems[i].startTime > _duplicatedItems[i].clipItem->sampleClip()->startTime()
                        && _duplicatedItems[i].endTime > _duplicatedItems[i].clipItem->sampleClip()->endTime()) {

                        double startTime = currentIndicator;
                        double endTime = currentIndicator - _duplicatedItems[i].startTimeDiff + _duplicatedItems[i].length;

                        auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        auto diff = _duplicatedItems[i].startTimeDiff;

                        QString clipIndex = _tracksModel->trackItem(trackIndex)->addSampleClip(currentIndicator, newItem);

                        _track_clips.append(QPair<int, QString>(trackIndex, clipIndex));
                        _clip_list[trackIndex].append(clipIndex);

                        newItem->sampleClip()->sampleBuffer()->setOffsetStartFrame(newItem->sampleClip()->sampleBuffer()->offsetStartFrame()
                            + diff / 1000.0 * AudioManager::audioEngine()->outputSampleRate());
                        newItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                        newItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

                        newItem->sampleClip()->setStartTime(startTime);
                        newItem->sampleClip()->setEndTime(endTime);

                        newItem->sampleClip()->setStartTimeOffset(newItem->sampleClip()->sampleBuffer()->offsetStartFrame() / 256.0);
                        newItem->sampleClip()->movePosition(startFrame / 256.0);
                        newItem->sampleClip()->changeLength((endFrame - startFrame) / 256.0);

                        QJsonObject jObject;
                        jObject["clip_index"] = clipIndex;
                        jObject["operation"] = ActionStack::Operation_InsertClipItem;
                        jObject["track_index"] = trackIndex;
                        jArray.append(jObject);

                    } else if (_duplicatedItems[i].endTime < _duplicatedItems[i].clipItem->sampleClip()->endTime()
                        && _duplicatedItems[i].startTime < _duplicatedItems[i].clipItem->sampleClip()->startTime()) {

                        double length = _duplicatedItems[i].endTime - _duplicatedItems[i].startTime;
                        double startTime = currentIndicator + length + _duplicatedItems[i].endTimeDiff - _duplicatedItems[i].length;
                        double endTime = currentIndicator + length;

                        auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        double diff = -_duplicatedItems[i].endTimeDiff;

                        auto clipIndex = _tracksModel->trackItem(trackIndex)->addSampleClip(currentIndicator + length, newItem);
                        _track_clips.append(QPair<int, QString>(trackIndex, clipIndex));
                        _clip_list[trackIndex].append(clipIndex);
                        newItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                        newItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

                        newItem->sampleClip()->sampleBuffer()->setOffsetEndFrame(newItem->sampleClip()->sampleBuffer()->offsetEndFrame()
                            + diff / 1000.0 * AudioManager::audioEngine()->outputSampleRate());

                        newItem->sampleClip()->setStartTime(startTime);
                        newItem->sampleClip()->setEndTime(endTime);

                        newItem->sampleClip()->movePosition(startFrame / 256.0);
                        newItem->sampleClip()->changeLength((endFrame - startFrame) / 256.0);

                        QJsonObject jObject;
                        jObject["clip_index"] = clipIndex;
                        jObject["operation"] = ActionStack::Operation_InsertClipItem;
                        jObject["track_index"] = trackIndex;
                        jArray.append(jObject);

                    } else if (_duplicatedItems[i].endTime < _duplicatedItems[i].clipItem->sampleClip()->endTime()
                        && _duplicatedItems[i].startTime > _duplicatedItems[i].clipItem->sampleClip()->startTime()) {

                        double length = _duplicatedItems[i].endTime - _duplicatedItems[i].startTime;
                        double startTime = currentIndicator;
                        double endTime = currentIndicator + length;

                        auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        double diff_start = _duplicatedItems[i].startTimeDiff;
                        double diff_end = _duplicatedItems[i].endTimeDiff;

                        auto clipIndex = _tracksModel->trackItem(trackIndex)->addSampleClip(currentIndicator, newItem);
                        _track_clips.append(QPair<int, QString>(trackIndex, clipIndex));
                        _clip_list[trackIndex].append(clipIndex);
                        newItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                        newItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

                        newItem->sampleClip()->sampleBuffer()->setOffsetStartFrame(newItem->sampleClip()->sampleBuffer()->offsetStartFrame()
                            + diff_start / 1000.0 * AudioManager::audioEngine()->outputSampleRate());

                        newItem->sampleClip()->sampleBuffer()->setOffsetEndFrame(newItem->sampleClip()->sampleBuffer()->offsetEndFrame()
                            - diff_end / 1000.0 * AudioManager::audioEngine()->outputSampleRate());

                        newItem->sampleClip()->setStartTime(startTime);
                        newItem->sampleClip()->setEndTime(endTime);

                        newItem->sampleClip()->setStartTimeOffset((newItem->sampleClip()->sampleBuffer()->offsetStartFrame()) / 256.0);
                        newItem->sampleClip()->movePosition(startFrame / 256.0);
                        newItem->sampleClip()->changeLength((endFrame - startFrame) / 256.0);

                        QJsonObject jObject;
                        jObject["clip_index"] = clipIndex;
                        jObject["operation"] = ActionStack::Operation_InsertClipItem;
                        jObject["track_index"] = trackIndex;
                        jArray.append(jObject);

                    } else {

                        double startTime = currentIndicator - _duplicatedItems[i].startTimeDiff;
                        double endTime = startTime + _duplicatedItems[i].length;

                        auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        auto clipIndex = _tracksModel->trackItem(trackIndex)->addSampleClip(currentIndicator - _duplicatedItems[i].startTimeDiff, newItem);
                        _track_clips.append(QPair<int, QString>(trackIndex, clipIndex));
                        _clip_list[trackIndex].append(clipIndex);
                        newItem->sampleClip()->setStartTime(startTime);
                        newItem->sampleClip()->setEndTime(endTime);

                        newItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                        newItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

                        newItem->sampleClip()->movePosition(startFrame / 256.0);
                        newItem->sampleClip()->changeLength((endFrame - startFrame) / 256.0);

                        QJsonObject jObject;
                        jObject["clip_index"] = clipIndex;
                        jObject["operation"] = ActionStack::Operation_InsertClipItem;
                        jObject["track_index"] = trackIndex;
                        jArray.append(jObject);
                    }
                }

                QMapIterator<int, QVector<QString>> itr(_clip_list);
                while (itr.hasNext()) {
                    itr.next();
                    auto clist = itr.value();
                    int trackIndex = itr.key();

                    for (int i = 0; i < clist.size(); i++) {

                        double start = _tracksModel->trackItem(trackIndex)->clipItems(clist[i])->sampleClip()->startTime();
                        double end = _tracksModel->trackItem(trackIndex)->clipItems(clist[i])->sampleClip()->endTime();
                        auto remove_list = _tracksModel->trackItem(trackIndex)->findOverlayItems(start, end);

                        for (int j = 0; j < remove_list.size(); j++) {
                            if (!clist.contains(remove_list[j])) {
                                auto array = _tracksModel->trackItem(trackIndex)->cutClipItem(remove_list[j], start, end);
                                jArray.append(array);
                            }
                        }
                    }
                }

                if (endIndicator - _areaInfo->startTime() > _areaInfo->clipDuration()) {

                    double offset = _areaInfo->stepPlotDuration() * 4;

                    _areaInfo->setClipProperties(_areaInfo->endTime() - _areaInfo->clipDuration(), endIndicator + offset, _areaInfo->clipDuration());

                    if (endIndicator > _areaInfo->maxEndTime()) {
                        _areaInfo->setMaxEndTime(endIndicator);
                    }
                }

                for (int i = 0; i < _track_clips.size(); i++) {
                    auto trackIndex = _track_clips[i].first;
                    auto clipIndex = _track_clips[i].second;
                    playbackSampleClipPositionChanged(trackIndex, clipIndex);
                }

                _areaInfo->selectionArea()->setLeft(currentIndicator);
                _areaInfo->selectionArea()->setRight(endIndicator);
            }
        }
        saveInUndoItems(jArray);
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_S) {

        if (_project_name != "") {
            saveAsyncProject(_savedObjects, _project_name);
        } else {
            Q_EMIT sigSaveAsProject();
        }
    }
    if (modifier == Qt::ControlModifier && key == Qt::Key_L) {

        if (AudioManager::getSong()->isPause() == false) {
            AudioManager::getSong()->setPause();
        }
        Q_EMIT sigLoadProject();
    }
    _areaInfo->updateUI();
}

void ClipArea::saveProject(QStringList object, QString project_name)
{
    QJsonObject obj;
    QJsonArray tracks_obj;
    for (int trackIndex = 0; trackIndex < _tracksModel->size(); trackIndex++) {

        QJsonObject track;
        QJsonArray plugins_obj;
        QJsonArray automation_array;
        QJsonArray clips;
        auto trackItem = _tracksModel->trackItem(trackIndex);

        if (trackIndex < object.size()) {
            QByteArray byteArray = object[trackIndex].toUtf8();
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(byteArray, &parseError);

            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()) {
                    QJsonObject obj = jsonDoc.object();
                    track["objects"] = obj;
                } else {
                    track["objects"] = QJsonObject();
                }
            } else {
                track["objects"] = QJsonObject();
            }
        } else {
            track["objects"] = QJsonObject();
        }

        QStringList main_effect = { "Volume", "Pan", "SpeakerOn" };
        for (int j = 0; j < main_effect.size(); j++) {

            QString menu = "Mixer";
            QString subMenu = main_effect[j];
            QJsonObject automation_obj;
            automation_obj["menu"] = menu;
            automation_obj["sub_menu"] = subMenu;
            automation_obj["track_index"] = trackIndex;
            automation_obj["sub_track_index"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->getSubTrackIndex();
            automation_obj["type"] = "";
            automation_obj["state"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->getLinesState();
            automation_obj["active"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->isAutomated();
            automation_obj["enabled"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->enabled();
            automation_obj["normalized"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->normalized();
            automation_array.append(automation_obj);
        }

        track["automations"] = automation_array;

        // track["keyframes"] = _tracksModel->trackItem(trackIndex)->keyFramesItem()->save();
        // track["object_enabled"] = _tracksModel->trackItem(trackIndex)->objectEnabled();

        QMapIterator<QString, QSharedPointer<ClipItem>> i(trackItem->getClipItems());
        while (i.hasNext()) {
            i.next();

            auto rect = i.value()->rect();
            QJsonObject clip;
            clip["file_name"] = i.value()->sampleClip()->sampleBuffer()->audioFile();
            clip["base_start_time"] = i.value()->sampleClip()->baseStartTime();
            clip["base_end_time"] = i.value()->sampleClip()->baseEndTime();
            clip["start_time"] = i.value()->sampleClip()->startTime();
            clip["end_time"] = i.value()->sampleClip()->endTime();

            clip["rect"] = QString("%1, %2, %3, %4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
            clip["file_duration"] = i.value()->sampleClip()->currentFileDuration();
            clip["offset_start"] = i.value()->sampleClip()->sampleBuffer()->offsetStartFrame();
            clip["offset_end"] = i.value()->sampleClip()->sampleBuffer()->offsetEndFrame();
            clips.append(clip);
        }

        auto sampleTrack = (SampleTrack*)_tracksModel->trackItem(trackIndex)->track();
        auto effect_count = sampleTrack->audioPort()->effects()->size();

        for (int effectIndex = 0; effectIndex < effect_count; effectIndex++) {
            auto effect = sampleTrack->audioPort()->effects()->effect(effectIndex);

            if (effect) {

                if (effect->type() == "Vst3") {
                    QJsonObject vst3_json;
                    auto vst3 = effect.dynamicCast<Vst3Effect>();
                    vst3_json["plugin_type"] = effect->type();
                    vst3_json["plugin_name"] = vst3->pluginName();
                    vst3_json["plugin_index"] = vst3->pluginIndex();
                    vst3_json["plugin_path"] = vst3->pluginPath();
                    vst3_json["track_index"] = trackIndex;
                    vst3_json["plugin_enabled"] = vst3->isEnabled();

                    auto list = effect->controls()->shortTitleList();

                    QJsonArray plugin_automations;
                    for (int j = 0; j < list.size(); j++) {
                        QString menu = effect->controls()->name();
                        QString subMenu = list[j];

                        QJsonObject automation_obj;
                        automation_obj["menu"] = menu;
                        automation_obj["sub_menu"] = subMenu;
                        automation_obj["track_index"] = trackIndex;
                        automation_obj["sub_track_index"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->getSubTrackIndex();
                        automation_obj["type"] = effect->type();
                        automation_obj["active"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->isAutomated();
                        automation_obj["state"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->getLinesState();
                        automation_obj["enabled"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->enabled();
                        automation_obj["normalized"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->normalized();
                        automation_obj["show_envelopes"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->showEnvelopes();

                        plugin_automations.append(automation_obj);
                    }
                    vst3_json["automations"] = plugin_automations;

                    QJsonArray configured_parameters;
                    auto parameters = vst3->parameters();
                    for (int j = 0; j < parameters.size(); j++) {
                        if (parameters[j]._configured)
                            configured_parameters.append(parameters[j]._id);
                    }
                    vst3_json["configured_parameters"] = configured_parameters;
                    plugins_obj.append(vst3_json);
                }
                if (effect->type() == "EQ") {
                    QJsonObject vst3_json;
                    auto multiEqEffect = effect.dynamicCast<MultiEqEffect>();
                    vst3_json = multiEqEffect->getLastStatus();

                    auto list = effect->controls()->shortTitleList();

                    QJsonArray plugin_automations;
                    for (int j = 0; j < list.size(); j++) {
                        QString menu = effect->controls()->name();
                        QString subMenu = list[j];

                        QJsonObject automation_obj;
                        automation_obj["menu"] = menu;
                        automation_obj["sub_menu"] = subMenu;
                        automation_obj["track_index"] = trackIndex;
                        automation_obj["sub_track_index"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->getSubTrackIndex();
                        automation_obj["type"] = effect->type();
                        automation_obj["active"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->isAutomated();
                        automation_obj["state"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->getLinesState();
                        automation_obj["enabled"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->enabled();
                        automation_obj["normalized"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->normalized();
                        automation_obj["show_envelopes"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->showEnvelopes();

                        plugin_automations.append(automation_obj);
                    }

                    vst3_json["automations"] = plugin_automations;
                    plugins_obj.append(vst3_json);
                }
            }
        }

        if (sampleTrack->audioPort()->visrHelper()) {
            QJsonObject vst3_json;

            auto visrEffect = sampleTrack->audioPort()->visrEfect().dynamicCast<VisrEffect>();

            vst3_json["track_index"] = trackIndex;
            vst3_json["reverb_enabled"] = visrEffect->reverbEnabled();
            vst3_json["plugin_enabled"] = visrEffect->isEnabled();
            vst3_json["plugin_type"] = "Visr";

            auto list = visrEffect->controls()->shortTitleList();

            QJsonArray plugin_automations;
            for (int j = 0; j < list.size(); j++) {
                QString menu = visrEffect->controls()->name();
                QString subMenu = list[j];

                QJsonObject automation_obj;
                automation_obj["menu"] = menu;
                automation_obj["sub_menu"] = subMenu;
                automation_obj["track_index"] = trackIndex;
                automation_obj["sub_track_index"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->getSubTrackIndex();
                automation_obj["type"] = "visr";
                automation_obj["active"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->isAutomated();
                automation_obj["state"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->getLinesState();
                automation_obj["enabled"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->enabled();
                automation_obj["normalized"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->automation()->normalized();
                automation_obj["show_envelopes"] = _tracksModel->trackItem(trackIndex)->automationItem(menu, subMenu)->showEnvelopes();

                plugin_automations.append(automation_obj);
            }

            vst3_json["automations"] = plugin_automations;
            plugins_obj.append(vst3_json);
        }
        track["automation_menu"] = _tracksModel->trackItem(trackIndex)->getAutomationMenuTitle();
        track["automation_sub_menu"] = _tracksModel->trackItem(trackIndex)->automationSubMenuTitle();
        track["automation_lane_enabled"] = _tracksModel->trackItem(trackIndex)->automationLaneEnabled();
        track["plugins"] = plugins_obj;
        track["index"] = trackIndex;
        track["clips"] = clips;

        track["color"] = QVariant(_tracksModel->trackItem(trackIndex)->getTitleColor()).toString();
        tracks_obj.append(track);
    }
    obj["version"] = "0.1.0";
    obj["updated_at"] = QDateTime::currentDateTime().toMSecsSinceEpoch();
    obj["tracks"] = tracks_obj;
    obj["max_end_time"] = _areaInfo->maxEndTime();
    obj["min_start_time"] = _areaInfo->minStartTime();

    QJsonDocument doc(obj);

    // Write the JSON data to a file
    QFile file(project_name);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing: project.json";
        return;
    }

    file.write(doc.toJson());
    file.close();
}
void ClipArea::loadProject(QString project_name)
{

    QFile file(project_name);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file for reading: project.json";
        return;
    }

    while (_tracksModel->size() > 0) {
        removeSampleClip(0);
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    QJsonObject main_obj = doc.object();

    _loadedObjects.clear();
    _areaInfo->setMaxEndTime(main_obj["max_end_time"].toDouble());
    _areaInfo->setMinStartTime(main_obj["min_start_time"].toDouble());
    if (main_obj.contains("tracks")) {

        QJsonArray tracks = main_obj["tracks"].toArray();

        for (int i = 0; i < tracks.size(); i++) {

            addSampleClip();

            _tracksModel->setObjectModelEnabled(i, false);
            if (tracks[i].toObject().contains("objects")) {
                auto obj = tracks[i].toObject()["objects"].toObject();
                QJsonDocument doc(obj);
                _loadedObjects.append(doc.toJson(QJsonDocument::Compact));
            }

            if (tracks[i].toObject().contains("color")) {
                auto color_obj = tracks[i].toObject()["color"].toString();
                QColor color;
                color.setNamedColor(color_obj);
                _tracksModel->trackItem(i)->setTitleColor(color);
            }

            if (tracks[i].toObject().contains("clips")) {
                QJsonArray clips = tracks[i].toObject()["clips"].toArray();

                for (int j = 0; j < clips.size(); j++) {
                    auto clip = clips[j].toObject();

                    auto startTime = clip["start_time"].toDouble();
                    auto endTime = clip["end_time"].toDouble();
                    auto offsetStart = clip["offset_start"].toDouble();
                    auto offsetEnd = clip["offset_end"].toDouble();
                    auto baseStartTime = clip["base_start_time"].toDouble();
                    auto baseEndTime = clip["base_end_time"].toDouble();
                    auto file_duration = clip["file_duration"].toDouble();

                    auto rect_str = (clip["rect"]).toString();
                    QStringList list1 = rect_str.split(',');
                    QRect rect = QRect(QString(list1[0]).toInt(), QString(list1[1]).toInt(), QString(list1[2]).toInt(), QString(list1[3]).toInt());

                    auto clipItem = TrackItem::createNewSample(clip["file_name"].toString(), _tracksModel->trackItem(i)->getTitleColor(),
                        _areaInfo, i, "", _tracksModel->trackItem(i)->track(), rect, false);

                    auto clipIndex = _tracksModel->trackItem(i)->addSampleClip(startTime, endTime, offsetStart, offsetEnd, clipItem);

                    playbackSampleClipPositionChanged(i, clipIndex);
                }
            }

            if (tracks[i].toObject().contains("plugins")) {
                QJsonArray plugins = tracks[i].toObject()["plugins"].toArray();
                for (int j = 0; j < plugins.size(); j++) {
                    auto plugin_obj = plugins[j].toObject();
                    if (plugin_obj["plugin_type"].toString() == "Vst3") {

                        QJsonObject obj;
                        obj["automations"] = plugin_obj["automations"].toArray();

                        if (plugin_obj.contains("configured_parameters"))
                            obj["configured_parameters"] = plugin_obj["configured_parameters"].toArray();

                        obj["automation_menu"] = tracks[i].toObject()["automation_menu"].toString();
                        obj["automation_sub_menu"] = tracks[i].toObject()["automation_sub_menu"].toString();
                        obj["plugin_enabled"] = plugin_obj["plugin_enabled"].toBool();

                        m_loadingEffectsAutomations.insert(plugin_obj["plugin_name"].toString(), obj);
                        addVst3Effect(i, plugin_obj["plugin_path"].toString(), plugin_obj["plugin_name"].toString(), plugin_obj["plugin_index"].toInt());
                    }
                    if (plugin_obj["plugin_type"].toString() == "Visr") {

                        addVisr(i, plugin_obj["plugin_enabled"].toBool(), plugin_obj["reverb_enabled"].toBool());

                        auto automation_array = plugin_obj["automations"].toArray();
                        for (int k = 0; k < automation_array.size(); k++) {
                            auto automation_obj = automation_array[k].toObject();
                            bool enabled = automation_obj["enabled"].toBool();
                            QString type = automation_obj["type"].toString();
                            QJsonArray state = automation_obj["state"].toArray();
                            int sub_track_index = automation_obj["sub_track_index"].toInt();
                            int track_index = automation_obj["track_index"].toInt();
                            QString sub_menu = automation_obj["sub_menu"].toString();
                            QString menu = automation_obj["menu"].toString();
                            double normalized = automation_obj["normalized"].toDouble();

                            bool show_envelopes = automation_obj["show_envelopes"].toBool();

                            if (tracks[i].toObject()["automation_menu"].toString() == menu && tracks[i].toObject()["automation_sub_menu"].toString() == sub_menu) {
                                _tracksModel->setAutomationMenuTitle(i, tracks[i].toObject()["automation_menu"].toString(), tracks[i].toObject()["automation_sub_menu"].toString());
                            }

                            if (sub_track_index == -1) {
                                if (_tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)) {
                                    _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setLinesState(state);
                                    _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setEnabled(enabled);
                                    _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setNormalized(normalized);

                                    _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setShowEnvelopes(show_envelopes);
                                } else {
                                    qDebug() << "BUG2" << sub_track_index << track_index << menu << sub_menu;
                                }
                            } else {
                                if (_tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)) {

                                    _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setLinesState(state);
                                    _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setEnabled(enabled);
                                    _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setNormalized(normalized);
                                    _tracksModel->automationLaneModel(track_index)->insert(menu, sub_menu, sub_track_index, 3);
                                    _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->setShowEnvelopes(show_envelopes);

                                } else {
                                    qDebug() << "BUG3" << sub_track_index << track_index << menu << sub_menu;
                                }
                            }
                        }
                    }
                }
            }

            if (tracks[i].toObject().contains("automation_lane_enabled")) {
                bool automation_lane_enabled = tracks[i].toObject()["automation_lane_enabled"].toBool();
                _tracksModel->setAutomationLaneEnabled(i, automation_lane_enabled);
            }

            if (tracks[i].toObject().contains("automations")) {
                QJsonArray automation_array = tracks[i].toObject()["automations"].toArray();
                for (int k = 0; k < automation_array.size(); k++) {
                    QJsonObject automation_obj = automation_array[k].toObject();
                    bool enabled = automation_obj["enabled"].toBool();
                    QString type = automation_obj["type"].toString();
                    QJsonArray state = automation_obj["state"].toArray();
                    int sub_track_index = automation_obj["sub_track_index"].toInt();
                    int track_index = automation_obj["track_index"].toInt();
                    QString sub_menu = automation_obj["sub_menu"].toString();
                    QString menu = automation_obj["menu"].toString();
                    double normalized = automation_obj["normalized"].toDouble();

                    if (tracks[i].toObject()["automation_menu"].toString() == menu && tracks[i].toObject()["automation_sub_menu"].toString() == sub_menu) {
                        _tracksModel->setAutomationMenuTitle(i, tracks[i].toObject()["automation_menu"].toString(), tracks[i].toObject()["automation_sub_menu"].toString());
                    }

                    if (sub_track_index == -1) {
                        if (_tracksModel->trackItem(i)->automationItem(menu, sub_menu)) {
                            _tracksModel->trackItem(i)->automationItem(menu, sub_menu)->setNormalized(normalized);
                            _tracksModel->trackItem(i)->automationItem(menu, sub_menu)->setEnabled(enabled);
                            _tracksModel->trackItem(i)->automationItem(menu, sub_menu)->setLinesState(state);
                        } else {
                            qDebug() << "BUG2" << sub_track_index << track_index << menu << sub_menu;
                        }
                    } else {
                        if (_tracksModel->trackItem(i)->automationItem(menu, sub_menu)) {
                            _tracksModel->trackItem(i)->automationItem(menu, sub_menu)->setNormalized(normalized);
                            _tracksModel->trackItem(i)->automationItem(menu, sub_menu)->setLinesState(state);
                            _tracksModel->trackItem(i)->automationItem(menu, sub_menu)->setEnabled(enabled);
                            _tracksModel->automationLaneModel(i)->append(menu, sub_menu, 3);
                        } else {
                            qDebug() << "BUG3" << sub_track_index << track_index << menu << sub_menu;
                        }
                    }
                }
            }

            _tracksModel->trackItem(i)->setAutomationSubMenuTitle(tracks[i].toObject()["automation_sub_menu"].toString());
            _tracksModel->setAutomationMenuTitle(i, tracks[i].toObject()["automation_menu"].toString(), tracks[i].toObject()["automation_sub_menu"].toString());

            if (tracks[i].toObject().contains("keyframes")) {
                auto keyFrames = tracks[i].toObject()["keyframes"].toObject();
                //_tracksModel->trackItem(i)->keyFramesItem()->load(keyFrames);
            }
        }
    }
    for (int i = 0; i < _loadedObjects.size(); i++) {

        if (_loadedObjects[i].isNull() || _loadedObjects[i].isEmpty() || _loadedObjects[i] == "{}") {
            continue;
        }

        Q_EMIT sigLoad3dView();
        break;
    }

    _areaInfo->updateUI();
}
void ClipArea::sltKeyReleased(int modifiers, int key)
{

    _lastKey = 0;

    _areaInfo->setCtrlkeyPressed(false);
    _areaInfo->setShiftkeyPressed(false);
    _areaInfo->setAltKeyPressed(false);

    Q_EMIT shiftPressedChanged();

    for (int i = 0; i < _tracksModel->size(); i++) {
        _tracksModel->trackItem(i)->keyReleaseEvent(modifiers, key);
    }
}

void ClipArea::sltFocusOut()
{
    _lastKey = 0;
    _areaInfo->setCtrlkeyPressed(false);
    _areaInfo->setShiftkeyPressed(false);
    _areaInfo->setAltKeyPressed(false);
}

void ClipArea::saveInUndoItems(QJsonArray array)
{

    ActionStack::instance()->pushInUndo(array);
    ActionStack::instance()->clearAllRedo();
}

void ClipArea::saveInRedoItems(QJsonArray array)
{

    ActionStack::instance()->pushInRedo(array);
}

void ClipArea::clearMovingItems()
{
    _mutex.lock();

    if (_movingItems.size() > 0) {
        for (int i = 0; i < _movingItems.size(); i++) {
            _movingItems[i].clipItem.reset();
        }
        _movingItems.clear();
    }
    Q_EMIT sigClipItemRemoved();
    _mutex.unlock();
}

void ClipArea::clearCopiedItems()
{
    _mutex.lock();
    for (int i = 0; i < _copiedItems.size(); i++) {
        _copiedItems[i].clipItem.reset();
    }
    _copiedItems.clear();
    _mutex.unlock();
}

void ClipArea::clearDuplicatedItems()
{
    _mutex.lock();
    for (int i = 0; i < _duplicatedItems.size(); i++) {
        _duplicatedItems[i].clipItem.reset();
    }
    _duplicatedItems.clear();
    _mutex.unlock();
}

QPoint ClipArea::lastMouseMovingPoint() const
{
    return _lastMouseMovingPoint;
}

void ClipArea::setLastMouseMovingPoint(QPoint newLastMouseMovingPoint)
{
    _lastMouseMovingPoint = newLastMouseMovingPoint;
}

QJsonObject ClipArea::performOperation(QJsonObject obj)
{

    QJsonObject jObject;

    if (obj["operation"] == ActionStack::Operation_ObjectPosition

        || obj["operation"] == ActionStack::Operation_EditKeyFramesPoint) {

        qDebug() << "obj" << obj;
        jObject = _tracksModel->performObjectPosition(obj);
    }

    if (obj["operation"] == ActionStack::Operation_EffectStatus) {
        int track_index = obj["track_index"].toInt();
        QString effect_name = obj["effect"].toString();

        auto effectMap = ((SampleTrack*)_tracksModel->trackItem(track_index)->track())->audioPort()->effects()->getEffectList();

        QMapIterator<QString, int> i(effectMap);
        while (i.hasNext()) {
            i.next();

            auto effect = ((SampleTrack*)_tracksModel->trackItem(track_index)->track())->audioPort()->effects()->effect(i.value());

            if (effect->name().toLower() == effect_name) {

                jObject = effect->performOperation(obj);
            }
        }
    }

    if (obj["operation"] == ActionStack::Operation_NormalizedValueChanged) {
        int track_index = obj["track_index"].toInt();
        QString menu = obj["menu"].toString();
        QString sub_menu = obj["sub_menu"].toString();

        jObject["type"] = "normalized";
        jObject["track_index"] = track_index;
        jObject["sub_track_index"] = obj["sub_track_index"].toInt();
        jObject["menu"] = menu;
        jObject["sub_menu"] = sub_menu;
        jObject["automation"] = _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->automation()->enabled();
        jObject["value"] = _tracksModel->trackItem(track_index)->automationItem(menu, sub_menu)->automation()->normalized();
        jObject["operation"] = ActionStack::Operation_NormalizedValueChanged;
        _tracksModel->performOperation(obj);
    }

    if (obj["operation"] == ActionStack::Operation_EffectAdded) {
        int track_index = obj["track_index"].toInt();
        int effect_index = obj["effect_index"].toInt();
        auto effect_name = obj["effect"];
        if (effect_name == "eq") {
            removeMultiEqEffect(track_index);
            jObject["track_index"] = track_index;
            jObject["effect"] = effect_name;
            jObject["operation"] = ActionStack::Operation_EffectRemoved;
        } else if (effect_name == "visr") {
            removeVisr(track_index);
            jObject["track_index"] = track_index;
            jObject["effect"] = effect_name;
            jObject["operation"] = ActionStack::Operation_EffectRemoved;
        } else if (effect_name == "vst") {

            removeVst3Effect(track_index, effect_index);
        }
    }
    if (obj["operation"] == ActionStack::Operation_EffectRemoved) {
        int track_index = obj["track_index"].toInt();
        auto effect_name = obj["effect"];
        if (effect_name == "visr") {
            addVisr(track_index);
            jObject["track_index"] = track_index;
            jObject["effect"] = effect_name;
            jObject["operation"] = ActionStack::Operation_EffectAdded;
        } /*else {
            addVst3Effect(track_index, effect_name);
            jObject["track_index"] = track_index;
            jObject["effect"] = effect_name;
            jObject["operation"] = ActionStack::Operation_EffectRemoved;
        }*/
    }
    if (obj["operation"] == ActionStack::Operation_DisableAutomationPoint) {

        int track_index = obj["track_index"].toInt();
        int sub_track_index = obj["sub_track_index"].toInt();

        QString type = "None";
        QString sub_type = "None";

        if (obj.contains("menu")) {
            type = obj["menu"].toString();
        }

        if (obj.contains("sub_menu")) {
            sub_type = obj["sub_menu"].toString();
        }

        if (type != "None" && sub_type != "None") {
            _tracksModel->trackItem(track_index)->automationItem(type, sub_type)->automation()->setEnabled1(true);

            jObject["track_index"] = track_index;
            jObject["sub_track_index"] = sub_track_index;
            jObject["menu"] = type;
            jObject["sub_menu"] = sub_type;
            jObject["operation"] = ActionStack::Operation_EnableAutomationPoint;
        }
    }

    if (obj["operation"] == ActionStack::Operation_EnableAutomationPoint) {

        int track_index = obj["track_index"].toInt();
        int sub_track_index = obj["sub_track_index"].toInt();

        QString type = "None";
        QString sub_type = "None";

        if (obj.contains("menu")) {
            type = obj["menu"].toString();
        }

        if (obj.contains("sub_menu")) {
            sub_type = obj["sub_menu"].toString();
        }

        if (type != "None" && sub_type != "None") {
            _tracksModel->trackItem(track_index)->automationItem(type, sub_type)->automation()->setEnabled1(false);

            jObject["track_index"] = track_index;
            jObject["sub_track_index"] = sub_track_index;
            jObject["menu"] = type;
            jObject["sub_menu"] = sub_type;
            jObject["operation"] = ActionStack::Operation_DisableAutomationPoint;
        }
    }
    if (obj["operation"] == ActionStack::Operation_DeleteClipItem) {
        QJsonObject state = obj["state"].toObject();
        auto audiofile = state["audio_file"].toString();
        auto rect_str = (obj["rect"]).toString();
        int track_index = obj["track_index"].toInt();
        QString clip_index = obj["clip_index"].toString();
        QStringList list1 = rect_str.split(',');
        QRect rect = QRect(QString(list1[0]).toInt(), QString(list1[1]).toInt(), QString(list1[2]).toInt(), QString(list1[3]).toInt());
        auto item = TrackItem::createNewSample(audiofile, _tracksModel->trackItem(track_index)->getTitleColor(), _areaInfo, track_index, clip_index, _tracksModel->trackItem(track_index)->track(), rect, false);

        auto startTime = state["start_time"].toDouble();
        auto endTime = state["end_time"].toDouble();
        auto offsetStart = state["offset_start"].toDouble();
        auto offsetEnd = state["offset_end"].toDouble();

        _tracksModel->trackItem(track_index)->addSampleClip(startTime, endTime, offsetStart, offsetEnd, item);

        jObject["clip_index"] = clip_index;
        jObject["operation"] = ActionStack::Operation_InsertClipItem;
        jObject["track_index"] = track_index;

        sltReadyToPlay();
    }
    if (obj["operation"] == ActionStack::Operation_InsertClipItem) {

        QString id = obj["clip_index"].toString();
        int track_index = obj["track_index"].toInt();
        jObject = _tracksModel->trackItem(track_index)->removeSampleItem(id);

        sltReadyToPlay();
    }
    if (obj["operation"] == ActionStack::Operation_ResizeClipItem) {

        int track_index = obj["track_index"].toInt();
        QString clip_index = obj["clip_index"].toString();
        auto rect_str = (obj["rect"]).toString();
        QJsonObject state = obj["state"].toObject();
        auto startTime = state["start_time"].toDouble();
        auto endTime = state["end_time"].toDouble();
        auto offsetStart = state["offset_start"].toDouble();
        auto offsetEnd = state["offset_end"].toDouble();
        QStringList list1 = rect_str.split(',');
        QRect rect = QRect(QString(list1[0]).toInt(), QString(list1[1]).toInt(), QString(list1[2]).toInt(), QString(list1[3]).toInt());
        jObject = _tracksModel->trackItem(track_index)->resizeSampleClip(clip_index, rect, startTime, endTime, offsetStart, offsetEnd);

        sltReadyToPlay();
    }
    if (obj["operation"] == ActionStack::Operation_SelectionArea) {
        jObject = _areaInfo->selectionArea()->state();
        _areaInfo->selectionArea()->setState(obj);
        _areaInfo->updateUI();
    }

    if (obj["operation"] == ActionStack::Operation_RefreshAutomationPoint) {
        int track_index = obj["track_index"].toInt();

        int sub_track_index = -1;
        if (obj.contains("sub_track_index"))
            sub_track_index = obj["sub_track_index"].toInt();

        int line_index = obj["line_index"].toInt();

        QString type = "None";
        QString sub_type = "None";

        if (obj.contains("menu")) {
            type = obj["menu"].toString();
        }

        if (obj.contains("sub_menu")) {
            sub_type = obj["sub_menu"].toString();
        }

        auto newState = obj["state"].toArray();

        QJsonArray oldState;
        if (sub_track_index == -1) {
            if (type == "None") {

                if (_tracksModel->trackItem(track_index)->automationItem()) {
                    oldState = _tracksModel->trackItem(track_index)->automationItem()->getLinesState();
                    _tracksModel->trackItem(track_index)->automationItem()->setLinesState(newState);
                } else {
                    qDebug() << "BUG1" << sub_track_index << track_index << type << sub_type;
                }

            } else {
                if (_tracksModel->trackItem(track_index)->automationItem(type, sub_type)) {
                    oldState = _tracksModel->trackItem(track_index)->automationItem(type, sub_type)->automation()->getLinesState();
                    _tracksModel->trackItem(track_index)->automationItem(type, sub_type)->setLinesState(newState);

                    if (obj.contains("automation")) {
                        jObject["automation"] = _tracksModel->trackItem(track_index)->automationItem(type, sub_type)->automation()->enabled();

                        _tracksModel->trackItem(track_index)->automationItem(type, sub_type)->automation()->setEnabled1(obj["automation"].toBool());
                    }

                } else {
                    qDebug() << "BUG2" << sub_track_index << track_index << type << sub_type;
                }
            }

        } else {

            auto laneItem = _tracksModel->trackItem(track_index)->automationLaneModel()->getItem(sub_track_index);

            if (laneItem) {
                oldState = _tracksModel->trackItem(track_index)
                               ->automationItem(laneItem->automationLaneMenuTitle, laneItem->automationLaneSubMenuTitle)
                               ->getLinesState();

                if (_tracksModel->trackItem(track_index)
                        ->automationItem(laneItem->automationLaneMenuTitle, laneItem->automationLaneSubMenuTitle)) {
                    _tracksModel->trackItem(track_index)
                        ->automationItem(laneItem->automationLaneMenuTitle, laneItem->automationLaneSubMenuTitle)
                        ->setLinesState(newState);

                    if (obj.contains("automation")) {
                        jObject["automation"] = _tracksModel->trackItem(track_index)
                                                    ->automationItem(laneItem->automationLaneMenuTitle, laneItem->automationLaneSubMenuTitle)
                                                    ->automation()
                                                    ->enabled();

                        _tracksModel->trackItem(track_index)
                            ->automationItem(laneItem->automationLaneMenuTitle, laneItem->automationLaneSubMenuTitle)
                            ->automation()
                            ->setEnabled1(obj["automation"].toBool());
                    }

                } else {
                    qDebug() << "BUG3" << sub_track_index << track_index << type << sub_type;
                }
            }
        }

        jObject["track_index"] = track_index;
        jObject["sub_track_index"] = sub_track_index;
        jObject["state"] = oldState;
        jObject["menu"] = type;
        jObject["sub_menu"] = sub_type;

        jObject["operation"] = ActionStack::Operation_RefreshAutomationPoint;
    }

    return jObject;
}

QVector<LineSeg> ClipArea::trackSegLines(int index)
{
    if (index >= 0 && index < m_trackSegLines.size()) {
        return m_trackSegLines[index];
    }
    if (index < 0) {
        return m_plotLines;
    }
    return QVector<LineSeg>();
}

QVector<RectSeg> ClipArea::masterTrackSelectedRects()
{
    return m_masterTrackSelectedRects;
}

QVector<RectSeg> ClipArea::trackSelectedRects(int index)
{

    if (index >= 0 && index < m_trackSelectedRects.size()) {
        return m_trackSelectedRects[index];
    }

    return QVector<RectSeg>();
}

QVector<RectSeg> ClipArea::trackSegRects(int index)
{

    if (index >= 0 && index < m_trackSegRects.size()) {
        return m_trackSegRects[index];
    }
    if (index < 0) {
        return m_plotRects;
    }
    return QVector<RectSeg>();
}

QMap<QString, QSharedPointer<Effect>> ClipArea::loadingEffects() const
{
    return m_loadingEffects;
}

int ClipArea::listViewPositionChanged() const
{
    return _listViewPositionChanged;
}

void ClipArea::setListViewPositionChanged(int newListViewPositionChanged)
{
    _listViewPositionChanged = newListViewPositionChanged;
}

int ClipArea::currentSelectedTime() const
{
    return _currentSelectedTime;
}

void ClipArea::setCurrentSelectedTime(int newCurrentSelectedTime)
{
    _currentSelectedTime = newCurrentSelectedTime;
    Q_EMIT currentSelectedTimeChanged();
}

bool ClipArea::isCtrlPressed()
{
    return _areaInfo->ctrlkeyPressed();
}

void ClipArea::sltSampleChanged()
{
    _areaInfo->updateUI();
}

void ClipArea::sltTimerCheckPosition()
{

    double timePerPixel = _areaInfo->lastWidth() / _areaInfo->clipDuration();
    float _sutoScrollSpeedRight = qAbs(_areaInfo->lastWidth() - _lastMouseMovingPoint.x() - _areaInfo->selectionArea()->leftDiff * timePerPixel) / 20.0;
    float _sutoScrollSpeedLeft = qAbs(_lastMouseMovingPoint.x() - _areaInfo->selectionArea()->leftDiff * timePerPixel) / 20.0;

    float startTime = _areaInfo->startTime();
    float endTime = _areaInfo->startTime() + _areaInfo->clipDuration();

    if (_selectingAreaMoving) {
        if (_lastMouseMovingPoint.x() > _areaInfo->lastWidth()) {

            float rate = _sutoScrollSpeedRight * (_areaInfo->clipDuration() / (double)_areaInfo->lastWidth());

            _areaInfo->setClipProperties(startTime + rate, endTime + rate, endTime - startTime);

            auto width = _areaInfo->selectionArea()->area().width();
            auto height = _areaInfo->selectionArea()->area().height();

            double newLeft = endTime + rate;

            _areaInfo->selectionArea()->setLeft(newLeft);
            _areaInfo->selectionArea()->setWidth(width);
            //_areaInfo->selectionArea()->area().setHeight(height);
            int firstIndex = _areaInfo->selectionArea()->firstTrackIndex();

            if (_areaInfo->selectionArea()->area().right() > _areaInfo->maxEndTime()) {
                _areaInfo->setMaxEndTime(_areaInfo->selectionArea()->area().right());
            }

            int first_track_index_order = -1;

            for (int i = 0; i < _movingItems.size(); i++) {

                if (i == 0) {
                    first_track_index_order = _movingItems[i].trackIndex;
                }
                if (_movingItems[i].clipItem == Q_NULLPTR) {
                    continue;
                }

                auto trackRect = _tracksModel->trackItem(firstIndex + (_movingItems[i].trackIndex - first_track_index_order))->rect();

                auto clipRect = _movingItems[i].clipItem->rect();
                double startX = _areaInfo->time2Pixel(_areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff);
                double width = _areaInfo->time2Pixel(_areaInfo->selectionArea()->area().right() - _areaInfo->selectionArea()->area().left());
                double endX = startX + width;

                clipRect.setLeft(startX);
                clipRect.setRight(endX);
                clipRect.setTop(trackRect.top());
                clipRect.setBottom(trackRect.bottom());
                _movingItems[i].clipItem->setRect(clipRect);
                _movingItems[i].clipItem->setBaseColor(_tracksModel->trackItem(_movingItems[i].trackIndex)->getTitleColor());
                _movingItems[i].trackIndex = firstIndex + (_movingItems[i].trackIndex - first_track_index_order);

                if (_movingItems[i].type == 0) {

                    double startTime = _areaInfo->selectionArea()->area().left();
                    double endTime = _areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff + _movingItems[i].length;

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
                if (_movingItems[i].type == 1) {

                    double length = _movingItems[i].endTime - _movingItems[i].startTime;
                    double startTime = _areaInfo->selectionArea()->area().left() + length + _movingItems[i].endTimeDiff - _movingItems[i].length;
                    double endTime = _areaInfo->selectionArea()->area().left() + length;

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
                if (_movingItems[i].type == 2) {

                    double startTime = _areaInfo->selectionArea()->area().left();
                    double length = _movingItems[i].endTime - _movingItems[i].startTime;
                    double endTime = startTime + length;

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
                if (_movingItems[i].type == 3) {

                    double startTime = _areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff;
                    double endTime = startTime + _movingItems[i].length;

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
                if (_movingItems[i].type == 4) {

                    double startTime = _areaInfo->selectionArea()->area().left();
                    double endTime = _areaInfo->selectionArea()->area().right();

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
            }

            Q_EMIT sigClipItemEdited();
            _areaInfo->updateUI();
        }

        if (_lastMouseMovingPoint.x() < 0) {

            float rate = _sutoScrollSpeedLeft * (_areaInfo->clipDuration() / (double)_areaInfo->lastWidth());
            if (startTime - rate < 0) {

                _areaInfo->setClipProperties(0, endTime - startTime, endTime - startTime);

            } else {

                _areaInfo->setClipProperties(startTime - rate, endTime - rate, endTime - startTime);
            }

            auto width = _areaInfo->selectionArea()->area().width();
            auto height = _areaInfo->selectionArea()->area().height();

            double newLeft = _areaInfo->startTime() - width;
            if (newLeft < 0)
                newLeft = 0;

            _areaInfo->selectionArea()->setLeft(newLeft);
            _areaInfo->selectionArea()->setWidth(width);
            //_areaInfo->selectionArea()->area().setHeight(height);
            int firstIndex = _areaInfo->selectionArea()->firstTrackIndex();

            int first_track_index_order = -1;
            for (int i = 0; i < _movingItems.size(); i++) {

                if (i == 0) {
                    first_track_index_order = _movingItems[i].trackIndex;
                }
                if (_movingItems[i].clipItem == Q_NULLPTR) {
                    continue;
                }

                auto trackRect = _tracksModel->trackItem(firstIndex + (_movingItems[i].trackIndex - first_track_index_order))->rect();

                auto clipRect = _movingItems[i].clipItem->rect();
                double startX = _areaInfo->time2Pixel(_areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff);
                double width = _areaInfo->time2Pixel(_areaInfo->selectionArea()->area().right() - _areaInfo->selectionArea()->area().left());
                double endX = startX + width;

                clipRect.setLeft(startX);
                clipRect.setRight(endX);
                clipRect.setTop(trackRect.top());
                clipRect.setBottom(trackRect.bottom());
                _movingItems[i].clipItem->setRect(clipRect);
                _movingItems[i].clipItem->setBaseColor(_tracksModel->trackItem(_movingItems[i].trackIndex)->getTitleColor());
                _movingItems[i].trackIndex = firstIndex + (_movingItems[i].trackIndex - first_track_index_order);

                if (_movingItems[i].type == 0) {

                    double startTime = _areaInfo->selectionArea()->area().left();
                    double endTime = _areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff + _movingItems[i].length;

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
                if (_movingItems[i].type == 1) {

                    double length = _movingItems[i].endTime - _movingItems[i].startTime;
                    double startTime = _areaInfo->selectionArea()->area().left() + length + _movingItems[i].endTimeDiff - _movingItems[i].length;
                    double endTime = _areaInfo->selectionArea()->area().left() + length;

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
                if (_movingItems[i].type == 2) {

                    double startTime = _areaInfo->selectionArea()->area().left();
                    double length = _movingItems[i].endTime - _movingItems[i].startTime;
                    double endTime = startTime + length;

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
                if (_movingItems[i].type == 3) {

                    double startTime = _areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff;
                    double endTime = startTime + _movingItems[i].length;

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
                if (_movingItems[i].type == 4) {

                    double startTime = _areaInfo->selectionArea()->area().left();
                    double endTime = _areaInfo->selectionArea()->area().right();

                    auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                    auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                    _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                    _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                    _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                }
            }

            Q_EMIT sigClipItemEdited();
            _areaInfo->updateUI();
        }
    }
}

void ClipArea::sltFirstBufferFilled(QString clipIndex, bool toPlay)
{

    _numberOfSampleBuffer.removeAll(clipIndex);

    if (_numberOfSampleBuffer.size() == 0 && _requestForLoading) {

        _requestForLoading = false;
        if (_exporing && m_renderManager) {

            double samples = _areaInfo->playheadMarker() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
            AudioManager::getSong()->setToTime(samples / 256.0);

            _exporing = false;
            m_renderManager->renderProject();
        } else {
            if (toPlay) {
                double samples = _areaInfo->playheadMarker() / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                AudioManager::getSong()->setToTime(samples / 256.0);

                AudioManager::getSong()->setPlay();

                Q_EMIT pausedChanged();

            } else {

                AudioManager::getSong()->setPause();
                Q_EMIT pausedChanged();
            }
        }
    }
}
void ClipArea::playbackSampleClipPositionChanged(int trackIndex, QString clipIndex)
{
    if (_requestForLoading == false) {

        _numberOfSampleBuffer.clear();

        _numberOfSampleBuffer.append(_tracksModel->trackItem(trackIndex)->countPositionChanged(_areaInfo->arrangementStartTime()));

        if (_numberOfSampleBuffer.size() > 0 && _numberOfSampleBuffer.contains(clipIndex)) {

            bool isPause = AudioManager::getSong()->isPause();

            if (isPause == false) {
                AudioManager::getSong()->setPause();
                Q_EMIT pausedChanged();
            }

            _tracksModel->trackItem(trackIndex)->setPositionChanged(clipIndex, _areaInfo->arrangementStartTime(), !isPause);
        }
    }
}

static inline bool doubleGreaterOrEqual(double a, double b)
{
    constexpr double EPS = 1e-9;
    return (a > b - EPS);
}

static inline bool doubleLessOrEqual(double a, double b)
{
    constexpr double EPS = 1e-9;
    return (a < b + EPS);
}
static int findValueForNumber(double number, const QList<RangeItem>& ranges)
{
    int last_one = -1;
    for (const auto& r : ranges) {
        if (doubleGreaterOrEqual(number, r.start) && doubleLessOrEqual(number, r.end)) {
            last_one = r.value;
        }
    }
    return last_one; // empty result if not found
}
void ClipArea::drawGrids()
{

    double final_size = 0;
    double stepDuration = 0;
    double offsetPix = 0;
    double offsetTime = 0;

    auto _currentDuration = _areaInfo->clipDuration();
    auto _lastWidth = _areaInfo->lastWidth();
    auto _startTime = _areaInfo->startTime();
    auto _lenghtOfBeat = _areaInfo->lenghtOfBeat();
    auto _beatPerBar = _areaInfo->beatPerBar();

    auto _tracksModel = tracksModel();

    auto _tempo = _areaInfo->tempo();

    auto adaptiveGrid = _areaInfo->adaptiveGrid();
    auto fixedGrid = _areaInfo->fixedGrid();

    bool _offGrid;
    if (adaptiveGrid == AdaptiveGrid_Off && fixedGrid == AdaptiveGrid_Off) {
        _offGrid = true;
    } else {
        _offGrid = false;
    }

    double startTime = _startTime;
    double clipDuration = _currentDuration;

    int lob = _lenghtOfBeat;
    int bpb = _beatPerBar;
    auto m = Measure::timeToMeasure(clipDuration, lob, bpb, _tempo);

    double beatDuration = m.getBeatDuration();
    double barDuration = m.getBarDuration();
    double sixDuration = m.getSixteenthDuration();
    int min_color = ScreenInterface::scaleSize2(80, _areaInfo->zoomFactor());
    int max_color_2 = ScreenInterface::scaleSize2(600, _areaInfo->zoomFactor());
    double plotStepDuration;
    double posX = 0;

    float startBraceMarker = _areaInfo->time2Pixel(_areaInfo->startBraceMarker());

    float endBraceMarker = _areaInfo->time2Pixel(_areaInfo->endBraceMarker());

    if (adaptiveGrid != AdaptiveGrid_Off) {

        int min = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
        int max = ScreenInterface::scaleSize2(50, _areaInfo->zoomFactor());

        switch (adaptiveGrid) {
        case AdaptiveGrid_Narrow:
            min = ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(40, _areaInfo->zoomFactor());
            break;
        case AdaptiveGrid_Narrowest:
            min = ScreenInterface::scaleSize2(8, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(16, _areaInfo->zoomFactor());
            break;
        case AdaptiveGrid_Noraml:
            min = ScreenInterface::scaleSize2(50, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(100, _areaInfo->zoomFactor());
            break;
        case AdaptiveGrid_Wide:
            min = ScreenInterface::scaleSize2(200, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(400, _areaInfo->zoomFactor());
            break;
        case AdaptiveGrid_Widest:
            min = ScreenInterface::scaleSize2(400, _areaInfo->zoomFactor());
            max = ScreenInterface::scaleSize2(800, _areaInfo->zoomFactor());
            break;
        }

        if (adaptiveGrid == AdaptiveGrid_Narrow
            || adaptiveGrid == AdaptiveGrid_Narrowest
            || adaptiveGrid == AdaptiveGrid_Noraml) {
            int coeff_six = 1;
            int coeff_beat = 1;
            int coeff_bar = 1;
            final_size = (beatDuration * 1000.0) / clipDuration * _lastWidth;
            stepDuration = beatDuration;
            if (final_size < min) {

                coeff_beat = 1;
                while (final_size < min && coeff_beat < lob) {
                    stepDuration = beatDuration * coeff_beat;
                    final_size = (stepDuration * 1000.0) / clipDuration * _lastWidth;
                    coeff_beat *= 2;
                }

            } else if (final_size > max) {
                coeff_beat = 1;
                while (final_size > max) {
                    stepDuration = beatDuration / coeff_beat;
                    final_size = (stepDuration * 1000.0) / clipDuration * _lastWidth;
                    coeff_beat *= 2;
                }
            }

            coeff_bar = 1;
            if (final_size < min) {
                while (final_size < min) {
                    stepDuration = barDuration * coeff_bar;
                    final_size = (stepDuration * 1000.0) / clipDuration * _lastWidth;
                    coeff_bar *= 2;
                }
            }

            offsetTime = (qCeil((startTime) / (stepDuration * 1000.0))) * (stepDuration * 1000.0);
            offsetPix = (offsetTime - (startTime)) / clipDuration * _lastWidth;
            posX = offsetPix;

            plotStepDuration = stepDuration * 1000.0;
        }
        if (adaptiveGrid == AdaptiveGrid_Wide
            || adaptiveGrid == AdaptiveGrid_Widest) {
            int coeff_six = 0;
            int coeff_beat = 0;
            int coeff_bar = 0;

            final_size = (sixDuration * 1000.0) / clipDuration * _lastWidth;

            stepDuration = sixDuration;

            if (final_size > max) {

                coeff_six = 1;
                while (final_size > max) {
                    stepDuration = sixDuration / coeff_six;
                    final_size = (sixDuration / coeff_six * 1000.0) / clipDuration * _lastWidth;
                    coeff_six *= 2;
                }
                if (coeff_six >= 2)
                    coeff_six /= 2;

            } else {

                coeff_beat = 1;
                while (coeff_beat <= lob / 2 && final_size < min) {
                    stepDuration = beatDuration * coeff_beat;
                    final_size = (beatDuration * coeff_beat * 1000.0) / clipDuration * _lastWidth;
                    coeff_beat *= 2;
                }
                if (final_size < min) {
                    coeff_bar = 1;
                    while (final_size < min) {
                        stepDuration = barDuration * coeff_bar;
                        final_size = (barDuration * coeff_bar * 1000.0) / clipDuration * _lastWidth;
                        coeff_bar *= 2;
                    }
                }
            }

            offsetTime = (qCeil((startTime) / (stepDuration * 1000.0))) * (stepDuration * 1000.0);
            offsetPix = (offsetTime - (startTime)) / clipDuration * _lastWidth;

            plotStepDuration = stepDuration * 1000.0;

            posX = offsetPix;
        }
    }

    if (adaptiveGrid == AdaptiveGrid_Off && fixedGrid != FixedGrid_Off) {

        int gridLevel = 1;
        int baseStep = 0;
        int barStep = 1;
        int baseSubStep = 1;
        switch (fixedGrid) {
        case FixedGrid_1_8:
            gridLevel = 8;
            break;
        case FixedGrid_1_4:
            gridLevel = 4;
            break;
        case FixedGrid_1_16:
            gridLevel = 16;
            break;
        case FixedGrid_1_32:
            gridLevel = 32;
            break;
        case FixedGrid_1_2:
            gridLevel = 2;
            break;
        case FixedGrid_1:
            barStep = 1;
            break;
        case FixedGrid_2:
            barStep = 2;
            break;
        case FixedGrid_4:
            barStep = 4;
            break;
        case FixedGrid_8:
            barStep = 8;
            break;
        }

        if (lob / gridLevel < 1) {
            baseSubStep = gridLevel / lob;
            baseStep = 1;
        } else {
            baseStep = lob / gridLevel;
            baseSubStep = 1;
        }

        if (barStep >= 1 && gridLevel == 1) {
            stepDuration = barDuration * (double)barStep * 1000.0;
            final_size = stepDuration / clipDuration * _lastWidth;
            int coef = 1;
            while (final_size < ScreenInterface::instance()->freeResolution(8)) {
                stepDuration = coef * barDuration * (double)barStep * 1000.0;
                final_size = stepDuration / clipDuration * _lastWidth;
                coef *= 2;
            }

        } else {
            if (baseStep > 1 && baseSubStep == 1) {

                stepDuration = beatDuration * (double)baseStep * 1000.0;

                final_size = stepDuration / clipDuration * _lastWidth;
                int coef = 1;
                while (final_size < ScreenInterface::instance()->freeResolution(8)) {
                    stepDuration = coef * barDuration * 1000.0;
                    final_size = stepDuration / clipDuration * _lastWidth;
                    coef *= 2;
                }
            } else {

                stepDuration = beatDuration / (double)baseSubStep * 1000.0;

                final_size = stepDuration / clipDuration * _lastWidth;

                while (final_size < ScreenInterface::instance()->freeResolution(8) && baseSubStep > 0) {
                    baseSubStep /= 2;
                    if (baseSubStep > 0) {
                        stepDuration = beatDuration / (double)baseSubStep * 1000.0;
                        final_size = stepDuration / clipDuration * _lastWidth;
                    }
                }

                if (baseSubStep == 0) {
                    int k = 1;
                    while (k * beatDuration < barDuration) {
                        if (barDuration / (double)(k * beatDuration) - qFloor(barDuration / (double)(k * beatDuration)) == 0.0) {
                            stepDuration = k * beatDuration * 1000.0;
                            final_size = stepDuration / clipDuration * _lastWidth;
                            if (final_size > ScreenInterface::instance()->freeResolution(8)) {
                                break;
                            }
                        }
                        k *= 2;
                    }
                    if (final_size < ScreenInterface::instance()->freeResolution(8)) {

                        int coef = 1;
                        while (final_size < ScreenInterface::instance()->freeResolution(8)) {
                            stepDuration = coef * barDuration * 1000.0;
                            final_size = stepDuration / clipDuration * _lastWidth;
                            coef *= 2;
                        }
                    }
                }
            }
        }

        offsetTime = (qCeil((startTime) / (stepDuration))) * (stepDuration);
        offsetPix = (offsetTime - (startTime)) / clipDuration * _lastWidth;

        posX = offsetPix;

        plotStepDuration = stepDuration;

        stepDuration /= 1000.0;
    }

    if (_offGrid) {

        _areaInfo->setStepPlotDuration(0);
    } else {

        _areaInfo->setStepPlotDuration(plotStepDuration);
    }

    _areaInfo->setStartOffsetX(offsetPix);

    _areaInfo->setStepPlotSize(final_size);
    _areaInfo->setGridOff(_offGrid);

    QColor grid1c = "#454545";
    QColor grid1f = "#424242";
    QColor grid1i = "#3F3F3F";
    QColor grid1aBG = "#494949";

    QColor grid2c = "#484848";
    QColor grid2f = "#464646";
    QColor grid2i = "#3C3C3C";
    QColor grid2aBG = "#4C4C4C";

    if (_areaInfo->themeType() == AreaInfo::ThemeType_Theme2) {
        grid1c = "#6E6E6E";
        grid1f = "#696969";
        grid1i = "#5A5A5A";
        grid1aBG = "#6F6F6F";

        grid2c = "#686868";
        grid2f = "#646464";
        grid2i = "#5F5F5F";
        grid2aBG = "#747474";
    }

    QColor grid3c = "#5A5A5A";
    QColor grid3f = "#565656";
    QColor grid3i = "#535353";
    QColor grid3aBG = "#606060";

    QColor grid4c = "#5F5F5F";
    QColor grid4f = "#5B5B5B";
    QColor grid4i = "#4F4F4F";
    QColor grid4aBG = "#656565";

    if (_areaInfo->themeType() == AreaInfo::ThemeType_Theme2) {
        grid3c = "#8C8C8C";
        grid3f = "#868686";
        grid3i = "#747474";
        grid3aBG = "#8D8D8D";

        grid4c = "#858585";
        grid4f = "#808080";
        grid4i = "#7A7A7A";
        grid4aBG = "#949494";
    }

    m = Measure::timeToMeasure(1000, lob, bpb, _tempo);

    double barSize = (m.barDuration * 1000.0) / clipDuration * _lastWidth;
    double beatSize = (m.beatDuration * 1000.0) / clipDuration * _lastWidth;
    double stepColorSize = barSize;
    double stepColorDuration = m.barDuration;

    if (stepColorSize < min_color) {

        int step = 1;
        stepColorSize = barSize;
        stepColorDuration = m.barDuration;
        while (stepColorSize < min_color) {
            stepColorSize = pow(2, step) * barSize;
            stepColorDuration = pow(2, step) * m.barDuration;

            step++;
        }

    } else if (stepColorSize > max_color_2) {
        stepColorSize = beatSize;
        stepColorDuration = m.beatDuration;

    } else {
    }

    _areaInfo->setStepColorDuration(stepColorDuration);
    _areaInfo->setStepColorSize(stepColorSize);

    _areaInfo->setStepDuration(stepColorSize / _lastWidth * clipDuration);

    double off = startTime / clipDuration * _lastWidth;
    double EndPicX = (1 - (off / stepColorSize - qFloor(off / stepColorSize))) * stepColorSize;
    double StartPicX = 0;
    if (startTime == 0) {
        EndPicX = stepColorSize;
    }

    double realStartX = off;
    int stepCounter = qFloor(realStartX / stepColorSize);

    double endX = (startTime + clipDuration) * (_lastWidth / clipDuration);

    QMap<int, QPair<double, double>> areaList;

    stepColorDuration = (clipDuration / 1000.0) / (double)_lastWidth * final_size;

    auto selected_track = _areaInfo->selectedTrackItem();

    m_trackSegRects.clear();
    m_trackSegLines.clear();
    m_trackSelectedRects.clear();
    m_plotLines.clear();
    m_plotRects.clear();

    auto master_track_rect = _masterTrack->totalRect();

    QList<RangeItem> ranges;
    QVector<QPointF> _rect_points;
    QVector<int> _rect_colors;

    while (realStartX < endX) {

        QColor color;
        if (stepCounter % 2 == 0) {
            color = grid1aBG;
        } else {
            color = grid2aBG;
        }
        m_plotRects.append(RectSeg {
            QRectF(StartPicX, master_track_rect.top(), EndPicX - StartPicX, master_track_rect.bottom()), color });

        _rect_points.append(QPointF(StartPicX, EndPicX - StartPicX));
        _rect_colors.append(stepCounter);

        areaList.insert(stepCounter, QPair<double, double>(StartPicX, EndPicX));

        realStartX += (EndPicX - StartPicX) + 0.00001;

        StartPicX = EndPicX;
        EndPicX = EndPicX + stepColorSize;

        stepCounter = qFloor(realStartX / stepColorSize);
    }

    QVector<QPointF> _line_points;
    QVector<int> _line_colors;
    QVector<Measure> _line_measures;

    off = startTime / clipDuration * _lastWidth;
    EndPicX = (1 - (off / stepColorSize - qFloor(off / stepColorSize))) * stepColorSize;
    StartPicX = 0;
    if (startTime == 0) {
        EndPicX = stepColorSize;
    }

    realStartX = off;
    stepCounter = qFloor(realStartX / stepColorSize);

    double currentTime = offsetTime / 1000.0;

    endX = (startTime + clipDuration) * (_lastWidth / clipDuration);
    posX = offsetPix;

    stepColorDuration = (clipDuration / 1000.0) / (double)_lastWidth * final_size;

    while (posX < _lastWidth && stepColorDuration > 0) {

        m = Measure::timeToMeasure(currentTime * 1000.0, lob, bpb, _tempo);

        auto pairs = areaList.values();
        int index = 0;

        for (int i = 0; i < areaList.size(); i++) {

            if (pairs[i].first <= posX && posX < pairs[i].second) {
                index = i;
                break;
            }
        }

        QColor color;
        if (stepCounter % 2 == 0) {
            if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                color = grid1i;
            } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                color = grid1f;
            } else {
                color = grid1c;
            }

        } else {
            if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                color = grid2i;
            } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                color = grid2f;
            } else {
                color = grid2c;
            }
        }

        auto stepCounter = areaList.keys()[index];

        m_plotLines.append(LineSeg { QPointF(posX, master_track_rect.top()), QPointF(posX, master_track_rect.bottom()), color });

        _line_points.append(QPointF(posX, posX));
        _line_colors.append(stepCounter);
        _line_measures.append(m);

        posX += final_size;

        currentTime += stepColorDuration;
    }

    for (int _trackIndex = 0; _trackIndex < _tracksModel->size(); _trackIndex++) {

        QVector<RectSeg> m_rectsA;
        QVector<LineSeg> m_linesA;
        QVector<RectSeg> m_selected_rects;
        auto trackItem = _tracksModel->getTrackItem(_trackIndex);
        auto selected_area = trackItem->drawSelectedArea();

        auto _rect = trackItem->totalRect();
        int Y = _rect.y();
        int B = _rect.bottom();
        int H = _rect.height();
        int T = _rect.top();

        for (int j = 0; j < _rect_points.size(); j++) {

            QColor color;
            if (selected_track != _trackIndex) {

                if (_rect_colors[j] % 2 == 0) {
                    color = grid1aBG;
                } else {
                    color = grid2aBG;
                }
            } else {

                if (_rect_colors[j] % 2 == 0) {
                    color = grid3aBG;
                } else {
                    color = grid4aBG;
                }
            }

            auto rect = QRectF(_rect_points[j].x(), Y, _rect_points[j].y(), B - Y);
            m_rectsA.append(RectSeg {
                rect, color });

            ranges.append({ rect.left(), rect.right(), _rect_colors[j] % 2 });

            if (final_size > stepColorSize || final_size == 0) {

                const double EPSILON = 1e-9;
                auto rect = QRectF(_rect_points[j].x(), selected_area.top(), _rect_points[j].y(), selected_area.height());

                if (rect.left() >= selected_area.left() - EPSILON && rect.right() <= selected_area.right() + EPSILON) {

                    if (_rect_colors[j] % 2 == 0) {
                        m_selected_rects.append(RectSeg {
                            rect, "#96D4E8" });
                    }

                    if (_rect_colors[j] % 2 == 1) {
                        m_selected_rects.append(RectSeg {
                            rect, "#88C0D2" });
                    }
                }
            }
        }

        for (int j = 0; j < _line_points.size(); j++) {

            QColor color;
            auto m = _line_measures[j];

            if (selected_track != _trackIndex) {
                if (_line_colors[j] % 2 == 0) {
                    if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                        color = grid1i;
                    } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                        color = grid1f;
                    } else {
                        color = grid1c;
                    }

                } else {
                    if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                        color = grid2i;
                    } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                        color = grid2f;
                    } else {
                        color = grid2c;
                    }
                }
            } else {
                if (_line_colors[j] % 2 == 0) {
                    if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                        color = grid3i;
                    } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                        color = grid3f;
                    } else {
                        color = grid3c;
                    }

                } else {
                    if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                        color = grid4i;
                    } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                        color = grid4f;
                    } else {
                        color = grid4c;
                    }
                }
            }

            if (final_size <= stepColorSize) {
                const double EPSILON = 1e-9;
                auto rect = QRectF(_line_points[j].x(), selected_area.top(), final_size, selected_area.height());

                if (rect.left() >= selected_area.left() - EPSILON && rect.right() <= selected_area.right() + EPSILON) {
                    int value = findValueForNumber(_line_points[j].x(), ranges);

                    if (value == 0) {
                        m_selected_rects.append(RectSeg {
                            rect, "#96D4E8" });
                    }

                    if (value == 1) {
                        m_selected_rects.append(RectSeg {
                            rect, "#88C0D2" });
                    }
                }
            }

            m_linesA.append(LineSeg { QPointF(_line_points[j].x(), Y), QPointF(_line_points[j].y(), B), color });
        }

        m_trackSegRects.append(m_rectsA);

        m_trackSegLines.append(m_linesA);

        m_trackSelectedRects.append(m_selected_rects);

        if (m_selected_rects.size() > 0) {

            auto rect_first = m_selected_rects.first().rect.toRect();
            auto rect_last = m_selected_rects.last().rect.toRect();
            if (rect_first.width() > 0) {
                int w = qAbs(rect_first.left() - rect_last.right());

                _areaInfo->setSelectionAreaRect(QRect(rect_first.left(), rect_first.top(),
                    w, rect_first.bottom()));
            }
        }
    }

    // MasterTrack 911

    {
        auto _clipDuration = _areaInfo->clipDuration();

        auto _selectedArea = _areaInfo->masterTrackSelectedArea()->area();
        double startX = ((_selectedArea.left() - _startTime) / (double)_clipDuration) * _lastWidth;
        double endX = ((_selectedArea.right() - _startTime) / (double)_clipDuration) * _lastWidth;
        if (_selectedArea.left() < _selectedArea.right()) {
            startX = ((_selectedArea.left() - _startTime) / (double)_clipDuration) * _lastWidth;
            endX = ((_selectedArea.right() - _startTime) / (double)_clipDuration) * _lastWidth;
        } else {
            endX = ((_selectedArea.left() - _startTime) / (double)_clipDuration) * _lastWidth;
            startX = ((_selectedArea.right() - _startTime) / (double)_clipDuration) * _lastWidth;
        }

        QVector<RectSeg> m_rectsA;
        QVector<LineSeg> m_linesA;
        QVector<RectSeg> m_selected_rects;
        auto trackItem = _masterTrack;
        auto _rect = trackItem->totalRect();

        auto selected_area = QRectF(startX, _rect.top(), endX - startX, _rect.height());

        // qDebug() << "_rect" << _rect << _selectedArea << selected_area << startX << endX << _clipDuration << _lastWidth;
        int Y = _rect.y();
        int B = _rect.bottom();
        int H = _rect.height();
        int T = _rect.top();

        for (int j = 0; j < _rect_points.size(); j++) {

            QColor color;
            if (_rect_colors[j] % 2 == 0) {
                color = grid1aBG;
            } else {
                color = grid2aBG;
            }
            if (final_size > stepColorSize || final_size == 0) {

                const double EPSILON = 1e-9;
                auto rect = QRectF(_rect_points[j].x(), selected_area.top(), stepColorSize, selected_area.height());

                if (rect.left() >= selected_area.left() - EPSILON && rect.right() <= selected_area.right() + EPSILON) {

                    if (_rect_colors[j] % 2 == 0) {
                        m_selected_rects.append(RectSeg {
                            rect, "#96D4E8" });
                    }

                    if (_rect_colors[j] % 2 == 1) {
                        m_selected_rects.append(RectSeg {
                            rect, "#88C0D2" });
                    }
                }

                if (rect.left() < selected_area.left() + EPSILON && rect.right() > selected_area.left() - EPSILON) {

                    rect.setLeft(selected_area.left());
                    rect.setWidth(selected_area.width());
                    if (_rect_colors[j] % 2 == 0) {
                        m_selected_rects.append(RectSeg {
                            rect, "#96D4E8" });
                    }

                    if (_rect_colors[j] % 2 == 1) {
                        m_selected_rects.append(RectSeg {
                            rect, "#88C0D2" });
                    }
                }

                if (rect.left() < selected_area.right() + EPSILON && rect.right() > selected_area.right() - EPSILON) {

                    rect.setWidth(selected_area.right() - rect.left());

                    if (_rect_colors[j] % 2 == 0) {
                        m_selected_rects.append(RectSeg {
                            rect, "#96D4E8" });
                    }

                    if (_rect_colors[j] % 2 == 1) {
                        m_selected_rects.append(RectSeg {
                            rect, "#88C0D2" });
                    }
                }
            }

            auto rect = QRectF(_rect_points[j].x(), Y, _rect_points[j].y(), B - Y);
            m_rectsA.append(RectSeg {
                rect, color });
        }

        for (int j = 0; j < _line_points.size(); j++) {

            QColor color;
            auto m = _line_measures[j];

            if (_line_colors[j] % 2 == 0) {
                if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                    color = grid1i;
                } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                    color = grid1f;
                } else {
                    color = grid1c;
                }

            } else {
                if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                    color = grid2i;
                } else if (m.sixteenth == 1 && m.subSixteenth == 0) {
                    color = grid2f;
                } else {
                    color = grid2c;
                }
            }
            if (final_size <= stepColorSize) {
                const double EPSILON = 1e-9;
                auto rect = QRectF(_line_points[j].x(), selected_area.top(), final_size, selected_area.height());

                if (rect.left() >= selected_area.left() - EPSILON && rect.right() <= selected_area.right() + EPSILON) {

                    if (_line_colors[j] % 2 == 0) {
                        m_selected_rects.append(RectSeg {
                            rect, "#96D4E8" });
                    }

                    if (_line_colors[j] % 2 == 1) {
                        m_selected_rects.append(RectSeg {
                            rect, "#88C0D2" });
                    }
                }

                if (rect.left() <= selected_area.right() && rect.right() >= selected_area.right() + EPSILON) {

                    rect.setWidth(selected_area.right() - rect.left());
                    if (_line_colors[j] % 2 == 0) {
                        m_selected_rects.append(RectSeg {
                            rect, "#96D4E8" });
                    }

                    if (_line_colors[j] % 2 == 1) {
                        m_selected_rects.append(RectSeg {
                            rect, "#88C0D2" });
                    }
                }
            }

            m_linesA.append(LineSeg { QPointF(_line_points[j].x(), Y), QPointF(_line_points[j].y(), B), color });
        }
        m_masterTrackSelectedRects = m_selected_rects;
    }

    Q_EMIT sigUpdateUI();
}

QSharedPointer<TrackItem> ClipArea::masterTrack() const
{
    return _masterTrack;
}

void ClipArea::setSelectingStateBeforeResizing(const QJsonArray& newSelectingStateBeforeResizing)
{
    _selectingStateBeforeResizing = newSelectingStateBeforeResizing;
}

QJsonArray ClipArea::selectingStateBeforeResizing() const
{
    return _selectingStateBeforeResizing;
}

int ClipArea::trackLineResizingStartY() const
{
    return _trackLineResizingStartY;
}

void ClipArea::setTrackLineResizingStartY(int newTrackLineResizingStartY)
{
    _trackLineResizingStartY = newTrackLineResizingStartY;
}

QVector<AutomationList> ClipArea::movingAutomationPoint() const
{
    return _movingAutomationPoint;
}

double ClipArea::startSelectedTime() const
{
    return _startSelectedTime;
}

void ClipArea::setStartSelectedTime(double newStartSelectedTime)
{
    _startSelectedTime = newStartSelectedTime;
}

int ClipArea::trackLineResizing() const
{
    return _trackLineResizing;
}

void ClipArea::setTrackLineResizing(int newTrackLineResizing)
{
    _trackLineResizing = newTrackLineResizing;
}

bool ClipArea::selectedAreaMoved() const
{
    return _selectedAreaMoved;
}

void ClipArea::setSelectedAreaMoved(bool newSelectedAreaMoved)
{
    _selectedAreaMoved = newSelectedAreaMoved;
}

int ClipArea::startSelectedSubTrackIndex() const
{
    return _startSelectedSubTrackIndex;
}

void ClipArea::setStartSelectedSubTrackIndex(int newStartSelectedSubTrackIndex)
{
    _startSelectedSubTrackIndex = newStartSelectedSubTrackIndex;
}

int ClipArea::startSelectedTrackIndex() const
{
    return _startSelectedTrackIndex;
}

void ClipArea::setStartSelectedTrackIndex(int newStartSelectedTrackIndex)
{
    _startSelectedTrackIndex = newStartSelectedTrackIndex;
}

bool ClipArea::keyFramesPointSelected() const
{
    return _keyFramesPointSelected;
}

void ClipArea::setKeyFramesPointSelected(bool newKeyFramesPointSelected)
{
    _keyFramesPointSelected = newKeyFramesPointSelected;
}

bool ClipArea::selectingAreaMoving() const
{
    return _selectingAreaMoving;
}

void ClipArea::setSelectingAreaMoving(bool newSelectingAreaMoving)
{
    _selectingAreaMoving = newSelectingAreaMoving;
}

bool ClipArea::automationChanging() const
{
    return _automationChanging;
}

void ClipArea::setAutomationChanging(bool newAutomationChanging)
{
    _automationChanging = newAutomationChanging;
}

bool ClipArea::selectingAreaCreating() const
{
    return _selectingAreaCreating;
}

void ClipArea::setSelectingAreaCreating(bool newSelectingAreaCreating)
{
    _selectingAreaCreating = newSelectingAreaCreating;
}

QString ClipArea::startSelectedClipIndex() const
{
    return _startSelectedClipIndex;
}

void ClipArea::setStartSelectedClipIndex(const QString& newStartSelectedClipIndex)
{
    _startSelectedClipIndex = newStartSelectedClipIndex;
}

bool ClipArea::selectingAreaResizing() const
{
    return _selectingAreaResizing;
}

void ClipArea::setSelectingAreaResizing(bool newSelectingAreaResizing)
{
    _selectingAreaResizing = newSelectingAreaResizing;
}

void ClipArea::sltReadyToPlay()
{

    if (AudioManager::getSong() == Q_NULLPTR) {
        return;
    }

    if (_requestForLoading == false) {

        _numberOfSampleBuffer.clear();
        for (int i = 0; i < _tracksModel->size(); i++) {
            _numberOfSampleBuffer.append(_tracksModel->trackItem(i)->countPositionChanged(_areaInfo->playheadMarker()));
        }

        if (_numberOfSampleBuffer.size() > 0) {
            bool isPause = AudioManager::getSong()->isPause();

            if (isPause == false) {
                AudioManager::getSong()->setPause();
                Q_EMIT pausedChanged();
            }

            _requestForLoading = true;

            for (int i = 0; i < _tracksModel->size(); i++) {

                _tracksModel->trackItem(i)->setPositionChanged(_areaInfo->playheadMarker(), true);
            }
        }
    }
}

void ClipArea::applySelectedAreaToTracks(double endSelectedTime, QPointF lastPos, int trackIndex, int subTrack)
{

    if (endSelectedTime < _startSelectedTime) {
        _areaInfo->selectionArea()->setLeft(endSelectedTime);
        _areaInfo->selectionArea()->setRight(_startSelectedTime);
    } else {
        _areaInfo->selectionArea()->setRight(endSelectedTime);
        _areaInfo->selectionArea()->setLeft(_startSelectedTime);
    }

    if (trackIndex > _startSelectedTrackIndex) {

        _areaInfo->selectionArea()->setfirstTrackIndex(_startSelectedTrackIndex);
        _areaInfo->selectionArea()->setLastTrackIndex(trackIndex);

        _areaInfo->selectionArea()->setfirstSubTrackIndex(_startSelectedSubTrackIndex);
        _areaInfo->selectionArea()->setLastSubTrackIndex(subTrack);

    } else if (trackIndex < _startSelectedTrackIndex) {

        _areaInfo->selectionArea()->setfirstTrackIndex(trackIndex);
        _areaInfo->selectionArea()->setLastTrackIndex(_startSelectedTrackIndex);

        _areaInfo->selectionArea()->setfirstSubTrackIndex(subTrack);
        _areaInfo->selectionArea()->setLastSubTrackIndex(_startSelectedSubTrackIndex);

    } else {

        _areaInfo->selectionArea()->setfirstTrackIndex(trackIndex);
        _areaInfo->selectionArea()->setLastTrackIndex(trackIndex);

        if (subTrack > _startSelectedSubTrackIndex) {

            _areaInfo->selectionArea()->setfirstSubTrackIndex(_startSelectedSubTrackIndex);
            _areaInfo->selectionArea()->setLastSubTrackIndex(subTrack);

        } else if (subTrack < _startSelectedSubTrackIndex) {

            _areaInfo->selectionArea()->setfirstSubTrackIndex(subTrack);
            _areaInfo->selectionArea()->setLastSubTrackIndex(_startSelectedSubTrackIndex);

        } else {

            _areaInfo->selectionArea()->setfirstSubTrackIndex(subTrack);
            _areaInfo->selectionArea()->setLastSubTrackIndex(subTrack);
        }
    }

    _areaInfo->selectionArea()->lastPoint = lastPos;

    if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true) {
        _areaInfo->selectionArea()->isClipSelectedAtLast = true;
    } else {
        if (!_tracksModel->trackItem(trackIndex)->automationLaneEnabled()) {
            _areaInfo->selectionArea()->isClipSelectedAtLast = false;
        } else {
            if (_tracksModel->trackItem(trackIndex)->containTitleGlobal(lastPos)) {
                _areaInfo->selectionArea()->isClipSelectedAtLast = true;
            } else {
                _areaInfo->selectionArea()->isClipSelectedAtLast = false;
            }
        }
    }

    if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true || _areaInfo->selectionArea()->isClipSelectedAtLast == true) {
        _areaInfo->selectionArea()->setfirstSubTrackIndex(-1);
        _areaInfo->selectionArea()->setLastSubTrackIndex(_tracksModel->trackItem(_areaInfo->selectionArea()->lastTrackIndex())->automationLaneModel()->count());
    }

    _areaInfo->selectionArea()->update();
    _areaInfo->selectionArea()->last_width = _areaInfo->selectionArea()->area().width();
}

QPair<int, int> ClipArea::findTrackIndexByPoint(QPointF point)
{
    int index = -1;
    int subTrack = -1;

    for (int i = 0; i < _tracksModel->size(); i++) {
        bool result = _tracksModel->trackItem(i)->contains_global(QPoint(point.x(), point.y()));

        if (result) {
            subTrack = _tracksModel->trackItem(i)->automationLaneModel()->getIndexGlobal(point);

            index = i;
            break;
        }
    }

    if (index == -1) {
        if (_tracksModel->size() > 0) {

            // qDebug() << "point.y()" << point.y() << _tracksModel->trackItem(0)->getTotalRect().top() << _tracksModel->trackItem(_tracksModel->size() - 1)->getTotalRect().bottom();
            if (point.y() < _tracksModel->trackItem(0)->getTotalRect().top()) {
                return QPair<int, int>(0, -1);
            }
            if (point.y() > _tracksModel->trackItem(_tracksModel->size() - 1)->getTotalRect().bottom()) {
                if (_tracksModel->trackItem(_tracksModel->size() - 1)->automationLaneEnabled()) {
                    if (_tracksModel->trackItem(_tracksModel->size() - 1)->automationLaneModel()->count() > 0) {
                        return QPair<int, int>(_tracksModel->size() - 1, _tracksModel->trackItem(_tracksModel->size() - 1)->automationLaneModel()->count() - 1);
                    }
                }
                return QPair<int, int>(_tracksModel->size() - 1, -1);
            }
        }
    }

    return QPair<int, int>(index, subTrack);
}

void ClipArea::createSelectedArea(QSharedPointer<ClipItem> item, int firstTrack, int lastTrack, double startTime, double endTime, int top,
    int bottom, int stratSubTrack, int lastSubTrack)
{
    if (item) {
        if (item->sampleClip()->startTime() < startTime) {
            startTime = item->sampleClip()->startTime();
        }
        if (item->sampleClip()->endTime() > endTime) {
            endTime = item->sampleClip()->endTime();
        }
    }

    double midSelectedAreaY = (top + bottom) / 2.0;
    double midY = (_tracksModel->trackItem(lastTrack)->rect().top() + _tracksModel->trackItem(lastTrack)->rect().bottom()) / 2.0;

    if (midY <= midSelectedAreaY) {
        top = _tracksModel->trackItem(lastTrack)->rect().top();
    } else {
        bottom = _tracksModel->trackItem(lastTrack)->rect().bottom();
    }

    _areaInfo->selectionArea()->setfirstTrackIndex(firstTrack);
    _areaInfo->selectionArea()->setLastTrackIndex(lastTrack);
    _areaInfo->selectionArea()->setLeft(startTime);
    _areaInfo->selectionArea()->setRight(endTime);
    _areaInfo->selectionArea()->update();

    _areaInfo->selectionArea()->setfirstSubTrackIndex(stratSubTrack);
    _areaInfo->selectionArea()->setLastSubTrackIndex(lastSubTrack);

    if (!_tracksModel->trackItem(firstTrack)->automationLaneEnabled()) {
        _areaInfo->selectionArea()->isClipSelectedAtFirst = true;
        _areaInfo->selectionArea()->isClipSelectedAtLast = true;

    } else {
        if (_areaInfo->selectionArea()->isClipSelectedAtFirst == true) {
            _areaInfo->selectionArea()->isClipSelectedAtLast = true;
        } else {
            _areaInfo->selectionArea()->isClipSelectedAtFirst = false;
            _areaInfo->selectionArea()->isClipSelectedAtLast = false;
        }
    }
}

double ClipArea::time2Frame(double time)
{
    return time / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
}

ClipArea::ClipArea(AreaInfo* areaInfo, QObject* parent)
    : m_renderManager(nullptr)
    , _areaInfo(areaInfo)
{
    _defaultSavePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    _timerCursorPosition.setInterval(5);
    connect(&_timerCursorPosition, &QTimer::timeout, this, &ClipArea::sltTimerCheckPosition);

    connect(AudioManager::getSong(), &Song::sigReadyToPlay,
        this, &ClipArea::sltReadyToPlay);

    connect(&_timerCheckStartContentYChanged, &QTimer::timeout, this, &ClipArea::sltTimerCheckStartContentYChanged);
    _timerCheckStartContentYChanged.setInterval(10);
    _timerCheckStartContentYChanged.start();

    initialize();
}

ClipArea::~ClipArea()
{
    qDebug() << "~ClipArea()";

    _tracksModel.clear();
    ActionStack::destroy();
}

void ClipArea::sltZoomFactorChanged()
{
    _tracksModel->updateTracksHeight();
    _areaInfo->updateUI();
}

void ClipArea::initialize()
{

    if (AudioManager::getSong() == Q_NULLPTR) {
        return;
    }

    _tracksModel = QSharedPointer<TracksModel>(new TracksModel(_areaInfo));
    connect(_tracksModel.data(), &TracksModel::sigUpdate, this, &ClipArea::drawGrids);
    connect(_tracksModel.data(), &TracksModel::sigClipChanged, this, &ClipArea::drawGrids);

    connect(_areaInfo, &AreaInfo::sigPlayBackUpdateTimeout, this, &ClipArea::sltPlayBackUpdateTimeout);
    connect(_areaInfo, &AreaInfo::arrangementStartTimeChanged, [this]() {
        auto paused = AudioManager::inst()->getSong()->isPause();
        auto playing = AudioManager::inst()->getSong()->isPlaying();

        if (paused || !playing) {
            for (int i = 0; i < _tracksModel->size(); i++) {
                _tracksModel->trackItem(i)->setIndicatorEnable(TrackItem::IndicatorType_Disable);
            }
        }
    });

    connect(_areaInfo, &AreaInfo::zoomFactorChanged, this, &ClipArea::sltZoomFactorChanged);

    connect(_areaInfo->keyEventFilter(), &KeyEventFilter::keyPressed, this, &ClipArea::sltKeyPressed);
    connect(_areaInfo->keyEventFilter(), &KeyEventFilter::keyRelease, this, &ClipArea::sltKeyReleased);
    connect(_areaInfo->keyEventFilter(), &KeyEventFilter::focusOut, this, &ClipArea::sltFocusOut);

    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &ClipArea::sltFocusActiveChanged);
    connect(_areaInfo, &AreaInfo::startBraceMarkerChanged, this, &ClipArea::sltFocusActiveChanged);
    connect(_areaInfo, &AreaInfo::endBraceMarkerChanged, this, &ClipArea::sltFocusActiveChanged);

    connect(_areaInfo, &AreaInfo::sig3dEngineStarted, this, &ClipArea::slt3dEngineStarted);

    connect(_areaInfo, &AreaInfo::sigSelectedTrackItemChanged, this, &ClipArea::sltSelectedTrackItemChanged);

    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &ClipArea::drawGrids);
    connect(_areaInfo, &AreaInfo::timeSelectionChanged, this, &ClipArea::drawGrids);
    connect(_areaInfo, &AreaInfo::themeTypeChanged, this, &ClipArea::drawGrids);

    connect(AudioManager::getSong(), &Song::playbackStateChanged, this, &ClipArea::playingChanged);

    Q_EMIT playingChanged();
    auto masterTrack = AudioManager::getSong()->addSampleTrack(_areaInfo, -1);
    _masterTrack = QSharedPointer<TrackItem>(new TrackItem(-1, masterTrack, _areaInfo));
    _masterTrack->setMasterTrack(true);

    addSampleClip();
    addSampleClip();
    addSampleClip();
    addSampleClip();

    Q_EMIT sigInitialized();
}

void ClipArea::sltSelectedTrackItemChanged()
{

    int totalH = -_areaInfo->startContentY();
    if (_tracksModel->size() >= 1) {
        for (int i = 0; i < _tracksModel->size(); i++) {

            auto new_rect = _tracksModel->trackItem(i)->getTotalRect();

            if (_areaInfo->selectedTrackItem() == i) {
                auto last_rect = _tracksModel->trackItem(i)->getTotalRect();

                last_rect = QRect(last_rect.left(), totalH, last_rect.width(), last_rect.height());
                _areaInfo->setSelectedTrackItemRect(last_rect);
            }
            totalH += new_rect.height();
        }
    }

    drawGrids();

    //    auto rect_tmp = _rootItem->child(i)->trackItem()->getTotalRect();
    //    rect_tmp.setTop(rect_tmp.top());
    //    rect_tmp.setHeight(_rootItem->child(i)->trackItem()->getTotalRect().height());
    //    m_areaInfo->setSelectedTrackItemRect(rect_tmp);
}

void ClipArea::slt3dEngineStarted()
{
    for (int i = 0; i < _loadedObjects.size(); i++) {

        if (_loadedObjects[i].isNull() || _loadedObjects[i].isEmpty() || _loadedObjects[i] == "{}") {
            continue;
        }

        _tracksModel->setObjectModelEnabled(i, true);

        add3dObject(i, true);
        _areaInfo->updateUI();
    }

    Q_EMIT sigProjectLoaded(_loadedObjects);
}

void ClipArea::sltFocusActiveChanged()
{
    if (_areaInfo->loopFocusActive()) {
        _areaInfo->selectionArea()->setfirstTrackIndex(0);
        _areaInfo->selectionArea()->setfirstSubTrackIndex(-1);
        _areaInfo->selectionArea()->setLastTrackIndex(_tracksModel->size() - 1);
        _areaInfo->selectionArea()->setLastSubTrackIndex(_tracksModel->trackItem(_tracksModel->size() - 1)->automationLaneModel()->count() - 1);
        _areaInfo->selectionArea()->setLeft(_areaInfo->startBraceMarker());
        _areaInfo->selectionArea()->setRight(_areaInfo->endBraceMarker());

        _areaInfo->selectionArea()->update();

        _areaInfo->selectionArea()->isClipSelectedAtFirst = true;
        _areaInfo->selectionArea()->isClipSelectedAtLast = true;
    }
    drawGrids();
}

void ClipArea::sltPlayBackUpdateTimeout()
{

    if (AudioManager::getSong()->isPause() == false) {

        double duration_in_seconds = (AudioManager::getSong()->getPlayPos().getTicks() * 256.0) / AudioManager::audioEngine()->outputSampleRate();

        auto _playbackMarker = duration_in_seconds * 1000.0;
        _areaInfo->setPlayheadMarker(_playbackMarker);

        for (int i = 0; i < _tracksModel->size(); i++) {

            _tracksModel->automatedValueChanged(i, _playbackMarker);
        }

        _areaInfo->updateUI();
    } else {

        for (int i = 0; i < _tracksModel->size(); i++) {
            _tracksModel->updateTruePeak(i);
        }
    }
}

void ClipArea::sltStartContentYChanged()
{

    auto offset = _areaInfo->startContentY();

    int totalH = -_areaInfo->startContentY();
    if (_tracksModel->size() >= 1) {
        for (int i = 0; i < _tracksModel->size(); i++) {

            auto new_rect = _tracksModel->trackItem(i)->getTotalRect();

            if (_areaInfo->selectedTrackItem() == i) {
                auto last_rect = _tracksModel->trackItem(i)->getTotalRect();

                last_rect = QRect(last_rect.left(), totalH, last_rect.width(), last_rect.height());
                _areaInfo->setSelectedTrackItemRect(last_rect);
            }
            totalH += new_rect.height();
        }
    }

    if (_tracksModel->size() > 0) {

        int sY = -offset;

        _tracksModel->trackItem(0)->setGlobalYPosition(sY);

        for (int i = 1; i < _tracksModel->size(); i++) {

            int sumH = _tracksModel->trackItem(i - 1)->getTotalRect().height();
            sY += sumH;
            _tracksModel->trackItem(i)->setGlobalYPosition(sY);
        }
    }

    //    _areaInfo->selectionArea()->setfirstTrackIndex(0);
    //    _areaInfo->selectionArea()->setLastTrackIndex(0);
    //    _areaInfo->lastSelectedArea()->setfirstTrackIndex(0);
    //    _areaInfo->lastSelectedArea()->setLastTrackIndex(0);

    QList<QRectF> _rects;
    for (int i = 0; i < _tracksModel->size(); i++) {
        _rects.append(_tracksModel->trackItem(i)->rect());
    }
    _areaInfo->selectionArea()->setTracksRect(_rects);
    _areaInfo->lastSelectedArea()->setTracksRect(_rects);

    for (int i = 0; i < _tracksModel->size(); i++) {

        auto _boundingRect = _tracksModel->trackItem(i)->rect();
        auto rect = _boundingRect;
        rect.setY(tracksModel()->trackItem(i)->globalYPosition());
        rect.setHeight(_boundingRect.height());

        _areaInfo->selectionArea()->setTracksRect(i, rect);
        _areaInfo->lastSelectedArea()->setTracksRect(i, rect);
    }

    _areaInfo->updateUI();
}

void ClipArea::sltTimerCheckStartContentYChanged()
{

    if (_lastStartContentY != _areaInfo->startContentY()) {
        sltStartContentYChanged();
        _lastStartContentY = _areaInfo->startContentY();
    }

    if (_automationChanging == false && _mouseMoveEvent) {

        if (_selectingAreaMoving) {

            _lastMouseMovingPoint = QPoint(_currentMouseX, 0);

            double final_xpos = 0;

            final_xpos = _areaInfo->findSnap(_currentMouseX);
            auto time = _areaInfo->pixel2Time(final_xpos);

            auto width = _areaInfo->selectionArea()->area().width();
            auto height = _areaInfo->selectionArea()->area().height();

            double newLeft = time - _areaInfo->selectionArea()->leftDiff;

            double newleftPixel = _areaInfo->time2Pixel(newLeft);
            final_xpos = _areaInfo->findSnap(newleftPixel);
            newLeft = _areaInfo->pixel2Time(final_xpos);

            double newTopPixel = _currentMouseY - _areaInfo->selectionArea()->topDiff;

            int diffIndex = _areaInfo->selectionArea()->lastTrackIndex() - _areaInfo->selectionArea()->firstTrackIndex();
            // int firstIndex = findTrackIndexByPoint(QPointF(newleftPixel, newTopPixel));
            auto tmp = findTrackIndexByPoint(QPointF(newleftPixel, newTopPixel));

            // qDebug() << "tmp" << tmp.first << tmp.second << newleftPixel << newTopPixel;
            int firstIndex = tmp.first;

            auto trackArea = _boundingRect;
            trackArea.setY(0);
            trackArea.setHeight(_tracksModel->trackItem(_tracksModel->size() - 1)->getTotalRect().bottom());

            if (_currentMouseY > _boundingRect.bottom() - _tracksModel->trackItem(_tracksModel->size() - 1)->getTotalRect().height()) {
                _listViewPositionChanged = 10;
            }

            if (_currentMouseY < _boundingRect.top()) {
                _listViewPositionChanged = -10;
            }

            if (_areaInfo->selectionArea()->area() != _areaInfo->lastSelectedArea()->area() && _selectedAreaMoved == false) {
                _selectedAreaMoved = true;
            }

            if (firstIndex + diffIndex < _tracksModel->size() && firstIndex >= 0) {
                _areaInfo->selectionArea()->setLeft(newLeft);
                _areaInfo->selectionArea()->setWidth(width);

                _areaInfo->selectionArea()->setfirstTrackIndex(firstIndex);
                _areaInfo->selectionArea()->setLastTrackIndex(firstIndex + diffIndex);
                _areaInfo->selectionArea()->update();

                int first_track_index_order = -1;

                for (int i = 0; i < _movingItems.size(); i++) {

                    if (i == 0) {
                        first_track_index_order = _movingItems[i].trackIndex;
                    }

                    auto trackRect = _tracksModel->trackItem(firstIndex + (_movingItems[i].trackIndex - first_track_index_order))->rect();

                    _movingItems[i].trackIndex = firstIndex + (_movingItems[i].trackIndex - first_track_index_order);

                    if (_movingItems[i].clipItem == Q_NULLPTR) {

                        continue;
                    }

                    auto clipRect = _movingItems[i].clipItem->rect();

                    double startX = _areaInfo->time2Pixel(_areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff);
                    double width = _areaInfo->time2Pixel(_areaInfo->selectionArea()->area().right() - _areaInfo->selectionArea()->area().left());

                    double endX = startX + width;

                    clipRect.setLeft(startX);
                    clipRect.setRight(endX);
                    clipRect.setTop(trackRect.top());
                    clipRect.setBottom(trackRect.bottom());

                    _movingItems[i].clipItem->setRect(clipRect);

                    _movingItems[i].clipItem->setBaseColor(_tracksModel->trackItem(_movingItems[i].trackIndex)->getTitleColor());
                    _movingItems[i].clipItem->setRowsExpandedCount(_tracksModel->trackItem(_movingItems[i].trackIndex)->rowsExpandedCount());

                    if (_movingItems[i].type == 0) {

                        double startTime = _areaInfo->selectionArea()->area().left();
                        double endTime = _areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff + _movingItems[i].length;

                        auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                        _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                    }
                    if (_movingItems[i].type == 1) {

                        double length = _movingItems[i].endTime - _movingItems[i].startTime;
                        double startTime = _areaInfo->selectionArea()->area().left() + length + _movingItems[i].endTimeDiff - _movingItems[i].length;
                        double endTime = _areaInfo->selectionArea()->area().left() + length;

                        auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                        _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                    }
                    if (_movingItems[i].type == 2) {

                        double startTime = _areaInfo->selectionArea()->area().left();
                        double length = _movingItems[i].endTime - _movingItems[i].startTime;
                        double endTime = startTime + length;

                        auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                        _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                    }
                    if (_movingItems[i].type == 3) {

                        double startTime = _areaInfo->selectionArea()->area().left() - _movingItems[i].startTimeDiff;
                        double endTime = startTime + _movingItems[i].length;

                        auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                        _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);
                    }
                    if (_movingItems[i].type == 4) {

                        double startTime = _areaInfo->selectionArea()->area().left();
                        double endTime = _areaInfo->selectionArea()->area().right();

                        auto startFrame = startTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();
                        auto endFrame = endTime / 1000.0 * AudioManager::audioEngine()->outputSampleRate();

                        _movingItems[i].clipItem->sampleClip()->setStartTime(startTime);
                        _movingItems[i].clipItem->sampleClip()->setEndTime(endTime);

                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseStartFrame(startFrame);
                        _movingItems[i].clipItem->sampleClip()->sampleBuffer()->setBaseEndFrame(endFrame);

                        // qDebug() << "P1" << _movingItems[i].type << i << startFrame << endFrame;
                    }
                }

                Q_EMIT sigClipItemEdited();
            }
        }
    }

    if (_listViewPositionChanged != 0) {
        Q_EMIT sigListViewPositionChanged(_listViewPositionChanged);
    }
}

void ClipArea::play()
{

    if (_requestForLoading) {
        return;
    }
    auto _playbackMarker = _areaInfo->arrangementStartTime();
    if (_areaInfo->loopFocusActive()) {

        _playbackMarker = _areaInfo->startBraceMarker();
    } else {

        _areaInfo->setArrangementStartTime(_playbackMarker);
        _areaInfo->setPlayheadMarker(_playbackMarker);
    }

    _areaInfo->setArrangementStartTime(_playbackMarker);
    _areaInfo->setPlayheadMarker(_playbackMarker);
    _areaInfo->setPlayheadMarkerVisible(true);

    AudioManager::getSong()->readyToPlay();

    _areaInfo->sigPlayChanged(true);

    Q_EMIT pausedChanged();
}

bool ClipArea::paused()
{
    return AudioManager::getSong()->isPause();
}

bool ClipArea::playing()
{
    return AudioManager::getSong()->isPlaying();
}

void ClipArea::pause()
{
    //    if (AudioManager::getSong()->isPause() == true) {

    //        for (int i = 0; i < _trackModel->size(); i++) {
    //            if ( _trackModel->getItem(i)->currentIndicatorEnabled()) {
    //                 _trackModel->getItem(i)->setCurrentIndicator(0);
    //            }
    //        }

    //    } else {

    //        bool contain = false;
    //        for (int i = 0; i < _trackModel->size(); i++) {
    //            if ( _trackModel->getItem(i)->currentIndicatorEnabled()) {
    //                _playbackMarker =  _trackModel->getItem(i)->currentIndicator();
    //                contain = true;
    //            }
    //        }

    //        AudioManager::getSong()->setPause();
    //    }
}

void ClipArea::stop()
{

    if (AudioManager::getSong()->isPause() == false) {
        AudioManager::getSong()->setPause();
        _areaInfo->sigPlayChanged(false);
    } else {

        _areaInfo->setArrangementStartTime(0);
        _areaInfo->setPlayheadMarker(0);
        AudioManager::getSong()->stop();

        for (int i = 0; i < _tracksModel->size(); i++) {
            if (_tracksModel->trackItem(i)->indicatorEnabled()) {
                _tracksModel->trackItem(i)->setIndicator(0);
            }
        }

        _areaInfo->setPlayheadMarkerVisible(false);
    }

    Q_EMIT pausedChanged();
}

int ClipArea::trackListHeight() const
{
    return _trackListHeight;
}

void ClipArea::setTrackListHeight(int newTrackListHeight)
{
    _trackListHeight = newTrackListHeight;
}

void ClipArea::closeAllEffectWindow()
{

    for (int trackIndex = 0; trackIndex < _tracksModel->size(); trackIndex++) {

        auto sampleTrack = (SampleTrack*)_tracksModel->trackItem(trackIndex)->track();

        sampleTrack->audioPort()->effects()->clear();
    }
}
