#include "audiodevicemodel.h"

DeviceInfo::DeviceInfo(QObject* parent)
    : QObject(parent)
{
    _isAsioDevice = false;
}

QString DeviceInfo::name() const
{
    return _name;
}

void DeviceInfo::setName(const QString& name)
{
    _name = name;
}

bool DeviceInfo::isDefaultInput() const
{
    return _isDefaultInput;
}

void DeviceInfo::setIsDefaultInput(bool isDefaultInput)
{
    _isDefaultInput = isDefaultInput;
}

int DeviceInfo::hostApiIndex() const
{
    return _hostApiIndex;
}

void DeviceInfo::setHostApiIndex(int hostApiIndex)
{
    _hostApiIndex = hostApiIndex;
}

int DeviceInfo::maxInputChannel() const
{
    return _maxInputChannel;
}

void DeviceInfo::setMaxInputChannel(int maxInputChannel)
{
    _maxInputChannel = maxInputChannel;
}

int DeviceInfo::maxOutputChannel() const
{
    return _maxOutputChannel;
}

void DeviceInfo::setMaxOutputChannel(int maxOutputChannel)
{
    _maxOutputChannel = maxOutputChannel;
}

double DeviceInfo::defaultSampleRate() const
{
    return _defaultSampleRate;
}

void DeviceInfo::setDefaultSampleRate(double defaultSampleRate)
{
    _defaultSampleRate = defaultSampleRate;
}

bool DeviceInfo::isDefaultOutput() const
{
    return _isDefaultOutput;
}

void DeviceInfo::setIsDefaultOutput(bool isDefaultOutput)
{
    _isDefaultOutput = isDefaultOutput;
}

QString DeviceInfo::hostApiName() const
{
    return _hostApiName;
}

void DeviceInfo::setHostApiName(const QString& hostApiName)
{
    _hostApiName = hostApiName;
}

QStringList DeviceInfo::sampleRates() const
{
    return _sampleRates;
}

void DeviceInfo::setSampleRates(const QStringList& sampleRates)
{
    _sampleRates = sampleRates;
    Q_EMIT sampleRatesChanged();
}

bool DeviceInfo::isAsioDevice() const
{
    return _isAsioDevice;
}

void DeviceInfo::setIsAsioDevice(bool isAsioDevice)
{
    _isAsioDevice = isAsioDevice;
    Q_EMIT isAsioDeviceChanged();
}

double DeviceInfo::bufferSize() const
{
    return _bufferSize;
}

void DeviceInfo::setBufferSize(double bufferSize)
{
    _bufferSize = bufferSize;
    Q_EMIT bufferSizeChanged();
}

double DeviceInfo::inputLatency() const
{
    return _inputLatency;
}

void DeviceInfo::setInputLatency(double inputLatency)
{
    _inputLatency = inputLatency;
    Q_EMIT latencyChanged();
}

double DeviceInfo::outputLatency() const
{
    return _outputLatency;
}

void DeviceInfo::setOutputLatency(double outputLatency)
{
    _outputLatency = outputLatency;
    Q_EMIT latencyChanged();
}

double DeviceInfo::inputBufferSize() const
{
    return _inputBufferSize;
}

void DeviceInfo::setInputBufferSize(double inputBufferSize)
{
    _inputBufferSize = inputBufferSize;
    double _bufferSize = 0;
}

double DeviceInfo::outputBufferSize() const
{
    return _outputBufferSize;
}

void DeviceInfo::setOutputBufferSize(double outputBufferSize)
{
    _outputBufferSize = outputBufferSize;
    Q_EMIT bufferSizeChanged();
}

DeviceInfo::DeviceType DeviceInfo::deviceType() const
{
    return _deviceType;
}

void DeviceInfo::setDeviceType(const DeviceType& deviceType)
{
    _deviceType = deviceType;
}
