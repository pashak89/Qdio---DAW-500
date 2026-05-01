
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
        AudioManager::init(false);

        initQEngineResources(true);
        auto appManager = FluxAppManager::getInstance(argc, argv);
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

        auto connection = QObject::connect(engine.get(), SIGNAL(startedEngine()), objectCreator,
            SLOT(onStartedEngine()), ::Qt::DirectConnection);

        //        QSurfaceFormat fmt;
        //        fmt.setSamples(2); // 4x MSAA (8 if you can afford it)
        //        QSurfaceFormat::setDefaultFormat(fmt);

        qmlRegisterType<CursorManager>("Utility", 1, 0, "CursorManager");
        QObject::connect(
            appManager.get(), &FluxAppManager::startInitialization, appManager.get(),
            [appManager, objectCreator, engine]() {
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

                    QObject::connect(_areaInfo, &AreaInfo::sigCurrentSelectedTime, objectCreator,
                        &ObjectCreator::setCurrentTime, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigKeyFrameClear, objectCreator,
                        &ObjectCreator::keyFrameClear, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigEditKeyFrame, objectCreator,
                        &ObjectCreator::editKeyFrame, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigKeyFrameChanged, objectCreator,
                        &ObjectCreator::keyFrameChanged, Qt::DirectConnection);

                    QObject::connect(_areaInfo, &AreaInfo::sigPlayBackUpdateTimeout, [objectCreator, _areaInfo]() {
                        if (AudioManager::getSong()->isPause() == false) {
                            objectCreator->setCurrentTime(_areaInfo->playheadMarker());
                        }
                    });

                    QObject::connect(_areaInfo, &AreaInfo::sigObjectPosition, objectCreator,
                        &ObjectCreator::setObjectLocation, Qt::DirectConnection);

                    qmlEngine->rootContext()->setContextProperty("_homePath", _homePath);
                    qmlEngine->rootContext()->setContextProperty("cursorPositionClass", cursorPosition);
                    qmlEngine->rootContext()->setContextProperty("_engine", AudioManager::inst());
                    qmlEngine->rootContext()->setContextProperty("_areaInfo", _areaInfo);
                    qmlEngine->rootContext()->setContextProperty("globalValues", properties);
                    qmlEngine->rootContext()->setContextProperty("vst3", vst3);
                    qmlEngine->rootContext()->setContextProperty("objectCreator", objectCreator);
                    qmlEngine->rootContext()->setContextProperty("_clipArea", _clipArea);

                    // qmlEngine->rootContext()->setContextProperty("closeClass", &closeClass);
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
