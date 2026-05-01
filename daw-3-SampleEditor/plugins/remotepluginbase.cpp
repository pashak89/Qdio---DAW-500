//#include "remotepluginbase.h"

//RemotePluginBase::RemotePluginBase(shmFifo* _in, shmFifo* _out)
//    : m_in(_in)
//    , m_out(_out)

//{
//}

//RemotePluginBase::~RemotePluginBase()
//{

//    delete m_in;
//    delete m_out;
//}

//int RemotePluginBase::sendMessage(const message& _m)
//{

//    m_out->lock();
//    m_out->writeInt(_m.id);
//    m_out->writeInt(_m.data.size());
//    int j = 8;
//    for (unsigned int i = 0; i < _m.data.size(); ++i) {
//        m_out->writeString(_m.data[i]);
//        j += 4 + _m.data[i].size();
//    }
//    m_out->unlock();
//    m_out->messageSent();

//    return j;
//}

//RemotePluginBase::message RemotePluginBase::receiveMessage()
//{

//    m_in->waitForMessage();
//    m_in->lock();
//    message m;
//    m.id = m_in->readInt();
//    const int s = m_in->readInt();
//    for (int i = 0; i < s; ++i) {
//        m.data.push_back(m_in->readString());
//    }
//    m_in->unlock();

//    return m;
//}

//RemotePluginBase::message RemotePluginBase::waitForMessage(
//    const message& _wm,
//    bool _busy_waiting)
//{

//    while (!isInvalid()) {

//        message m = receiveMessage();
//        processMessage(m);
//        if (m.id == _wm.id) {
//            return m;
//        } else if (m.id == IdUndefined) {
//            return m;
//        }
//    }

//    return message();
//}
