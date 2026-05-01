#ifndef AUDIOPORT_H
#define AUDIOPORT_H

#include "PlayHandle.h"
#include "effectchain.h"
#include "memory"
#include "threadablejob.h"
#include "visrhelper.h"
#include <QAbstractListModel>
#include <QString>
#include <libvisr/constants.hpp>
#include <vector>

class Automation;
class SampleTrack;

class AudioPort : public ThreadableJob {
    Q_OBJECT
public:
    explicit AudioPort(const QString& _name, SampleTrack* sampleTrack);
    ~AudioPort();
    void addPlayHandle(PlayHandle* handle);
    void removePlayHandle(PlayHandle* handle);

    void addVisr(QSharedPointer<VisrHelper> handle);
    void removeVisr();

    inline sampleFrame* buffer()
    {
        return m_portBuffer;
    }

    // next mixer-channel after this audio-port
    // (-1 = none  0 = master)
    inline mix_ch_t nextMixerChannel() const
    {
        return m_nextMixerChannel;
    }

    void setNextMixerChannel(const mix_ch_t _chnl)
    {
        m_nextMixerChannel = _chnl;
    }

    inline QSharedPointer<EffectChain> effects()
    {
        return m_effects;
    }

    inline QSharedPointer<Effect> visrEfect()
    {
        if (m_visrHelper) {
            return m_visrHelper->effect();
        }
        return Q_NULLPTR;
    }

private:
    int m_channels;
    QString m_name;
    sampleFrame* m_portBuffer = Q_NULLPTR;
    PlayHandleList m_playHandles;
    volatile bool m_bufferUsage;
    volatile bool m_bufferUsed;
    QMutex m_playHandleLock;
    mix_ch_t m_nextMixerChannel;
    SampleTrack* m_sampleTrack;
    QSharedPointer<VisrHelper> m_visrHelper = Q_NULLPTR;
    QSharedPointer<EffectChain> m_effects;
    std::vector<visr::SampleType*> mCaptureSampleBuffers;
    std::vector<visr::SampleType*> mPlaybackSampleBuffers;

    bool processEffects();

public:
    bool requiresProcessing() const override
    {
        return true;
    }

    volatile bool bufferUsage() const;

    volatile bool bufferUsed() const;

    QSharedPointer<VisrHelper> visrHelper() const;

protected:
    void doProcessing();

    // ThreadableJob interface
public:
    QString name() override
    {
        return "audio_port";
    }
private slots:
    void audioEngineChanged();
};

#endif // AUDIOPORT_H
