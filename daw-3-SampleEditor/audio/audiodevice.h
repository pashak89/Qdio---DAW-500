#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include "audio_global.h"
#include "audiodevicemodel.h"
#include "audioengine.h"
#include <QMutex>
#include <QObject>
#include <soxr.h>
class AudioEngine;

#undef AL
#define AL(a) (sizeof(a) / sizeof((a)[0])) /* Array Length */

class ISetupWidget : public QObject {
    Q_OBJECT
    Q_PROPERTY(DeviceModel* backend READ backend NOTIFY driverModelChanged)
    Q_PROPERTY(DeviceModel* device READ device NOTIFY driverModelChanged)

public:
    ISetupWidget(QObject* parent = Q_NULLPTR)
        : QObject(parent)
    {
    }

protected:
    Q_INVOKABLE virtual int findCurrentBackendIndex(QString device) = 0;
    Q_INVOKABLE virtual int findCurrentDeviceIndex(QString device) = 0;
    Q_INVOKABLE virtual void setBackend(int hostApiType) = 0;
    Q_INVOKABLE virtual void openAsioControlPanel(int index) = 0;

    Q_INVOKABLE virtual QStringList deviceSampleRates(int index) = 0;

    Q_INVOKABLE virtual int findSampleRates(int deviceIndex, int sampleRate) = 0;

    Q_INVOKABLE virtual double defaultOutoutLowLatency(int index) = 0;

    virtual DeviceModel* backend() const
    {
        return m_backend;
    }

    virtual DeviceModel* device() const
    {
        return m_device;
    }

public:
    Q_INVOKABLE virtual int hostIndexToDeviceIndex(int hostIndex) = 0;
    Q_INVOKABLE virtual bool isAsioDevice(int index) = 0;
    Q_INVOKABLE virtual DeviceInfo* getDevice(int index) = 0;

protected:
    QVector<DeviceInfo*> _devices;
    DeviceModel* m_backend;
    DeviceModel* m_device;

signals:
    void driverModelChanged();
};

class AudioDevice : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ getDeviceName NOTIFY setupDeviceChanged)
    Q_PROPERTY(QString hostApiName READ getHostApiName NOTIFY setupDeviceChanged)
    Q_PROPERTY(int outDevIdx READ getOutDevIdx NOTIFY setupDeviceChanged)
    Q_PROPERTY(int maxOutChannels READ getMaxOutChannels NOTIFY setupDeviceChanged)
    Q_PROPERTY(ISetupWidget* setupDevice READ setupDevice NOTIFY setupDeviceChanged)
public:
    explicit AudioDevice(AudioEngine* _audioEngine = Q_NULLPTR);

    fpp_t resample(int channel, const sampleFrame* _src, const fpp_t _frames,
        sampleFrame* _dst, const sample_rate_t _src_sr, const sample_rate_t _dst_sr);

    fpp_t resample_flush(int channel, const fpp_t outSize, sampleFrame* _dst);

    void processFlush();

    inline sample_rate_t sampleRate() const
    {
        return m_sampleRate;
    }

    AudioEngine* audioEngine()
    {
        return m_audioEngine;
    }

    inline void lock()
    {
        m_devMutex.lock();
    }

    inline void unlock()
    {
        m_devMutex.unlock();
    }

    ch_cnt_t channels() const
    {
        return m_channels;
    }

    virtual void startProcessing()
    {
        m_inProcess = true;
    }

    virtual void stopProcessing();

    QString getDeviceName() const;

    QString getHostApiName() const;

    int getOutDevIdx() const;

    int getMaxOutChannels() const;

    Q_INVOKABLE virtual ISetupWidget* setupDevice()
    {
        return s_instanceOfMe;
    }

    void applyQualitySettings();

    ~AudioDevice();

    void processNextBuffer();

    void setSampleRate(sample_rate_t newSampleRate);

    virtual QString name()
    {
        return "";
    }

protected:
    fpp_t getNextBuffer(sampleFrame* _ab);
    QString _deviceName;
    QString _hostApiName;
    int m_outDevIdx;
    int m_maxOutChannels;

    void stopProcessingThread(QThread* thread);

    ISetupWidget* s_instanceOfMe;
    ch_cnt_t m_channels;

    int convertToS16(const sampleFrame* _ab,
        const fpp_t _frames,
        const float _master_gain,
        int_sample_t* _output_buffer,
        const bool _convert_endian = false);

    virtual void writeBuffer(const sampleFrame* /* _buf*/,
        const fpp_t /*_frames*/,
        const float /*_master_gain*/)
    {
    }

private:
    //    SRC_DATA m_srcData;
    //    SRC_STATE* m_srcState;

    bool m_running = false;
    AudioEngine* m_audioEngine;
    QMutex m_devMutex;
    sample_rate_t m_sampleRate;
    bool m_inProcess;
    int m_frames;

    soxr_t m_soxr_left = nullptr;
    soxr_t m_soxr_right = nullptr;

    sampleFrame* m_buffer;

Q_SIGNALS:
    void setupDeviceChanged();
};

#endif // AUDIODEVICE_H
