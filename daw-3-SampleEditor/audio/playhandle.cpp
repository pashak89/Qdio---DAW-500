#include "playhandle.h"
#include "audioengine.h"
#include "engine.h"
#include <QDebug>
PlayHandle::PlayHandle(const Type type, f_cnt_t offset)
    : m_type(type)
    , m_offset(offset)
    , m_affinity(QThread::currentThread())
    , m_bufferReleased(true)
    , m_usesBuffer(true)
{
    m_playHandleBuffer = new sampleFrame(AudioManager::audioEngine()->channels(), AudioManager::audioEngine()->framesPerPeriod());
}

PlayHandle::~PlayHandle()
{
    delete m_playHandleBuffer;
}

void PlayHandle::doProcessing()
{

    if (m_usesBuffer) {
        m_bufferReleased = false;
        m_playHandleBuffer->clear();
        play(buffer());

    } else {
        play(nullptr);
    }
}

void PlayHandle::releaseBuffer()
{
    m_bufferReleased = true;
}

sampleFrame* PlayHandle::buffer()
{
    return m_bufferReleased ? nullptr : reinterpret_cast<sampleFrame*>(m_playHandleBuffer);
}
