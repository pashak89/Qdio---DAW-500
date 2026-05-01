#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "audio_global.h"
#include "fifobuffer.h"
#include "locklesslist.h"
#include "playhandle.h"
#include <QObject>
#include <QWaitCondition>
class AudioDevice;
class PlayHandle;
class AudioEngineWorkerThread;
class PlayHandle;
static thread_local bool s_renderingThread;
using LocklessListElement = LocklessList<PlayHandle*>::Element;
const int BYTES_PER_INT_SAMPLE = sizeof(int_sample_t);
const float OUTPUT_SAMPLE_MULTIPLIER = 32767.0f;
class AudioEngine : public QObject {

    Q_OBJECT
    Q_PROPERTY(AudioDevice* aduioDevice READ audioDev NOTIFY aduioDeviceChanged)

    Q_PROPERTY(int outputSampleRate READ outputSampleRate WRITE setOutputSampleRate NOTIFY sampleRateChanged)
    Q_PROPERTY(int outputBufferSize READ framesPerPeriod WRITE setFramesPerPeriod NOTIFY outputBufferSizeChanged)
    Q_PROPERTY(double outputLatency READ outputLatency WRITE setOutputLatency NOTIFY outputLatencyChanged)
    Q_PROPERTY(int channels READ channels NOTIFY outputLatencyChanged)
public:
    explicit AudioEngine(int channels = 2, QObject* parent = Q_NULLPTR);
    ~AudioEngine();

    bool initDevices(bool renderOnly = false, int channels = 2, int deviceIndex = -1);
    void freeDevices();
    void clear();

    using Fifo = FifoBuffer<sampleFrame*>;

    class fifoWriter : public QThread {
    public:
        fifoWriter(AudioEngine* audioEngine, Fifo* fifo);

        void finish();

    private:
        AudioEngine* m_audioEngine;
        Fifo* m_fifo;
        volatile bool m_writing;

        void run() override;

        void write(sampleFrame* buffer);
    };

    struct qualitySettings {
        enum Mode {
            Mode_Draft,
            Mode_HighQuality,
            Mode_FinalMix
        };

        enum Interpolation {
            Interpolation_Linear,
            Interpolation_SincFastest,
            Interpolation_SincMedium,
            Interpolation_SincBest
        };

        enum Oversampling {
            Oversampling_None,
            Oversampling_2x,
            Oversampling_4x,
            Oversampling_8x
        };

        Interpolation interpolation;
        Oversampling oversampling;

        qualitySettings(Mode m)
        {
            switch (m) {
            case Mode_Draft:
                interpolation = Interpolation_Linear;
                oversampling = Oversampling_None;
                break;
            case Mode_HighQuality:
                interpolation = Interpolation_SincFastest;
                oversampling = Oversampling_2x;
                break;
            case Mode_FinalMix:
                interpolation = Interpolation_SincBest;
                oversampling = Oversampling_8x;
                break;
            }
        }

        qualitySettings(Interpolation i, Oversampling o)
            : interpolation(i)
            , oversampling(o)
        {
        }

        int sampleRateMultiplier() const
        {
            switch (oversampling) {
            case Oversampling_None:
                return 1;
            case Oversampling_2x:
                return 2;
            case Oversampling_4x:
                return 4;
            case Oversampling_8x:
                return 8;
            }
            return 1;
        }

        int libsrcInterpolation() const
        {
            return 0;
            // Sample Rate Lib with soxr
            //            switch (interpolation) {
            //            case Interpolation_Linear:
            //                return SRC_ZERO_ORDER_HOLD;
            //            case Interpolation_SincFastest:
            //                return SRC_SINC_FASTEST;
            //            case Interpolation_SincMedium:
            //                return SRC_SINC_MEDIUM_QUALITY;
            //            case Interpolation_SincBest:
            //                return SRC_SINC_BEST_QUALITY;
            //            }
            //            return SRC_LINEAR;
        }
    };

    // audio-port-stuff
    inline void addAudioPort(AudioPort* port)
    {
        requestChangeInModel();
        m_audioPorts.push_back(port);
        doneChangeInModel();
    }

    void removeAudioPort(AudioPort* port);

    // methods providing information for other classes
    inline fpp_t framesPerPeriod() const
    {
        return m_framesPerPeriod;
    }

    // methods providing information for other classes
    inline fpp_t channels() const
    {
        return m_channels;
    }

    inline sampleFrame* nextBuffer()
    {
        return renderNextBuffer();
    }

    // play-handle stuff
    bool addPlayHandle(PlayHandle* handle);

    void removePlayHandle(PlayHandle* handle);

    inline PlayHandleList& playHandles()
    {
        return m_playHandles;
    }

    inline double outputLatency() const
    {
        return m_outputLatency;
    }

    inline bool hasFifoWriter() const
    {
        return m_fifoWriter != nullptr;
    }

    const qualitySettings& currentQualitySettings() const
    {
        return m_qualitySettings;
    }

    void changeQuality(const struct qualitySettings& qs);

    sampleFrame* renderNextBuffer();
    sample_rate_t processingSampleRate() const;

    void removePlayHandlesOfTypes(Track* track, const quint8 types);

    void startProcessing(bool needsFifo = false);
    void stopProcessing();

    void requestChangeInModel();
    void doneChangeInModel();
    void runChangesInModel();

    void setOutputSampleRate(int outputSampleRate);
    sample_rate_t outputSampleRate() const;

    void sltSampleRateChanged();

    void setFramesPerPeriod(fpp_t framesPerPeriod);

    void setOutputLatency(double outputLatency);

    Q_INVOKABLE AudioDevice* audioDev() const;

    void setAudioDevice(AudioDevice* _dev,
        const struct qualitySettings& _qs,
        bool _needs_fifo,
        bool startNow);
    void storeAudioDevice();
    void restoreAudioDevice();
    inline AudioDevice* audioDev()
    {
        return m_audioDev;
    }

    static inline sample_t clip(const sample_t s)
    {
        if (s > 1.0f) {
            return 1.0f;
        } else if (s < -1.0f) {
            return -1.0f;
        }
        return s;
    }

private:
    double m_outputLatency = 0;
    bool m_audioDevStartFailed;
    int m_numWorkers;
    sample_rate_t _outputSampleRate;
    QVector<AudioEngineWorkerThread*> m_workers;

    fpp_t m_framesPerPeriod;
    int m_channels;
    AudioDevice* m_audioDev = Q_NULLPTR;
    AudioDevice* m_oldAudioDev = Q_NULLPTR;

    bool m_renderOnly;
    bool m_isProcessing;
    // playhandle stuff
    PlayHandleList m_playHandles;
    ConstPlayHandleList m_playHandlesToRemove;
    LocklessList<PlayHandle*> m_newPlayHandles;

    unsigned int m_changes;
    bool m_clearSignal;
    bool m_changesSignal;
    bool m_waitingForWrite;
    QMutex m_changesMutex;
    QMutex m_doChangesMutex;
    QMutex m_waitChangesMutex;
    QWaitCondition m_changesRequestCondition;
    QWaitCondition m_changesAudioEngineCondition;

    int m_inputBufferRead;
    int m_inputBufferWrite;
    sampleFrame* m_outputBufferRead = Q_NULLPTR;
    sampleFrame* m_outputBufferWrite = Q_NULLPTR;
    f_cnt_t m_inputBufferFrames[2];

    QVector<AudioPort*> m_audioPorts;

    struct qualitySettings m_qualitySettings;
    Fifo* m_fifo;
    fifoWriter* m_fifoWriter;

    AudioDevice* tryAudioDevices(int channels = 2, int deviceIndex = -1);
    void clearInternal();

    void swapBuffers();
    bool criticalXRuns() const;
    void doSetAudioDevice(AudioDevice* _dev);
signals:
    void sampleRateChanged();
    void outputLatencyChanged();
    void outputBufferSizeChanged();
    void nextAudioBuffer(const sampleFrame* buffer);
    void aduioDeviceChanged();
    void qualitySettingsChanged();
};

#endif // AUDIOENGINE_H
