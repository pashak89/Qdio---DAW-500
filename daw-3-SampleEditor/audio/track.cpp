#include "track.h"
#include "audioengine.h"
#include "clip.h"
#include "engine.h"
#include "sampletrack.h"
#include "song.h"
#include "trackcontainer.h"
/*! \brief Create a new (empty) track object
 *
 *  The track object is the whole track, linking its contents, its
 *  automation, name, type, and so forth.
 *
 * \param type The type of track (Song Editor or Pattern Editor)
 * \param tc The track Container object to encapsulate in this track.
 *
 * \todo check the definitions of all the properties - are they OK?
 */
Track::Track(int trackIndex, TrackTypes type, AreaInfo* areaInfo, QObject* parent)
    : QObject(parent)
    , m_trackIndex(trackIndex)
    , /*!< The track Model */
    m_type(type)
    , /*!< The track type */
    m_name()
    , /*!< For controlling track muting */
    m_solo(false)
    , /*!< For controlling track soloing */
    m_simpleSerializingMode(false)
    , m_clips()
    , /*!< The clips (segments) */
    m_color(0, 0, 0)
    , m_hasColor(false)
    , _areaInfo(areaInfo)
{
    m_muted = false;

    m_height = -1;
}

/*! \brief Destroy this track
 *
 *  Delete the clips and remove this track from the track container.
 */
Track::~Track()
{
    lock();
    emit destroyedTrack();

    while (!m_clips.isEmpty()) {
        delete m_clips.last();
    }

    unlock();
}

/*! \brief Create a track based on the given track type and container.
 *
 *  \param tt The type of track to create
 *  \param tc The track container to attach to
 */
Track* Track::create(TrackTypes tt, AreaInfo* areaInfo, int trackIndex)
{

    // AudioManager::audioEngine()->requestChangeInModel();

    Track* t = nullptr;

    switch (tt) {
    case TrackTypes::TrackTypes_SampleTrack:
        t = new class SampleTrack(areaInfo, trackIndex);
        break;
    default:
        break;
    }

    // tc->updateAfterTrackAdd();

    //  AudioManager::audioEngine()->doneChangeInModel();

    return t;
}

/*! \brief Add another Clip into this track
 *
 *  \param clip The Clip to attach to this track.
 */
Clip* Track::addClip(Clip* clip)
{
    m_clips.push_back(clip);

    emit clipAdded(clip);

    return clip; // just for convenience
}

/*! \brief Remove a given Clip from this track
 *
 *  \param clip The Clip to remove from this track.
 */
void Track::removeClip(Clip* clip)
{
    clipVector::iterator it = std::find(m_clips.begin(), m_clips.end(), clip);
    if (it != m_clips.end()) {
        m_clips.erase(it);
        if (AudioManager::getSong()) {
            AudioManager::getSong()->updateLength();
            AudioManager::getSong()->setModified();
        }
    }
}

/*! \brief Remove all Clips from this track */
void Track::deleteClips()
{
    while (!m_clips.isEmpty()) {
        delete m_clips.first();
    }
}

/*! \brief Return the number of clips we contain
 *
 *  \return the number of clips we currently contain.
 */
int Track::numOfClips()
{

    return m_clips.size();
}

/*! \brief Get a Clip by number
 *
 *  If the Clip number is less than our Clip array size then fetch that
 *  numbered object from the array.  Otherwise we warn the user that
 *  we've somehow requested a Clip that is too large, and create a new
 *  Clip for them.
 *  \param clipNum The number of the Clip to fetch.
 *  \return the given Clip or a new one if out of range.
 *  \todo reject Clip numbers less than zero.
 *  \todo if we create a Clip here, should we somehow attach it to the
 *     track?
 */
Clip* Track::getClip(int clipNum)
{
    if (clipNum < m_clips.size()) {
        return m_clips[clipNum];
    }
    printf("called Track::getClip( %d ), "
           "but Clip %d doesn't exist\n",
        clipNum, clipNum);
    return createClip(clipNum * TimePos::ticksPerBar());
}

/*! \brief Determine the given Clip's number in our array.
 *
 *  \param clip The Clip to search for.
 *  \return its number in our array.
 */
int Track::getClipNum(const Clip* clip)
{
    //	for( int i = 0; i < getTrackContentWidget()->numOfClips(); ++i )
    clipVector::iterator it = std::find(m_clips.begin(), m_clips.end(), clip);
    if (it != m_clips.end()) {
        /*		if( getClip( i ) == _clip )
                {
                        return i;
                }*/
        return it - m_clips.begin();
    }
    qWarning("Track::getClipNum(...) -> _clip not found!\n");
    return 0;
}

/*! \brief Retrieve a list of clips that fall within a period.
 *
 *  Here we're interested in a range of clips that intersect
 *  the given time period.
 *
 *  We return the Clips we find in order by time, earliest Clips first.
 *
 *  \param clipV The list to contain the found clips.
 *  \param start The MIDI start time of the range.
 *  \param end   The MIDI endi time of the range.
 */
void Track::getClipsInRange(clipVector& clipV, const TimePos& start,
    const TimePos& end)
{
    for (Clip* clip : m_clips) {
        int s = clip->startPosition();
        int e = clip->endPosition();
        if ((s <= end) && (e >= start)) {
            // Clip is within given range
            // Insert sorted by Clip's position
            clipV.insert(std::upper_bound(clipV.begin(), clipV.end(), clip, Clip::comparePosition),
                clip);
        }
    }
}

/*! \brief Swap the position of two clips.
 *
 *  First, we arrange to swap the positions of the two Clips in the
 *  clips list.  Then we swap their start times as well.
 *
 *  \param clipNum1 The first Clip to swap.
 *  \param clipNum2 The second Clip to swap.
 */
void Track::swapPositionOfClips(int clipNum1, int clipNum2)
{
    qSwap(m_clips[clipNum1], m_clips[clipNum2]);

    const TimePos pos = m_clips[clipNum1]->startPosition();

    m_clips[clipNum1]->movePosition(m_clips[clipNum2]->startPosition());
    m_clips[clipNum2]->movePosition(pos);
}

// void Track::createClipsForPattern(int pattern)
//{
//     while (numOfClips() < pattern + 1) {
//         TimePos position = TimePos(numOfClips(), 0);
//         Clip* clip = createClip(position);
//         clip->changeLength(TimePos(1, 0));
//     }
// }

/*! \brief Move all the clips after a certain time later by one bar.
 *
 *  \param pos The time at which we want to insert the bar.
 *  \todo if we stepped through this list last to first, and the list was
 *    in ascending order by Clip time, once we hit a Clip that was earlier
 *    than the insert time, we could fall out of the loop early.
 */
void Track::insertBar(const TimePos& pos)
{
    // we'll increase the position of every Clip, positioned behind pos, by
    // one bar
    for (const auto& clip : m_clips) {
        if (clip->startPosition() >= pos) {
            clip->movePosition(clip->startPosition() + TimePos::ticksPerBar());
        }
    }
}

/*! \brief Move all the clips after a certain time earlier by one bar.
 *
 *  \param pos The time at which we want to remove the bar.
 */
void Track::removeBar(const TimePos& pos)
{
    // we'll decrease the position of every Clip, positioned behind pos, by
    // one bar
    for (const auto& clip : m_clips) {
        if (clip->startPosition() >= pos) {
            clip->movePosition(clip->startPosition() - TimePos::ticksPerBar());
        }
    }
}

/*! \brief Return the length of the entire track in bars
 *
 *  We step through our list of Clips and determine their end position,
 *  keeping track of the latest time found in ticks.  Then we return
 *  that in bars by dividing by the number of ticks per bar.
 */
tick_t Track::length() const
{
    // find last end-position
    tick_t last = 0;
    for (const auto& clip : m_clips) {
        if (AudioManager::getSong()->isExporting() && clip->isMuted()) {
            continue;
        }

        const tick_t cur = clip->endPosition();
        if (cur > last) {
            last = cur;
        }
    }

    return last;
}

void Track::setColor(const QColor& c)
{
    m_hasColor = true;
    m_color = c;
    emit colorChanged();
}

void Track::resetColor()
{
    m_hasColor = false;
    emit colorChanged();
}

QSharedPointer<Automation> Track::speakerAutomation() const
{
    return _speakerAutomation;
}

int Track::trackIndex() const
{
    return m_trackIndex;
}

void Track::setSelected(bool selected)
{
    m_selected = selected;
}

void Track::setMuted(bool muted)
{
    m_muted = muted;
}

QSharedPointer<Automation> Track::volumeAutomation() const
{
    return _volumeAutomation;
}

QSharedPointer<Automation> Track::panAutomation() const
{
    return _panAutomation;
}

void Track::setSolo(bool solo)
{
    m_solo = solo;
}
