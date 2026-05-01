#ifndef AUDIOFILEDEVICE_H
#define AUDIOFILEDEVICE_H

#include "audiodevice.h"
#include "outputsettings.h"

#include <QFile>

class AudioFileDevice : public AudioDevice {
public:
    AudioFileDevice(OutputSettings const& outputSettings,
        const ch_cnt_t _channels, const QString& _file,
        AudioEngine* audioEngine);
    ~AudioFileDevice() override;

    QString outputFile() const
    {
        return m_outputFile.fileName();
    }

    OutputSettings const& getOutputSettings() const { return m_outputSettings; }

protected:
    int writeData(const void* data, int len);

    inline bool outputFileOpened() const
    {
        return m_outputFile.isOpen();
    }

    inline int outputFileHandle() const
    {
        return m_outputFile.handle();
    }

private:
    QFile m_outputFile;
    OutputSettings m_outputSettings;

    // AudioDevice interface
public:
    QString name() override
    {
        return "AudioFileDevice";
    }
};

using AudioFileDeviceInstantiaton
    = AudioFileDevice* (*)(const QString&, const OutputSettings&, const ch_cnt_t, AudioEngine*, bool&);

#endif // AUDIOFILEDEVICE_H
