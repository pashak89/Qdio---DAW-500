#include "trackcontainer.h"
#include "audio/mixer.h"
#include "engine.h"
#include "song.h"
#include "track.h"
TrackContainer::TrackContainer(QObject* parent)
    : QObject(parent)
    , m_tracksMutex()
    , m_tracks()
{
}

TrackContainer::~TrackContainer()
{
    clearAllTracks();
}

int TrackContainer::countTracks(Track::TrackTypes _tt) const
{
    int cnt = 0;
    m_tracksMutex.lockForRead();
    for (const auto& track : m_tracks) {
        if (track->type() == _tt || _tt == Track::TrackTypes_NumTrackTypes) {
            ++cnt;
        }
    }
    m_tracksMutex.unlock();
    return (cnt);
}

void TrackContainer::addTrack(Track* _track)
{
    if (_track->type() != Track::TrackTypes_HiddenAutomationTrack) {
        _track->lock();
        m_tracksMutex.lockForWrite();
        m_tracks.push_back(_track);
        m_tracksMutex.unlock();
        _track->unlock();
        emit trackAdded(_track);
    }
}

void TrackContainer::removeTrack(Track* _track)
{
    // need a read locker to ensure that m_tracks doesn't change after reading index.
    //   After checking that index != -1, we need to upgrade the lock to a write locker before changing m_tracks.
    //   But since Qt offers no function to promote a read lock to a write lock, we must start with the write locker.
    QWriteLocker lockTracksAccess(&m_tracksMutex);
    int index = m_tracks.indexOf(_track);
    if (index != -1) {
        // If the track is solo, all other tracks are muted. Change this before removing the solo track:
        if (_track->isSolo()) {
            _track->setSolo(false);
        }
        AudioManager::mixer()->removeFromSolo(_track->trackIndex());
        m_tracks.remove(index);
        lockTracksAccess.unlock();

        if (AudioManager::getSong()) {
            AudioManager::getSong()->setModified();
        }
    }
}

void TrackContainer::updateAfterTrackAdd()
{
}

void TrackContainer::clearAllTracks()
{
    // m_tracksMutex.lockForWrite();
    //    while (!m_tracks.isEmpty()) {
    //        if(m_tracks.first())
    //            delete m_tracks.first();
    //    }
    qDeleteAll(m_tracks);
    m_tracks.clear();
    // m_tracksMutex.unlock();
}

bool TrackContainer::isEmpty() const
{
    for (const auto& track : m_tracks) {
        if (!track->getClips().isEmpty()) {
            return false;
        }
    }
    return true;
}
