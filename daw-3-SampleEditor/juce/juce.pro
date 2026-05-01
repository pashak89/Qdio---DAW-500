QT -= gui

TARGET = juce_library
TEMPLATE = lib
CONFIG += staticlib
QMAKE_CXXFLAGS += /bigobj
CONFIG += c++17
DEFINES += _USE_MATH_DEFINES

# Path to JUCE modules
INCLUDEPATH += $$PWD\modules
INCLUDEPATH += /home/u0681/Projects/mixer/juce/modules
INCLUDEPATH += $$PWD\libraries

# Juce defines
DEFINES += JUCE_APP_VERSION=1.0.0 \
           JUCE_APP_VERSION_HEX=0x10000 \
           JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 \
           JUCE_MODULE_AVAILABLE_juce_graphics=1 \
           JUCE_MODULE_AVAILABLE_juce_gui_extra=1 \
           JUCE_MODULE_AVAILABLE_juce_gui_basics=1 \
           JUCE_MODULE_AVAILABLE_juce_opengl=0 \
           JucePlugin_Build_VST=1 \
           JucePlugin_Build_VST3=1 \
           JucePlugin_Build_AU=1 \
           JucePlugin_Build_AUv3=1 \
           JucePlugin_Build_RTAS=1 \
           JucePlugin_Build_AAX=1 \
           JucePlugin_Build_Standalone=0 \
           JucePlugin_Build_Unity=0 \
           JUCE_USE_MP3AUDIOFORMAT=1 \
           JUCE_VST3HEADERS_INCLUDE_HEADERS_ONLY=1 \
           JUCE_PLUGINHOST_AU=1 \
           JUCE_PLUGINHOST_LADSPA=1 \
           JUCE_PLUGINHOST_VST=1 \
           JUCE_PLUGINHOST_VST3=1 \
           JUCE_WINDOWS=1 \
           DEBUG=1 \
           JUCE_MSVC=1

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    JuceLibraryCode/include_juce_dsp.cpp \
    JuceLibraryCode/include_juce_graphics.cpp \
    JuceLibraryCode/include_juce_gui_basics.cpp \
    JuceLibraryCode/include_juce_gui_extra.cpp \
    JuceLibraryCode/juce_core_CompilationTime.cpp \
    Klangfreund/Ebu128LoudnessMeter.cpp \
    Klangfreund/SecondOrderIIRFilter.cpp \
    juce.cpp \
# Juce generated sources
        JuceLibraryCode/include_juce_audio_basics.cpp \
        JuceLibraryCode/include_juce_audio_devices.cpp \
        JuceLibraryCode/include_juce_audio_formats.cpp \
        JuceLibraryCode/include_juce_audio_processors.cpp \
        JuceLibraryCode/include_juce_audio_utils.cpp \
        JuceLibraryCode/include_juce_core.cpp \
        JuceLibraryCode/include_juce_cryptography.cpp \
        JuceLibraryCode/include_juce_data_structures.cpp \
        JuceLibraryCode/include_juce_events.cpp \
        JuceLibraryCode/include_juce_dsp.cpp \
    kmeter/average_level_filtered.cpp \
    kmeter/frut/amalgamated/include_frut_audio.cpp \
    kmeter/frut/amalgamated/include_frut_dsp.cpp \
    kmeter/frut/amalgamated/include_frut_math.cpp \
    kmeter/frut/amalgamated/include_frut_parameters.cpp \
    kmeter/frut/amalgamated/include_frut_skin.cpp \
    kmeter/frut/amalgamated/include_frut_widgets.cpp \
    kmeter/kmeter.cpp \
    kmeter/meter_ballistics.cpp \
    kmeter/plugin_parameters.cpp

HEADERS += \
    Klangfreund/Ebu128LoudnessMeter.h \
    Klangfreund/SecondOrderIIRFilter.h \
    juce.h \
#Juce generated headers
    JuceLibraryCode/JuceHeader.h \
    JuceLibraryCode/AppConfig.h \
    kmeter/average_level_filtered.h \
    kmeter/frut/FrutHeader.h \
    kmeter/frut/amalgamated/include_frut_audio.h \
    kmeter/frut/amalgamated/include_frut_dsp.h \
    kmeter/frut/amalgamated/include_frut_math.h \
    kmeter/frut/amalgamated/include_frut_parameters.h \
    kmeter/frut/amalgamated/include_frut_skin.h \
    kmeter/frut/amalgamated/include_frut_widgets.h \
    kmeter/kmeter.h \
    kmeter/meter_ballistics.h \
    kmeter/plugin_parameters.h

INCLUDEPATH += /usr/include/freetype2


INCLUDEPATH += $$PWD/../3rdparty/vst2/include
DEPENDPATH += $$PWD/../3rdparty/vst2/include


INCLUDEPATH += $$PWD/vst/include
DEPENDPATH += $$PWD/vst/include


