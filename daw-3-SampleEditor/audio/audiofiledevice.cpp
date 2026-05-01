#include "audiofiledevice.h"

#include <QMessageBox>

AudioFileDevice::AudioFileDevice(OutputSettings const& outputSettings,
    const ch_cnt_t _channels,
    const QString& _file,
    AudioEngine* _audioEngine)
    : AudioDevice(_audioEngine)
    , m_outputFile(_file)
    , m_outputSettings(outputSettings)
{

    setSampleRate(outputSettings.getSampleRate());

    m_outputFile.close();

    if (m_outputFile.open(QFile::WriteOnly | QFile::Truncate) == false) {

        QMessageBox msgBox;
        msgBox.setText("Cannot open file:" + m_outputFile.errorString());
        msgBox.exec();
    }

    qDebug() << "AudioFileDevice created";
}

AudioFileDevice::~AudioFileDevice()
{
    qDebug() << "AudioFileDevice::~AudioFileDevice()";
    m_outputFile.flush();
    m_outputFile.close();
}

int AudioFileDevice::writeData(const void* data, int len)
{
    if (m_outputFile.isOpen()) {
        return m_outputFile.write((const char*)data, len);
    }
    return -1;
}
