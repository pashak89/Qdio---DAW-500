#ifndef PORTAUDIODEVICE_H
#define PORTAUDIODEVICE_H

#include "audio_global.h"
#include "audiodevice.h"
#include "audiodevicemodel.h"

#include <portaudio.h>
#include <vector>

#ifdef WIN32

#include <QTimer>
#include <windows.h>
#endif

#include "pa_asio.h"

class SetupWidget : public ISetupWidget {
    Q_OBJECT

public:
    SetupWidget(int hostApi = 0, QObject* _parent = Q_NULLPTR);
    Q_INVOKABLE void setBackend(int hostApiType);

    void loadDeviceInfo();

    Q_INVOKABLE int findCurrentBackendIndex(QString value)
    {
        return backend()->find(value);
    }
    Q_INVOKABLE int findCurrentDeviceIndex(QString value)
    {
        return device()->find(value);
    }
    DeviceInfo* getDevice(int index)
    {
        if (index >= 0 && index < _devices.size())
            return _devices[index];
        else
            return Q_NULLPTR;
    }
    Q_INVOKABLE int hostIndexToDeviceIndex(int hostIndex)
    {
        for (int i = 0; i < _devices.size(); i++) {
            if (_devices[i]->hostApiIndex() == hostIndex) {
                return i;
            }
        }
        return -1;
    }

    Q_INVOKABLE QStringList deviceSampleRates(int index)
    {
        if (index >= 0 && index < _devices.size())
            return _devices[index]->sampleRates();
        else
            return QStringList();
    }
    Q_INVOKABLE int findSampleRates(int deviceIndex, int sampleRate)
    {
        if (deviceIndex >= 0 && deviceIndex < _devices.size()) {
            auto rates = _devices[deviceIndex]->sampleRates();
            for (int i = 0; i < rates.size(); i++) {
                if (rates[i] == QString::number(sampleRate)) {
                    return i;
                }
            }
        } else {
            return -1;
        }
    }

    Q_INVOKABLE void openAsioControlPanel(int index)
    {

        if (index >= _devices.size())
            return;

        if (index < 0)
            return;

        if (!isAsioDevice(index))
            return;

        // #ifdef WIN32

        //         DeviceInfo* device = _dFevices[index];

        //         if (device->isAsioDevice())
        //             PaAsio_ShowControlPanel(index, GetDesktopWindow());

        // #endif
    }

    Q_INVOKABLE bool isAsioDevice(int index)
    {
        if (index >= 0 && index < _devices.size()) {
            return _devices[index]->isAsioDevice();
        }
        return false;
    }

    Q_INVOKABLE double defaultOutoutLowLatency(int index)
    {
        qDebug() << "defaultOutoutLowLatency" << index;
        if (index >= 0 && index < _devices.size()) {
            return _devices.at(index)->outputLatency();
        }
        return 0.0;
    }

Q_SIGNALS:
    void driverModelChanged();
};

class PortAudioDevice : public AudioDevice {
    Q_OBJECT

public:
    PortAudioDevice(bool& _success_ful, int channels, AudioEngine* _audioEngine, int deviceIndex = -1);
    ~PortAudioDevice();

    int process_callback(const float* _inputBuffer,
        float* _outputBuffer,
        unsigned long _framesPerBuffer);

    PaStream* m_paStream;
    PaStreamParameters m_outputParameters;
    int m_outBufSize;
    bool m_portAudioInithasError;
    int m_outBufPos;

    void startProcessing();
    void stopProcessing();

    sampleFrame* m_outBuf;

private:
    QTimer _asioChangesTimer;
    bool m_stopped = false;
    static int _process_callback(const void* _inputBuffer, void* _outputBuffer,
        unsigned long _framesPerBuffer,
        const PaStreamCallbackTimeInfo* _timeInfo,
        PaStreamCallbackFlags _statusFlags,
        void* arg);

    // AudioDevice interface
public:
    QString name() override
    {
        return "PortAudioDevice";
    }
};

#endif // PORTAUDIODEVICE_H
