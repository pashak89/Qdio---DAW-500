#include "clip.h"
#include "audioengine.h"
#include "engine.h"
#include "song.h"
#include "track.h"
#include <QDebug>
/*! \brief Create a new Clip
 *
 *  Creates a new clip for the given track.
 *
 * \param _track The track that will contain the new object
 */
Clip::Clip(Track* track, bool isFake, QObject* parent)
    : QObject(parent)
    , m_track(track)
    , m_startPosition()
    , m_length()
    , m_mutedModel(false)
    , m_muted(false)
    , m_selectViewOnCreate(false)
    , m_color(128, 128, 128)
    , m_useCustomClipColor(false)
{
    _isFake = isFake;
    if (getTrack() && _isFake == false) {
        getTrack()->addClip(this);
        changeLength(0);
    }
}

Clip::Clip(const Clip& clip, Track* track, bool isFake, QObject* parent)
    : QObject(parent)
    , _isFake(isFake)
    , m_track(track)
    , m_startPosition(clip.m_startPosition)
    , m_length(clip.m_length)
    , m_startTimeOffset(clip.m_startTimeOffset)
    , m_mutedModel(false)
    , m_muted(false)
    , m_selectViewOnCreate(false)
    , m_color(128, 128, 128)
    , m_useCustomClipColor(false)
{

    if (getTrack() && _isFake == false) {
        getTrack()->addClip(this);
        setStartTimeOffset(clip.m_startTimeOffset);
        movePosition(clip.m_startPosition);
        changeLength(clip.m_length);
    }
}

/*! \brief Destroy a Clip
 *
 *  Destroys the given clip.
 *
 */
Clip::~Clip()
{

    emit destroyedClip();

    if (getTrack()) {
        getTrack()->removeClip(this);
    }
}

/*! \brief Move this Clip's position in time
 *
 *  If the clip has moved, update its position.  We
 *  also add a journal entry for undo and update the display.
 *
 * \param _pos The new position of the clip.
 */
void Clip::movePosition(const TimePos& pos)
{

    double value = pos.getTicks();
    TimePos newPos = qMax(0.0, value);

    if (m_startPosition != newPos) {
        AudioManager::audioEngine()->requestChangeInModel();
        m_startPosition = newPos;
        AudioManager::audioEngine()->doneChangeInModel();
        AudioManager::getSong()->updateLength();
        emit positionChanged(m_clipIndex);
    }
}

/*! \brief Change the length of this Clip
 *
 *  If the clip's length has changed, update it.  We
 *  also add a journal entry for undo and update the display.
 *
 * \param _length The new length of the clip.
 */
void Clip::changeLength(const TimePos& length)
{

    m_length = length;
    AudioManager::getSong()->updateLength();
    emit lengthChanged(m_clipIndex);
}

bool Clip::comparePosition(const Clip* a, const Clip* b)
{
    return a->startPosition() < b->startPosition();
}

/*! \brief Mutes this Clip
 *
 *  Restore the previous state of this clip. This will
 *  restore the position or the length of the clip
 *  depending on what was changed.
 *
 * \param _je The journal entry to undo
 */
void Clip::toggleMute()
{
    m_mutedModel = !m_mutedModel;
    emit dataChanged();
}

void Clip::setClipIndex(const QString& clipIndex)
{

    m_clipIndex = clipIndex;
}

QString Clip::clipIndex() const
{
    return m_clipIndex;
}

void Clip::setFake(bool isFake)
{

    _isFake = isFake;
}

bool Clip::isFake() const
{
    return _isFake;
}

TimePos Clip::startTimeOffset() const
{
    return m_startTimeOffset;
}

void Clip::setStartTimeOffset(const TimePos& startTimeOffset)
{
    m_startTimeOffset = startTimeOffset;
}

void Clip::useCustomClipColor(bool b)
{
    if (b == m_useCustomClipColor) {
        return;
    }
    m_useCustomClipColor = b;
    emit colorChanged();
}

bool Clip::hasColor()
{
    return usesCustomClipColor() || getTrack()->useColor();
}
