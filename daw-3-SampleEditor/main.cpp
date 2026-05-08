
#define JUCE_PLUGINHOST_VST 1
#define JUCE_PLUGINHOST_VST3 1
#define JUCE_PLUGINHOST_AU 1
#define JUCE_PLUGINHOST_LADSPA 1
#define JUCE_WINDOWS 1
#define JUCE_MSVC 1

#include "juceapplication.h"
#include <boost/filesystem.hpp>
#include <plugins/vst3effect.h>
#include <plugins/vst3plugin.h>

#include <Core/Utility/Singleton/DebugSingleton.h>
#include <FluxAppManager.h>
#include <UI/CursorManager.h>

#include <KeyEventFilter.h>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickWindow>
#include <QScreen>

#include <iostream>

#include <audio/audioengine.h>
#include <audio/effectcontrols.h>
#include <audio/sampleclip.h>
#include <core/backgroundarea.h>
#include <core/bottomarea.h>
#include <core/cliparea.h>
#include <core/properties.h>
#include <core/sampleeditor.h>
#include <core/screeninterface.h>
#include <core/toparea.h>
#include <core/trackarea.h>

#include "audio/engine.h"
#include "audio/song.h"

#include "ipp.h"

#include <eq/filtervisualizer.h>
#include <eq/multieq.h>
#include <ui/TrackList/CursorPosition.h>
#include <ui/TrackList/MouseZone.h>
#include <ui/TrackList/eventitem.h>
#include <ui/automationlaneitem.h>
#include <ui/automationshapeitem.h>
#include <ui/clipshapeitem.h>
#include <ui/indicatoritem.h>
#include <ui/markershapeitem.h>
#include <ui/plotshapeitem.h>
#include <ui/rubberbanditem.h>
#include <ui/trackshapeitem.h>
#ifdef Q_OS_WIN
#define stdin (__acrt_iob_func(0))
#define stdout (__acrt_iob_func(1))
#define stderr (__acrt_iob_func(2))
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE* __cdecl __iob_func(void)
{
    return _iob;
}
#endif

#include <Core/Utility/Singleton/DebugSingleton.h>
#include <FluxAppManager.h>
#include <QCoreApplication>
#include <QDCore/OTAManager.h>
#include <QQmlContext>
#include <UI/AnimationManager.h>
#include <UI/CursorManager.h>
#include <qnamespace.h>

#include <Qt/QEngine.h>

#include "EngineHelper.h"
#include "Scene3DController.h"

#include <QtWebEngine/QtWebEngine>
#include <QWebChannel>
#include <QtWebChannel/QQmlWebChannel>
#include <QFile>

#if DGE_Platform == DGE_Windows_Platform
#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h> //for malloc and free
#include <stdlib.h>
#endif

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned>(time(nullptr)));

    int result = 0;
#if DGE_Platform == DGE_Windows_Platform
    //_crtBreakAlloc = 1728;
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    {
        QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
        QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

        // QtWebEngine must be initialised before the QGuiApplication is
        // constructed (FluxAppManager creates it internally below).
        QtWebEngine::initialize();

        AudioManager::init(false);
        initQEngineResources(true);
        auto appManager = FluxAppManager::getInstance(argc, argv);

        qInstallMessageHandler([](QtMsgType type, const QMessageLogContext&, const QString& msg) {
            static QFile logFile(QStringLiteral("C:/Users/AmirPasha/dawlog.txt"));
            static bool opened = logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
            if (!opened) return;
            const char* tag = (type == QtDebugMsg) ? "DBG" : (type == QtInfoMsg) ? "INF" : (type == QtWarningMsg) ? "WRN" : "ERR";
            const QString line = QStringLiteral("[%1] %2\n").arg(tag, msg);
            logFile.write(line.toUtf8());
            logFile.flush();
        });
        qRegisterMetaType<QList<qint64>>("QList<qint64>");
        qRegisterMetaType<QMap<qint64, int>>("QMap<qint64,int>");
        qRegisterMetaType<QMap<qint64, double>>("QMap<qint64,double>");
        qmlRegisterType<TrackItem>("TrackItem", 1, 0, "TrackItem");
        qmlRegisterType<AudioManager>("AudioManager", 1, 0, "AudioManager");
        qmlRegisterType<AudioEngine>("AudioEngine", 1, 0, "AudioEngine");
        qmlRegisterType<AudioDevice>("AudioDevice", 1, 0, "AudioDevice");
        qmlRegisterType<EventItem>("EventItem", 1, 0, "EventItem");
        qmlRegisterType<MouseZone>("MouseZone_sat", 1, 0, "MouseZone");
        qmlRegisterType<CursorPosition>("Tools", 1, 0, "CursorPosition");
        qmlRegisterType<ScreenInterface>("ScreenInterface", 1, 0, "ScreenInterface");
        qmlRegisterType<SampleEditor>("SampleEditor", 1, 0, "SampleEditor");

        qmlRegisterType<BottomArea>("BottomArea", 1, 0, "BottomArea");
        qmlRegisterType<TopArea>("TopArea", 1, 0, "TopArea");
        qmlRegisterType<BackgroundArea>("BackgroundArea", 1, 0, "BackgroundArea");
        qmlRegisterType<TrackArea>("TrackArea", 1, 0, "TrackArea");
        qmlRegisterType<AreaInfo>("AreaInfo", 1, 0, "AreaInfo");
        qmlRegisterType<TracksModel>("TracksModel", 1, 0, "TracksModel");
        qmlRegisterType<ClipArea>("ClipArea", 1, 0, "ClipArea");
        qRegisterMetaType<TruePeakModel*>("TruePeakModel");
        qRegisterMetaType<MenuModel*>("MenuModel");
        qRegisterMetaType<AutomationLaneModel*>("AutomationLaneModel");

        qRegisterMetaType<EffectControls*>("EffectControls");
        qRegisterMetaType<VisrEffect*>("VisrEffect");
        qRegisterMetaType<Effect*>("Effect");
        qRegisterMetaType<MultiEQ*>("MultiEQ");
        qRegisterMetaType<QVector<Effect*>>();
        // qRegisterMetaType<EffectChain*>("EffectChain");

        qmlRegisterType<TrackItem>("TrackItem", 1, 0, "TrackItem");
        qmlRegisterType<Vst3PluginManager>("Vst3PluginManager", 1, 0, "Vst3PluginManager");
        qmlRegisterType<Vst3Effect>("Vst3Effect", 1, 0, "Vst3Effect");

        qmlRegisterType<FilterVisualizer>("FilterVisualizer", 1, 0, "FilterVisualizer");
        qmlRegisterType<MultiEQ>("MultiEQ", 1, 0, "MultiEQ");
        qmlRegisterType<EffectChain>("EffectChain", 1, 0, "EffectChain");
        qmlRegisterType<FiltersModel>("FiltersModel", 1, 0, "FiltersModel");
        qmlRegisterType<ObjectCreator>("ObjectCreator", 1, 0, "ObjectCreator");

        qmlRegisterType<PlotShapeItem>("PlotShapeItem", 1, 0, "PlotShapeItem");
        qmlRegisterType<ClipShapeItem>("ClipShapeItem", 1, 0, "ClipShapeItem");
        qmlRegisterType<TrackShapeItem>("TrackShapeItem", 1, 0, "TrackShapeItem");
        qmlRegisterType<RubberBandItem>("RubberBandItem", 1, 0, "RubberBandItem");
        qmlRegisterType<MarkerShapeItem>("MarkerShapeItem", 1, 0, "MarkerShapeItem");
        qmlRegisterType<IndicatorShapeItem>("IndicatorShapeItem", 1, 0, "IndicatorShapeItem");
        qmlRegisterType<AutomationLaneItem>("AutomationLaneItem", 1, 0, "AutomationLaneItem");
        qmlRegisterType<AutomationShapeItem>("AutomationShapeItem", 1, 0, "AutomationShapeItem");

        qmlRegisterSingletonType(
            QUrl(QStringLiteral("qrc:/Global.qml")),
            "Global", 1, 0, "Global");

        //        RegisterQmlSingletonType(AnimationEditor, "Tutorial2", 1, 0, "AnimationEditor",
        //            animationEditorProvider);

        appManager->setDefaultQMLFilePath("qrc:/main.qml");

        auto engine = DGE::QtWrapper::getQEngine();
        auto timelineManager = AnimationManager::getInstance((QObject*)engine.get()).get();
        // timelineManager->setCanAddTrack(true);

        auto objectCreator = new ObjectCreator(timelineManager, engine);
        auto scene3D = new Scene3DController();

        auto connection = QObject::connect(engine.get(), SIGNAL(startedEngine()), objectCreator,
            SLOT(onStartedEngine()), ::Qt::DirectConnection);

        //        QSurfaceFormat fmt;
        //        fmt.setSamples(2); // 4x MSAA (8 if you can afford it)
        //        QSurfaceFormat::setDefaultFormat(fmt);

        qmlRegisterType<CursorManager>("Utility", 1, 0, "CursorManager");
        QObject::connect(
            appManager.get(), &FluxAppManager::startInitialization, appManager.get(),
            [appManager, objectCreator, scene3D, engine]() {
                auto qmlEngine = appManager->qmlAppEngine();
                auto rootContext = qmlEngine->rootContext();
                if (qmlEngine && rootContext != nullptr) {

                    KeyEventFilter* keyFilter = new KeyEventFilter();
                    qApp->installEventFilter(keyFilter);

                    QString data;
                    QString fileName(":/resources/stereo.xml");

                    QFile file(fileName);
                    if (!file.open(QIODevice::ReadOnly)) {
                        qDebug() << "filenot opened" << endl;
                    } else {
                        qDebug() << "file opened" << endl;
                        data = file.readAll();
                    }
                    file.close();

                    QFile file_stereo("stereo.xml");
                    if (file_stereo.open(QIODevice::ReadWrite)) {

                        file_stereo.write(data.toLatin1().data());
                    } else {
                        qDebug() << "file_stereofile_stereo" << file_stereo.errorString();
                    }
                    file_stereo.close();

                    AreaInfo* _areaInfo = new AreaInfo(keyFilter, AreaInfo::AreaType_ClipArea);

                    QObject::connect(engine.get(), &QEngine::startedEngine, [_areaInfo]() {
                        _areaInfo->sig3dEngineStarted();
                    });

                    ClipArea* _clipArea = new ClipArea(_areaInfo);
                    Properties* properties = new Properties();

                    rootContext->setContextProperty("mainQMLFileName",
                        appManager->defaultQMLFilePath());

                    CursorPosition* cursorPosition = new CursorPosition();
                    // CloseClass closeClass(nullptr, &app, &engine);

                    QString _homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
                    Vst3PluginManager* vst3 = new Vst3PluginManager();

                    QObject::connect(_areaInfo, &AreaInfo::sigAddObject, objectCreator,
                        &ObjectCreator::onAddingObject, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigAddKeyFrame, [objectCreator](int trackIndex, quint64 time, int type) {
                        QMetaObject::invokeMethod(objectCreator, "addKeyFrame", Q_ARG(int, trackIndex), Q_ARG(quint64, time), Q_ARG(int, type));
                    });

                    QObject::connect(_areaInfo, &AreaInfo::sigEditKeyFrameWithPosition, objectCreator,
                        &ObjectCreator::editKeyFrameWithPosition, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigRemoveKeyFrame, objectCreator,
                        &ObjectCreator::removeKeyFrame, Qt::DirectConnection);

                    // Mirror kf removal into the visible lane (orange dot in
                    // KeyFramesAutomationItem._points + _cLines). Without this,
                    // the right-click Delete updates the engine + 3D scene but
                    // the lane marker stays.
                    QObject::connect(_areaInfo, &AreaInfo::sigRemoveKeyFrame,
                        [_clipArea](int trackIndex, quint64 time) {
                            if (auto* tm = _clipArea->tracksModel())
                                tm->removeKeyFrame(trackIndex, qint64(time));
                        });

                    QObject::connect(_areaInfo, &AreaInfo::sigCurrentSelectedTime, objectCreator,
                        &ObjectCreator::setCurrentTime, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigKeyFrameClear, objectCreator,
                        &ObjectCreator::keyFrameClear, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigEditKeyFrame, objectCreator,
                        &ObjectCreator::editKeyFrame, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigKeyFrameChanged, objectCreator,
                        &ObjectCreator::keyFrameChanged, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigPlayBackUpdateTimeout, [objectCreator, scene3D, _areaInfo]() {
                        if (AudioManager::getSong()->isPause() == false) {
                            const qint64 t = _areaInfo->playheadMarker();
                            objectCreator->setCurrentTime(t);
                            scene3D->setPlayhead(t);
                        }
                    });

                    // Forward every playhead change to ObjectCreator. During playback the
                    // sigPlayBackUpdateTimeout path also updates time at 60Hz; outside of
                    // playback (stopped or initial state) we still need m_currentTime to
                    // follow the user's clicks/scrubs on the timeline ruler so kf commits
                    // land at the correct playhead time.
                    QObject::connect(_areaInfo, &AreaInfo::playheadMarkerChanged,
                        [objectCreator, scene3D, _areaInfo]() {
                            const qint64 t = _areaInfo->playheadMarker();
                            objectCreator->setCurrentTime(t);
                            scene3D->setPlayhead(t);
                        });

                    QObject::connect(_areaInfo, &AreaInfo::sigObjectPosition, objectCreator,
                        &ObjectCreator::setObjectLocation, Qt::DirectConnection);

                    // Forward entity position from DAW → QtQuick3D scene
                    QObject::connect(objectCreator, &ObjectCreator::sigObjectMoved,
                        scene3D, &Scene3DController::setEntityPosition, Qt::QueuedConnection);

                    // Wire each per-track ObjectPosAutomation to the scene controller
                    // so keyframe markers and the trajectory line are pushed to JS.
                    QObject::connect(objectCreator, &ObjectCreator::sigAutomationCreated,
                        scene3D, &Scene3DController::attachAutomation, Qt::DirectConnection);

                    // Right-click context menu (Bezier/Linear/Hold) → interp change
                    // + tangent handle display for Bezier + lane marker shape update.
                    QObject::connect(_areaInfo, &AreaInfo::sigInterpChanged,
                        [objectCreator, scene3D, _clipArea](int trackIndex, qint64 time, int interp) {
                            objectCreator->setKeyFrameInterp(trackIndex, time, interp);
                            // Update the lane's per-kf shape cache so the orange
                            // marker reflects the new interp (circle/diamond/square).
                            if (auto* tm = _clipArea->tracksModel()) {
                                if (auto trackItem = tm->trackItem(trackIndex)) {
                                    if (auto kfLane = trackItem->keyFramesAutomationItem())
                                        kfLane->setInterpForTime(time, interp);
                                }
                            }
                            if (interp == 2) // Bezier: push seeded tangents to viewports
                                scene3D->setSelectedKeyframe(trackIndex, time);
                            else
                                scene3D->clearTangentSelection();
                        });

                    // 3D/2D viewport kf-marker click → update lane selection
                    // halo, then route through the same sigKFSelected path so
                    // tangent display stays consistent.
                    QObject::connect(scene3D, &Scene3DController::kfClickedInScene,
                        [_clipArea, _areaInfo](int trackIndex, qint64 time) {
                            if (auto* tm = _clipArea->tracksModel()) {
                                if (auto ti = tm->trackItem(trackIndex)) {
                                    if (auto kfLane = ti->keyFramesAutomationItem())
                                        kfLane->setSelectedKfTime(time);
                                }
                            }
                            Q_EMIT _areaInfo->sigKFSelected(trackIndex, time);
                        });

                    // Lane left-click → select kf → push tangents (Bezier) /
                    // clear (Linear/Hold) to BOTH 3D and 2D viewports. The lane
                    // already updated its own selection halo before emitting.
                    QObject::connect(_areaInfo, &AreaInfo::sigKFSelected,
                        [objectCreator, scene3D, _clipArea](int trackIndex, qint64 time) {
                            // Look up the interp at this time to decide whether to
                            // show tangent handles (Bezier only).
                            int interp = 1; // Linear default
                            if (auto* tm = _clipArea->tracksModel()) {
                                if (auto ti = tm->trackItem(trackIndex)) {
                                    if (auto kfLane = ti->keyFramesAutomationItem()) {
                                        const int t = kfLane->interpTypeAt(time);
                                        // KeyFramesType (0=Bezier,1=Linear,2=Hold)
                                        // → KeyInterp (0=Hold,1=Linear,2=Bezier)
                                        interp = (t == 0) ? 2 : (t == 2) ? 0 : 1;
                                    }
                                }
                            }
                            if (interp == 2)
                                scene3D->setSelectedKeyframe(trackIndex, time);
                            else
                                scene3D->clearTangentSelection();
                        });

                    // Auto-record from 3D/2D viewport drag → timeline lane orange marker.
                    // Use the QML-exposed TracksModel::addKeyFrame which actually
                    // updates the visible KeyFramesAutomationItem (the lane). The
                    // sigAddKeyFrame route only updates ObjectCreator's engine
                    // _tracklist + ObjectPosAutomation, not the visible lane.
                    QObject::connect(objectCreator, &ObjectCreator::sigAutoRecordedKeyFrame,
                        [_clipArea](int trackIndex, quint64 time, int type) {
                            qDebug() << "[KFLane] sigAutoRecordedKeyFrame track="
                                     << trackIndex << "time=" << time << "type=" << type;
                            auto* tm = _clipArea->tracksModel();
                            if (!tm) {
                                qDebug() << "[KFLane] tracksModel() is null — skipping";
                                return;
                            }
                            tm->addKeyFrame(trackIndex, qint64(time), type);
                            // Auto-expand the keyframe lane so the orange dot is immediately
                            // visible without the user having to manually open it.
                            tm->setAutomationLaneEnabled(trackIndex, true);
                        });

                    // Reverse sync: live drag (every pointer-move) → ObjectCreator::setObjectLocationLive
                    QObject::connect(scene3D, &Scene3DController::entityMovedFromSceneLive,
                        objectCreator, &ObjectCreator::setObjectLocationLive, Qt::QueuedConnection);
                    // Reverse sync: drag release → ObjectCreator::commitObjectLocation
                    // (Auto Key Mode decides whether to write a kf.)
                    QObject::connect(scene3D, &Scene3DController::entityCommittedFromScene,
                        objectCreator, &ObjectCreator::commitObjectLocation, Qt::QueuedConnection);
                    // Back-compat — legacy entityMovedFromScene signal aliases to live.
                    QObject::connect(scene3D, &Scene3DController::entityMovedFromScene,
                        objectCreator, &ObjectCreator::setObjectLocationLive, Qt::QueuedConnection);

                    // Keyframe lane toggle → enable/disable auto-record for that track
                    QObject::connect(_areaInfo, &AreaInfo::sigKeyframeLaneToggled,
                        objectCreator, &ObjectCreator::setKeyframeLaneActive, Qt::DirectConnection);

                    // Keyframe lane lines changed (user clicked orange line) → sync ObjectPosAutomation
                    QObject::connect(_areaInfo, &AreaInfo::sigKeyFrameLinesChanged,
                        objectCreator, &ObjectCreator::syncObjectKeyframes, Qt::DirectConnection);

                    qmlEngine->rootContext()->setContextProperty("_homePath", _homePath);
                    qmlEngine->rootContext()->setContextProperty("cursorPositionClass", cursorPosition);
                    qmlEngine->rootContext()->setContextProperty("_engine", AudioManager::inst());
                    qmlEngine->rootContext()->setContextProperty("_areaInfo", _areaInfo);
                    qmlEngine->rootContext()->setContextProperty("globalValues", properties);
                    qmlEngine->rootContext()->setContextProperty("vst3", vst3);
                    qmlEngine->rootContext()->setContextProperty("objectCreator", objectCreator);
                    qmlEngine->rootContext()->setContextProperty("scene3D", scene3D);

                    // Expose scene3D over QWebChannel so the Three.js page can
                    // connect to its signals/slots from JavaScript.
                    auto* webChannel = new QQmlWebChannel(scene3D);
                    webChannel->registerObject(QStringLiteral("scene3D"), scene3D);
                    qmlEngine->rootContext()->setContextProperty("webChannelObj", webChannel);

                    qmlEngine->rootContext()->setContextProperty("_clipArea", _clipArea);

                    // qmlEngine->rootContext()->setContextProperty("closeClass", &closeClass);

                    // Three.js viewports are embedded in main.qml via WebEngineView
                    // and connect to scene3D through the webChannelObj above.
                }
            },
            Qt::DirectConnection);
        appManager->init();
        auto rootItem = appManager->rootItem();

        result = appManager->exec();
        appManager->release();
    }
    DGE::QtWrapper::releaseQEngine();
    FluxAppManager::releaseInstance();
    deleteSingletons();
    shutdownJuce_GUI();
    AudioManager::destroy();

#if DGE_Platform == DGE_Windows_Platform
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif
    DGE::Core::Utility::g_isExitingApp = true;
    return result;
}
