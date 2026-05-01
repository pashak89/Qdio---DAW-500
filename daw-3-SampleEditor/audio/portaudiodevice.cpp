#include "portaudiodevice.h"
#include "audioengine.h"
#include <QDebug>

PortAudioDevice::PortAudioDevice(bool& _success_ful, int channels, AudioEngine* _audioEngine, int deviceIndex)
    : AudioDevice(_audioEngine)
    , m_portAudioInithasError(false)
    , m_paStream(nullptr)
    , m_outBufPos(0)
{

    _success_ful = false;

    Pa_Terminate();
    PaError err = Pa_Initialize();

    if (err != paNoError) {
        printf("Couldn't initialize PortAudio: %s\n", Pa_GetErrorText(err));
        m_portAudioInithasError = true;
        return;
    }

    if (Pa_GetDeviceCount() <= 0) {
        return;
    }

    m_outDevIdx = deviceIndex;
    if (m_outDevIdx < 0) {
        m_outDevIdx = Pa_GetDefaultOutputDevice();
    }

    if (m_outDevIdx < 0) {
        return;
    }

    double inLatency = 0; //(double)audioEngine()->framesPerPeriod() / (double)sampleRate();
    double outLatency = 0; //(double)audioEngine()->framesPerPeriod() / (double)sampleRate();

    // inLatency = Pa_GetDeviceInfo( inDevIdx )->defaultLowInputLatency;
    // outLatency = Pa_GetDeviceInfo( outDevIdx )->defaultLowOutputLatency;
    const int samples = audioEngine()->framesPerPeriod();

    double latency = audioEngine()->outputLatency();

    m_outBufSize = samples;

    // Configure output parameters.
    m_outputParameters.device = m_outDevIdx;
    m_outputParameters.channelCount = channels;
    m_outputParameters.sampleFormat = paFloat32; // 32 bit floating point output
    m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(m_outputParameters.device)->defaultLowOutputLatency;
    m_outputParameters.hostApiSpecificStreamInfo = nullptr;

    auto deviceInfo = Pa_GetDeviceInfo(m_outputParameters.device);

    wchar_t wideName[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, deviceInfo->name, -1, wideName, MAX_PATH - 1);
    _deviceName = (QString::fromWCharArray(wideName));
    _hostApiName = Pa_GetHostApiInfo(deviceInfo->hostApi)->name;
    m_maxOutChannels = Pa_GetDeviceInfo(m_outputParameters.device)->maxOutputChannels;

    qDebug() << "DEFAULT:" << m_outputParameters.device << Pa_GetDeviceInfo(m_outputParameters.device)->defaultLowOutputLatency
             << Pa_GetDeviceInfo(m_outputParameters.device)->maxOutputChannels << m_outputParameters.channelCount;

    if (Pa_GetDeviceInfo(m_outputParameters.device)->maxOutputChannels == 0 || m_outputParameters.channelCount == 0 || sampleRate() == 0) {
        return;
    }

    latency = Pa_GetDeviceInfo(m_outputParameters.device)->defaultLowOutputLatency;

    m_channels = m_outputParameters.channelCount;
    m_outBuf = new sampleFrame(m_channels, audioEngine()->framesPerPeriod());
    auto hostApiIndex = Pa_GetDeviceInfo(m_outputParameters.device)->hostApi;

    // Open an audio I/O stream.
    err = Pa_OpenStream(
        &m_paStream,
        nullptr, // The input parameter
        &m_outputParameters, // The outputparameter
        sampleRate(),
        samples,
        paNoFlag, // Don't use any flags
        _process_callback, // our callback function
        this);

    if (err != paNoError) {
        printf("Couldn't open PortAudio: %s\n", Pa_GetErrorText(err));
        qDebug() << "sampleRate()" << sampleRate();
        return;
    }

    _success_ful = true;

    _asioChangesTimer.setInterval(300);
    connect(&_asioChangesTimer, &QTimer::timeout, [this]() {
        auto deviceInddex = setupDevice()->hostIndexToDeviceIndex(m_outDevIdx);
        if (deviceInddex >= 0) {

            auto isAsio = setupDevice()->isAsioDevice(deviceInddex);

            if (isAsio) {
                long minLatency, maxLatency, preferredLatency, granularity;
                long inputLatency, outputLatency;

                PaError err = PaAsio_GetAvailableBufferSizes(deviceInddex,
                    &minLatency, &maxLatency, &preferredLatency, &granularity);

                if (err == paNoError)
                    setupDevice()->getDevice(deviceInddex)->setBufferSize(preferredLatency);

                const PaStreamInfo* streamInfo = Pa_GetStreamInfo(m_paStream);

                err = PaAsio_GetAvailableLatencyValues(deviceInddex,
                    &minLatency, &maxLatency, &preferredLatency, &granularity);

                if (err == paNoError) {
                    setupDevice()->getDevice(deviceInddex)->setInputLatency(inputLatency / streamInfo->sampleRate * 1000.0);
                    setupDevice()->getDevice(deviceInddex)->setOutputLatency(outputLatency / streamInfo->sampleRate * 1000.0);
                }
            }
        }
    });

    s_instanceOfMe = new SetupWidget(hostApiIndex);

    Q_EMIT setupDeviceChanged();
}

PortAudioDevice::~PortAudioDevice()
{

    stopProcessing();
    _asioChangesTimer.stop();
    if (!m_portAudioInithasError) {
        Pa_Terminate();
    }
    delete s_instanceOfMe;
    delete m_outBuf;
    qDebug() << "~PortAudioDevice()";
}

int PortAudioDevice::process_callback(const float* _inputBuffer, float* _outputBuffer, unsigned long _framesPerBuffer)
{

    if (m_stopped) {
        memset(_outputBuffer, 0, _framesPerBuffer * channels() * sizeof(float));
        return paComplete;
    }

    while (_framesPerBuffer) {
        if (m_outBufPos == 0) {
            // frames depend on the sample rate
            const fpp_t frames = getNextBuffer(m_outBuf);
            if (!frames) {
                m_stopped = true;
                memset(_outputBuffer, 0, _framesPerBuffer * channels() * sizeof(float));
                return paComplete;
            }
            m_outBufSize = frames;
        }
        const int min_len = qMin((int)_framesPerBuffer,
            m_outBufSize - m_outBufPos);

        QString bufferContents;
        bool allZero = true;
        for (int j = 0; j < channels(); j++) {
            for (int i = 0; i < m_outBuf->getNumSamples(); ++i) {
                _outputBuffer[channels() * i + j] = m_outBuf->getSample(j, i); // Store elements from array 'a'
            }
        }

        _outputBuffer += min_len * channels();
        _framesPerBuffer -= min_len;
        m_outBufPos += min_len;
        m_outBufPos %= m_outBufSize;
    }
    return paContinue;
}

void PortAudioDevice::startProcessing()
{

    m_stopped = false;
    PaError err = Pa_StartStream(m_paStream);

    if (err != paNoError) {
        m_stopped = true;
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    } else {
        _asioChangesTimer.start();
    }
}

void PortAudioDevice::stopProcessing()
{
    if (m_paStream && Pa_IsStreamActive(m_paStream)) {
        m_stopped = true;
        PaError err = Pa_StopStream(m_paStream);

        if (err != paNoError) {
            printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        }
    }
}

int PortAudioDevice::_process_callback(
    const void* _inputBuffer,
    void* _outputBuffer,
    unsigned long _framesPerBuffer,
    const PaStreamCallbackTimeInfo* _timeInfo,
    PaStreamCallbackFlags _statusFlags,
    void* _arg)
{
    Q_UNUSED(_timeInfo);
    Q_UNUSED(_statusFlags);

    // qDebug() << "_framesPerBuffer" << _framesPerBuffer;
    auto _this = static_cast<PortAudioDevice*>(_arg);
    return _this->process_callback((const float*)_inputBuffer,
        (float*)_outputBuffer, _framesPerBuffer);
}

SetupWidget::SetupWidget(int hostApi, QObject* _parent)
    : ISetupWidget(_parent)
{
    m_backend = new DeviceModel();
    m_device = new DeviceModel();

    for (int i = 0; i < Pa_GetHostApiCount(); i++) {
        const PaHostApiInfo* info = Pa_GetHostApiInfo(i);

        if (info->type == paMME || info->type == paASIO) {
            qDebug() << i << info->name << info->type;
            m_backend->addItem(info->name, info->type);
        }
    }
    const PaHostApiInfo* info = Pa_GetHostApiInfo(hostApi);

    setBackend(info->type);

    loadDeviceInfo();
}

void SetupWidget::setBackend(int hostApiType)
{
    int numDevices = Pa_GetDeviceCount();

    m_device->clear();

    if (hostApiType == -1)
        return;

    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo* padeviceInfo;
        padeviceInfo = Pa_GetDeviceInfo(i);

        if (padeviceInfo->maxOutputChannels <= 0) {
            continue;
        }

        if (hostApiType == Pa_GetHostApiInfo(padeviceInfo->hostApi)->type) {

            QString _name;
#ifdef WIN32

            { /* Use wide char on windows, so we can show UTF-8 encoded device names */
                wchar_t wideName[MAX_PATH];
                MultiByteToWideChar(CP_UTF8, 0, padeviceInfo->name, -1, wideName, MAX_PATH - 1);
                // wprintf(L"Name                        = %s\n", wideName);
                _name = (QString::fromWCharArray(wideName));
            }
#else
            _name = (padeviceInfo->name);
#endif

            m_device->addItem(_name, i);
        }
    }
}

void SetupWidget::loadDeviceInfo()
{

    int numDevices = Pa_GetDeviceCount();

    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo* padeviceInfo;
        padeviceInfo = Pa_GetDeviceInfo(i);

        DeviceInfo* deviceInfo = new DeviceInfo();

#ifdef WIN32
        /* ASIO specific latency information */
        if (Pa_GetHostApiInfo(padeviceInfo->hostApi)->type == paASIO) {
            long minLatency, maxLatency, preferredLatency, granularity;
            PaError err = PaAsio_GetAvailableLatencyValues(i,
                &minLatency, &maxLatency, &preferredLatency, &granularity);

            deviceInfo->setBufferSize(preferredLatency);
        }

        if (padeviceInfo->hostApi == Pa_HostApiTypeIdToHostApiIndex(paASIO)) {
            deviceInfo->setIsAsioDevice(true);
        }

#else
        deviceInfo->setName(padeviceInfo->name);
#endif

        deviceInfo->setHostApiIndex(padeviceInfo->hostApi);
        deviceInfo->setMaxInputChannel(padeviceInfo->maxInputChannels);
        deviceInfo->setMaxOutputChannel(padeviceInfo->maxOutputChannels);
        deviceInfo->setDefaultSampleRate(padeviceInfo->defaultSampleRate);

        if (i == Pa_GetDefaultInputDevice()) {
            deviceInfo->setIsDefaultInput(true);
        } else if (i == Pa_GetDefaultOutputDevice()) {
            deviceInfo->setIsDefaultOutput(true);
        }

        deviceInfo->setHostApiIndex(Pa_GetHostApiInfo(padeviceInfo->hostApi)->type);
        deviceInfo->setHostApiName(Pa_GetHostApiInfo(padeviceInfo->hostApi)->name);

        static double standardSampleRates[] = {
            8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
            44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1 /* negative terminated  list */
        };

        PaStreamParameters inputParameters, outputParameters;

        inputParameters.device = i;
        inputParameters.channelCount = padeviceInfo->maxInputChannels;
        inputParameters.sampleFormat = paFloat32;
        inputParameters.suggestedLatency = 0; /* ignored by Pa_IsFormatSupported() */
        inputParameters.hostApiSpecificStreamInfo = NULL;

        outputParameters.device = i;
        outputParameters.channelCount = padeviceInfo->maxOutputChannels;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = 0; /* ignored by Pa_IsFormatSupported() */
        outputParameters.hostApiSpecificStreamInfo = NULL;

        QStringList sampleSupported;

        if (inputParameters.channelCount > 0) {
            for (int j = 0; standardSampleRates[j] > 0; j++) {
                PaError err;
                err = Pa_IsFormatSupported(&inputParameters, nullptr, standardSampleRates[j]);
                if (err == paFormatIsSupported) {
                    sampleSupported.append(QString::number(standardSampleRates[j]));
                }
            }
            deviceInfo->setDeviceType(DeviceInfo::DeviceType_Input);
        }

        if (outputParameters.channelCount > 0) {
            for (int j = 0; standardSampleRates[j] > 0; j++) {
                PaError err;
                err = Pa_IsFormatSupported(nullptr, &outputParameters, standardSampleRates[j]);
                if (err == paFormatIsSupported) {
                    sampleSupported.append(QString::number(standardSampleRates[j]));
                }
            }
            deviceInfo->setDeviceType(DeviceInfo::DeviceType_Output);
            deviceInfo->setOutputLatency(Pa_GetDeviceInfo(outputParameters.device)->defaultLowInputLatency);
        }

        if (inputParameters.channelCount > 0 && outputParameters.channelCount > 0) {
            for (int j = 0; standardSampleRates[j] > 0; j++) {
                PaError err;
                err = Pa_IsFormatSupported(&inputParameters, &outputParameters, standardSampleRates[j]);
                if (err == paFormatIsSupported) {
                    sampleSupported.append(QString::number(standardSampleRates[j]));
                }
            }
            deviceInfo->setDeviceType(DeviceInfo::DeviceType_InputOutput);
            deviceInfo->setOutputLatency(Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency);
        }

        deviceInfo->setSampleRates(sampleSupported);
        _devices.append(deviceInfo);
    }
}
