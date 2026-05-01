#ifndef SAMPLEBUFFER_H
#define SAMPLEBUFFER_H

#include "audio_global.h"
#include "sampleblock.h"
#include "sharedobject.h"
#include "ui/ipainteritem.h"
#include <QBuffer>
#include <QObject>
#include <QReadWriteLock>
#include <QThread>
#include <QTimer>
#include <soxr.h>
const float DefaultBaseFreq = 440.f;
const int BYTES_PER_FRAME = sizeof(sampleFrame);
constexpr ch_cnt_t DEFAULT_CHANNELS = 2;
using SampleBlockPtr = std::shared_ptr<SampleBlock>;

class SampleClip;
class SampleBuffer;
#undef AL
#define AL(a) (sizeof(a) / sizeof((a)[0])) /* Array Length */

class AsyncFileReader : public QThread {
    Q_OBJECT
public:
    AsyncFileReader(QString filename, SampleBuffer* sampleBuffer, QObject* parent = Q_NULLPTR);
    ~AsyncFileReader();
    void start();

    void quit();
    void setFrame(f_cnt_t currentFrame);
    f_cnt_t getFrame()
    {
        return m_currentFrame;
    }

protected:
    void run() override;

private:
    bool m_quit = false;
    SF_INFO sfInfo;
    SNDFILE* sndFile;
    int myindex = -1;
    f_cnt_t m_currentFrame;
    f_cnt_t m_nextFrame;

    SampleBuffer* _sampleBuffer = Q_NULLPTR;
    QString m_fileName;
    void flush(int index);
};

class FillFirstBuffer : public QThread {
    Q_OBJECT
public:
    FillFirstBuffer(QString filename, int currentFrame, SampleBuffer* _sampleBuffer, bool toPlay, QObject* parent = Q_NULLPTR);

    ~FillFirstBuffer();
    void quit();

protected:
    void run() override;

private:
    QString m_fileName;
    bool m_quit = false;
    SF_INFO sfInfo;
    SNDFILE* sndFile;
    bool _toPlay;

    int m_currentFrame;
    SampleBuffer* _sampleBuffer;

Q_SIGNALS:
    void sigFinished(QString clipIndex, bool toPlay);
};

class SampleBuffer : public QObject, public sharedObject {
    Q_OBJECT

public:
    enum LoopMode {
        LoopOff = 0,
        LoopOn,
        LoopPingPong
    };
    class handleState {

    public:
        handleState(bool varyingPitch = false);
        virtual ~handleState();

        const f_cnt_t frameIndex() const
        {
            return m_frameIndex;
        }

        void setFrameIndex(f_cnt_t index)
        {
            m_frameIndex = index;
        }

        bool isBackwards() const
        {
            return m_isBackwards;
        }

        void setBackwards(bool backwards)
        {
            m_isBackwards = backwards;
        }

        f_cnt_t renderedFrames() const
        {
            return m_rendered_frames;
        }
        void setRenderedFrames(f_cnt_t newRendered_frames)
        {
            m_rendered_frames = newRendered_frames;
        }

    private:
        f_cnt_t m_frameIndex;
        f_cnt_t m_rendered_frames = 0;
        const bool m_varyingPitch;
        bool m_isBackwards;

        friend class SampleBuffer;
    };

    SampleBuffer(bool isFake);
    // constructor which either loads sample _audio_file or decodes
    // base64-data out of string
    SampleBuffer(const QString& audioFile, bool isFake, bool isBase64Data = false);
    SampleBuffer(const SampleBuffer& orig, bool isFake);
    explicit SampleBuffer(const f_cnt_t frames, const f_cnt_t channels);
    // friend void swap(SampleBuffer& first, SampleBuffer& second) noexcept;
    // SampleBuffer& operator=(const SampleBuffer that);

    fpp_t resample(int channel, sampleFrame* _src, const fpp_t _frames, sampleFrame* _dst, sample_rate_t _src_sr, sample_rate_t _dst_sr, int offset = 0);

    SampleBuffer* clone(bool fake)
    {

        return new SampleBuffer(*this, fake);
    }

    ~SampleBuffer() override;

    bool play(
        sampleFrame* ab,
        handleState* state,
        const fpp_t frames,
        const float freq,
        const LoopMode loopMode = LoopOff);

    void visualize(IPainter& p, const QRect& dr, int channelNumber, double startTime, double endTime, float* max, float* min, int size);
    void visualize2(const QRect& dr, int channelNumber, double startTime, double endTime, float* max, float* min, int size,
        QVector<QPointF>& minPoints, QVector<QPointF>& maxPoints, QVector<QPointF>& points);
    void get(const QRectF& dr, int witdh,
        int channelNumber, double startTime, double endTime, float* arr_max,
        float* arr_min);
    inline const QString& audioFile() const
    {
        return m_audioFile;
    }

    inline f_cnt_t startFrame() const
    {
        return m_startFrame;
    }

    inline f_cnt_t endFrame() const
    {
        return m_endFrame;
    }

    inline f_cnt_t loopStartFrame() const
    {
        return m_loopStartFrame;
    }

    inline f_cnt_t loopEndFrame() const
    {
        return m_loopEndFrame;
    }

    void setLoopStartFrame(f_cnt_t start)
    {
        m_loopStartFrame = start;
    }

    void setLoopEndFrame(f_cnt_t end)
    {
        m_loopEndFrame = end;
    }

    void setAllPointFrames(
        f_cnt_t start,
        f_cnt_t end,
        f_cnt_t loopStart,
        f_cnt_t loopEnd)
    {
        m_startFrame = start;
        m_endFrame = end;
        m_loopStartFrame = loopStart;
        m_loopEndFrame = loopEnd;
    }

    inline f_cnt_t frames() const
    {
        return m_frames;
    }

    inline f_cnt_t channels()
    {
        return m_channels;
    }

    inline float amplification() const
    {
        return m_amplification;
    }

    inline bool reversed() const
    {
        return m_reversed;
    }

    inline float frequency() const
    {
        return m_frequency;
    }

    sample_rate_t sampleRate() const
    {
        return m_sampleRate;
    }

    int sampleLength() const
    {
        return double(m_endFrame - m_startFrame) / m_sampleRate * 1000;
    }

    inline void setFrequency(float freq)
    {
        m_frequency = freq;
    }

    inline void setSampleRate(sample_rate_t rate)
    {
        m_sampleRate = rate;
    }

    QString openAudioFile() const;
    QString openAndSetAudioFile();
    QString openAndSetWaveformFile();

    QString& toBase64(QString& dst) const;

    void normalizeSampleRate(const sample_rate_t srcSR, bool keepSettings = false);

    void dataReadLock()
    {
        m_varLock.lockForRead();
    }

    void dataUnlock()
    {
        m_varLock.unlock();
    }

    std::mutex mtx;
    FillFirstBuffer* _workerThread = Q_NULLPTR;
    std::condition_variable bufferNotEmpty;
    std::atomic_int m_current_data_index = 0;
    std::atomic_int m_prev_data_index = -1;
    std::atomic_int m_current_frame_index = -1;
    size_t frame_count = MINIMUM_FRAME_COUNT;

    f_cnt_t baseStartFrame() const;
    void setBaseStartFrame(const f_cnt_t& baseStartFrame);

    f_cnt_t baseEndFrame() const;
    void setBaseEndFrame(const f_cnt_t& baseEndFrame);

    f_cnt_t offsetStartFrame() const;
    void setOffsetStartFrame(const f_cnt_t& offsetStartFrame);

    QString clipIndex() const;
    void setClipIndex(const QString& clipIndex);

    f_cnt_t offsetEndFrame() const;
    void setOffsetEndFrame(const f_cnt_t& offsetEndFrame);

    void init_sox(int samplerate, int channels);

    QVector<soxr_t> m_soxr;
    //  soxr_t m_soxr_right[64] = nullptr;
public slots:
    QString audioFile();
    void setAudioFile(const QString& audioFile);
    void setStartFrame(const f_cnt_t s);
    void setEndFrame(const f_cnt_t e);
    void setAmplification(float a);
    // void setReversed(bool on);
    void sampleRateChanged();
    void sltStopped();
    void sltPositionChanged(quint64 frame, QString clipItem, bool toPlay);
    void sltPositionChanged(quint64 frame);
    void sltFillFirstBufferChanged(QString clipItem, bool toPlay);
    void sltFillFirsFinished();

public:
    QVector<sampleFrame*> m_data_;

    fpp_t resample_flush(int channel, const fpp_t outSize, sampleFrame* _dst);

private:
    static sample_rate_t audioEngineSampleRate();

    bool _bufferFill = true;
    int _bufferFillCounter = 0;
    QTimer m_timebufferFill;

    void update(bool keepSettings = false);

    QString m_clipIndex;
    QString m_audioFile;
    f_cnt_t m_origFrames;
    ch_cnt_t m_channels;

    mutable QReadWriteLock m_dataLocker;

    mutable QReadWriteLock m_varLock;

    f_cnt_t m_baseStartFrame = 0;
    f_cnt_t m_baseEndFrame = 0;
    f_cnt_t m_offsetStartFrame = 0;
    f_cnt_t m_offsetEndFrame = 0;

    f_cnt_t m_current_frame;
    f_cnt_t m_frames;
    f_cnt_t m_startFrame;
    f_cnt_t m_endFrame;
    f_cnt_t m_loopStartFrame;
    f_cnt_t m_loopEndFrame;
    float m_amplification;
    bool m_reversed;
    float m_frequency;
    sample_rate_t m_sampleRate;
    QMap<double, double> where;
    SampleBlockPtr pBlock;

    quint64 m_minSamples; // min samples per block
    quint64 m_maxSamples; // max samples per block
    static size_t sMaxDiskBlockSize;

    QByteArray m_summeyArray;
    QBuffer m_buffer;
    bool m_fake = false;
    float* getSampleFragment(int channel, f_cnt_t playFrame, f_cnt_t playIndex,
        f_cnt_t frames,
        LoopMode loopMode, sampleFrame* _data, sampleFrame* _nextData,
        sampleFrame** tmp,
        bool* backwards,
        f_cnt_t loopStart,
        f_cnt_t loopEnd,
        f_cnt_t end) const;

    f_cnt_t getLoopedIndex(f_cnt_t index, f_cnt_t startf, f_cnt_t endf) const;
    f_cnt_t getPingPongIndex(f_cnt_t index, f_cnt_t startf, f_cnt_t endf) const;

    AsyncFileReader* async_writer = Q_NULLPTR;
    f_cnt_t decodeSampleSF(QString fileName);
    void fillDataBuffers(QString fileName);
    double fillWhere(double startX, double endX, double t0, double t1, double s0, double samplesPerPixel);

signals:
    void sampleUpdated();

    void sigFirstBufferFilled(QString clipItem, bool toPlay);
};

#endif // SAMPLEBUFFER_H
