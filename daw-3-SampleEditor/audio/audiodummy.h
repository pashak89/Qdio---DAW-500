#ifndef AUDIODUMMY_H
#define AUDIODUMMY_H

#include "audiodevice.h"
#include "audioengine.h"
#include "microtimer.h"
#include <QThread>

class SetupWidgetDeummy : public ISetupWidget {

    // ISetupWidget interface
public:
    SetupWidgetDeummy(QObject* parent = Q_NULLPTR)
        : ISetupWidget(parent)
    {
        m_backend = new DeviceModel();
    }

protected:
    int findCurrentBackendIndex(QString device)
    {
        return -1;
    }
    int findCurrentDeviceIndex(QString device)
    {
        return -1;
    }
    void setBackend(int hostApiType)
    {
    }
    void openAsioControlPanel(int index)
    {
    }
    bool isAsioDevice(int index)
    {
        return false;
    }
    QStringList deviceSampleRates(int index)
    {
        return QStringList();
    }
    int findSampleRates(int deviceIndex, int sampleRate)
    {
        return -1;
    }
    double defaultOutoutLowLatency(int index)
    {
        return 0;
    }

    int hostIndexToDeviceIndex(int hostIndex)
    {
        return -1;
    }

    DeviceInfo* getDevice(int index)
    {
        return Q_NULLPTR;
    }
};

class AudioDummy : public QThread, public AudioDevice {
    Q_OBJECT
public:
    AudioDummy(bool& _success_ful, AudioEngine* audioEngine)
        : AudioDevice(audioEngine)
    {
        s_instanceOfMe = new SetupWidgetDeummy();
        _success_ful = true;
    }

    ~AudioDummy() override
    {
        delete s_instanceOfMe;
        stopProcessing();
    }

    QString name() override
    {
        return "Dummy";
    }

private:
    void startProcessing() override
    {
        start();
    }

    void stopProcessing() override
    {
        stopProcessingThread(this);
    }

    void run() override
    {
        MicroTimer timer;
        while (true) {
            timer.reset();
            sampleFrame* b = audioEngine()->nextBuffer();

            if (!b) {
                break;
            }

            const int microseconds = static_cast<int>(audioEngine()->framesPerPeriod() * 1000000.0f / audioEngine()->processingSampleRate() - timer.elapsed());
            if (microseconds > 0) {
                usleep(microseconds);
            }
        }
    }
};
#endif // AUDIODUMMY_H
