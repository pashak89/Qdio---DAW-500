#ifndef AUDIODEVICEMODEL_H
#define AUDIODEVICEMODEL_H

#include <QAbstractListModel>
#include <QDebug>

class DeviceInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList sampleRates READ sampleRates WRITE setSampleRates NOTIFY sampleRatesChanged)
    Q_PROPERTY(bool isAsioDevice READ isAsioDevice WRITE setIsAsioDevice NOTIFY isAsioDeviceChanged)

    Q_PROPERTY(double inputLatency READ inputLatency WRITE setInputLatency NOTIFY latencyChanged)
    Q_PROPERTY(double outputLatency READ outputLatency WRITE setOutputLatency NOTIFY latencyChanged)
    Q_PROPERTY(double bufferSize READ bufferSize WRITE setBufferSize NOTIFY bufferSizeChanged)

    Q_PROPERTY(double inputBufferSize READ inputBufferSize WRITE setInputBufferSize NOTIFY bufferSizeChanged)
    Q_PROPERTY(double outputBufferSize READ outputBufferSize WRITE setOutputBufferSize NOTIFY bufferSizeChanged)
public:
    enum HostApi {
        HostAPI_DirectSound = 1,
        HostAPI_MME = 2,
        HostAPI_ASIO = 3,
        HostAPI_SoundManager = 4,
        HostAPI_CoreAudio = 5,
        HostAPI_OSS = 7,
        HostAPI_ALSA = 8,
        HostAPI_AL = 9,
        HostAPI_BeOS = 10,
        HostAPI_WDMKS = 11,
        HostAPI_JACK = 12,
        HostAPI_WASAPI = 13,
        HostAPI_AudioScienceHPI = 14
    };
    Q_ENUM(HostApi)

    enum DeviceType {
        DeviceType_Input,
        DeviceType_Output,
        DeviceType_InputOutput,
    };

    explicit DeviceInfo(QObject* parent = nullptr);

    QString name() const;
    void setName(const QString& name);

    bool isDefaultInput() const;
    void setIsDefaultInput(bool isDefaultInput);

    int hostApiIndex() const;
    void setHostApiIndex(int hostApiIndex);

    int maxInputChannel() const;
    void setMaxInputChannel(int maxInputChannel);

    int maxOutputChannel() const;
    void setMaxOutputChannel(int maxOutputChannel);

    double defaultSampleRate() const;
    void setDefaultSampleRate(double defaultSampleRate);

    bool isDefaultOutput() const;
    void setIsDefaultOutput(bool isDefaultOutput);

    QString hostApiName() const;
    void setHostApiName(const QString& hostApiName);

    QStringList sampleRates() const;
    void setSampleRates(const QStringList& sampleRates);

    bool isAsioDevice() const;
    void setIsAsioDevice(bool isAsioDevice);

    double bufferSize() const;
    void setBufferSize(double bufferSize);

    double inputLatency() const;
    void setInputLatency(double inputLatency);

    double outputLatency() const;
    void setOutputLatency(double outputLatency);

    double inputBufferSize() const;
    void setInputBufferSize(double inputBufferSize);

    double outputBufferSize() const;
    void setOutputBufferSize(double outputBufferSize);

    DeviceType deviceType() const;
    void setDeviceType(const DeviceType& deviceType);

private:
    QString _name;
    QString _hostApiName;
    DeviceType _deviceType;
    bool _isDefaultInput;
    bool _isDefaultOutput;
    int _hostApiIndex;
    int _maxInputChannel;
    int _maxOutputChannel;
    double _defaultSampleRate;

    bool _isAsioDevice;

    QStringList _sampleRates;

    // ASIO parameters
    double _inputLatency = 0;
    double _outputLatency = 0;
    double _bufferSize = 0;

    double _inputBufferSize = 0;
    double _outputBufferSize = 0;

Q_SIGNALS:
    void sampleRatesChanged();
    void isAsioDeviceChanged();
    void latencyChanged();
    void bufferSizeChanged();
};

class DeviceModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum DriverRole {
        ROLE_Index = Qt::UserRole + 1,
        ROLE_Name,
    };
    Q_ENUM(DriverRole)

    explicit DeviceModel(QObject* parent = Q_NULLPTR)
        : QAbstractListModel(parent)
    {
        m_roleNames[ROLE_Name] = "Name";
        m_roleNames[ROLE_Index] = "Index";
    }
    void addItem(QString item, int index)
    {
        beginResetModel();
        //_list[index] = item;
        _list.append(item);
        list_index.insert(item, index);

        qDebug() << "addItem" << item << index << _list.size();

        endResetModel();
    }
    void clear()
    {
        beginResetModel();
        endResetModel();

        _list.clear();
        list_index.clear();
    }

    int find(QString value)
    {
        for (int i = 0; i < _list.size(); i++) {
            if (_list[i] == value)
                return i;
        }
        if (_list.size() > 0) {
            return 0;
        }
        return -1;
    }

protected:
    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }

    QMap<QString, int> list_index;
    QList<QString> _list;

    QHash<int, QByteArray> m_roleNames;

public:
    virtual int rowCount(const QModelIndex& parent) const
    {
        return _list.size();
    }
    virtual QVariant data(const QModelIndex& index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        if (_list.size() == 0 || index.row() < 0)
            return QVariant();

        switch (role) {
        case ROLE_Name:
            return _list[index.row()];
        case ROLE_Index:
            return list_index[_list[index.row()]];
        }

        return QVariant();
    }
};

#endif // AUDIODEVICEMODEL_H
