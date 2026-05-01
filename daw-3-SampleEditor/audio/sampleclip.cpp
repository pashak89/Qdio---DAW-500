#include "sampleclip.h"
#include "audioengine.h"
#include "engine.h"
#include "metermodel.h"
#include "samplebuffer.h"
#include "sampletrack.h"
#include "song.h"

SampleClip::SampleClip(Track* _track, bool isFake)
    : Clip(_track, isFake)
    , m_sampleBuffer(new SampleBuffer(isFake))
    , m_isPlaying(false)
{

    if (_isFake == false) {
        setSampleFile("");
        initialize();
    }
}

SampleClip::SampleClip(Track* _track, QString fileName, bool isFake)
    : Clip(_track, isFake)
    , m_sampleBuffer(new SampleBuffer(fileName, isFake))
    , m_isPlaying(false)
{
    if (_isFake == false) {
        setSampleFile(fileName);
        initialize();
    }
}

SampleClip::SampleClip(SampleClip& item, Track* track, bool fake)
    : Clip(item, track, fake)
    , m_sampleBuffer(item.m_sampleBuffer->clone(fake))
    , m_recordModel(item.m_recordModel)
    , m_isPlaying(item.m_isPlaying)
    , _baseStartTime(item._baseStartTime)
    , _baseEndTime(item._baseEndTime)
    , _startTime(item._startTime)
    , _endTime(item._endTime)
    , _offsetStartTime(item._offsetStartTime)
    , _offsetEndTime(item._offsetEndTime)
    , _currentFileDuration(item._currentFileDuration)
    , _currentClipDuration(item._currentClipDuration)

{

    m_startPosition = item.m_startPosition;
    m_length = item.m_length;

    if (_isFake == false) {
        initialize();
    }
}

void SampleClip::initialize()
{

    connect(AudioManager::getSong(), SIGNAL(positionMarkerMoved()), this, SLOT(playbackPositionChanged()));
    connect(AudioManager::getSong(), SIGNAL(playbackStateChanged()), this, SLOT(playbackPositionChanged()));

    connect(this, SIGNAL(dataChanged()), this, SLOT(playbackPositionChanged()));
    connect(this, SIGNAL(positionChanged(QString)), this, SLOT(updateTrackClips(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(lengthChanged(QString)), this, SLOT(updateTrackClips(QString)), Qt::QueuedConnection);

    updateTrackClips(m_clipIndex);
}

SampleClip::~SampleClip()
{

    if (_isFake == false) {
        auto sampletrack = dynamic_cast<SampleTrack*>(getTrack());

        if (sampletrack) {
            sampletrack->updateClips();
        }
    }

    AudioManager::audioEngine()->requestChangeInModel();

    delete m_sampleBuffer;
    m_sampleBuffer = Q_NULLPTR;

    AudioManager::audioEngine()->doneChangeInModel();
}

void SampleClip::changeLength(const TimePos& _length)
{
    if (_length == 0) {
        Clip::changeLength(1);
    } else {
        Clip::changeLength(_length);
    }
}

const QString& SampleClip::sampleFile() const
{
    return m_sampleBuffer->audioFile();
}

void SampleClip::setSampleFile(const QString& _sf)
{
    int length;
    if (_sf.isEmpty()) { // When creating an empty sample clip make it a bar long
        float nom = AudioManager::getSong()->getTimeSigModel().numeratorModel();
        float den = AudioManager::getSong()->getTimeSigModel().denominatorModel();
        length = DefaultTicksPerBar * (nom / den);
    } else { // Otherwise set it to the sample's length
        m_sampleBuffer->setAudioFile(_sf);
        length = sampleLength();
        changeLength(length);
        setStartTimeOffset(0);

        if (_isFake == false) {
            emit playbackPositionChanged();
        }
    }
}

void SampleClip::playbackPositionChanged()
{

    AudioManager::audioEngine()->removePlayHandlesOfTypes(getTrack(), PlayHandle::TypeSamplePlayHandle);
    auto st = dynamic_cast<SampleTrack*>(getTrack());
    st->setPlayingClips(false);
}

QList<Warp*> SampleClip::warpMarkers() const
{
    return _warpMarkers;
}

void SampleClip::updateTrackClips(QString clipIndex)
{

    if (m_clipIndex < 0 || clipIndex < 0) {
        return;
    }

    auto sampletrack = dynamic_cast<SampleTrack*>(getTrack());
    if (sampletrack) {
        sampletrack->updateClips();
    }
}

void SampleClip::setClipIndex(const QString& clipIndex)
{
    Clip::setClipIndex(clipIndex);
    m_sampleBuffer->setClipIndex(clipIndex);
}

void SampleClip::sampleChange()
{

    double current_frame = (AudioManager::getSong()->getPlayPos().getTicks() * 256.0);

    if ((current_frame < m_sampleBuffer->baseStartFrame() || m_sampleBuffer->baseEndFrame() < current_frame)) {

        if (isPlaying() == false) {

            m_sampleBuffer->sltPositionChanged(m_sampleBuffer->baseStartFrame());
        }
    }
}

QJsonObject SampleClip::state()
{
    QJsonObject json;
    json["audio_file"] = sampleBuffer()->audioFile();

    json["start_time"] = _startTime;
    json["end_time"] = _endTime;

    json["offset_start"] = sampleBuffer()->offsetStartFrame();
    json["offset_end"] = sampleBuffer()->offsetEndFrame();

    json["start_time_offset"] = sampleBuffer()->offsetStartFrame() / 256.0;

    return json;
}

double SampleClip::currentFileDuration() const
{
    return _currentFileDuration;
}

void SampleClip::setCurrentFileDuration(double currentFileDuration)
{
    _currentFileDuration = currentFileDuration;
}

double SampleClip::endTime() const
{
    return _endTime;
}

void SampleClip::setEndTime(double endTime)
{
    _endTime = endTime;
}

double SampleClip::startTime() const
{
    return _startTime;
}

void SampleClip::setStartTime(double startTime)
{
    _startTime = startTime;
}

double SampleClip::baseEndTime() const
{
    return _baseEndTime;
}

void SampleClip::setBaseEndTime(double baseEndTime)
{
    _baseEndTime = baseEndTime;
}

double SampleClip::baseStartTime() const
{
    return _baseStartTime;
}

void SampleClip::setBaseStartTime(double baseStartTime)
{
    _baseStartTime = baseStartTime;
}

bool SampleClip::isPlaying() const
{
    return m_isPlaying;
}

void SampleClip::setIsPlaying(bool isPlaying)
{

    if (m_isPlaying != isPlaying) {
        m_isPlaying = isPlaying;
        Q_EMIT sigPlayingChanged(isPlaying, m_clipIndex);

        //        if (isPlaying == false) {
        //            if (AudioManager::getSong()->getPlayPos().getTicks() < startPosition() || AudioManager::getSong()->getPlayPos().getTicks() > endPosition()) {

        //                m_sampleBuffer->sltPositionChanged(m_sampleBuffer->baseStartFrame());
        //            }
        //        }
    }
}

TimePos SampleClip::sampleLength() const
{
    return (int)(m_sampleBuffer->frames() / AudioManager::framesPerTick());
}

void SampleClip::setSampleStartFrame(f_cnt_t startFrame)
{
    m_sampleBuffer->setStartFrame(startFrame);
}

void SampleClip::setSamplePlayLength(f_cnt_t length)
{
    m_sampleBuffer->setEndFrame(length);
}

void SampleClip::addWarpMarker(int index, double p0, double p1, double t0, double base0, double t1, double base1, double tempo1)
{
    Warp* w = new Warp();
    w->setT0(t0);
    w->setBase_t0(base0);
    w->setT1(t1);
    w->setBsase_t1(base1);
    w->setWidth(p1 - p0);
    w->setTempo(tempo1);
    w->setPps((base1 - base0) / w->width());
    _warpMarkers.insert(index, w);
}

void SampleClip::removeWarpMarker(int index)
{
    if (index >= 0 && index < _warpMarkers.size()) {
        if (_warpMarkers.at(index))
            delete _warpMarkers.at(index);
        _warpMarkers.removeAt(index);
    }
}

int SampleClip::findWarpByTime(double time)
{
    QList<Warp*> list = warpMarkers();
    for (int i = 0; i < list.size(); i++) {
        if (list[i]->t0() < time && list[i]->t1() > time) {
            return i;
        }
    }
    return -1;
}
