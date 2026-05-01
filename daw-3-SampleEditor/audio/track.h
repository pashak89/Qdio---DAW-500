#ifndef TRACK_H
#define TRACK_H

#include "journallingobject.h"
#include "timepos.h"
#include "visrhelper.h"
#include <QAbstractListModel>
#include <QColor>
#include <QMutex>
#include <QObject>
#include <eq/multieq.h>
class Clip;
class AreaInfo;
class TrackContainer;
class Automation;

class Track : public QObject, public JournallingObject {
    Q_OBJECT

public:
    using clipVector = QVector<Clip*>;

    enum TrackTypes {
        TrackTypes_InstrumentTrack,
        TrackTypes_PatternTrack,
        TrackTypes_SampleTrack,
        TrackTypes_EventTrack,
        TrackTypes_VideoTrack,
        TrackTypes_AutomationTrack,
        TrackTypes_HiddenAutomationTrack,
        TrackTypes_NumTrackTypes,
    };

    Track(int trackIndex, TrackTypes type, AreaInfo* areaInfo, QObject* parent = Q_NULLPTR);
    ~Track() override;

    static Track* create(TrackTypes tt, AreaInfo* areaInfo, int trackIndex);

    Track* clone();

    // pure virtual functions
    TrackTypes type() const
    {
        return m_type;
    }

    virtual bool play(const TimePos& start, const fpp_t frames,
        const f_cnt_t frameBase, int clipNum = -1)
        = 0;

    virtual Clip* createClip(const TimePos& pos, bool isFake = false) = 0;
    virtual Clip* createClip(const TimePos& pos, QString filename, bool isFake = false) = 0;

    void setSimpleSerializing()
    {
        m_simpleSerializingMode = true;
    }

    // -- for usage by Clip only ---------------
    Clip* addClip(Clip* clip);
    void removeClip(Clip* clip);
    // -------------------------------------------------------
    void deleteClips();

    int numOfClips();
    Clip* getClip(int clipNum);
    int getClipNum(const Clip* clip);

    const clipVector& getClips() const
    {
        return m_clips;
    }
    void getClipsInRange(clipVector& clipV, const TimePos& start,
        const TimePos& end);
    void swapPositionOfClips(int clipNum1, int clipNum2);

    // void createClipsForPattern(int pattern);

    void insertBar(const TimePos& pos);
    void removeBar(const TimePos& pos);

    tick_t length() const;

    //    inline TrackContainer* trackContainer() const
    //    {
    //        return m_trackContainer;
    //    }

    void lock()
    {
        m_processingLock.lock();
    }
    void unlock()
    {
        m_processingLock.unlock();
    }
    bool tryLock()
    {
        return m_processingLock.tryLock();
    }

    QColor color()
    {
        return m_color;
    }
    bool useColor()
    {
        return m_hasColor;
    }

    bool isMutedBeforeSolo() const
    {
        return m_mutedBeforeSolo;
    }

    bool isMuted() const
    {
        return m_muted;
    }

    bool isSolo() const
    {
        return m_solo;
    }

    bool isSelected() const
    {
        return m_selected;
    }

    QSharedPointer<Automation> volumeAutomation() const;
    QSharedPointer<Automation> panAutomation() const;

    void setSolo(bool solo);
    void setMuted(bool muted);

    void setSelected(bool selected);

    int trackIndex() const;

    QSharedPointer<Automation> speakerAutomation() const;

public slots:
    virtual void setName(const QString& newName)
    {
        m_name = newName;
        emit nameChanged();
    }

    void setMutedBeforeSolo(const bool muted)
    {
        m_mutedBeforeSolo = muted;
    }

    void setColor(const QColor& c);
    void resetColor();

private:
    // TrackContainer* m_trackContainer;
    TrackTypes m_type;
    QString m_name;
    int m_height;
    int m_trackIndex;

protected:
    QSharedPointer<Automation> _volumeAutomation;
    QSharedPointer<Automation> _panAutomation;
    QSharedPointer<Automation> _speakerAutomation;
    AreaInfo* _areaInfo;

    QSharedPointer<MultiEQ> _multiEq;

private:
    bool m_solo
        = false;
    bool m_mutedBeforeSolo;
    bool m_muted = false;
    bool m_selected = true;

    bool m_simpleSerializingMode;

    clipVector m_clips;

    QMutex m_processingLock;

    QMutex m_new_track;
    QList<int> m_new_track_list;

    QColor m_color;
    bool m_hasColor;

signals:
    void destroyedTrack();
    void nameChanged();
    void clipAdded(Clip*);
    void colorChanged();
};

#endif // TRACK_H
