//#ifndef REMOTEPLUGIN_H
//#define REMOTEPLUGIN_H

//#include "audio/audio_global.h"
//#include "remotepluginbase.h"
//#include "sharedmemory.h"

//#include <QProcess>
//#include <QRecursiveMutex>
//#include <QThread>

//class RemotePlugin;
//class ProcessWatcher : public QThread {
//    Q_OBJECT
//public:
//    ProcessWatcher(RemotePlugin*);
//    ~ProcessWatcher() override = default;

//    void stop()
//    {
//        m_quit = true;
//        quit();
//    }

//    void reset()
//    {
//        m_quit = false;
//    }

//private:
//    void run() override;

//    RemotePlugin* m_plugin;
//    volatile bool m_quit;
//};

//class RemotePlugin : public QObject, public RemotePluginBase {
//    Q_OBJECT
//public:
//    RemotePlugin();
//    ~RemotePlugin() override;

//    inline bool isRunning()
//    {
//        return m_process.state() != QProcess::NotRunning;
//    }

//    bool init(const QString& pluginExecutable, bool waitForInitDoneMsg, QStringList extraArgs = {});

//    inline void waitForHostInfoGotten()
//    {
//        m_failed = waitForMessage(IdHostInfoGotten).id
//            != IdHostInfoGotten;
//    }

//    inline void waitForInitDone(bool _busyWaiting = true)
//    {
//        m_failed = waitForMessage(IdInitDone, _busyWaiting).id != IdInitDone;
//    }

//    bool processMessage(const message& _m) override;

//    bool process(const sampleFrame* _in_buf, sampleFrame* _out_buf);

//    void updateSampleRate(sample_rate_t _sr)
//    {
//        lock();
//        sendMessage(message(IdSampleRateInformation).addInt(_sr));
//        waitForMessage(IdInformationUpdated, true);
//        unlock();
//    }

//    virtual void toggleUI()
//    {
//        lock();
//        sendMessage(IdToggleUI);
//        unlock();
//    }

//    int isUIVisible()
//    {
//        lock();
//        sendMessage(IdIsUIVisible);
//        unlock();
//        message m = waitForMessage(IdIsUIVisible);
//        return m.id != IdIsUIVisible ? -1 : m.getInt() ? 1
//                                                       : 0;
//    }

//    inline bool failed() const
//    {
//        return m_failed;
//    }

//    inline void lock()
//    {
//        m_commMutex.lock();
//    }

//    inline void unlock()
//    {
//        m_commMutex.unlock();
//    }

//public slots:
//    virtual void showUI();
//    virtual void hideUI();

//protected:
//    inline void setSplittedChannels(bool _on)
//    {
//        m_splitChannels = _on;
//    }

//    bool m_failed;

//private:
//    void resizeSharedProcessingMemory();

//    QProcess m_process;
//    ProcessWatcher m_watcher;

//    QString m_exec;
//    QStringList m_args;

//    QRecursiveMutex m_commMutex;

//    bool m_splitChannels;

//    SharedMemory<float[]> m_audioBuffer;
//    std::size_t m_audioBufferSize;

//    int m_inputCount;
//    int m_outputCount;

//    int m_server;
//    QString m_socketFile;

//    friend class ProcessWatcher;

//private slots:
//    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
//    void processErrored(QProcess::ProcessError err);
//};

//#endif // REMOTEPLUGIN_H
