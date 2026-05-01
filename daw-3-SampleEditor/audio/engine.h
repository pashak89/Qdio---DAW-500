#ifndef ENGINE_H
#define ENGINE_H

#include "audio_global.h"
#include "projectjournal.h"
#include <QObject>

class AudioEngine;
class Song;
class Mixer;
class AudioManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(AudioEngine* audioEngine READ audioEngine NOTIFY audioEngineChanged)
public:
    static void init(bool renderOnly);
    static void destroy();

    Q_INVOKABLE static void audioEngineOff();
    Q_INVOKABLE static void audioEngineOn(int deviceIndex = -1, int channel = 2);

    // core
    static AudioEngine* audioEngine()
    {
        return s_audioEngine;
    }

    static Mixer* mixer()
    {
        return s_mixer;
    }

    static Song* getSong()
    {
        return s_song;
    }

    static float framesPerTick(sample_rate_t sample_rate);
    static float framesPerTick()
    {
        return 256; // s_framesPerTick;
    }
    static void updateFramesPerTick();

    static inline AudioManager* inst()
    {
        if (s_instanceOfMe == nullptr) {
            s_instanceOfMe = new AudioManager();
        }
        return s_instanceOfMe;
    }

    static ProjectJournal* projectJournal()
    {
        return s_projectJournal;
    }

signals:
    void initProgress(const QString& msg);
    void audioEngineChanged();

private:
    // small helper function which sets the pointer to NULL before actually deleting
    // the object it refers to
    template <class T>
    static inline void deleteHelper(T** ptr)
    {
        T* tmp = *ptr;
        *ptr = nullptr;
        delete tmp;
    }

    static float s_framesPerTick;

    // core
    static AudioEngine* s_audioEngine;
    static Song* s_song;
    static Mixer* s_mixer;
    // even though most methods are static, an instance is needed for Qt slots/signals
    static AudioManager* s_instanceOfMe;
    static ProjectJournal* s_projectJournal;
};

#endif // ENGINE_H
