#include "samplebuffer.h"
#include "audioengine.h"
#include "engine.h"
#include "ipp.h"
#include "sampleblock.h"
#include "sampleclip.h"
#include "sampleframe.h"
#include "sndfile.h"
#include "song.h"
#include "sox.h"
#include "soxr.h"
#include <QDir>
#include <QFileInfo>
#include <QFuture>
#include <qtconcurrentrun.h>

size_t SampleBuffer::sMaxDiskBlockSize = 1048576;

SampleBuffer::SampleBuffer(bool isFake)
    : m_audioFile("")
    , m_origFrames(0)
    , m_frames(0)
    , m_startFrame(0)
    , m_endFrame(0)
    , m_loopStartFrame(0)
    , m_loopEndFrame(0)
    , m_amplification(1.0f)
    , m_reversed(false)
    , m_frequency(DefaultBaseFreq)
    , m_sampleRate(audioEngineSampleRate())
    , m_minSamples(sMaxDiskBlockSize / sizeof(float) / 2)
    , m_maxSamples(m_minSamples * 2)
    , m_fake(isFake)

{

    _bufferFill = true;

    connect(AudioManager::getSong(), SIGNAL(stopped()),
        this, SLOT(sltStopped()), Qt::DirectConnection);

    update();
}

SampleBuffer::SampleBuffer(const QString& audioFile, bool isFake, bool isBase64Data)
    : m_origFrames(0)
    , m_frames(0)
    , m_startFrame(0)
    , m_endFrame(0)
    , m_loopStartFrame(0)
    , m_loopEndFrame(0)
    , m_amplification(1.0f)
    , m_reversed(false)
    , m_frequency(DefaultBaseFreq)
    , m_sampleRate(audioEngineSampleRate())
    , m_minSamples(sMaxDiskBlockSize / sizeof(float) / 2)
    , m_maxSamples(m_minSamples * 2)
    , m_fake(isFake)
{

    m_audioFile = audioFile;

    fillDataBuffers(m_audioFile);

    update();
}

SampleBuffer::SampleBuffer(const SampleBuffer& orig, bool isFake)
{

    m_fake = isFake;
    orig.m_varLock.lockForRead();

    _bufferFill = true;
    m_audioFile = orig.m_audioFile;
    m_origFrames = orig.m_origFrames;
    m_channels = orig.m_channels;

    m_frames = orig.m_frames;
    m_startFrame = orig.m_startFrame;
    m_endFrame = orig.m_endFrame;
    m_loopStartFrame = orig.m_loopStartFrame;
    m_loopEndFrame = orig.m_loopEndFrame;
    m_amplification = orig.m_amplification;
    m_reversed = orig.m_reversed;
    m_frequency = orig.m_frequency;
    m_sampleRate = orig.m_sampleRate;
    m_maxSamples = orig.m_maxSamples;
    m_audioFile = orig.m_audioFile;
    m_offsetStartFrame = orig.m_offsetStartFrame;
    m_offsetEndFrame = orig.m_offsetEndFrame;
    m_baseStartFrame = orig.m_baseStartFrame;
    m_baseEndFrame = orig.m_baseEndFrame;

    fillDataBuffers(m_audioFile);

    sltPositionChanged(baseStartFrame());

    pBlock = orig.pBlock->clone();

    orig.m_varLock.unlock();
}

SampleBuffer::SampleBuffer(const f_cnt_t frames, const f_cnt_t channels)
{

    if (frames > 0) {

        m_origFrames = frames;
        update();
    }
}

fpp_t SampleBuffer::resample(int channel, sampleFrame* _src, const fpp_t _frames, sampleFrame* _dst, sample_rate_t _src_sr, sample_rate_t _dst_sr, int offset)
{
    size_t odone = _frames;
    if (_src->getReadPointer(channel)) {

        size_t outSize = (_frames * _dst_sr / (double)_src_sr + 1);

        soxr_error_t error = soxr_process(m_soxr[channel],
            (void*)_src->getReadPointer(channel), _frames, NULL,
            (void*)_dst->getWritePointer(channel), outSize, &odone);
    }

    return odone;
}

fpp_t SampleBuffer::resample_flush(int channel, const fpp_t _frames, sampleFrame* _dst)
{

    size_t odone = _frames;

    size_t outSize = _frames;

    soxr_error_t error = soxr_process(m_soxr[channel],
        NULL, 0, NULL,
        (void*)_dst->getWritePointer(channel), outSize, &odone);

    return odone;
}

f_cnt_t SampleBuffer::decodeSampleSF(QString fileName)
{

    SNDFILE* sndFile;
    SF_INFO sfInfo;
    sfInfo.format = 0;

    QDir fileDir = QFileInfo(fileName).dir();

    QFileInfo tempFile(fileDir, QFileInfo(fileName).baseName() + ".qdio");
    QString filename = tempFile.absoluteFilePath();

    QFile file_summery(filename);

    sndFile = sf_open(fileName.toLatin1().data(), SFM_READ, &sfInfo);
    size_t block = 0;
    auto maxBlock = m_maxSamples;
    BlockBuffer buffer;

    m_sampleRate = sfInfo.samplerate;
    f_cnt_t frames = (sfInfo.frames * AudioManager::audioEngine()->outputSampleRate() / (double)sfInfo.samplerate) + 32;

    pBlock = std::make_shared<SampleBlock>(&file_summery);

    if (file_summery.exists()) {

        file_summery.open(QFile::ReadOnly);

        m_summeyArray = file_summery.readAll();

        QBuffer buffer_(&m_summeyArray);
        buffer_.open(QBuffer::ReadWrite);
        bool result = pBlock->loadSample(&buffer_);
        buffer_.close();
        file_summery.close();

        if (result) {
            sf_close(sndFile);
            return sfInfo.frames;
        }
    }

    file_summery.open(QFile::ReadWrite);

    pBlock->initSamples(sfInfo.frames, sfInfo.channels);
    do {

        block = maxBlock;

        float* samples_input = (float*)malloc(block * sfInfo.channels * sizeof(float));

        buffer.Allocate(block);

        block = sf_readf_float(sndFile, samples_input, block);
        if (block > maxBlock * sfInfo.channels) {
            block = maxBlock * sfInfo.channels;
        }

        if (block) {
            for (int c = 0; c < sfInfo.channels; c++) {
                for (ulong j = 0; j < block; j++) {
                    ((float*)buffer.ptr())[j] = samples_input[sfInfo.channels * j + c];
                }

                pBlock->addSample(buffer.ptr(), block, c);
            }
        }
        delete[] samples_input;

    } while (block > 0);

    file_summery.flush();
    file_summery.seek(0);
    m_summeyArray = file_summery.read(file_summery.size());
    QBuffer buffer_(&m_summeyArray);
    buffer_.open(QBuffer::ReadWrite);
    pBlock->loadSample(&buffer_);
    buffer_.close();

    // SampleBlock::setBlockCount(SampleBlock::m_blockCounter - 1, &file_summery);

    sf_close(sndFile);
    file_summery.close();

    return frames;
}

void SampleBuffer::fillDataBuffers(QString fileName)
{
    SNDFILE* sndFile;
    SF_INFO sfInfo;
    sfInfo.format = 0;
    f_cnt_t frames = 0;
    sf_count_t sfItemsRead;

    // Use QFile to handle unicode file names on Windows
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly) && (sndFile = sf_open(fileName.toLatin1().data(), SFM_READ, &sfInfo))) {
        frames = sfInfo.frames;

        m_sampleRate = sfInfo.samplerate;
        auto base_frame = (MINIMUM_FRAME_COUNT * AudioManager::audioEngine()->outputSampleRate() / (double)sfInfo.samplerate) + 32;
        size_t oexpframes = base_frame;
        frame_count = oexpframes;

        sf_close(sndFile);
        f.close();
    }
}

double SampleBuffer::fillWhere(double startX, double endX, double t0, double t1,
    double s0, double sampleRate)
{

    double s = s0;
    double x = 0;
    where[0] = (std::max(0.0, floor(s0)));
    double spp = sampleRate * ((t1 - t0) / 1000.0) / (endX - startX);

    for (x = 1; x <= endX; x++) {
        s += spp;
        where[x] = s;
    }

    return x;
}

void SampleBuffer::init_sox(int samplerate, int channels)
{

    for (int i = 0; i < m_soxr.size(); i++) {
        if (m_soxr[i])
            soxr_delete(m_soxr[i]);
    }

    soxr_error_t error;
    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_FLOAT32_I, SOXR_FLOAT32_I);
    soxr_quality_spec_t q_spec = soxr_quality_spec(SOXR_HQ, 0);
    soxr_runtime_spec_t runtime_spec = soxr_runtime_spec(1);

    m_soxr.resize(channels);
    for (int i = 0; i < channels; i++) {
        auto sox = soxr_create(samplerate, AudioManager::audioEngine()->outputSampleRate(), 1,
            &error, &io_spec, &q_spec, &runtime_spec);

        m_soxr[i] = sox;
    }

    if (error) {
        qDebug() << "SOXR right init error";
    }
}

SampleBuffer::~SampleBuffer()
{

    if (async_writer) {
        async_writer->quit();
        delete async_writer;
        async_writer = Q_NULLPTR;
    }

    if (_workerThread) {

        _workerThread->quit();
        delete _workerThread;
        _workerThread = Q_NULLPTR;
    }

    pBlock.reset();

    for (int i = 0; i < m_data_.size(); i++) {
        if (m_data_[i]) {
            delete m_data_[i];
            m_data_[i] = Q_NULLPTR;
        }
    }
}

bool SampleBuffer::play(sampleFrame* ab, handleState* state, const fpp_t frames, const float freq, const LoopMode loopMode)
{

    f_cnt_t startFrame = m_startFrame;
    f_cnt_t endFrame = m_endFrame;

    f_cnt_t loopStartFrame = m_loopStartFrame;
    f_cnt_t loopEndFrame = m_loopEndFrame;

    if (endFrame == 0 || frames == 0) {
        return false;
    }

    // variable for determining if we should currently be playing backwards in a ping-pong loop
    bool isBackwards = state->isBackwards();

    // The SampleBuffer can play a given sample with increased or decreased pitch. However, only
    // samples that contain a tone that matches the default base note frequency of 440 Hz will
    // produce the exact requested pitch in [Hz].
    const double freqFactor = (double)freq / (double)m_frequency * m_sampleRate / AudioManager::audioEngine()->processingSampleRate();

    // calculate how many frames we have in requested pitch
    const auto totalFramesForCurrentPitch = static_cast<f_cnt_t>((endFrame - startFrame) / freqFactor);

    if (totalFramesForCurrentPitch == 0) {
        return false;
    }

    // this holds the index of the first frame to play
    f_cnt_t playFrame = qMax(state->m_frameIndex, startFrame);

    if (loopMode == LoopOff) {
        if (playFrame >= endFrame || (endFrame - playFrame) / freqFactor == 0) {
            return false;
        }
    } else if (loopMode == LoopOn) {
        playFrame = getLoopedIndex(playFrame, loopStartFrame, loopEndFrame);
    } else {
        playFrame = getPingPongIndex(playFrame, loopStartFrame, loopEndFrame);
    }

    sampleFrame* tmp = nullptr;

    if (m_current_data_index < 0) {
        return false;
    }

    if (m_current_data_index >= m_data_.size())
        return false;

    int offset = (m_current_frame - baseStartFrame() + offsetStartFrame());
    if (offset < 0) {
        offset = 0;
    }
    int playIndex = playFrame - state->renderedFrames() - offset;

    int next_data_index = (m_current_data_index + 1) % m_data_.size();

    if (m_data_[m_current_data_index]->getNumSamples() <= 1) {
        return false;
    }
    if (playIndex < 0) {
        playIndex = 0;
    }

    int count = ab->getNumChannels();
    if (m_data_[m_current_data_index]->getNumChannels() < count)
        count = m_data_[m_current_data_index]->getNumChannels();

    for (int i = 0; i < count; i++) {
        float* p = getSampleFragment(i, playFrame, playIndex, frames, loopMode, m_data_[m_current_data_index], m_data_[next_data_index], &tmp, &isBackwards,
            loopStartFrame, loopEndFrame, endFrame);

        if (tmp != nullptr) {

            memcpy(ab->getWritePointer(i), tmp->getReadPointer(i), (size_t)(tmp->getNumSamples() * sizeof(float)));
            delete tmp;

        } else {
            memcpy(ab->getWritePointer(i), p, (size_t)(frames * sizeof(float)));
        }
    }

    if (playIndex + frames >= m_data_[m_current_data_index]->getNumSamples()) {

        state->setRenderedFrames(state->renderedFrames() + m_data_[m_current_data_index]->getNumSamples());
        m_current_data_index += 1;
        if (m_current_data_index >= m_data_.size())
            m_current_data_index = 0;

        bufferNotEmpty.notify_one();
    }

    // Advance
    switch (loopMode) {
    case LoopOff:
        playFrame += frames;
        break;
    }

    state->setBackwards(isBackwards);
    state->setFrameIndex(playFrame);

    return true;
}

float* SampleBuffer::getSampleFragment(
    int channel,
    f_cnt_t playBuffer,
    f_cnt_t playIndex,
    f_cnt_t frames,
    LoopMode loopMode,
    sampleFrame* _data,
    sampleFrame* _nextData,
    sampleFrame** tmp,
    bool* backwards,
    f_cnt_t loopStart,
    f_cnt_t loopEnd,
    f_cnt_t end) const
{

    if (loopMode == LoopOff) {

        if (playBuffer + frames <= end) {
            if (playIndex + frames <= _data->getNumSamples()) {

                return _data->getWritePointer(channel) + playIndex;
            } else {

                *tmp = new sampleFrame(_data->getNumChannels(), frames);
                f_cnt_t available = (_data->getNumSamples() - playIndex);

                memcpy((*tmp)->getWritePointer(channel), _data->getReadPointer(channel) + playIndex, (size_t)(available * sizeof(float)));

                memcpy((*tmp)->getWritePointer(channel) + (available), _nextData->getReadPointer(channel), (frames - available) * (sizeof(float)));

                return (*tmp)->getWritePointer(channel);
            }
        }
    }

    *tmp = new sampleFrame(_data->getNumChannels(), frames);
    if (loopMode == LoopOff) {

        f_cnt_t available = (end - playBuffer);

        memcpy((*tmp)->getWritePointer(channel), _data->getReadPointer(channel) + playIndex, (size_t)(available * sizeof(float)));
        memset((*tmp)->getWritePointer(channel) + (available), 0, (frames - available) * (sizeof(float)));

        //        QString str = "";

        //        if (channel == 1)
        //            for (int k = 0; k < (*tmp)->getNumSamples(); k++) {
        //                str += QString::number((*tmp)->getSample(1, k)) + " ";
        //            }

        //        qDebug() << "p2" << str;
    }

    return (*tmp)->getWritePointer(channel);
}

void SampleBuffer::visualize(IPainter& painter, const QRect& dr, int channelNumber, double startTime, double endTime, float* max, float* min, int size)
{
    if (m_frames == 0 && size <= 0) {
        return;
    }

    if (m_channels < channelNumber) {
        return;
    }

    qint64 pixel0 = dr.left();
    qint64 pixel1 = dr.right();

    const int w = dr.width();
    const int h = dr.height();

    const float ySpace = h * 0.5f;
    const int yb = h / 2 + dr.y();
    QVector<QPointF> fEdgeMin;
    QVector<QPointF> fEdgeMax;
    QVector<QPointF> fPoints;
    const qint64 xb = dr.x();
    qint64 curPixel = 0;
    auto py = ySpace * m_amplification;

    double fromFrame = sampleRate() * (startTime / 1000.0);
    double toFrame = sampleRate() * (endTime / 1000.0);

    const double fpp = static_cast<double>(toFrame - fromFrame) / (double)(w);

    if (fpp >= 1) {

        int currentPix = pixel0;

        if (currentPix < 0)
            currentPix = 0;

        for (qint64 x = 0; x < size; x++) {
            fEdgeMin.append(QPointF(currentPix, (yb - (min[x] * py))));
            fEdgeMax.append(QPointF(currentPix, (yb - (max[x] * py))));

            currentPix++;
        }

        painter.beginPath();
        for (int j = 0; j < fEdgeMin.size(); ++j) {
            if (j == 0) {
                painter.moveTo(fEdgeMin[j].x(), fEdgeMin[j].y());
            } else {

                painter.lineTo(fEdgeMin[j].x(), fEdgeMin[j].y());
            }
        }

        for (int j = 0; j < fEdgeMin.size(); ++j) {
            if (j == 0) {
                painter.moveTo(fEdgeMax[j].x(), fEdgeMax[j].y());
            } else {
                painter.lineTo(fEdgeMax[j].x(), fEdgeMax[j].y());
            }
        }

        for (int j = 0; j < fEdgeMin.size(); ++j) {
            painter.moveTo(fEdgeMin[j].x(), fEdgeMin[j].y());
            painter.lineTo(fEdgeMax[j].x(), fEdgeMax[j].y());
        }
        painter.stroke();
    } else {

        double w = pixel1 - pixel0;
        double duration = endTime - startTime;

        painter.beginPath();
        bool first_round = true;
        for (int j = fromFrame; j < toFrame; j++) {
            double k = j * 1000.0 / (double)sampleRate();
            double x = k - startTime;
            QPointF point = QPointF(x / (double)duration * w + pixel0, (yb - (min[j - (int)fromFrame] * py)));

            if (first_round) {
                first_round = false;
                painter.moveTo(point);
            } else {
                painter.lineTo(point);
            }
        }
        painter.stroke();
    }
}

void SampleBuffer::visualize2(const QRect& dr, int channelNumber, double startTime, double endTime, float* max, float* min, int size,
    QVector<QPointF>& minPoints, QVector<QPointF>& maxPoints, QVector<QPointF>& points)
{

    if (m_frames == 0 && size <= 0) {
        return;
    }

    if (m_channels < channelNumber) {
        return;
    }

    qint64 pixel0 = dr.left();
    qint64 pixel1 = dr.right();

    const int w = dr.width();
    const int h = dr.height();

    const float ySpace = h * 0.5f;
    const int yb = h / 2 + dr.y();
    QVector<QPointF> fEdgeMin;
    QVector<QPointF> fEdgeMax;
    QVector<QPointF> fPoints;
    const qint64 xb = dr.x();
    qint64 curPixel = 0;
    auto py = ySpace * m_amplification;

    double fromFrame = sampleRate() * (startTime / 1000.0);
    double toFrame = sampleRate() * (endTime / 1000.0);

    const double fpp = static_cast<double>(toFrame - fromFrame) / (double)(w);

    if (fpp >= 1) {

        int currentPix = pixel0;

        if (currentPix < 0)
            currentPix = 0;

        for (qint64 x = 0; x < size; x++) {
            fEdgeMin.append(QPointF(currentPix, (yb - (min[x] * py))));
            fEdgeMax.append(QPointF(currentPix, (yb - (max[x] * py))));

            currentPix++;
        }

        for (int j = 0; j < fEdgeMin.size(); ++j) {
            minPoints.append(QPointF(fEdgeMin[j].x(), fEdgeMin[j].y()));
        }

        for (int j = 0; j < fEdgeMin.size(); ++j) {
            maxPoints.append(QPointF(fEdgeMax[j].x(), fEdgeMax[j].y()));
        }

        for (int j = 0; j < fEdgeMin.size(); ++j) {
            points.append(QPointF(fEdgeMin[j].x(), fEdgeMin[j].y()));
            points.append(QPointF(fEdgeMax[j].x(), fEdgeMax[j].y()));
        }

    } else {

        double w = pixel1 - pixel0;
        double duration = endTime - startTime;

        for (int j = fromFrame; j < toFrame; j++) {
            double k = j * 1000.0 / (double)sampleRate();
            double x = k - startTime;
            QPointF point = QPointF(x / (double)duration * w + pixel0, (yb - (min[j - (int)fromFrame] * py)));

            minPoints.append(point);
        }
    }
}

void SampleBuffer::get(const QRectF& dr, int witdh, int channelNumber, double startTime, double endTime, float* arr_max, float* arr_min)
{

    if (m_frames == 0 || m_channels < channelNumber) {
        return;
    }

    double p0 = dr.x();
    double p1 = dr.width() + dr.x();

    if (p1 > witdh)
        p1 = witdh;

    QString file = m_audioFile;

    SNDFILE* sndFile = Q_NULLPTR;
    QElapsedTimer timer;
    timer.start();
    QBuffer buffer(&m_summeyArray);
    buffer.open(QBuffer::ReadWrite);

    SF_INFO sfInfo;
    sndFile = sf_open(file.toLatin1().data(), SFM_READ, &sfInfo);

    double fromFrame = sampleRate() * (startTime / 1000.0);
    double toFrame = sampleRate() * (endTime / 1000.0);

    fillWhere(p0, p1, startTime, endTime, fromFrame, sampleRate());

    double width = p1 - p0;

    qint64 numSamples = ((endTime - startTime) / 1000.0) * sampleRate();

    const double pps = (numSamples) / (double)width;

    double preWhere = 0;

    const int divisor = (pps >= 65536 * 1) ? 65536
        : (pps >= 256 * 1)                 ? 256
                                           : 1;

    int startPosition = qCeil(fromFrame / (double)divisor);
    int endPosition = qFloor(toFrame / (double)divisor);

    if (divisor > 1)
        startPosition = qCeil(fromFrame / (double)divisor);
    else {
        startPosition = fromFrame;
    }

    if (divisor > 1)
        endPosition = qFloor(toFrame / (double)divisor);
    else {
        endPosition = toFrame;
    }

    int num = endPosition - startPosition + 1;

    QVector<float> temp;

    int pixel0 = qRound(p0);
    int pixel1 = qRound(p1);

    switch (divisor) {
    case 1:
        temp.resize(num);
        SampleBlock::getSamples(temp.data(), channelNumber, m_channels, startPosition, num, 0, sndFile);
    }

    if (numSamples > width) {
        if (divisor > 1) {

            double p1 = 0, p2 = 0, p3 = 0, p4 = 0;
            int i = 0;
            int size = 0;
            int offset = 0;
            int last = i;

            for (int pixel = pixel0; pixel < pixel1;) {

                if (where[pixel] == 0.0) {
                    p1 = 0;
                    p2 = 0;

                    preWhere = where[pixel - pixel0];
                    pixel++;
                    size = qFloor((where[pixel - pixel0] - preWhere) / (double)divisor);
                    last += size;

                    p4 = where[pixel - pixel0];
                    p3 = (last + startPosition) * divisor;
                    size = last - i;
                    offset = i;
                    i = last + 1;
                    last = i;

                } else {

                    p1 = where[pixel - pixel0];
                    p2 = (i + startPosition) * divisor;
                    preWhere = p2;
                    pixel++;
                    size = qFloor((where[pixel - pixel0] - preWhere) / (double)divisor);
                    last += size;

                    p4 = where[pixel - pixel0];
                    p3 = (last + startPosition) * divisor;
                    size = last - i + 1;
                    offset = i;
                    i = last + 1;
                    last = i;
                }

                float* pvMin;
                float* pvMax;

                int len = 0;

                if (divisor == 256) {
                    pvMin = pBlock->getSumMin256(channelNumber, len) + startPosition + offset;
                    pvMax = pBlock->getSumMax256(channelNumber, len) + startPosition + offset;
                }

                if (divisor == 65536) {
                    pvMin = pBlock->getSumMin64k(channelNumber, len) + startPosition + offset;
                    pvMax = pBlock->getSumMax64k(channelNumber, len) + startPosition + offset;
                }

                if (size + startPosition + offset < len && pixel <= pixel1) {
                    Ipp32f min;
                    ippsMin_32f(pvMin, size, &min);

                    Ipp32f max;
                    ippsMax_32f(pvMax, size, &max);

                    arr_min[(pixel - 1) - pixel0] = min;
                    arr_max[(pixel - 1) - pixel0] = max;
                }
            }

        } else {

            double i = 0;
            double size = pps;
            int pixel = pixel0;
            while (pixel < pixel1) {

                const float* const pv = temp.data() + qRound(i);

                Ipp32f min;
                ippsMin_32f(pv, qRound(size), &min);

                Ipp32f max;
                ippsMax_32f(pv, qRound(size), &max);

                arr_min[pixel - pixel0] = min;
                arr_max[pixel - pixel0] = max;

                i += pps;
                pixel++;
            }
        }

    } else {

        SampleBlock::getSamples(arr_min, channelNumber, m_channels, startPosition, numSamples + 1, 0, sndFile);
        SampleBlock::getSamples(arr_max, channelNumber, m_channels, startPosition, numSamples + 1, 0, sndFile);
    }

    sf_close(sndFile);
    buffer.close();
}

void SampleBuffer::sampleRateChanged()
{
    update(true);
}

void SampleBuffer::sltStopped()
{

    //    async_writer.reset();
    //    async_writer.set(0);
    //    async_writer.start();
    //    FillFirstBuffer* workerThread = new FillFirstBuffer();
    //    workerThread->init(m_audioFile, 0, &m_data_, this);
    //    connect(workerThread, &FillFirstBuffer::finished, workerThread, &QObject::deleteLater);
    //    workerThread->start();
}

void SampleBuffer::sltPositionChanged(quint64 frame, QString clipItem, bool toPlay)
{

    if (m_fake == true) {
        return;
    }

    m_current_frame = frame;

    if (_workerThread) {
        _workerThread->quit();
        delete _workerThread;
        _workerThread = Q_NULLPTR;
    }

    if (async_writer) {
        async_writer->quit();
        delete async_writer;
        async_writer = Q_NULLPTR;
    }
    async_writer = new AsyncFileReader(m_audioFile, this);
    async_writer->setFrame(frame);

    _workerThread = new FillFirstBuffer(m_audioFile, frame, this, toPlay);
    connect(_workerThread, &FillFirstBuffer::finished, AudioManager::getSong(), &Song::positionMarkerMoved);
    connect(_workerThread, &FillFirstBuffer::sigFinished, this, &SampleBuffer::sltFillFirstBufferChanged);
    connect(_workerThread, &FillFirstBuffer::sigFinished, this, &SampleBuffer::sltFillFirsFinished);
    _workerThread->start();
}
void SampleBuffer::sltFillFirstBufferChanged(QString clipItem, bool toPlay)
{
    Q_EMIT sigFirstBufferFilled(clipItem, toPlay);
}

void SampleBuffer::sltFillFirsFinished()
{

    if (async_writer)
        async_writer->start();
}

void SampleBuffer::sltPositionChanged(quint64 frame)
{

    if (m_fake == true) {
        return;
    }

    m_current_frame = frame;

    if (_workerThread) {
        _workerThread->quit();
        delete _workerThread;
        _workerThread = Q_NULLPTR;
    }

    if (async_writer) {
        async_writer->quit();
        delete async_writer;
        async_writer = Q_NULLPTR;
    }

    async_writer = new AsyncFileReader(m_audioFile, this);
    async_writer->setFrame(frame);

    _workerThread = new FillFirstBuffer(m_audioFile, frame, this, false);
    connect(_workerThread, &FillFirstBuffer::sigFinished, this, &SampleBuffer::sltFillFirsFinished);
    _workerThread->start();
}

sample_rate_t SampleBuffer::audioEngineSampleRate()
{
    return AudioManager::audioEngine()->processingSampleRate();
}

void SampleBuffer::update(bool keepSettings)
{
    const bool lock = true;
    if (lock) {
        AudioManager::audioEngine()->requestChangeInModel();
        m_varLock.lockForWrite();
    }

    // File size and sample length limits
    const quint64 fileSizeMax = 3000; // MB
    const quint64 sampleLengthMax = 900; // Minutes

    bool fileLoadError = false;
    if (m_audioFile.isEmpty() && m_origFrames > 0) {
        // TODO: reverse- and amplification-property is not covered
        // by following code...
        // m_data = new sampleFrame(m_channels, m_origFrames);
        // m_origData->copyTo(m_data);
        if (keepSettings == false) {
            m_frames = m_origFrames;
            m_loopStartFrame = m_startFrame = 0;
            m_loopEndFrame = m_endFrame = m_frames;
        }
    } else if (!m_audioFile.isEmpty()) {
        QString file = m_audioFile; // PathUtil::toAbsolute(m_audioFile);
        int_sample_t* buf = nullptr;
        sample_t* fbuf = nullptr;
        ch_cnt_t channels = DEFAULT_CHANNELS;
        sample_rate_t samplerate = audioEngineSampleRate();
        m_frames = 0;

        const QFileInfo fileInfo(file);

        if (fileInfo.size() > fileSizeMax * 1024 * 1024) {
            fileLoadError = true;
        } else {
            // Use QFile to handle unicode file names on Windows
            SNDFILE* sndFile;
            SF_INFO sfInfo;
            sfInfo.format = 0;
            if ((sndFile = sf_open(file.toLatin1().data(), SFM_READ, &sfInfo))) {
                f_cnt_t frames = sfInfo.frames;
                int rate = sfInfo.samplerate;
                m_sampleRate = rate;
                m_channels = sfInfo.channels;
                if (frames / rate > sampleLengthMax * 60) {
                    fileLoadError = true;
                }
                sf_close(sndFile);
            }
        }

        if (!fileLoadError) {
            if (m_frames == 0) {

                m_frames = decodeSampleSF(file);
            }
        }

        if (m_frames == 0 || fileLoadError) // if still no frames, bail
        {
            // sample couldn't be decoded, create buffer containing
            // one sample-frame
            m_frames = 1;
            m_loopStartFrame = m_startFrame = 0;
            m_loopEndFrame = m_endFrame = 1;
        } else // otherwise normalize sample rate
        {
            // normalizeSampleRate(samplerate, keepSettings);
        }
    } else {
        // neither an audio-file nor a buffer to copy from, so create
        // buffer containing one sample-frame
        m_frames = 1;
        m_loopStartFrame = m_startFrame = 0;
        m_loopEndFrame = m_endFrame = 1;
    }

    if (lock) {
        m_varLock.unlock();
        AudioManager::audioEngine()->doneChangeInModel();
    }

    emit sampleUpdated();

    if (fileLoadError) {
        QString title = tr("Fail to open file");
        QString message = tr("Audio files are limited to %1 MB "
                             "in size and %2 minutes of playing time")
                              .arg(fileSizeMax)
                              .arg(sampleLengthMax);
    }
}

f_cnt_t SampleBuffer::offsetEndFrame() const
{
    return m_offsetEndFrame;
}

void SampleBuffer::setOffsetEndFrame(const f_cnt_t& offsetEndFrame)
{

    //    if (offsetEndFrame > m_frames) {
    //        m_offsetEndFrame = m_frames;
    //    } else {
    //        m_offsetEndFrame = offsetEndFrame;
    //    }

    m_offsetEndFrame = offsetEndFrame;
}

QString SampleBuffer::clipIndex() const
{
    return m_clipIndex;
}

void SampleBuffer::setClipIndex(const QString& clipIndex)
{

    m_clipIndex = clipIndex;
}

f_cnt_t SampleBuffer::offsetStartFrame() const
{

    return m_offsetStartFrame;
}

void SampleBuffer::setOffsetStartFrame(const f_cnt_t& offsetStartFrame)
{

    if (offsetStartFrame < 0) {
        m_offsetStartFrame = 0;
    } else {
        m_offsetStartFrame = offsetStartFrame;
    }
}

f_cnt_t SampleBuffer::baseEndFrame() const
{
    return m_baseEndFrame;
}

void SampleBuffer::setBaseEndFrame(const f_cnt_t& baseEndFrame)
{
    m_baseEndFrame = baseEndFrame;
}

f_cnt_t SampleBuffer::baseStartFrame() const
{
    return m_baseStartFrame;
}

void SampleBuffer::setBaseStartFrame(const f_cnt_t& baseStartFrame)
{

    m_baseStartFrame = baseStartFrame;
}

f_cnt_t SampleBuffer::getLoopedIndex(f_cnt_t index, f_cnt_t startf, f_cnt_t endf) const
{
    if (index < endf) {
        return index;
    }
    return startf + (index - startf) % (endf - startf);
}

f_cnt_t SampleBuffer::getPingPongIndex(f_cnt_t index, f_cnt_t startf, f_cnt_t endf) const
{
    if (index < endf) {
        return index;
    }
    const f_cnt_t loopLen = endf - startf;
    const f_cnt_t loopPos = (index - endf) % (loopLen * 2);

    return (loopPos < loopLen)
        ? endf - loopPos
        : startf + (loopPos - loopLen);
}

QString SampleBuffer::audioFile()
{
    return m_audioFile;
}

void SampleBuffer::setAudioFile(const QString& audioFile)
{
    m_audioFile = audioFile;
    update();
}

void SampleBuffer::setStartFrame(const f_cnt_t s)
{
    m_startFrame = s;
}

void SampleBuffer::setEndFrame(const f_cnt_t e)
{
    m_endFrame = e;
}

void SampleBuffer::setAmplification(float a)
{
    m_amplification = a;
    emit sampleUpdated();
}

// void SampleBuffer::setReversed(bool on)
//{
//     AudioManager::audioEngine()->requestChangeInModel();
//     m_varLock.lockForWrite();
//     if (m_reversed != on) {
//         std::reverse(m_data, m_data + m_frames);
//     }
//     m_reversed = on;
//     m_varLock.unlock();
//     AudioManager::audioEngine()->doneChangeInModel();
//     emit sampleUpdated();
// }

SampleBuffer::handleState::handleState(bool varyingPitch)
    : m_frameIndex(0)
    , m_rendered_frames(0)
    , m_varyingPitch(varyingPitch)
    , m_isBackwards(false)
{
}

SampleBuffer::handleState::~handleState()
{
}

AsyncFileReader::AsyncFileReader(QString filename, SampleBuffer* sampleBuffer, QObject* parent)
    : QThread(parent)
    , _sampleBuffer(sampleBuffer)
    , m_fileName(filename)

{
    sndFile = sf_open(m_fileName.toLatin1().data(), SFM_READ, &sfInfo);
}

AsyncFileReader::~AsyncFileReader()
{
    sf_close(sndFile);
}

void AsyncFileReader::start()
{

    QThread::start();
}

void AsyncFileReader::quit()
{
    m_quit = true;

    if (_sampleBuffer)
        _sampleBuffer->bufferNotEmpty.notify_one();

    wait();
}

void AsyncFileReader::setFrame(f_cnt_t currentFrame)
{
    auto frame_offset = currentFrame - _sampleBuffer->baseStartFrame() + _sampleBuffer->offsetStartFrame();

    double r = frame_offset / (double)AudioManager::audioEngine()->outputSampleRate();
    m_nextFrame = r * sfInfo.samplerate;

    _sampleBuffer->m_current_data_index = 0;

    m_nextFrame += 5 * MINIMUM_FRAME_COUNT;
}
void AsyncFileReader::flush(int index)
{

    if (_sampleBuffer->m_data_[index]) {
        delete _sampleBuffer->m_data_[index];
        _sampleBuffer->m_data_[index] = Q_NULLPTR;
    }

    double base_frame = (MINIMUM_FRAME_COUNT * AudioManager::audioEngine()->outputSampleRate() / (double)sfInfo.samplerate) + 32;

    auto m_realFrames = base_frame;
    auto tmp = new sampleFrame(sfInfo.channels, base_frame);

    for (int k = 0; k < tmp->getNumChannels(); k++) {
        m_realFrames = _sampleBuffer->resample_flush(k, base_frame, tmp);
    }
    _sampleBuffer->m_data_[index] = new sampleFrame(sfInfo.channels, m_realFrames);

    for (int k = 0; k < tmp->getNumChannels(); k++) {
        _sampleBuffer->m_data_[index]->copyFrom(k, 0, tmp->getReadPointer(k), m_realFrames);
    }
    delete tmp;
}
void AsyncFileReader::run()
{

    while (!m_quit) {

        std::unique_lock<std::mutex> lock(_sampleBuffer->mtx);

        if (_sampleBuffer == Q_NULLPTR) {
            continue;
        }

        _sampleBuffer->bufferNotEmpty.wait(lock);

        if (m_quit == true) {
            lock.unlock();
            break;
        }

        if (_sampleBuffer == Q_NULLPTR) {
            continue;
        }

        int index = _sampleBuffer->m_current_data_index - 1;
        if (index < 0)
            index = _sampleBuffer->m_data_.size() - 1;

        if (_sampleBuffer->m_data_.size() < 5) {
            continue;
        }

        auto read = sf_seek(sndFile, m_nextFrame, SEEK_SET);

        if (read < 0) {

            qDebug() << "Bug33";
            flush(index);
            return;
        }
        int channels = sfInfo.channels;
        if (channels <= 2) {
            channels = 2;
        }

        float* audioIn = new float[channels * MINIMUM_FRAME_COUNT];
        auto sfItemsRead = sf_read_float(sndFile, audioIn, channels * MINIMUM_FRAME_COUNT);

        if (sfItemsRead == 0) {

            flush(index);
            return;
        }

        auto sfFramesRead = sfItemsRead / sfInfo.channels;

        auto m_tmp = new sampleFrame(channels, sfFramesRead);

        if (m_quit == true) {
            delete m_tmp;
            delete[] audioIn;
            return;
        }

        for (int i = 0; i < sfFramesRead; i++) {

            if (sfInfo.channels <= 1) {
                m_tmp->setSample(0, i, audioIn[i]);
                m_tmp->setSample(1, i, audioIn[i]);
            } else {
                for (int j = 0; j < sfInfo.channels; j++)
                    m_tmp->setSample(j, i, audioIn[i * sfInfo.channels + j]);
            }
        }

        delete[] audioIn;

        if (m_quit == true) {
            delete m_tmp;
            return;
        }

        double oexpframes = (MINIMUM_FRAME_COUNT * AudioManager::audioEngine()->outputSampleRate() / (double)sfInfo.samplerate) + 32;

        if (AudioManager::audioEngine()->outputSampleRate() != sfInfo.samplerate) {

            if (_sampleBuffer->m_data_[index]) {
                delete _sampleBuffer->m_data_[index];
                _sampleBuffer->m_data_[index] = Q_NULLPTR;
            }

            auto m_realFrames = oexpframes;
            auto tmp = new sampleFrame(channels, oexpframes);
            for (int j = 0; j < m_tmp->getNumChannels(); j++) {
                m_realFrames = _sampleBuffer->resample(j, m_tmp, sfFramesRead, tmp, sfInfo.samplerate, AudioManager::audioEngine()->outputSampleRate());
            }

            _sampleBuffer->m_data_[index] = new sampleFrame(channels, m_realFrames);

            for (int k = 0; k < tmp->getNumChannels(); k++)
                _sampleBuffer->m_data_[index]->copyFrom(k, 0, tmp->getReadPointer(k), m_realFrames);

            delete tmp;

        } else {

            if (_sampleBuffer->m_data_[index]) {
                delete _sampleBuffer->m_data_[index];
                _sampleBuffer->m_data_[index] = Q_NULLPTR;
            }

            _sampleBuffer->m_data_[index] = new sampleFrame(channels, sfFramesRead);

            for (int i = 0; i < _sampleBuffer->m_data_[index]->getNumChannels(); i++)
                _sampleBuffer->m_data_[index]->copyFrom(i, 0, m_tmp->getReadPointer(i), m_tmp->getNumSamples());
        }

        //        qDebug() << "READ" << index << _sampleBuffer->m_data_[index]->getNumSamples();

        //        QString str = "";
        //        if (sfFramesRead < MINIMUM_FRAME_COUNT)
        //            for (int k = _sampleBuffer->m_data_[index]->getNumSamples() - 512; k < _sampleBuffer->m_data_[index]->getNumSamples(); k++) {
        //                str += QString::number(_sampleBuffer->m_data_[index]->getSample(1, k)) + " ";
        //            }
        //        qDebug() << "P1 " << str;

        if (m_tmp)
            delete m_tmp;

        m_nextFrame += sfFramesRead;
        lock.unlock();

        QThread::msleep(10);
    }
}

FillFirstBuffer::FillFirstBuffer(QString filename, int currentFrame, SampleBuffer* sampleBuffer, bool toPlay, QObject* parent)
    : QThread(parent)
    , _toPlay(toPlay)
    , m_currentFrame(currentFrame)
    , m_fileName(filename)
    , _sampleBuffer(sampleBuffer)

{
}

FillFirstBuffer::~FillFirstBuffer()
{
}
void FillFirstBuffer::quit()
{
    m_quit = true;
    wait();
}

void FillFirstBuffer::run()
{

    SNDFILE* sndFile;
    SF_INFO sfInfo;
    sfInfo.format = 0;
    f_cnt_t frames = 0;
    sf_count_t sfItemsRead;

    if (m_quit == true)
        return;

    if ((sndFile = sf_open(m_fileName.toLatin1().data(), SFM_READ, &sfInfo))) {
        frames = sfInfo.frames;

        int channels = sfInfo.channels;
        if (channels <= 1) {
            channels = 2;
        }

        int frame_offset = 0;
        double base_frame = (MINIMUM_FRAME_COUNT * AudioManager::audioEngine()->outputSampleRate() / (double)sfInfo.samplerate) + 32;
        size_t oexpframes = base_frame;

        if ((m_currentFrame - _sampleBuffer->baseStartFrame()) < 0) {
            frame_offset = _sampleBuffer->offsetStartFrame();
            if (frame_offset % MINIMUM_FRAME_COUNT == 0) {
                frame_offset -= 1;
            }

        } else {
            frame_offset = m_currentFrame - _sampleBuffer->baseStartFrame() + _sampleBuffer->offsetStartFrame();
            if (frame_offset % MINIMUM_FRAME_COUNT == 0) {
                frame_offset -= 1;
            }
        }

        if (frame_offset < 0) {
            frame_offset = 0;
        }

        _sampleBuffer->init_sox(sfInfo.samplerate, channels);

        for (int i = 0; i < _sampleBuffer->m_data_.size(); i++) {
            if (_sampleBuffer->m_data_[i])
                delete _sampleBuffer->m_data_[i];
        }
        double r = frame_offset / (double)AudioManager::audioEngine()->outputSampleRate();
        int64 start = r * sfInfo.samplerate;

        _sampleBuffer->m_data_.clear();
        for (int i = 0; i < 5; i++) {

            if (m_quit == true)
                return;

            sf_seek(sndFile, start, SEEK_SET);

            float* audioIn = new float[sfInfo.channels * MINIMUM_FRAME_COUNT];

            sfItemsRead = sf_read_float(sndFile, audioIn, sfInfo.channels * MINIMUM_FRAME_COUNT);

            start += (sfItemsRead / sfInfo.channels);

            if (sfItemsRead == 0) {
                continue;
            }

            auto sfFramesRead = sfItemsRead / sfInfo.channels;

            if (m_quit == true) {

                delete[] audioIn;
                return;
            }

            auto m_tmp = new sampleFrame(channels, sfFramesRead);

            for (int j = 0; j < sfFramesRead; j++) {

                if (sfInfo.channels <= 1) {
                    m_tmp->setSample(0, j, audioIn[j]);
                    m_tmp->setSample(1, j, audioIn[j]);
                } else {
                    for (int k = 0; k < sfInfo.channels; k++) {
                        m_tmp->setSample(k, j, audioIn[j * sfInfo.channels + k]);
                    }
                }
            }
            delete[] audioIn;

            if (m_quit == true) {
                delete m_tmp;
                return;
            }

            if (AudioManager::audioEngine()->outputSampleRate() != sfInfo.samplerate) {

                auto m_realFrames = oexpframes;
                auto tmp = new sampleFrame(m_tmp->getNumChannels(), oexpframes);

                for (int j = 0; j < m_tmp->getNumChannels(); j++) {
                    m_realFrames = _sampleBuffer->resample(j, m_tmp, sfFramesRead, tmp, sfInfo.samplerate,
                        AudioManager::audioEngine()->outputSampleRate());
                }

                _sampleBuffer->m_data_.append(new sampleFrame(m_tmp->getNumChannels(), m_realFrames));

                for (int k = 0; k < tmp->getNumChannels(); k++)
                    _sampleBuffer->m_data_[i]->copyFrom(k, 0, tmp->getReadPointer(k), m_realFrames);

                delete tmp;
            } else {

                _sampleBuffer->m_data_.append(new sampleFrame(m_tmp->getNumChannels(), sfFramesRead));

                for (int k = 0; k < _sampleBuffer->m_data_[i]->getNumChannels(); k++)
                    _sampleBuffer->m_data_[i]->copyFrom(k, 0, m_tmp->getReadPointer(k), m_tmp->getNumSamples());
            }

            delete m_tmp;

            if (sfItemsRead < MINIMUM_FRAME_COUNT * sfInfo.channels) {
                break;
            }

            // qDebug() << "S" << i << _sampleBuffer->m_data_[i]->getNumSamples();
        }
    }

    if (_sampleBuffer->m_data_.size() < 5 && _sampleBuffer->m_data_.size() > 0) {

        int index = _sampleBuffer->m_data_.size() - 1;
        auto samples = _sampleBuffer->m_data_[index]->getNumSamples();
        auto tmp2 = new sampleFrame(_sampleBuffer->m_data_[index]->getNumChannels(), samples);

        for (int j = 0; j < _sampleBuffer->m_data_[index]->getNumChannels(); j++) {
            tmp2->copyFrom(j, 0, _sampleBuffer->m_data_[index]->getReadPointer(j), samples);
        }

        int channels = _sampleBuffer->m_data_[index]->getNumChannels();
        if (_sampleBuffer->m_data_[index]) {
            delete _sampleBuffer->m_data_[index];
            _sampleBuffer->m_data_[index] = Q_NULLPTR;
        }

        double base_frame = (MINIMUM_FRAME_COUNT * AudioManager::audioEngine()->outputSampleRate() / (double)sfInfo.samplerate) + 32;

        auto m_realFrames = base_frame;
        auto tmp = new sampleFrame(channels, base_frame);

        for (int k = 0; k < tmp->getNumChannels(); k++) {
            m_realFrames = _sampleBuffer->resample_flush(k, base_frame, tmp);
        }
        _sampleBuffer->m_data_[index] = new sampleFrame(channels, samples + m_realFrames);

        for (int k = 0; k < tmp->getNumChannels(); k++) {
            _sampleBuffer->m_data_[index]->copyFrom(k, 0, tmp2->getReadPointer(k), samples);
            _sampleBuffer->m_data_[index]->copyFrom(k, samples, tmp->getReadPointer(k), m_realFrames);
        }
        delete tmp;
        delete tmp2;
    }
    if (m_quit == true)
        return;

    Q_EMIT sigFinished(_sampleBuffer->clipIndex(), _toPlay);
}
