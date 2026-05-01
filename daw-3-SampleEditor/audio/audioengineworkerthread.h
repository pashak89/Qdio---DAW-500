#ifndef AUDIOENGINEWORKERTHREAD_H
#define AUDIOENGINEWORKERTHREAD_H

#include <QThread>

#include <atomic>

class QWaitCondition;
class ThreadableJob;
class AudioEngine;
class AudioEngineWorkerThread : public QThread {
    Q_OBJECT
public:
    // internal representation of the job queue - all functions are thread-safe
    class JobQueue {
    public:
        enum OperationMode {
            Static, // no jobs added while processing queue
            Dynamic // jobs can be added while processing queue
        };

        static constexpr size_t JOB_QUEUE_SIZE = 8192;

        JobQueue()
            : m_items()
            , m_writeIndex(0)
            , m_itemsDone(0)
            , m_opMode(Static)
        {
            std::fill(m_items, m_items + JOB_QUEUE_SIZE, nullptr);
        }

        void reset(OperationMode _opMode);

        void addJob(ThreadableJob* _job);

        void run();
        void wait();

    private:
        std::atomic<ThreadableJob*> m_items[JOB_QUEUE_SIZE];
        std::atomic_int m_writeIndex;
        std::atomic_int m_itemsDone;
        OperationMode m_opMode;
    };

    AudioEngineWorkerThread(AudioEngine* audioEngine);
    ~AudioEngineWorkerThread() override;

    virtual void quit();

    static void resetJobQueue(JobQueue::OperationMode _opMode = JobQueue::Static)
    {
        globalJobQueue.reset(_opMode);
    }

    static void addJob(ThreadableJob* _job)
    {
        globalJobQueue.addJob(_job);
    }

    // a convenient helper function allowing to pass a container with pointers
    // to ThreadableJob objects
    template <typename T>
    static void fillJobQueue(const T& _vec,
        JobQueue::OperationMode _opMode = JobQueue::Static)
    {
        resetJobQueue(_opMode);
        for (typename T::ConstIterator it = _vec.begin(); it != _vec.end(); ++it) {
            addJob(*it);
        }
    }

    static void startAndWaitForJobs();

private:
    void run() override;

    static JobQueue globalJobQueue;
    static QWaitCondition* queueReadyWaitCond;
    static QList<AudioEngineWorkerThread*> workerThreads;

    volatile bool m_quit;
};
#endif // AUDIOENGINEWORKERTHREAD_H
