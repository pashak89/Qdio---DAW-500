#ifndef THREADABLEJOB_H
#define THREADABLEJOB_H
#include "audio_global.h"
#include <QObject>
#include <atomic>

class ThreadableJob : public QObject {
public:
    enum class ProcessingState : int {
        Unstarted,
        Queued,
        InProgress,
        Done
    };

    ThreadableJob()
        : m_state(ProcessingState::Unstarted)
    {
    }

    inline ProcessingState state() const
    {
        return m_state.load();
    }

    inline void reset()
    {
        m_state = ProcessingState::Unstarted;
    }

    inline void queue()
    {
        m_state = ProcessingState::Queued;
    }

    inline void done()
    {
        m_state = ProcessingState::Done;
    }

    void process()
    {
        auto expected = ProcessingState::Queued;
        if (m_state.compare_exchange_strong(expected, ProcessingState::InProgress)) {
            doProcessing();
            m_state = ProcessingState::Done;
        }
    }

    virtual bool requiresProcessing() const = 0;

    virtual QString name() = 0;

protected:
    virtual void doProcessing() = 0;

    std::atomic<ProcessingState> m_state;
};

#endif // THREADABLEJOB_H
