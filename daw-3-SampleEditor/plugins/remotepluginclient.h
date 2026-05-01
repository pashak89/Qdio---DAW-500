
//#ifndef REMOTE_PLUGIN_CLIENT_H
//#define REMOTE_PLUGIN_CLIENT_H

//#define SYNC_WITH_SHM_FIFO
//#define LMMS_BUILD_WIN32
//#include "remotepluginbase.h"

//#include <stdexcept>

//#include "sharedmemory.h"
//#include "vstsyncdata.h"

//class RemotePluginClient : public RemotePluginBase {
//public:
//    RemotePluginClient(const std::string& _shm_in, const std::string& _shm_out);

//    RemotePluginClient(const char* socketPath);

//    ~RemotePluginClient() override;

//    const VstSyncData* getVstSyncData();

//    bool processMessage(const message& _m) override;

//    virtual void process(const sampleFrame* _in_buf,
//        sampleFrame* _out_buf)
//        = 0;

//    virtual void processMidiEvent(const MidiEvent&, const f_cnt_t /* _offset */)
//    {
//    }

//    virtual void updateSampleRate()
//    {
//    }

//    virtual void updateBufferSize()
//    {
//    }

//    inline sample_rate_t sampleRate() const
//    {
//        return m_sampleRate;
//    }

//    inline fpp_t bufferSize() const
//    {
//        return m_bufferSize;
//    }

//    void setInputCount(int _i)
//    {
//        m_inputCount = _i;
//        sendMessage(message(IdChangeInputCount).addInt(_i));
//    }

//    void setOutputCount(int _i)
//    {
//        m_outputCount = _i;
//        sendMessage(message(IdChangeOutputCount).addInt(_i));
//    }

//    void setInputOutputCount(int i, int o)
//    {
//        m_inputCount = i;
//        m_outputCount = o;
//        sendMessage(message(IdChangeInputOutputCount)
//                        .addInt(i)
//                        .addInt(o));
//    }

//    virtual int inputCount() const
//    {
//        return m_inputCount;
//    }

//    virtual int outputCount() const
//    {
//        return m_outputCount;
//    }

//    void debugMessage(const std::string& _s)
//    {
//        sendMessage(message(IdDebugMessage).addString(_s));
//    }

//private:
//    void setShmKey(const std::string& key);
//    void doProcessing();

//    SharedMemory<float[]> m_audioBuffer;
//    SharedMemory<const VstSyncData> m_vstSyncData;

//    int m_inputCount;
//    int m_outputCount;

//    sample_rate_t m_sampleRate;
//    fpp_t m_bufferSize;
//};

//RemotePluginClient::RemotePluginClient(const std::string& _shm_in, const std::string& _shm_out)
//    : RemotePluginBase(new shmFifo(_shm_in), new shmFifo(_shm_out))
//    ,

//    m_inputCount(0)
//    , m_outputCount(0)
//    , m_sampleRate(44100)
//    , m_bufferSize(0)
//{
//}

//RemotePluginClient::~RemotePluginClient()
//{
//    sendMessage(IdQuit);
//}

//const VstSyncData* RemotePluginClient::getVstSyncData()
//{
//    return m_vstSyncData.get();
//}

//bool RemotePluginClient::processMessage(const message& _m)
//{
//    message reply_message(_m.id);
//    bool reply = false;
//    switch (_m.id) {
//    case IdUndefined:
//        return false;

//    case IdSyncKey:
//        try {
//            m_vstSyncData.attach(_m.getString(0));
//        } catch (const std::runtime_error& error) {
//            debugMessage(std::string { "Failed to attach sync data: " } + error.what() + '\n');
//            std::exit(EXIT_FAILURE);
//        }
//        m_bufferSize = m_vstSyncData->m_bufferSize;
//        m_sampleRate = m_vstSyncData->m_sampleRate;
//        reply_message.id = IdHostInfoGotten;
//        reply = true;
//        break;

//    case IdSampleRateInformation:
//        m_sampleRate = _m.getInt();
//        updateSampleRate();
//        reply_message.id = IdInformationUpdated;
//        reply = true;
//        break;

//    case IdBufferSizeInformation:
//        // Should LMMS gain the ability to change buffer size
//        // without a restart, it must wait for this message to
//        // complete processing or else risk VST crashes
//        m_bufferSize = _m.getInt();
//        updateBufferSize();
//        break;

//    case IdQuit:
//        return false;

//    case IdMidiEvent:
//        processMidiEvent(
//            MidiEvent(static_cast<MidiEventTypes>(
//                          _m.getInt(0)),
//                _m.getInt(1),
//                _m.getInt(2),
//                _m.getInt(3)),
//            _m.getInt(4));
//        break;

//    case IdStartProcessing:
//        doProcessing();
//        reply_message.id = IdProcessingDone;
//        reply = true;
//        break;

//    case IdChangeSharedMemoryKey:
//        setShmKey(_m.getString(0));
//        break;

//    case IdInitDone:
//        break;

//    default: {
//        char buf[64];
//        sprintf(buf, "undefined message: %d\n", (int)_m.id);
//        debugMessage(buf);
//        break;
//    }
//    }
//    if (reply) {
//        sendMessage(reply_message);
//    }

//    return true;
//}

//void RemotePluginClient::setShmKey(const std::string& key)
//{
//    try {
//        m_audioBuffer.attach(key);
//    } catch (const std::runtime_error& error) {
//        debugMessage(std::string { "failed getting shared memory: " } + error.what() + '\n');
//    }
//}

//void RemotePluginClient::doProcessing()
//{
//    if (m_audioBuffer) {
//        process((sampleFrame*)(m_inputCount > 0 ? m_audioBuffer.get() : nullptr),
//            (sampleFrame*)(m_audioBuffer.get() + (m_inputCount * m_bufferSize)));
//    } else {
//        debugMessage("doProcessing(): have no shared memory!\n");
//    }
//}

//#endif // LMMS_REMOTE_PLUGIN_CLIENT_H
