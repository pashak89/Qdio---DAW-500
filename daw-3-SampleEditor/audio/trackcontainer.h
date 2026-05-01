#ifndef TRACKCONTAINER_H
#define TRACKCONTAINER_H

#include "track.h"
#include <QObject>
#include <QReadWriteLock>
class Track;
class TrackContainer : public QObject {
    Q_OBJECT
public:
    using TrackList = QVector<Track*>;
    enum TrackContainerTypes {
        PatternContainer,
        SongContainer
    };

    TrackContainer(QObject* parent = Q_NULLPTR);
    ~TrackContainer() override;
    int countTracks(Track::TrackTypes _tt = Track::TrackTypes_NumTrackTypes) const;

    void addTrack(Track* _track);
    void removeTrack(Track* _track);

    virtual void updateAfterTrackAdd();

    void clearAllTracks();

    const TrackList& tracks() const
    {
        return m_tracks;
    }

    bool isEmpty() const;

    static const QString classNodeName()
    {
        return "trackcontainer";
    }

    inline void setType(TrackContainerTypes newType)
    {
        m_TrackContainerType = newType;
    }

    inline TrackContainerTypes type() const
    {
        return m_TrackContainerType;
    }


signals:
    void trackAdded(Track* _track);

protected:

    mutable QReadWriteLock m_tracksMutex;

private:
    TrackList m_tracks;

    TrackContainerTypes m_TrackContainerType;
    friend class Track;
};

#endif // TRACKCONTAINER_H
