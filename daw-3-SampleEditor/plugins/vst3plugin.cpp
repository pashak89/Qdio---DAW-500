#include "vst3plugin.h"
#include "audio/audioengine.h"
#include "audio/engine.h"
#include <QDebug>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QStringList>
#include <audio/effect.h>
#include <filesystem>
#include <juce.h>
Vst3Plugin::Vst3Plugin(QSharedPointer<Effect> effect, QObject* parent)
    : QObject(parent)
    , _effect(effect)
{
}

Vst3Plugin::~Vst3Plugin()
{
    if (_editor) {
        jplugin->editorBeingDeleted(_editor);
        delete _editor;
        _editor = Q_NULLPTR;
    }

    if (_window) {
        _window->close();
        _window.clear();
    }
    if (jplugin) {
        jplugin.release();
        jplugin = Q_NULLPTR;
    }
}

void Vst3Plugin::addConfiguredParameter(int parameterIndex)
{
    if (jplugin) {
        Array<AudioProcessorParameter*> array = jplugin->getParameters();

        if (parameterIndex >= 0 && parameterIndex < array.size()) {

            for (int i = 0; i < _parameters.size(); i++) {
                if (_parameters[i]._id == parameterIndex) {
                    return;
                }
            }

            _parameters.append(Parameter {
                parameterIndex,
                QString::fromStdString(array[parameterIndex]->getName(255).toStdString()),
                QString::fromStdString(array[parameterIndex]->getLabel().toStdString()),
                array[parameterIndex]->getNumSteps(),
                array[parameterIndex]->getValue(),
                QString::fromStdString(array[parameterIndex]->getText(array[parameterIndex]->getValue(), 255).toStdString()), array[parameterIndex]->getValue(), false, true, true });

            Q_EMIT sigNewParameterAdded(parameterIndex);
        }
    }
}

static juce::AudioPluginInstance::BusesLayout createBusLayout(
    const juce::AudioPluginInstance& plugin,
    const juce::OwnedArray<juce::AudioFormatReader>& audioInputFileReaders,
    const std::optional<unsigned int>& outputChannelCountOpt,
    unsigned int& totalNumInputChannelsOut, unsigned int& totalNumOutputChannelsOut)
{

    juce::AudioPluginInstance::BusesLayout layout;

    layout.inputBuses.add(
        juce::AudioChannelSet::canonicalChannelSet((int)totalNumInputChannelsOut));

    layout.outputBuses.add(
        juce::AudioChannelSet::canonicalChannelSet((int)totalNumOutputChannelsOut));

    return layout;
}

std::unique_ptr<AudioPluginInstance> Vst3Plugin::createPluginInstance(const String& pluginPath, const String& pluginName, int pluginIndex,
    double initialSampleRate,
    int initialBlockSize)
{

    juce::AudioPluginFormatManager audioPluginFormatManager;
    audioPluginFormatManager.addDefaultFormats();

    qDebug() << "audioPluginFormatManager" << pluginIndex << audioPluginFormatManager.getFormats().size() << QString::fromStdString(pluginPath.toStdString()) << QString::fromStdString(pluginName.toStdString());

    juce::PluginDescription pluginDescription;

    auto path = QCoreApplication::applicationDirPath() + "/plugins.xml";

    QFileInfo fileInfo(path);
    bool found = false;
    if (fileInfo.exists()) {

        XmlDocument myDocument(juce::File(path.toStdString()));

        if (auto mainElement = myDocument.getDocumentElement()) {
            for (auto* e : mainElement->getChildIterator()) {
                pluginDescription.loadFromXml(*e);
                if (pluginDescription.uniqueId == pluginIndex) {
                    qDebug() << "FOUND" << pluginDescription.uniqueId << pluginIndex << pluginDescription.uniqueId;
                    found = true;
                    break;
                }
            }
        }
    }

    if (found == false) {
        juce::OwnedArray<juce::PluginDescription> pluginDescriptions;

        juce::KnownPluginList kpl;
        kpl.scanAndAddDragAndDroppedFiles(audioPluginFormatManager, juce::StringArray(pluginPath),
            pluginDescriptions);

        for (int i = 0; i < pluginDescriptions.size(); i++) {

            if (int(pluginDescriptions[i]->uniqueId) == int(pluginIndex)) {
                pluginDescription = *pluginDescriptions[i];
            }
        }
    }

    std::unique_ptr<juce::AudioPluginInstance> plugin;
    {
        juce::String err;
        plugin = audioPluginFormatManager.createPluginInstance(pluginDescription, initialSampleRate,
            initialBlockSize, err);
    }

    return plugin;
}

void Vst3Plugin::audioProcessorParameterChanged(AudioProcessor* processor, int parameterIndex, float newValue)
{

    if (_parameters.size() > 0)
        Q_EMIT sigPerformEdit(parameterIndex, newValue);
}

void Vst3Plugin::audioProcessorChanged(AudioProcessor* processor, const ChangeDetails& details)
{
}

void Vst3Plugin::componentMovedOrResized(Component& component, bool wasMoved, bool wasResized)
{
    if (_window && _editor) {

        QScreen* screen = QGuiApplication::primaryScreen();
        qreal scaleFactor = screen->logicalDotsPerInchY() / 96.0;

        qDebug() << "SCALE_FACTOR1" << scaleFactor << _editor->getDesktopScaleFactor();
        if (_editor->getDesktopScaleFactor() == scaleFactor) {
            _window->setWidth(_editor->getWidth());
            _window->setHeight(_editor->getHeight());
        } else {
            _window->setWidth(_editor->getWidth() * scaleFactor);
            _window->setHeight(_editor->getHeight() * scaleFactor);
        }

        _window->setX(screen->availableSize().width() / 2 - _window->width() / 2);
        _window->setY(screen->availableSize().height() / 2 - _window->height() / 2);
    }
}

void Vst3Plugin::audioProcessorParameterChangeGestureBegin(AudioProcessor* processor, int parameterIndex)
{

    if (_effect->configure() == false) {
        return;
    }

    Array<AudioProcessorParameter*> array = jplugin->getParameters();

    if (parameterIndex >= 0 && parameterIndex < array.size()) {

        for (int i = 0; i < _parameters.size(); i++) {
            if (_parameters[i]._id == parameterIndex) {
                return;
            }
        }

        _parameters.append(Parameter {
            parameterIndex,
            QString::fromStdString(array[parameterIndex]->getName(255).toStdString()),
            QString::fromStdString(array[parameterIndex]->getLabel().toStdString()),
            array[parameterIndex]->getNumSteps(),
            array[parameterIndex]->getValue(),
            QString::fromStdString(array[parameterIndex]->getText(array[parameterIndex]->getValue(), 255).toStdString()), array[parameterIndex]->getValue(), false, true, true });

        Q_EMIT sigNewParameterAdded(parameterIndex);
    }
}

void Vst3Plugin::audioProcessorParameterChangeGestureEnd(AudioProcessor* processor, int parameterIndex)
{
}

bool Vst3Plugin::init(const std::string& path, const std::string& name, uint32 pluginIndex, int sampleRate, int maxBlockSize, int channelIn, int channelOut)
{

    _name = name;
    m_initialSampleRate = sampleRate;
    m_maxBlockSize = maxBlockSize;
    qDebug() << "Vst3Plugin::init" << QString::fromStdString(path) << QString::fromStdString(name) << pluginIndex << channelOut;
    jplugin = createPluginInstance(path, name, pluginIndex, sampleRate, maxBlockSize);

    totalNumInputChannels = channelIn;
    totalNumOutputChannels = channelOut;

    if (!jplugin) {
        qDebug() << "FAILED";
        return false;
    }

    return true;
}

bool Vst3Plugin::process(sampleFrame* _buf)
{

    // jplugin->prepareToPlay(sampleRate, (int)maxBlockSize);

    if (jplugin) {
        juce::MidiBuffer midiBuffer;
        jplugin->processBlock(*_buf, midiBuffer);
        return true;
    }

    return true;
}

void Vst3Plugin::prepareToplay()
{

    juce::OwnedArray<juce::AudioFormatReader> audioInputFileReaders;
    auto layout = createBusLayout(*jplugin, audioInputFileReaders, 2,
        totalNumInputChannels, totalNumOutputChannels);
    jplugin->setNonRealtime(false);

    jplugin->disableNonMainBuses();
    if (jplugin->checkBusesLayoutSupported(layout)) {
        if (!jplugin->setBusesLayout(layout)) {
            qDebug() << "Plugin does not support requested bus layout0";
        }
    }

    jplugin->prepareToPlay(m_initialSampleRate, (int)m_maxBlockSize);
    jplugin->addListener(this);

    _parameters.clear();

    jplugin->refreshParameterList();

    Array<AudioProcessorParameter*> array = jplugin->getParameters();

    _parameters.append(Parameter { -1, "DeviceOn", "", 1, 1, "On", 1, false, false, false });

    if (array.size() <= 1) {

        for (int j = 0; j < array.size(); j++) {

            int index = array[j]->getParameterIndex();

            if (index >= 0) {

                _parameters.append(Parameter {
                    index,
                    QString::fromStdString(array[j]->getName(255).toStdString()),
                    QString::fromStdString(array[j]->getLabel().toStdString()),
                    array[j]->getNumSteps(),
                    array[j]->getValue(),
                    QString::fromStdString(array[j]->getText(array[j]->getValue(), 255).toStdString()), array[j]->getValue(), false, false, true });
            }
        }
    }
}

bool Vst3Plugin::createView()
{

    if (jplugin->hasEditor()) {

        if (_window) {
            _window->close();
            _window.clear();
        }
        if (_editor) {
            delete _editor;
        }

        _window = QSharedPointer<NewQuickView>(new NewQuickView());
        connect(_window.data(), &NewQuickView::sigClose, this, &Vst3Plugin::sltWindowClosed);

        bool _needs_scale = true;
        if (jplugin->getActiveEditor()) {
            _needs_scale = true;
        }
        _editor = jplugin->createEditorIfNeeded();
        _editor->addComponentListener(this);
        _editor->addToDesktop(ComponentPeer::windowIsResizable, (void*)_window->winId());
        _editor->setVisible(true);

        QScreen* screen = QGuiApplication::primaryScreen();
        qreal scaleFactor = screen->logicalDotsPerInchY() / 96.0;

        qDebug() << "SCALE_FACTOR" << scaleFactor << _editor->getDesktopScaleFactor();
        if (_editor->getDesktopScaleFactor() == scaleFactor) {
            _window->setWidth(_editor->getWidth());
            _window->setHeight(_editor->getHeight());
        } else {
            _window->setWidth(_editor->getWidth() * scaleFactor);
            _window->setHeight(_editor->getHeight() * scaleFactor);
        }

        _window->setX(screen->availableSize().width() / 2 - _window->width() / 2);
        _window->setY(screen->availableSize().height() / 2 - _window->height() / 2);
        _window->setFlags(_window->flags()
            | Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint
            | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);
        _window->showNormal();

        Q_EMIT sigVisibleChanged(true);
        return true;
    }
    return false;
}

void Vst3Plugin::sltWindowClosed()
{
    Q_EMIT sigVisibleChanged(false);
}

void Vst3Plugin::destroyView()
{

    if (_window) {
        _window->close();
        _window.clear();
    }

    Q_EMIT sigVisibleChanged(false);
}

const std::string& Vst3Plugin::name()
{
    return _name;
}

bool Vst3Plugin::failed() const
{
    return m_failed;
}

QVector<Parameter> Vst3Plugin::parameters() const
{
    return _parameters;
}

bool Vst3Plugin::parameter(int juceIndex, Parameter& parameter)
{
    for (int i = 0; i < _parameters.size(); i++) {
        if (_parameters[i]._id == juceIndex) {
            parameter = _parameters[i];
            return true;
        }
    }
    return false;
}

void Vst3Plugin::updateParameter(int index, double normalized)
{
    if (jplugin) {

        Array<AudioProcessorParameter*> array = jplugin->getParameters();

        if (index >= 0 && index < array.size()) {

            // qDebug() << "normalized" << index << QString::fromStdString(array[index]->getName(255).toStdString()) << normalized;
            // array[index]->beginChangeGesture();
            array[index]->setValue(normalized);
            // array[index]->endChangeGesture();
        }
    }
}

void Vst3Plugin::setParameter(int index, uint32 id, double normalized)
{
    if (jplugin) {

        Array<AudioProcessorParameter*> array = jplugin->getParameters();
        array[index]->setValue(normalized);
    }

    auto value = getParamValue(id, normalized);

    Q_EMIT sigUpdateParams(index, value, normalized);
}

QString Vst3Plugin::getParamValue(uint32 id, double normalized)
{

    if (jplugin) {

        Array<AudioProcessorParameter*> array = jplugin->getParameters();

        updateParameter(id, normalized);
        auto value = array[id]->getText(normalized, 255);

        return QString::fromStdString(value.toStdString());
    }
    return "";
}

void Vst3Plugin::destroy()
{
    destroyView();
}

Vst3PluginManager::Vst3PluginManager(QObject* parent)
    : QObject(parent)
{

    juce::initialiseJuce_GUI();
    _effectLoader = QSharedPointer<AsyncEffectLoader>(new AsyncEffectLoader());
    connect(_effectLoader.data(), &AsyncEffectLoader::resultReady, this, &Vst3PluginManager::sltResultReady);
    connect(_effectLoader.data(), &AsyncEffectLoader::newPluginFound, this, &Vst3PluginManager::newPluginFound);

    _vst2modules = QSharedPointer<TreeModel>(new TreeModel());
    _vst3modules = QSharedPointer<TreeModel>(new TreeModel());

    QVector<AsyncEffectLoader::EffectInfo> _effectInfo;

    juce::AudioPluginFormatManager audioPluginFormatManager;
    audioPluginFormatManager.addDefaultFormats();

    _effectInfo.clear();
    for (int i = 0; i < audioPluginFormatManager.getFormats().size(); i++) {

        if (QString::fromStdString(audioPluginFormatManager.getFormat(i)->getName().toStdString()) == "VST") {
            _vst2Path = QString::fromStdString(audioPluginFormatManager.getFormat(i)->getDefaultLocationsToSearch().toString().toStdString());
        }
        if (QString::fromStdString(audioPluginFormatManager.getFormat(i)->getName().toStdString()) == "VST3") {
            _vst3Path = QString::fromStdString(audioPluginFormatManager.getFormat(i)->getDefaultLocationsToSearch().toString().toStdString());
        }
    }

    auto path = qApp->applicationDirPath();
    _effectInfo = _effectLoader->getEffectInfo();

    if (_effectInfo.size() > 0) {
        qDebug() << "Data successfully loaded from plugins.xml!";
        fillModels(_effectInfo);
    } else {
        _effectIsLoading = true;
        Q_EMIT loadingChanged();
        _effectLoader->reload();
    }
}

Vst3PluginManager::~Vst3PluginManager()
{
    _effectLoader->stop();
}

QString Vst3PluginManager::vst3Path()
{
    return _vst3Path;
}

QString Vst3PluginManager::vst2Path()
{
    return _vst2Path;
}

void Vst3PluginManager::rescanEffects()
{
    _effectIsLoading = true;
    Q_EMIT loadingChanged();
    _vst3modules->clear();
    _vst2modules->clear();
    _effectLoader->reload();
}

void Vst3PluginManager::fillModels(QVector<AsyncEffectLoader::EffectInfo> effectInfo)
{
    _vst3modules->clear();
    _vst2modules->clear();
    for (int i = 0; i < effectInfo.size(); i++) {
        if (effectInfo[i]._type == "VST3") {
            auto rootItem = _vst3modules->getRootItem();
            auto manufacturer = _vst3modules->findItemByName(rootItem, effectInfo[i]._manufacturer);
            if (manufacturer) {
                auto category = _vst3modules->findItemByName(manufacturer, effectInfo[i]._category);
                if (category) {

                    auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._name, category, effectInfo[i]._path, effectInfo[i]._index));
                    category->appendChild(child1);
                } else {
                    auto category = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._category, manufacturer));
                    auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._name, category, effectInfo[i]._path, effectInfo[i]._index));
                    category->appendChild(child1);
                    manufacturer->appendChild(category);
                }
            } else {
                auto manufacturer = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._manufacturer, rootItem));
                auto category = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._category, manufacturer));
                auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._name, category, effectInfo[i]._path, effectInfo[i]._index));

                category->appendChild(child1);
                manufacturer->appendChild(category);
                rootItem->appendChild(manufacturer);
            }
        }
        if (effectInfo[i]._type == "VST") {
            auto rootItem = _vst2modules->getRootItem();
            auto manufacturer = _vst2modules->findItemByName(effectInfo[i]._manufacturer);
            if (manufacturer) {
                auto category = _vst2modules->findItemByName(manufacturer, effectInfo[i]._category);
                if (category) {
                    auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._name, category, effectInfo[i]._path, effectInfo[i]._index));
                    category->appendChild(child1);

                } else {
                    auto category = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._category, manufacturer));
                    auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._name, category, effectInfo[i]._path, effectInfo[i]._index));
                    category->appendChild(child1);
                    manufacturer->appendChild(category);
                }
            } else {
                auto manufacturer = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._manufacturer, rootItem));
                auto category = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._category, manufacturer));
                auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo[i]._name, category, effectInfo[i]._path, effectInfo[i]._index));
                category->appendChild(child1);
                manufacturer->appendChild(category);
                rootItem->appendChild(manufacturer);
            }
        }
    }

    _vst3modules->resetModel();
    _vst2modules->resetModel();
    qDebug() << "LOADED" << effectInfo.size();
    Q_EMIT vst2ModulesChanged();
    Q_EMIT vst3ModulesChanged();
}

void Vst3PluginManager::addEffectInfo(AsyncEffectLoader::EffectInfo effectInfo)
{
    if (effectInfo._type == "VST3") {
        auto rootItem = _vst3modules->getRootItem();
        auto manufacturer = _vst3modules->findItemByName(rootItem, effectInfo._manufacturer);
        if (manufacturer) {
            auto category = _vst3modules->findItemByName(manufacturer, effectInfo._category);
            if (category) {

                auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo._name, category, effectInfo._path, effectInfo._index));
                category->appendChild(child1);
            } else {
                auto category = QSharedPointer<TreeItem>(new TreeItem(effectInfo._category, manufacturer));
                auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo._name, category, effectInfo._path, effectInfo._index));
                category->appendChild(child1);
                manufacturer->appendChild(category);
            }
        } else {
            auto manufacturer = QSharedPointer<TreeItem>(new TreeItem(effectInfo._manufacturer, rootItem));
            auto category = QSharedPointer<TreeItem>(new TreeItem(effectInfo._category, manufacturer));
            auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo._name, category, effectInfo._path, effectInfo._index));

            category->appendChild(child1);
            manufacturer->appendChild(category);
            rootItem->appendChild(manufacturer);
        }
    }
    if (effectInfo._type == "VST") {
        auto rootItem = _vst2modules->getRootItem();
        auto manufacturer = _vst2modules->findItemByName(effectInfo._manufacturer);
        if (manufacturer) {
            auto category = _vst2modules->findItemByName(manufacturer, effectInfo._category);
            if (category) {
                auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo._name, category, effectInfo._path, effectInfo._index));
                category->appendChild(child1);

            } else {
                auto category = QSharedPointer<TreeItem>(new TreeItem(effectInfo._category, manufacturer));
                auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo._name, category, effectInfo._path, effectInfo._index));
                category->appendChild(child1);
                manufacturer->appendChild(category);
            }
        } else {
            auto manufacturer = QSharedPointer<TreeItem>(new TreeItem(effectInfo._manufacturer, rootItem));
            auto category = QSharedPointer<TreeItem>(new TreeItem(effectInfo._category, manufacturer));
            auto child1 = QSharedPointer<TreeItem>(new TreeItem(effectInfo._name, category, effectInfo._path, effectInfo._index));
            category->appendChild(child1);
            manufacturer->appendChild(category);
            rootItem->appendChild(manufacturer);
        }
    }
    _vst3modules->resetModel();
    _vst2modules->resetModel();
    Q_EMIT vst2ModulesChanged();
    Q_EMIT vst3ModulesChanged();
}

void Vst3PluginManager::sltResultReady()
{
    _effectIsLoading = false;
    Q_EMIT loadingChanged();
}

void Vst3PluginManager::newPluginFound(QString formatName, QString name, QString category, QString manufacturerName, QString path, int uniqueId)
{

    addEffectInfo(AsyncEffectLoader::EffectInfo { formatName, name, category, manufacturerName, path, uniqueId });
}

void AsyncEffectLoader::loadEffects()
{

    juce::AudioPluginFormatManager audioPluginFormatManager;
    audioPluginFormatManager.addDefaultFormats();

    pluginDescriptions.clear();
    _effectInfo.clear();

    _scanned.clear();
    for (int i = 0; i < audioPluginFormatManager.getFormats().size(); i++) {

        if (_quit == true) {
            return;
        }
        if (QString::fromStdString(audioPluginFormatManager.getFormat(i)->getName().toStdString()) == "VST3"
            || QString::fromStdString(audioPluginFormatManager.getFormat(i)->getName().toStdString()) == "VST") {

            auto list = audioPluginFormatManager.getFormat(i)->searchPathsForPlugins(audioPluginFormatManager.getFormat(i)->getDefaultLocationsToSearch(), true, false);

            juce::File deadManFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("deadman.txt");

            for (int j = 0; j < list.size(); j++) {
                qDebug() << QString::fromStdString(list[j].toStdString());
                if (QString::fromStdString(list[j].toStdString()).contains("14.12_x64.vst3"))
                    continue;

                if (QString::fromStdString(list[j].toStdString()).contains("16.0_x86.vst3"))
                    continue;

                // knownPluginList.scanAndAddDragAndDroppedFiles(audioPluginFormatManager, list[j], pluginDescriptions);

                auto* format = audioPluginFormatManager.getFormat(i);
                juce::PluginDirectoryScanner scanner(
                    knownPluginList,
                    *format,
                    format->getDefaultLocationsToSearch(),
                    true,
                    deadManFile,
                    true // enable async instantiation → out-of-process scan
                );
                scanner.applyBlacklistingsFromDeadMansPedal(knownPluginList, deadManFile);
                String nameOfPluginBeingScanned;
                while (scanner.scanNextFile(true, nameOfPluginBeingScanned)) {
                    qDebug() << "Plugin: " << QString::fromStdString(nameOfPluginBeingScanned.toStdString()) << " progress: " << scanner.getProgress() * 100.0;
                }
            }
        }
    }

    qDebug() << "LOADED";
    Q_EMIT resultReady();
}

void AsyncEffectLoader::changeListenerCallback(ChangeBroadcaster* source)
{

    if (source == &knownPluginList) {

        // save the plugin list every time it gets chnaged, so that if we're scanning
        // and it crashes, we've still saved the previous ones
        ScopedPointer<XmlElement> d;
        auto savedPluginList = knownPluginList.createXml();

        if (savedPluginList != nullptr) {
            auto path = QCoreApplication::applicationDirPath() + "/plugins.xml";
            savedPluginList->writeTo(juce::String(path.toStdString()));
            auto pluginDescriptions = knownPluginList.getTypes();

            for (int k = 0; k < pluginDescriptions.size(); k++) {

                if (_scanned.contains(pluginDescriptions[k].uniqueId)) {
                    continue;
                }
                try {
                    // Validate and convert strings safely
                    auto formatName = QString::fromStdString(pluginDescriptions[k].pluginFormatName.toStdString());
                    auto name = QString::fromStdString(pluginDescriptions[k].name.toStdString());
                    auto category = QString::fromStdString(pluginDescriptions[k].category.toStdString());
                    auto manufacturerName = QString::fromStdString(pluginDescriptions[k].manufacturerName.toStdString());
                    auto path = QString::fromStdString(pluginDescriptions[k].fileOrIdentifier.toStdString());

                    Q_EMIT newPluginFound(formatName, name, category, manufacturerName, path, pluginDescriptions[k].uniqueId);
                    _scanned.insert(pluginDescriptions[k].uniqueId, true);

                } catch (const std::exception& e) {
                    qDebug() << "Exception occurred:" << e.what();
                } catch (...) {
                    qDebug() << "An unknown exception occurred";
                }
            }

            // qDebug() << QString::fromStdString(savedPluginList->toString().toStdString());
        }
    }
}

AsyncEffectLoader::AsyncEffectLoader(QObject* parent)
    : QObject(parent)
{

    pluginDescriptions.clear();

    auto path = QCoreApplication::applicationDirPath() + "/plugins.xml";

    XmlDocument myDocument(juce::File(path.toStdString()));

    if (auto mainElement = myDocument.getDocumentElement()) {
        for (auto* e : mainElement->getChildIterator()) {
            auto p = std::make_unique<PluginDescription>();
            p->loadFromXml(*e);
            pluginDescriptions.add(std::move(p));
        }
    }

    for (int k = 0; k < pluginDescriptions.size(); k++) {

        try {
            // Validate and convert strings safely
            auto formatName = QString::fromStdString(pluginDescriptions[k]->pluginFormatName.toStdString());
            auto name = QString::fromStdString(pluginDescriptions[k]->name.toStdString());
            auto category = QString::fromStdString(pluginDescriptions[k]->category.toStdString());
            auto manufacturerName = QString::fromStdString(pluginDescriptions[k]->manufacturerName.toStdString());
            auto path = QString::fromStdString(pluginDescriptions[k]->fileOrIdentifier.toStdString());

            _effectInfo.append(EffectInfo { formatName, name, category, manufacturerName, path, pluginDescriptions[k]->uniqueId });

            // qDebug() << "name" << name << formatName << (uint32)pluginDescriptions[k]->uniqueId;

        } catch (const std::exception& e) {
            qDebug() << "Exception occurred:" << e.what();
        } catch (...) {
            qDebug() << "An unknown exception occurred";
        }
    }

    knownPluginList.addChangeListener(this);
}

AsyncEffectLoader::~AsyncEffectLoader()
{
    pluginDescriptions.clear();
}
