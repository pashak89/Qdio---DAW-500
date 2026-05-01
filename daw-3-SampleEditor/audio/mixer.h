#ifndef MIXER_H
#define MIXER_H

#include "audio_global.h"
#include "threadablejob.h"
#include <QMutex>
#include <QObject>
#include <QVector>
#include <kmeter/kmeter.h>

class MixerRoute;
using MixerRouteVector = QVector<MixerRoute*>;

class MixerChannel : public ThreadableJob {
public:
    MixerChannel(int idx);
    virtual ~MixerChannel();

    // set to true when input fed from mixToChannel or child channel
    bool m_hasInput;
    // set to true if any effect in the channel is enabled and running
    bool m_stillRunning;

    float m_peakLeft;
    float m_peakRight;
    sampleFrame* m_buffer;
    bool m_muteBeforeSolo;
    bool m_muteModel;
    bool m_soloModel;
    float m_volumeModel;
    QString m_name;
    QMutex m_lock;
    int m_channelIndex; // what channel index are we
    bool m_queued; // are we queued up for rendering yet?
    bool m_muted; // are we muted? updated per period so we don't have to call m_muteModel.value() twice

    // pointers to other channels that this one sends to
    MixerRouteVector m_sends;

    // pointers to other channels that send to this one
    MixerRouteVector m_receives;

    bool requiresProcessing() const override { return true; }
    void unmuteForSolo();

    std::atomic_int m_dependenciesMet;
    // void incrementDeps();
    // void processed();

    TruePeakAudioProcessor* truePeakMeter() const;

private:
    void doProcessing() override;

    // ThreadableJob interface
public:
    QString name() override
    {
        return "mixer_channel";
    }
};

class MixerRoute : public QObject {
    Q_OBJECT
public:
    MixerRoute(MixerChannel* from, MixerChannel* to, float amount);
    ~MixerRoute() override = default;

    mix_ch_t senderIndex() const
    {
        return m_from->m_channelIndex;
    }

    mix_ch_t receiverIndex() const
    {
        return m_to->m_channelIndex;
    }

    float amount()
    {
        return m_amount;
    }

    void setAmount(float amount)
    {
        m_amount = amount;
    }

    MixerChannel* sender() const
    {
        return m_from;
    }

    MixerChannel* receiver() const
    {
        return m_to;
    }

    void updateName();

private:
    MixerChannel* m_from;
    MixerChannel* m_to;
    float m_amount;
};

class Mixer : public QObject {
    Q_OBJECT
public:
    explicit Mixer(QObject* parent = Q_NULLPTR);
    void prepareMasterMix();
    void masterMix(sampleFrame* _buf);
    ~Mixer();

    inline mix_ch_t numChannels() const
    {
        return m_mixerChannels.size();
    }

    void mixToChannel(sampleFrame* _buf, mix_ch_t _ch);

    TruePeakAudioProcessor* truePeakMeter() const;

    float masterTrackVolume() const;
    void setMasterTrackVolume(float masterTrackVolume);
    void clearChannels();
    int createChannel();

    void addToSolo(int trackIndex)
    {
        m_soloTrackList.append(trackIndex);
    }
    void removeFromSolo(int trackIndex)
    {
        m_soloTrackList.removeAll(trackIndex);
    }
    bool isInSolo(int trackIndex)
    {
        return m_soloTrackList.contains(trackIndex);
    }
    bool hasSolo()
    {
        return m_soloTrackList.size() > 0;
    }

private:
    float m_masterTrackVolume = 0;

    TruePeakAudioProcessor* m_truePeakMeter;
    // the mixer channels in the mixer. index 0 is always master.
    QVector<MixerChannel*> m_mixerChannels;
    MixerRouteVector m_mixerRoutes;
    int m_lastSoloed;

    void deleteChannelSend(MixerRoute* route);
    void clearChannel(mix_ch_t index);

    MixerRoute* createChannelSend(mix_ch_t fromChannel, mix_ch_t toChannel, float amount = 1.0f);
    MixerRoute* createRoute(MixerChannel* from, MixerChannel* to, float amount);
    void deleteChannelSend(mix_ch_t fromChannel, mix_ch_t toChannel);

    QVector<int> m_soloTrackList;
};

#endif // MIXER_H
