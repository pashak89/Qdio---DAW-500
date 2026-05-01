#ifndef FILESAMPLEBLOCK_H
#define FILESAMPLEBLOCK_H

#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <Qt>
#include <QtMath>

#include <float.h>
#include <memory>

#include <sndfile.h>

typedef char* samplePtr;
typedef const char* constSamplePtr;

template <typename X>
class ArrayOf : public std::unique_ptr<X[]> {
public:
    ArrayOf() {}

    template <typename Integral>
    explicit ArrayOf(Integral count, bool initialize = false)
    {
        static_assert(std::is_unsigned<Integral>::value, "Unsigned arguments only");
        reinit(count, initialize);
    }

    // ArrayOf(const ArrayOf&) PROHIBITED;
    ArrayOf(const ArrayOf&) = delete;
    ArrayOf(ArrayOf&& that)
        : std::unique_ptr<X[]>(std::move((std::unique_ptr<X[]>&)(that)))
    {
    }
    ArrayOf& operator=(ArrayOf&& that)
    {
        std::unique_ptr<X[]>::operator=(std::move(that));
        return *this;
    }
    ArrayOf& operator=(std::unique_ptr<X[]>&& that)
    {
        std::unique_ptr<X[]>::operator=(std::move(that));
        return *this;
    }

    template <typename Integral>
    void reinit(Integral count,
        bool initialize = false)
    {
        static_assert(std::is_unsigned<Integral>::value, "Unsigned arguments only");
        if (initialize)
            // Initialize elements (usually, to zero for a numerical type)
            std::unique_ptr<X[]>::reset(new X[count]{});
        else
            // Avoid the slight initialization overhead
            std::unique_ptr<X[]>::reset(new X[count]);
    }
};
class MinMaxRMS {
public:
    float min = 0;
    float max = 0;
    float RMS = 0;
};

class BlockBuffer {

public:
    BlockBuffer()
        : mPtr(0)
    {
    }
    BlockBuffer(size_t count)
        : mPtr((samplePtr)malloc(count * sizeof(float)))
    {
    }
    ~BlockBuffer()
    {
        Free();
    }

    // WARNING!  May not preserve contents.
    BlockBuffer& Allocate(size_t count)
    {
        Free();
        mPtr = (samplePtr)malloc(count * sizeof(float));
        return *this;
    }

    void Free()
    {
        free(mPtr);
        mPtr = 0;
    }

    samplePtr ptr() const { return mPtr; }

private:
    samplePtr mPtr;
};

class SampleBlock {

    using Sizes = std::pair<size_t, size_t>;
    using Floats = ArrayOf<float>;

    enum {
        fields = 3, /* min, max, rms */
        bytesPerFrame = fields * sizeof(float),
    };

public:
    explicit SampleBlock(QFile* file);
    ~SampleBlock();
    SampleBlock(const SampleBlock& sb);

    std::shared_ptr<SampleBlock> clone()
    {        
        return std::shared_ptr<SampleBlock>(new SampleBlock(*this));
    }

    void addSample(constSamplePtr src, size_t numsamples, int channelNumber);
    void initSamples(quint64 nSamples, int nChannels);

    static size_t getSamples(float* dest, int channel, int channelCount, size_t sampleoffset, size_t numsamples, int offset, SNDFILE* sndFile);
    bool getSummary64k(float* dest, int channel, size_t frameoffset, size_t numframes, QBuffer* file);
    bool getSummary256(float* dest, int channel, size_t frameoffset, size_t numframes, QBuffer* file);

    static void setBlockCount(int nBlock, QFile* file);
    static int getBlockCount(QFile* file);

    static quint64 m_blockCounter;

    bool loadSample(QBuffer* file);

    quint64 getSummerySize256() const;

    quint64 getSummerySize64() const;

    float* getSumMin256(int channel, int& size);

    float* getSumMax256(int channel, int& size);

    float* getSumRms256(int channel, int& size);

    float* getSumMin64k(int channel, int& size);

    float* getSumMax64k(int channel, int& size);

    float* getSumRms64k(int channel, int& size);

private:
    void calcSummary(Sizes sizes, int channel);
    void commit(Sizes sizesint, int channelNummber);
    Sizes setSizes(size_t numsamples);

private:
    int m_blockIndex;
    int m_channelCount = 0;
    int m_version = 2;
    QByteArray m_header;
    QMap<int, quint64> m_summeryIndexMin256;
    QMap<int, quint64> m_summeryIndexMax256;
    QMap<int, quint64> m_summeryIndexRms256;
    QMap<int, quint64> m_summeryIndexMin64;
    QMap<int, quint64> m_summeryIndexMax64;
    QMap<int, quint64> m_summeryIndexRms64;
    quint64 m_summerySize256;
    quint64 m_summerySize64;
    quint64 m_sampleCount;
    quint64 m_blockSampleCount;
    quint64 m_sampleBytes;
    quint64 m_sampleOffset;
    QFile* m_file;

    ArrayOf<char> m_samples;
    ArrayOf<char> m_summary256;
    ArrayOf<char> m_summary64k;

    QVector<float> m_summaryMin256;
    QVector<float> m_summaryMax256;
    QVector<float> m_summaryRms256;

    QVector<float> m_summaryMin64k;
    QVector<float> m_summaryMax64k;
    QVector<float> m_summaryRms64k;

    QMap<int, QVector<float>> m_sumMin256;
    QMap<int, QVector<float>> m_sumMax256;
    QMap<int, QVector<float>> m_sumRms256;
    QMap<int, QVector<float>> m_sumMin64k;
    QMap<int, QVector<float>> m_sumMax64k;
    QMap<int, QVector<float>> m_sumRms64k;

    int m_dataOffset = 0;
    double m_sumMin;
    double m_sumMax;
    double m_sumRms;
};

#endif // FILESAMPLEBLOCK_H
