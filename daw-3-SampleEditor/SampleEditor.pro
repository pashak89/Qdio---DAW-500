QT += quick qml sql
QT += concurrent
QT += core gui qml quick multimedia quickcontrols2 \
        opengl widgets svg sql positioning network openglextensions \
        webengine webchannel

QML_IMPORT_PATH += $$PWD/TrackPanels/Eq8Plugin

CONFIG += qt warn_on depend_includepath
CONFIG += c++20
QMAKE_CXXFLAGS += /std:c++20 /TP /bigobj /MP

# Link-speed wins (massive reduction in incremental build time):
#  /INCREMENTAL       — patch the existing .exe instead of full relink
#  /DEBUG:FASTLINK    — partial PDB, debugger reads .obj files directly (much faster generation)
CONFIG(debug, debug|release) {
    QMAKE_LFLAGS_DEBUG += /INCREMENTAL /DEBUG:FASTLINK
    # Disable optimisations that block /INCREMENTAL (these are normally release-only anyway)
    QMAKE_LFLAGS_DEBUG -= /OPT:REF /OPT:ICF
}

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += DGE_USE_QT DGE_USE_GLM DGE_USE_Tweeny DGE_USE_FreeImage GLEW_STATIC FREEIMAGE_EXPORTS
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += JUCE_APP_VERSION=1.0.0 \
           JUCE_APP_VERSION_HEX=0x10000 \
           JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 \
           JUCE_MODULE_AVAILABLE_juce_graphics=1 \
           JUCE_MODULE_AVAILABLE_juce_gui_extra=1 \
           JUCE_MODULE_AVAILABLE_juce_gui_basics=1 \
            JUCE_MODULE_AVAILABLE_juce_opengl=0 \
           JucePlugin_Build_VST=1 \
           JucePlugin_Build_VST3=1 \
           JucePlugin_Build_AU=0 \
           JucePlugin_Build_AUv3=0 \
           JucePlugin_Build_RTAS=0 \
           JucePlugin_Build_AAX=0 \
           JucePlugin_Build_Standalone=0 \
           JucePlugin_Build_Unity=0 \
           JUCE_USE_MP3AUDIOFORMAT=1 \
           JUCE_STANDALONE_APPLICATION=1 \
           JUCE_PLUGINHOST_AU=0 \
           JUCE_PLUGINHOST_LADSPA=0 \
           JUCE_PLUGINHOST_VST=1 \
           JUCE_PLUGINHOST_VST3=1 \
           DEBUG=1 \
           JUCE_MSVC=1



include(./3rdparty/libqnanopainter/include.pri)
include(./audio/audio.pri)
include(./core/core.pri)
include(./ui/ui.pri)
include(./plugins/plugins.pri)

SOURCES += \
        EngineHelper.cpp \
        Scene3DController.cpp \
        animationeditor.cpp \
        eq/filtervisualizer.cpp \
        eq/multieq.cpp \
        eq/multieqeffect.cpp \
        eq/visreffect.cpp \
        juceapplication.cpp \
        keyeventfilter.cpp \
        main.cpp


HEADERS += \
    EngineHelper.h \
    Scene3DController.h \
    animationeditor.h \
    eq/filtervisualizer.h \
    eq/multieq.h \
    eq/multieqeffect.h \
    eq/visreffect.h \
    juceapplication.h \
    keyeventfilter.h \

RESOURCES += qml.qrc


INCLUDEPATH += $$PWD/3rdparty/visr/include/visr
DEPENDPATH += $$PWD/3rdparty/visr/include/visr

INCLUDEPATH += $$PWD/3rdparty/boost/include/boost-1_85
DEPENDPATH += $$PWD/3rdparty/boost/include/boost-1_85

INCLUDEPATH += $$PWD/juce
DEPENDPATH += $$PWD/juce
INCLUDEPATH += $$PWD/juce/modules
INCLUDEPATH += $$PWD/3rdparty/win32/include
DEPENDPATH += $$PWD/3rdparty/win32/include
INCLUDEPATH += $$PWD/3rdparty/win32/include/ipp
DEPENDPATH += $$PWD/3rdparty/win32/include/ipp
DEPENDPATH += $$PWD/3rdparty/win64/include
INCLUDEPATH += $$PWD/3rdparty/win64/include

INCLUDEPATH += $$PWD/3rdparty/vst/include
DEPENDPATH += $$PWD/3rdparty/vst/include
INCLUDEPATH += $$PWD/3rdparty/vst2/include
DEPENDPATH += $$PWD/3rdparty/vst2/include



INCLUDEPATH += $$PWD/3rdparty/DGE-QD-SDK/x64/include \
$$PWD/3rdparty/DGE-QD-SDK/x64/include/EngineLib \
$$PWD/3rdparty/DGE-QD-SDK/x64/include/EngineLib/Engine \
$$PWD/3rdparty/DGE-QD-SDK/x64/include/glm \
$$PWD/3rdparty/DGE-QD-SDK/x64/include/glew \
$$PWD/3rdparty/DGE-QD-SDK/x64/include/QDLib \
$$PWD/3rdparty/DGE-QD-SDK/x64/include/quickflux \
$$PWD/3rdparty/DGE-QD-SDK/x64/include/FreeImage
DEPENDPATH += $$PWD/3rdparty/DGE-QD-SDK/x64/include $$PWD/3rdparty/DGE-QD-SDK/x64/lib/ $$PWD/3rdparty/DGE-QD-SDK/x64/bin/

# Define library path
VST_LIB_PATH = $$PWD/3rdparty/vst/lib
VST2_LIB_PATH = $$PWD/3rdparty/vst2/lib
win32 {

    contains(QT_MAJOR_VERSION, 5) {
    contains(QT_MINOR_VERSION, 14) {
        message("Configuring for Visual Studio 2017")
        INCLUDEPATH += $$PWD/juce/include
        win32:CONFIG(release, debug|release): LIBS += -L$$PWD/juce/release -ljuce_library
        else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/juce/debug -ljuce_library

        IntelIPPlibs =  -lippcc -lippccmt -lippch -ippchmt -lippcore -lippcoremt -lippcv -lippdc -lippdcmt -lippe -lippemt -lippi -lipps -lippsmt -lippvm -lippvmmt
        ThirdPartyLibraryPath = $$PWD/3rdparty/win32/lib/

        IntelIPPPath = $${ThirdPartyLibraryPath}
        DEFINES += WINNT
        LIBS += -L$$IntelIPPPath $$IntelIPPlibs

        LIBS += -L$$PWD/3rdparty/win32/lib/ -lportaudio_x86
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lportaudio_static_x86
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lsndfile        
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibSndFileG72x
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibSndFileG72x.lib


        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibVorbis
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibSndFileG72x
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibFlac
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibGsm
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibId3Tag
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibLpc10
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibMad
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibOgg
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibPng
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibSpeex
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibVorbis
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibWavPack
        LIBS += -L$$PWD/3rdparty/win32/lib/ -lLibZLib

        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibWavPack.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibVorbis.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibSpeex.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibPng.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibOgg.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibMad.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibLpc10.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibId3Tag.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibFlac.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibGsm.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibVorbis.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/libsox.lib
        PRE_TARGETDEPS += $$PWD/3rdparty/win32/lib/LibZLib.lib



    } else: contains(QT_MINOR_VERSION, 15) {
        message("Configuring for Visual Studio 2019")
        INCLUDEPATH += $$PWD/juce/include
        win32:CONFIG(release, debug|release): LIBS += -L$$PWD/juce/release-2019/release -ljuce_library
        else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/juce/debug-2019/debug -ljuce_library

        win32:CONFIG(release, debug|release): LIBS += -L$$PWD/3rdparty/win64/lib/release -lportaudio_x64
        else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/3rdparty/win64/lib/debug -lportaudio_x64

        win32:CONFIG(release, debug|release): LIBS += -L$$PWD/3rdparty/win64/lib/release -lportaudio_static_x64
        else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/3rdparty/win64/lib/debug -lportaudio_static_x64


        LIBS += -L$$PWD/3rdparty/win64/lib/ -lsndfile
        PRE_TARGETDEPS += $$PWD/3rdparty/win64/lib/sndfile.lib


        win32:CONFIG(debug, debug|release) {
            LIBS += -L$$PWD/3rdparty/DGE-QD-SDK/x64/lib/Debug/ -L$$(QTDIR)/lib
            LIBS += -lCoreModule -lGFXModule -lSceneModule -lQtModule -lUIModule -lEngineModule -lScriptModule -lQDLib -lSDL3-static -lgtest -lgmock -lFreeImage
            LIBS += -lquickfluxd -llibglew32d -lopengl32 -lglu32 -llibGLESv2 -llibEGL
        } else:win32:CONFIG(release, debug|release) {
            LIBS += -L$$PWD/3rdparty/DGE-QD-SDK/x64/lib/RelWithDebInfo/ -L$$(QTDIR)/lib
            LIBS += -lCoreModule -lGFXModule -lSceneModule -lQtModule -lUIModule -lEngineModule -lScriptModule -lQDLib -lSDL3-static -lgtest -lgmock -lFreeImage
            LIBS += -lquickflux -llibglew32 -lopengl32 -lglu32 -llibGLESv2 -llibEGL
        }





        IntelIPPlibs =  -lippcc -lippccmt -lippch -ippchmt -lippcore -lippcoremt -lippcv -lippdc -lippdcmt -lippe -lippemt -lippi -lipps -lippsmt -lippvm -lippvmmt
        ThirdPartyLibraryPath = $$PWD/3rdparty/win32/lib/

        IntelIPPPath = $${ThirdPartyLibraryPath}
        DEFINES += WINNT
        LIBS += -L$$IntelIPPPath $$IntelIPPlibs



        LIBS += -L$$PWD/3rdparty/win64/lib/ -lsoxr
        LIBS += -L$$PWD/3rdparty/win64/lib/ -lsoxr-lsr


        win32:CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lvisr_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lrrl_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lrcl_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lsignalflows_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lefl_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -laudiointerfaces_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lpml_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lapputilities_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lpanning_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lrbbl_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lreverbobject_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/debug/ -lobjectmodel_shared

        LIBS += -L$$PWD/3rdparty/boost/lib/ -lboost_filesystem-vc142-mt-gd-x64-1_85
        LIBS += -L$$PWD/3rdparty/boost/lib/ -lboost_thread-vc142-mt-gd-x64-1_85
        LIBS += -L$$PWD/3rdparty/boost/lib/ -lboost_chrono-vc142-mt-gd-x64-1_85
        LIBS += -L$$PWD/3rdparty/boost/lib/ -lboost_atomic-vc142-mt-gd-x64-1_85


        } else:win32:CONFIG(release, debug|release) {
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lvisr_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lrrl_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lrcl_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lsignalflows_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lefl_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -laudiointerfaces_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lpml_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lapputilities_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lpanning_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lrbbl_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lreverbobject_shared
        LIBS += -L$$PWD/3rdparty/visr/lib/release/ -lobjectmodel_shared

        LIBS += -L$$PWD/3rdparty/boost/lib/ -lboost_filesystem-vc142-mt-x64-1_85
        LIBS += -L$$PWD/3rdparty/boost/lib/ -lboost_thread-vc142-mt-x64-1_85
        LIBS += -L$$PWD/3rdparty/boost/lib/ -lboost_chrono-vc142-mt-x64-1_85
        LIBS += -L$$PWD/3rdparty/boost/lib/ -lboost_atomic-vc142-mt-x64-1_85

        }

        win32:CONFIG(debug, debug|release) {
         LIBS += -L$${VST_LIB_PATH}/debug \
            -L$${VST2_LIB_PATH}/debug \
            -lsdk_common \
            -lsdk_hosting \
            -lbase \
            -lpluginterfaces \
            -ladelay \
            -lagain-sample-accurate \
            -lagain-simple \
            -lagain \
            -lchannel-context \
            -ldata-exchange \
            -lhost-checker \
            -llegacy-midicc-out \
            -lmda-vst3 \
            -lmultiple-program-changes \
            -lnote-expression-synth \
            -lnote-expression-text \
            -lpanner \
            -lpitch-names \
            -lprefetchable \
            -lprogram-change \
            -lremap-paramid \
            -lsdk \
            -lsync-delay \
            -lutf16-name \
            -lvstgui \
            -lvstgui_standalone \
            -lvstgui_support \
            -lVST_SDK

        PRE_TARGETDEPS += $${VST_LIB_PATH}/debug/base.lib \
                      $${VST_LIB_PATH}/debug/pluginterfaces.lib \
                      $${VST_LIB_PATH}/debug/sdk_common.lib \
                      $${VST_LIB_PATH}/debug/sdk_hosting.lib \
                      $${VST_LIB_PATH}/debug/vstgui.lib \
                      $${VST_LIB_PATH}/debug/adelay.lib \
                      $${VST_LIB_PATH}/debug/again-sample-accurate.lib \
                      $${VST_LIB_PATH}/debug/again-simple.lib \
                      $${VST_LIB_PATH}/debug/again.lib \
                      $${VST_LIB_PATH}/debug/channel-context.lib \
                      $${VST_LIB_PATH}/debug/data-exchange.lib \
                      $${VST_LIB_PATH}/debug/host-checker.lib \
                      $${VST_LIB_PATH}/debug/legacy-midicc-out.lib \
                      $${VST_LIB_PATH}/debug/mda-vst3.lib \
                      $${VST_LIB_PATH}/debug/multiple-program-changes.lib \
                      $${VST_LIB_PATH}/debug/note-expression-synth.lib \
                      $${VST_LIB_PATH}/debug/note-expression-text.lib \
                      $${VST_LIB_PATH}/debug/panner.lib \
                      $${VST_LIB_PATH}/debug/pitch-names.lib \
                      $${VST_LIB_PATH}/debug/prefetchable.lib \
                      $${VST_LIB_PATH}/debug/program-change.lib \
                      $${VST_LIB_PATH}/debug/remap-paramid.lib \
                      $${VST_LIB_PATH}/debug/sdk.lib \
                      $${VST_LIB_PATH}/debug/sync-delay.lib \
                      $${VST_LIB_PATH}/debug/utf16-name.lib \
                      $${VST_LIB_PATH}/debug/vstgui_standalone.lib \
                      $${VST_LIB_PATH}/debug/vstgui_support.lib \
                      $${VST2_LIB_PATH}/debug/VST_SDK.lib
        }

        # Release libraries and dependencies
        win32:CONFIG(release, debug|release) {
        LIBS += -L$${VST_LIB_PATH}/release \
            -L$${VST2_LIB_PATH}/release \
            -lsdk_common \
            -lsdk_hosting \
            -lbase \
            -lpluginterfaces \
            -ladelay \
            -lagain-sample-accurate \
            -lagain-simple \
            -lagain \
            -lchannel-context \
            -ldata-exchange \
            -lhost-checker \
            -llegacy-midicc-out \
            -lmda-vst3 \
            -lmultiple-program-changes \
            -lnote-expression-synth \
            -lnote-expression-text \
            -lpanner \
            -lpitch-names \
            -lprefetchable \
            -lprogram-change \
            -lremap-paramid \
            -lsdk \
            -lsync-delay \
            -lutf16-name \
            -lvstgui \
            -lvstgui_standalone \
            -lvstgui_support \
            -lVST_SDK

        PRE_TARGETDEPS += $${VST_LIB_PATH}/release/base.lib \
                      $${VST_LIB_PATH}/release/pluginterfaces.lib \
                      $${VST_LIB_PATH}/release/sdk_common.lib \
                      $${VST_LIB_PATH}/release/sdk_hosting.lib \
                      $${VST_LIB_PATH}/release/vstgui.lib \
                      $${VST_LIB_PATH}/release/adelay.lib \
                      $${VST_LIB_PATH}/release/again-sample-accurate.lib \
                      $${VST_LIB_PATH}/release/again-simple.lib \
                      $${VST_LIB_PATH}/release/again.lib \
                      $${VST_LIB_PATH}/release/channel-context.lib \
                      $${VST_LIB_PATH}/release/data-exchange.lib \
                      $${VST_LIB_PATH}/release/host-checker.lib \
                      $${VST_LIB_PATH}/release/legacy-midicc-out.lib \
                      $${VST_LIB_PATH}/release/mda-vst3.lib \
                      $${VST_LIB_PATH}/release/multiple-program-changes.lib \
                      $${VST_LIB_PATH}/release/note-expression-synth.lib \
                      $${VST_LIB_PATH}/release/note-expression-text.lib \
                      $${VST_LIB_PATH}/release/panner.lib \
                      $${VST_LIB_PATH}/release/pitch-names.lib \
                      $${VST_LIB_PATH}/release/prefetchable.lib \
                      $${VST_LIB_PATH}/release/program-change.lib \
                      $${VST_LIB_PATH}/release/remap-paramid.lib \
                      $${VST_LIB_PATH}/release/sdk.lib \
                      $${VST_LIB_PATH}/release/sync-delay.lib \
                      $${VST_LIB_PATH}/release/utf16-name.lib \
                      $${VST_LIB_PATH}/release/vstgui_standalone.lib \
                      $${VST_LIB_PATH}/release/vstgui_support.lib
                      $${VST2_LIB_PATH}/release/VST_SDK.lib \
        }
    } else {
        error("Unsupported Visual Studio version")
    }
    }
}

win32{
    LIBS += legacy_stdio_definitions.lib
    LIBS += Winmm.lib
    LIBS += Advapi32.lib
    LIBS += -lUser32
    LIBS += kernel32.lib
    LIBS += user32.lib
    LIBS += gdi32.lib
    LIBS += winspool.lib
    LIBS += comdlg32.lib
    LIBS += advapi32.lib
    LIBS += shell32.lib
    LIBS += ole32.lib
    LIBS += oleaut32.lib
    LIBS += uuid.lib
    LIBS += odbc32.lib
    LIBS += odbccp32.lib
    LIBS += -lgdi32
    LIBS += -lole32
    LIBS += -luser32
    LIBS += -lkernel32
    LIBS += -lgdi32
    LIBS += -lwinspool
    LIBS += -lshell32
    LIBS += -loleaut32
    LIBS += -luuid
    LIBS += -lcomdlg32
    LIBS += -ladvapi32
    LIBS += -ldinput8
    LIBS += -lsetupapi
    LIBS += -lversion
    LIBS += -lwinmm
    LIBS += -limm32
    LIBS += -llibEGLd
    LIBS += -llibGLESv2d
    LIBS += -lLibJXR
    LIBS += -lLibOpenJPEG
    LIBS += -lLibJpeg
    LIBS += -lLibPNG
    LIBS += -lLibTIFF4
    LIBS += -lLibWebP
    LIBS += -lMetaData
    LIBS += -lOpenEXR
    LIBS += -lLibRawLite
    LIBS += -lZLib
}


mac{
    INCLUDEPATH += /usr/local/include
    DEPENDPATH += /usr/local/include

    LIBS += -L/usr/local/lib -lportaudio
    LIBS += -L/usr/local/lib -lsndfile
    LIBS += -L/usr/local/lib -lsoxr
}

DISTFILES +=



