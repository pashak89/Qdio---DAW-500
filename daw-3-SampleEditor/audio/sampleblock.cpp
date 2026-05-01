#include "sampleblock.h"

#include <QBuffer>
#include <QDataStream>

SampleBlock::SampleBlock(QFile* file)
{
    m_file = file;
}

SampleBlock::~SampleBlock()
{
    m_samples.reset();
    m_summary256.reset();
    m_summary64k.reset();
}

SampleBlock::SampleBlock(const SampleBlock& sb)
    : m_version(sb.m_version)
    , m_channelCount(sb.m_channelCount)
    , m_summerySize256(sb.m_summerySize256)
    , m_summerySize64(sb.m_summerySize64)
    , m_header(sb.m_header)
    , m_dataOffset(sb.m_dataOffset)

{

    for (int i = 0; i < m_channelCount; i++) {

        m_sumMin256.insert(i, QVector<float>(m_summerySize256 / 12));
        m_sumMax256.insert(i, QVector<float>(m_summerySize256 / 12));
        m_sumRms256.insert(i, QVector<float>(m_summerySize256 / 12));

        m_sumMin64k.insert(i, QVector<float>(m_summerySize64 / 12));
        m_sumMax64k.insert(i, QVector<float>(m_summerySize64 / 12));
        m_sumRms64k.insert(i, QVector<float>(m_summerySize64 / 12));
    }

    for (int i = 0; i < m_channelCount; i++) {

        m_sumMin256[i] = QVector<float>(sb.m_sumMin256[i]);
        m_sumMax256[i] = QVector<float>(sb.m_sumMax256[i]);
        m_sumRms256[i] = QVector<float>(sb.m_sumRms256[i]);
        m_sumMin64k[i] = QVector<float>(sb.m_sumMin64k[i]);
        m_sumMax64k[i] = QVector<float>(sb.m_sumMax64k[i]);
        m_sumRms64k[i] = QVector<float>(sb.m_sumRms64k[i]);
    }
}

void SampleBlock::addSample(constSamplePtr src, size_t numBlockSample, int channelNumber)
{
    auto sizes = setSizes(numBlockSample);
    m_samples.reinit(m_sampleBytes);
    memcpy(m_samples.get(), src, m_sampleBytes);

    calcSummary(sizes, channelNumber);

    commit(sizes, channelNumber);
}

void SampleBlock::initSamples(quint64 nSamples, int nChannels)
{
    m_sampleCount = nSamples;
    m_channelCount = nChannels;

    m_summerySize64 = (m_sampleCount + 65535) / 65536;
    m_summerySize256 = m_summerySize64 * 256;

    m_summerySize256 = m_summerySize256 * 3 * 4;
    m_summerySize64 = m_summerySize64 * 3 * 4;

    m_file->seek(0);

    QDataStream out(m_file);
    out.setVersion(QDataStream::Qt_5_12);

    out.writeRawData("QDIO", 4);
    out << m_version;
    out << m_channelCount;
    out << m_summerySize256;
    out << m_summerySize64;

    m_dataOffset = m_file->pos();

    m_file->flush();

    for (int i = 0; i < nChannels; i++) {
        m_summeryIndexMin64.insert(i, 0);
        m_summeryIndexMax64.insert(i, 0);
        m_summeryIndexRms64.insert(i, 0);
        m_summeryIndexMin256.insert(i, 0);
        m_summeryIndexMax256.insert(i, 0);
        m_summeryIndexRms256.insert(i, 0);
    }
}

bool SampleBlock::loadSample(QBuffer* file)
{

    if (file->size() < 28) {
        return false;
    }

    QDataStream out(file);
    out.setVersion(QDataStream::Qt_5_12);
    m_header.resize(4);
    out.readRawData(m_header.data(), 4);
    out >> m_version;
    out >> m_channelCount;
    out >> m_summerySize256;
    out >> m_summerySize64;

    if (m_header != "QDIO") {
        return false;
    }

    if (m_version < 2) {
        return false;
    }

    m_dataOffset = file->pos();

    ArrayOf<float> m_summary256;
    ArrayOf<float> m_summary64k;

    for (int i = 0; i < m_channelCount; i++) {

        m_sumMin256.insert(i, QVector<float>(m_summerySize256 / 12));
        m_sumMax256.insert(i, QVector<float>(m_summerySize256 / 12));
        m_sumRms256.insert(i, QVector<float>(m_summerySize256 / 12));

        m_sumMin64k.insert(i, QVector<float>(m_summerySize64 / 12));
        m_sumMax64k.insert(i, QVector<float>(m_summerySize64 / 12));
        m_sumRms64k.insert(i, QVector<float>(m_summerySize64 / 12));
    }

    for (int i = 0; i < m_channelCount; i++) {

        getSummary256(m_sumMin256[i].data(), i, 0, m_summerySize256 / 3, file);
        getSummary256(m_sumMax256[i].data(), i, m_summerySize256 / 3, m_summerySize256 / 3, file);
        getSummary256(m_sumRms256[i].data(), i, 2 * m_summerySize256 / 3, m_summerySize256 / 3, file);

        getSummary64k(m_sumMin64k[i].data(), i, 0, m_summerySize64 / 3, file);
        getSummary64k(m_sumMax64k[i].data(), i, m_summerySize64 / 3, m_summerySize64 / 3, file);
        getSummary64k(m_sumRms64k[i].data(), i, 2 * m_summerySize64 / 3, m_summerySize64 / 3, file);
    }

    return true;
}

auto SampleBlock::setSizes(size_t numsamples) -> Sizes
{
    m_blockSampleCount = numsamples;
    m_sampleBytes = m_blockSampleCount * sizeof(float);

    int frames64k = (m_blockSampleCount + 65535) / 65536;
    int frames256 = frames64k * 256;

    return { frames256 * bytesPerFrame, frames64k * bytesPerFrame };
}

quint64 SampleBlock::getSummerySize64() const
{
    return m_summerySize64;
}

float* SampleBlock::getSumMin256(int channel, int& size)
{
    size = m_sumMin256[channel].size();
    return m_sumMin256[channel].data();
}

float* SampleBlock::getSumMax256(int channel, int& size)
{
    size = m_sumMax256[channel].size();
    return m_sumMax256[channel].data();
}

float* SampleBlock::getSumRms256(int channel, int& size)
{
    size = m_sumRms256[channel].size();
    return m_sumRms256[channel].data();
}

float* SampleBlock::getSumMin64k(int channel, int& size)
{
    size = m_sumMin64k[channel].size();
    return m_sumMin64k[channel].data();
}

float* SampleBlock::getSumMax64k(int channel, int& size)
{
    size = m_sumMax64k[channel].size();
    return m_sumMax64k[channel].data();
}

float* SampleBlock::getSumRms64k(int channel, int& size)
{
    size = m_sumRms64k[channel].size();
    return m_sumRms64k[channel].data();
}

quint64 SampleBlock::getSummerySize256() const
{
    return m_summerySize256;
}

void SampleBlock::calcSummary(Sizes sizes, int channel)
{
    const auto m_summary256Bytes = sizes.first;
    const auto m_summary64kBytes = sizes.second;

    Floats samplebuffer;
    float* samples;

    samples = (float*)m_samples.get();

    m_summary256.reinit(m_summary256Bytes);
    m_summary64k.reinit(m_summary64kBytes);

    m_summaryMin256.resize(m_summary256Bytes / 3);
    m_summaryMax256.resize(m_summary256Bytes / 3);
    m_summaryRms256.resize(m_summary256Bytes / 3);

    m_summaryMin64k.resize(m_summary64kBytes / 3);
    m_summaryMax64k.resize(m_summary64kBytes / 3);
    m_summaryRms64k.resize(m_summary64kBytes / 3);

    float* summary256 = (float*)m_summary256.get();
    float* summary64k = (float*)m_summary64k.get();

    float min;
    float max;
    float sumsq;
    double totalSquares = 0.0;
    double fraction = 0.0;

    // Recalc 256 summaries
    int sumLen = (m_blockSampleCount + 255) / 256;
    int summaries = 256;

    for (int i = 0; i < sumLen; ++i) {
        min = samples[i * 256];
        max = samples[i * 256];
        sumsq = min * min;

        int jcount = 256;
        if (jcount > m_blockSampleCount - i * 256) {
            jcount = m_blockSampleCount - i * 256;
            fraction = 1.0 - (jcount / 256.0);
        }

        for (int j = 1; j < jcount; ++j) {
            float f1 = samples[i * 256 + j];
            sumsq += f1 * f1;

            if (f1 < min) {
                min = f1;
            } else if (f1 > max) {
                max = f1;
            }
        }

        totalSquares += sumsq;

        summary256[i * fields] = min;
        summary256[i * fields + 1] = max;
        // The rms is correct, but this may be for less than 256 samples in last loop.
        summary256[i * fields + 2] = (float)qSqrt(sumsq / jcount);

        m_summaryMin256[i] = min;
        m_summaryMax256[i] = max;
        m_summaryRms256[i] = (float)qSqrt(sumsq / jcount);

        //        /qDebug() << "m_summaryMin256" << i <<channel <<  m_summaryMin256[i] << m_summaryMax256[i];
    }

    for (int i = sumLen, frames256 = m_summary256Bytes / bytesPerFrame;
         i < frames256; ++i) {
        // filling in the remaining bits with non-harming/contributing values
        // rms values are not "non-harming", so keep count of them:
        summaries--;
        summary256[i * fields] = FLT_MAX; // min
        summary256[i * fields + 1] = -FLT_MAX; // max
        summary256[i * fields + 2] = 0.0f; // rms

        m_summaryMin256[i] = FLT_MAX;
        m_summaryMax256[i] = -FLT_MAX;
        m_summaryRms256[i] = 0.0f;
    }

    // Calculate now while we can do it accurately
    m_sumRms = qSqrt(totalSquares / m_blockSampleCount);

    // Recalc 64K summaries
    sumLen = (m_blockSampleCount + 65535) / 65536;

    for (int i = 0; i < sumLen; ++i) {
        min = summary256[3 * i * 256];
        max = summary256[3 * i * 256 + 1];
        sumsq = summary256[3 * i * 256 + 2];
        sumsq *= sumsq;

        for (int j = 1; j < 256; ++j) {
            // we can overflow the useful summary256 values here, but have put
            // non-harmful values in them
            if (summary256[3 * (i * 256 + j)] < min) {
                min = summary256[3 * (i * 256 + j)];
            }

            if (summary256[3 * (i * 256 + j) + 1] > max) {
                max = summary256[3 * (i * 256 + j) + 1];
            }

            float r1 = summary256[3 * (i * 256 + j) + 2];
            sumsq += r1 * r1;
        }

        double denom = (i < sumLen - 1) ? 256.0 : summaries - fraction;
        float rms = (float)sqrt(sumsq / denom);

        summary64k[i * fields] = min;
        summary64k[i * fields + 1] = max;
        summary64k[i * fields + 2] = rms;

        m_summaryMin64k[i] = min;
        m_summaryMax64k[i] = max;
        m_summaryRms64k[i] = rms;
    }

    for (int i = sumLen, frames64k = m_summary64kBytes / bytesPerFrame;
         i < frames64k; ++i) {
        // wxASSERT_MSG(false, wxT("Out of data for mSummaryInfo")); // Do we ever get here?

        summary64k[i * fields] = 0.0f; // probably should be FLT_MAX, need a test case
        summary64k[i * fields + 1] = 0.0f; // probably should be -FLT_MAX, need a test case
        summary64k[i * fields + 2] = 0.0f; // just padding

        m_summaryMin64k[i] = 0.0f;
        m_summaryMax64k[i] = 0.0f;
        m_summaryRms64k[i] = 0.0f;
    }

    // Recalc block-level summary (mRMS already calculated)
    min = summary64k[0];
    max = summary64k[1];

    for (int i = 1; i < sumLen; ++i) {
        if (summary64k[i * fields] < min) {
            min = summary64k[i * fields];
        }

        if (summary64k[i * fields + 1] > max) {
            max = summary64k[i * fields + 1];
        }
    }

    m_sumMin = min;
    m_sumMax = max;
}

void SampleBlock::commit(Sizes sizes, int channelNummber)
{
    const quint64 mSummary256Bytes = sizes.first;
    const quint64 mSummary64kBytes = sizes.second;

    m_file->seek(m_dataOffset + m_summeryIndexMin256[channelNummber] + m_summerySize256 * channelNummber);
    m_file->write((char*)m_summaryMin256.data(), mSummary256Bytes / 3);

    m_file->seek(m_dataOffset + m_summerySize256 / 3 + m_summeryIndexMax256[channelNummber] + m_summerySize256 * channelNummber);
    m_file->write((char*)m_summaryMax256.data(), mSummary256Bytes / 3);

    m_file->seek(m_dataOffset + 2 * m_summerySize256 / 3 + m_summeryIndexRms256[channelNummber] + m_summerySize256 * channelNummber);
    m_file->write((char*)m_summaryRms256.data(), mSummary256Bytes / 3);

    m_file->seek(m_dataOffset + m_summerySize256 * m_channelCount + m_summeryIndexMin64[channelNummber] + m_summerySize64 * channelNummber);
    m_file->write((char*)m_summaryMin64k.data(), mSummary64kBytes / 3);

    m_file->seek(m_dataOffset + m_summerySize64 / 3 + m_summerySize256 * m_channelCount + m_summeryIndexMax64[channelNummber] + m_summerySize64 * channelNummber);
    m_file->write((char*)m_summaryMax64k.data(), mSummary64kBytes / 3);

    m_file->seek(m_dataOffset + 2 * m_summerySize64 / 3 + m_summerySize256 * m_channelCount + m_summeryIndexRms64[channelNummber] + m_summerySize64 * channelNummber);
    m_file->write((char*)m_summaryRms64k.data(), mSummary64kBytes / 3);

    m_summeryIndexMin256[channelNummber] += mSummary256Bytes / 3;
    m_summeryIndexMax256[channelNummber] += mSummary256Bytes / 3;
    m_summeryIndexRms256[channelNummber] += mSummary256Bytes / 3;

    m_summeryIndexMin64[channelNummber] += mSummary64kBytes / 3;
    m_summeryIndexMax64[channelNummber] += mSummary64kBytes / 3;
    m_summeryIndexRms64[channelNummber] += mSummary64kBytes / 3;

    m_file->flush();

    // Reset local arrays
    m_samples.reset();
    m_summary256.reset();
    m_summary64k.reset();
}

size_t SampleBlock::getSamples(float* dest, int channel, int channelCount, size_t sampleoffset, size_t numsamples, int offset, SNDFILE* sndFile)
{

    float* samples_input = (float*)malloc(numsamples * channelCount * sizeof(float));

    sf_seek(sndFile, sampleoffset, SF_SEEK_SET);
    int read = sf_readf_float(sndFile, samples_input, numsamples);

    for (ulong j = offset; j < numsamples; j++) {
        ((float*)dest)[j] = samples_input[channelCount * j + channel];
    }

    free(samples_input);

    return 0;
}

bool SampleBlock::getSummary64k(float* dest, int channel, size_t frameoffset, size_t numframes, QBuffer* file)
{

    file->seek(m_dataOffset + m_summerySize256 * m_channelCount + channel * m_summerySize64 + frameoffset);
    file->read((char*)dest, numframes);
    return 0;
}

bool SampleBlock::getSummary256(float* dest, int channel, size_t frameoffset, size_t numframes, QBuffer* file)
{

    file->seek(m_dataOffset + channel * m_summerySize256 + frameoffset);
    file->read((char*)dest, numframes);
    // qDebug() << dest[1304 * 3] << dest[1304 * 3 + 1] << dest[1304 * 2];
    return 0;
}

void SampleBlock::setBlockCount(int nBlock, QFile* file)
{
    QDataStream out(file);
    out.setVersion(QDataStream::Qt_5_12);
    file->seek(0);
    out << nBlock;
}

int SampleBlock::getBlockCount(QFile* file)
{
    QDataStream out(file);
    out.setVersion(QDataStream::Qt_5_12);
    file->seek(0);
    int nBlock = 0;
    out >> nBlock;
    return nBlock;
}
