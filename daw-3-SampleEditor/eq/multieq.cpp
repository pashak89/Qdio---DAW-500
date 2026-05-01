#include "multieq.h"
#include "multieqeffect.h"

static constexpr int filterTypePresets[] = { 2, 7, 7, 5 };
static constexpr float filterFrequencyPresets[] = { 40.0f, 200.0f, 1000.0f, 5000.0f };

MultiEQ::MultiEQ(QSharedPointer<MultiEqEffect> multiEqEffect, QObject* parent)
    : QObject(parent)
    , _multiEqEffect(multiEqEffect)
{

    _parameters = QSharedPointer<Parameters>(new Parameters);

    for (int i = 0; i < numFilterBands; ++i) {
        _parameters->filterEnabled[i] = false;
        _parameters->filterType[i] = filterTypePresets[i];
        _parameters->filterFrequency[i] = filterFrequencyPresets[i];
        _parameters->filterQ[i] = 0.7f;
        _parameters->filterGain[i] = 0.0;
    }

    _parameters->filterEnabled[0] = true;

    additionalTempCoefficients[0] = juce::dsp::IIR::Coefficients<float>::makeAllPass(48000.0, 20.0f);
    additionalTempCoefficients[1] = juce::dsp::IIR::Coefficients<float>::makeAllPass(48000.0, 20.0f);

    for (int i = 0; i < numFilterBands; ++i) {
        createFilterCoefficients(i, 48000.0);
    }

    for (int i = 0; i < numFilterBands; ++i) {
        processorCoefficients[i] = juce::dsp::IIR::Coefficients<float>::makeAllPass(48000.0, 20.0f);
    }

    additionalProcessorCoefficients[0] = juce::dsp::IIR::Coefficients<float>::makeAllPass(48000.0, 20.0f);
    additionalProcessorCoefficients[1] = juce::dsp::IIR::Coefficients<float>::makeAllPass(48000.0, 20.0f);

    copyFilterCoefficientsToProcessor();

    for (int i = 0; i < numFilterBands; ++i) {
        filterArrays[i].clear();
        for (int ch = 0; ch < ceil(64 / IIRfloat_elements); ++ch)
            filterArrays[i].add(new juce::dsp::IIR::Filter<IIRfloat>(processorCoefficients[i]));
    }

    additionalFilterArrays[0].clear();
    for (int ch = 0; ch < ceil(64 / IIRfloat_elements); ++ch)
        additionalFilterArrays[0].add(new juce::dsp::IIR::Filter<IIRfloat>(additionalProcessorCoefficients[0]));

    additionalFilterArrays[1].clear();
    for (int ch = 0; ch < ceil(64 / IIRfloat_elements); ++ch)
        additionalFilterArrays[1].add(new juce::dsp::IIR::Filter<IIRfloat>(additionalProcessorCoefficients[1]));

    updateGuiCoefficients();

    _effectParameters.clear();

    _effectParameters.append(EffectParameter {
        12, "DeviceOn", "DeviceOn", "", 1, 1, "On", 1, true, 0 });

    _effectParameters.append(EffectParameter {
        0, "Freq" + QString::number(0), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 0 });
    _effectParameters.append(EffectParameter {
        1, "Gain" + QString::number(0), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 0 });
    _effectParameters.append(EffectParameter {
        2, "Q" + QString::number(0), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 0 });

    _effectParameters.append(EffectParameter {
        3, "Freq" + QString::number(1), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 1 });
    _effectParameters.append(EffectParameter {
        4, "Gain" + QString::number(1), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 1 });
    _effectParameters.append(EffectParameter {
        5, "Q" + QString::number(1), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 1 });

    _effectParameters.append(EffectParameter {
        6, "Freq" + QString::number(2), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 2 });
    _effectParameters.append(EffectParameter {
        7, "Gain" + QString::number(2), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 2 });
    _effectParameters.append(EffectParameter {
        8, "Q" + QString::number(2), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 2 });

    _effectParameters.append(EffectParameter {
        9, "Freq" + QString::number(3), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 3 });
    _effectParameters.append(EffectParameter {
        10, "Gain" + QString::number(3), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 3 });
    _effectParameters.append(EffectParameter {
        11, "Q" + QString::number(3), "Freq", "HZ", 1, getParamNormalized(0, 30), "30", 30, true, 3 });

    for (int i = 0; i < numFilterBands; ++i) {

        if (i == 0) {
            _parameters->filterType[i] = 6;
        }
        if (i == 1) {
            _parameters->filterType[i] = 1;
        }
        if (i == 2) {
            _parameters->filterType[i] = 1;
        }
        if (i == 3) {
            _parameters->filterType[i] = 2;
        }
    }

    updateGuiCoefficients();
}

inline juce::dsp::IIR::Coefficients<float>::Ptr MultiEQ::createFilterCoefficients(const SpecialFilterType type, const double sampleRate, const float frequency, const float Q, const float gain)
{
    const auto f = juce::jmin(static_cast<float>(0.5 * sampleRate), frequency);

    switch (type) {
    case SpecialFilterType::SecondOrderHighPass:
        return juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, f, Q);
        break;
    case SpecialFilterType::LowShelf:
        return juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, f, Q, gain);
        break;
    case SpecialFilterType::PeakFilter:
        return juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, f, Q);
        break;
    case SpecialFilterType::BellFilter:

        return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, f, Q, gain);
        break;
    case SpecialFilterType::HighShelf:
        return juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, f, Q, gain);
        break;
    case SpecialFilterType::SecondOrderLowPass:
        return juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, f, Q);
        break;
    case SpecialFilterType::LinkwitzRileyHighPass:
        return juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, f);
        break;
    case SpecialFilterType::LinkwitzRileyLowPass:
        return juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, f);
        break;
    }
}
inline juce::dsp::IIR::Coefficients<double>::Ptr MultiEQ::createFilterCoefficientsForGui(const SpecialFilterType type, const double sampleRate, const float frequency, const float Q, const float gain)
{

    const auto f = juce::jmin(static_cast<float>(0.5 * sampleRate), frequency);
    switch (type) {
    case SpecialFilterType::SecondOrderHighPass:
        return juce::dsp::IIR::Coefficients<double>::makeHighPass(sampleRate, f, Q);
        break;
    case SpecialFilterType::LowShelf:
        return juce::dsp::IIR::Coefficients<double>::makeLowShelf(sampleRate, f, Q, gain);
        break;
    case SpecialFilterType::PeakFilter:
        return juce::dsp::IIR::Coefficients<double>::makeNotch(sampleRate, f, Q);
        break;
    case SpecialFilterType::BellFilter:

        return juce::dsp::IIR::Coefficients<double>::makePeakFilter(sampleRate, f, Q, gain);
        break;
    case SpecialFilterType::HighShelf:
        return juce::dsp::IIR::Coefficients<double>::makeHighShelf(sampleRate, f, Q, gain);
        break;
    case SpecialFilterType::SecondOrderLowPass:
        return juce::dsp::IIR::Coefficients<double>::makeLowPass(sampleRate, f, Q);
        break;
    case SpecialFilterType::LinkwitzRileyHighPass: {
        return juce::dsp::IIR::Coefficients<double>::makeHighPass(sampleRate, f, Q);
    } break;
    case SpecialFilterType::LinkwitzRileyLowPass: {
        return juce::dsp::IIR::Coefficients<double>::makeLowPass(sampleRate, f, Q);
    } break;
    }
}

void MultiEQ::copyFilterCoefficientsToProcessor()
{
    for (int b = 0; b < numFilterBands; ++b)
        *processorCoefficients[b] = *tempCoefficients[b];

    *additionalTempCoefficients[0];
    *additionalTempCoefficients[1];
    *additionalProcessorCoefficients[0] = *additionalTempCoefficients[0];
    *additionalProcessorCoefficients[1] = *additionalTempCoefficients[1];

    userHasChangedFilterSettings = false;
}

void MultiEQ::updateGuiCoefficients()
{

    const double sampleRate = getSampleRate() == 0 ? 48000.0 : getSampleRate();

    // regular bands

    for (int f = 0; f < numFilterBands; ++f) {

        const auto frequency = juce::jmin(static_cast<float>(0.5 * sampleRate), _parameters->filterFrequency[f]);
        const SpecialFilterType type = SpecialFilterType(static_cast<int>(_parameters->filterType[f]));

        switch (type) {
        case SpecialFilterType::LowShelf:
            guiCoefficients[f] = juce::dsp::IIR::Coefficients<double>::makeLowShelf(sampleRate, frequency, _parameters->filterQ[f],
                juce::Decibels::decibelsToGain(_parameters->filterGain[f]));
            break;
        case SpecialFilterType::PeakFilter:
            guiCoefficients[f] = juce::dsp::IIR::Coefficients<double>::makeNotch(sampleRate, frequency, _parameters->filterQ[f]);

            break;
        case SpecialFilterType::BellFilter:
            guiCoefficients[f] = juce::dsp::IIR::Coefficients<double>::makePeakFilter(sampleRate, frequency, _parameters->filterQ[f],
                juce::Decibels::decibelsToGain(_parameters->filterGain[f]));
            break;
        case SpecialFilterType::HighShelf:
            guiCoefficients[f] = juce::dsp::IIR::Coefficients<double>::makeHighShelf(sampleRate, frequency, _parameters->filterQ[f],
                juce::Decibels::decibelsToGain(_parameters->filterGain[f]));
            break;
        case SpecialFilterType::LinkwitzRileyLowPass: {
            auto coeffs = juce::dsp::IIR::Coefficients<double>::makeLowPass(sampleRate, frequency, _parameters->filterQ[f]);
            coeffs->coefficients = FilterVisualizerHelper<double>::cascadeSecondOrderCoefficients(coeffs->coefficients, coeffs->coefficients);
            guiCoefficients[f] = coeffs;
            break;
        }
        case SpecialFilterType::LinkwitzRileyHighPass: {
            auto coeffs = juce::dsp::IIR::Coefficients<double>::makeHighPass(sampleRate, frequency, _parameters->filterQ[f]);
            coeffs->coefficients = FilterVisualizerHelper<double>::cascadeSecondOrderCoefficients(coeffs->coefficients, coeffs->coefficients);
            guiCoefficients[f] = coeffs;
            break;
        }
        case SpecialFilterType::SecondOrderHighPass:
            guiCoefficients[f] = juce::dsp::IIR::Coefficients<double>::makeHighPass(sampleRate, frequency, _parameters->filterQ[f]);
            break;
        case SpecialFilterType::SecondOrderLowPass:
            guiCoefficients[f] = juce::dsp::IIR::Coefficients<double>::makeLowPass(sampleRate, frequency, _parameters->filterQ[f]);
            break;
        }

        createFilterCoefficients(f, getSampleRate());
    }

    userHasChangedFilterSettings = true;
}

QSharedPointer<MultiEQ::Parameters> MultiEQ::parameters() const
{
    return _parameters;
}

QString MultiEQ::getParamValue(int index, double normialized)
{
    if (index % 3 == 0) {

        return QString::number(20 * pow((22000.0 / 20.0), 1 - normialized));

        // return QString::number(21980 * (1 - normialized) + 20);
    } else if (index % 3 == 1) {
        return QString::number(40 * (1 - normialized) - 20);
    } else {
        return QString::number(17.9 * (1 - normialized) + 0.1);
    }
}

double MultiEQ::getParamNormalized(int index, double value)
{

    if (index % 3 == 0) {
        return 1 - (log(value / 20.0) / log(22000.0 / 20.0));
        // return double(1 - (double(value - 20.0) / 21980.0));
    } else if (index % 3 == 1) {
        return 1 - (value + 20) / 40.0;
    } else {
        return 1 - (value - 0.1) / 17.9;
    }
}

void MultiEQ::updateParameter(int index, QString title, double normalized)
{
    Q_EMIT sigUpdateParams(index, title, normalized);
}

void MultiEQ::parameterChanged()
{
    Q_EMIT sigParameterChanged();
}

void MultiEQ::parameterAutomated(int index, bool automated)
{
    Q_EMIT sigParameterAutomated(index, automated);
}

void MultiEQ::parameterActive(int index, bool active)
{
    Q_EMIT sigParameterActiveChanged(index, active);
}

void MultiEQ::deviceOnActive(bool active)
{
    Q_EMIT sigDeviceOnActiveChanged(active);
}

void MultiEQ::deviceOnAutomated(bool automated)
{
    Q_EMIT sigDeviceOnAutomatedChanged(automated);
}

void MultiEQ::deviceOnValue(bool value)
{
    Q_EMIT sigDeviceOnValueChanged(value);
}

void MultiEQ::deviceOnAutomatedEnabled()
{
    Q_EMIT sigDeviceOnAutomatedEnabled();
}

void MultiEQ::deviceOnEnabled(bool enabled)
{
    Q_EMIT sigDeviceOnEnabledChanged(enabled);
}

void MultiEQ::setActiveIndex(int index)
{
    m_activeIndex = index;
}

void MultiEQ::updateStatus(QJsonObject status)
{
    _status = status;
    Q_EMIT sigUpdateStatus(status);
}

void MultiEQ::saveUndoOperation()
{
    Q_EMIT sigSaveUndoOperation();
}
void MultiEQ::saveRedoOperation()
{
    Q_EMIT sigSaveRedoOperation();
}

QSharedPointer<MultiEqEffect> MultiEQ::multiEqEffect() const
{
    return _multiEqEffect;
}

int MultiEQ::activeIndex() const
{
    return m_activeIndex;
}

QJsonObject MultiEQ::status() const
{
    return _status;
}

void MultiEQ::parameterUpdated(int index)
{
    Q_EMIT sigParameterUpdated(index);
}

void MultiEQ::automatedEnabled(int index)
{
    Q_EMIT sigAutomatedEnabled(index);
}

QVector<MultiEQ::EffectParameter> MultiEQ::effectParameters() const
{
    return _effectParameters;
}

void MultiEQ::prepareToPlay(double sampleRate, int samplesPerBlock)
{

    for (int f = 0; f < numFilterBands; ++f) {
        createFilterCoefficients(f, sampleRate);
    }
    copyFilterCoefficientsToProcessor();

    interleavedData.clear();

    for (int i = 0; i < ceil(64 / IIRfloat_elements); ++i) {
        // reset filters
        for (int f = 0; f < numFilterBands; ++f) {
            filterArrays[f][i]->reset(IIRfloat(0.0f));
        }

        interleavedData.add(new juce::dsp::AudioBlock<IIRfloat>(interleavedBlockData[i], 1, samplesPerBlock));
        // interleavedData.getLast()->clear(); // this one's broken in JUCE 5.4.5
        clear(*interleavedData.getLast());
    }

    zero = juce::dsp::AudioBlock<float>(zeroData, IIRfloat_elements, samplesPerBlock);
    zero.clear();
}

void MultiEQ::processBlock(juce::AudioBuffer<float>& buffer)
{

    const int L = buffer.getNumSamples();

    const int maxNChIn = buffer.getNumChannels();
    if (maxNChIn < 1)
        return;

    const int nSIMDFilters = 1 + (maxNChIn - 1) / IIRfloat_elements;

    // update iir filter coefficients
    if (userHasChangedFilterSettings.get())
        copyFilterCoefficientsToProcessor();

    // interleave input data
    int partial = maxNChIn % IIRfloat_elements;

    if (partial == 0) {
        for (int i = 0; i < nSIMDFilters; ++i) {
            juce::AudioDataConverters::interleaveSamples((const float**)buffer.getArrayOfReadPointers() + i * IIRfloat_elements,
                reinterpret_cast<float*>(interleavedData[i]->getChannelPointer(0)), L,
                static_cast<int>(IIRfloat_elements));
        }
    } else {
        int i;
        for (i = 0; i < nSIMDFilters - 1; ++i) {
            juce::AudioDataConverters::interleaveSamples((const float**)buffer.getArrayOfReadPointers() + i * IIRfloat_elements,
                reinterpret_cast<float*>(interleavedData[i]->getChannelPointer(0)), L,
                static_cast<int>(IIRfloat_elements));
        }

        const float* addr[IIRfloat_elements];
        int ch;
        for (ch = 0; ch < partial; ++ch) {
            addr[ch] = buffer.getReadPointer(i * IIRfloat_elements + ch);
        }
        for (; ch < IIRfloat_elements; ++ch) {
            addr[ch] = zero.getChannelPointer(ch);
        }
        juce::AudioDataConverters::interleaveSamples(addr,
            reinterpret_cast<float*>(interleavedData[i]->getChannelPointer(0)), L,
            static_cast<int>(IIRfloat_elements));
    }

    // apply filters
    for (int f = 0; f < numFilterBands; ++f) {
        if (_parameters->filterEnabled[f] == true) {
            for (int i = 0; i < nSIMDFilters; ++i) {
                const IIRfloat* chPtr[1] = { interleavedData[i]->getChannelPointer(0) };
                juce::dsp::AudioBlock<IIRfloat> ab(const_cast<IIRfloat**>(chPtr), 1, L);
                juce::dsp::ProcessContextReplacing<IIRfloat> context(ab);
                filterArrays[f][i]->process(context);
            }
        }
    }

    // check and apply additional filters (Linkwitz Riley -> two BiQuads)
    if (((static_cast<int>(_parameters->filterType[0]) == (int)SpecialFilterType::LinkwitzRileyHighPass)
            || (static_cast<int>(_parameters->filterType[0]) == (int)SpecialFilterType::LinkwitzRileyLowPass))
        && _parameters->filterEnabled[0] == true) {
        for (int i = 0; i < nSIMDFilters; ++i) {
            const IIRfloat* chPtr[1] = { chPtr[0] = interleavedData[i]->getChannelPointer(0) };
            juce::dsp::AudioBlock<IIRfloat> ab(const_cast<IIRfloat**>(chPtr), 1, L);
            juce::dsp::ProcessContextReplacing<IIRfloat> context(ab);
            additionalFilterArrays[0][i]->process(context);
        }
    }
    if ((static_cast<int>(_parameters->filterType[numFilterBands - 1]) == (int)SpecialFilterType::LinkwitzRileyHighPass
            || static_cast<int>(_parameters->filterType[numFilterBands - 1]) == (int)SpecialFilterType::LinkwitzRileyHighPass)
        && _parameters->filterEnabled[numFilterBands - 1] == true) {
        for (int i = 0; i < nSIMDFilters; ++i) {
            const IIRfloat* chPtr[1] = { interleavedData[i]->getChannelPointer(0) };
            juce::dsp::AudioBlock<IIRfloat> ab(const_cast<IIRfloat**>(chPtr), 1, L);
            juce::dsp::ProcessContextReplacing<IIRfloat> context(ab);
            additionalFilterArrays[1][i]->process(context);
        }
    }

    // deinterleave
    if (partial == 0) {
        for (int i = 0; i < nSIMDFilters; ++i) {
            juce::AudioDataConverters::deinterleaveSamples(reinterpret_cast<float*>(interleavedData[i]->getChannelPointer(0)),
                buffer.getArrayOfWritePointers() + i * IIRfloat_elements,
                L,
                static_cast<int>(IIRfloat_elements));
        }
    } else {
        int i;
        for (i = 0; i < nSIMDFilters - 1; ++i) {
            juce::AudioDataConverters::deinterleaveSamples(reinterpret_cast<float*>(interleavedData[i]->getChannelPointer(0)),
                buffer.getArrayOfWritePointers() + i * IIRfloat_elements,
                L,
                static_cast<int>(IIRfloat_elements));
        }

        float* addr[IIRfloat_elements];
        int ch;
        for (ch = 0; ch < partial; ++ch) {
            addr[ch] = buffer.getWritePointer(i * IIRfloat_elements + ch);
        }
        for (; ch < IIRfloat_elements; ++ch) {
            addr[ch] = zero.getChannelPointer(ch);
        }
        juce::AudioDataConverters::deinterleaveSamples(reinterpret_cast<float*>(interleavedData[i]->getChannelPointer(0)),
            addr,
            L,
            static_cast<int>(IIRfloat_elements));
        zero.clear();
    }
}

void MultiEQ::createLinkwitzRileyFilter(const bool isUpperBand)
{
    if (isUpperBand) {
        const auto frequency = juce::jmin(static_cast<float>(0.5 * getSampleRate()), _parameters->filterFrequency[numFilterBands - 1]);
        tempCoefficients[numFilterBands - 1] = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), frequency,
            _parameters->filterQ[numFilterBands - 1]);
        additionalTempCoefficients[1] = processorCoefficients[numFilterBands - 1];
    } else {
        const auto frequency = juce::jmin(static_cast<float>(0.5 * getSampleRate()), _parameters->filterFrequency[0]);
        tempCoefficients[0] = juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), frequency, _parameters->filterQ[0]);
        additionalTempCoefficients[0] = processorCoefficients[0];
    }
}

void MultiEQ::createFilterCoefficients(const int filterIndex, const double sampleRate)
{
    const int type = juce::roundToInt(_parameters->filterType[filterIndex]);
    SpecialFilterType filterType = (SpecialFilterType)type;
    //    switch (type) {
    //    case 0:
    //        filterType = SpecialFilterType::SecondOrderHighPass;
    //        break;
    //    case 1:
    //        createLinkwitzRileyFilter(false);
    //        filterType = SpecialFilterType::LinkwitzRileyHighPass;
    //        break;
    //    case 2:
    //        filterType = SpecialFilterType::LowShelf;
    //        break;
    //    case 3:
    //        filterType = SpecialFilterType::SecondOrderLowPass;
    //        break;
    //    case 4:
    //        createLinkwitzRileyFilter(false);
    //        filterType = SpecialFilterType::LinkwitzRileyLowPass;
    //        break;
    //    case 5:
    //        filterType = SpecialFilterType::HighShelf;
    //        break;
    //    case 6:
    //        filterType = SpecialFilterType::PeakFilter;
    //        break;
    //    case 7:
    //        filterType = SpecialFilterType::BellFilter;
    //        break;
    //    }

    tempCoefficients[filterIndex] = createFilterCoefficients(filterType, sampleRate, _parameters->filterFrequency[filterIndex],
        _parameters->filterQ[filterIndex], juce::Decibels::decibelsToGain(_parameters->filterGain[filterIndex]));
}

void MultiEQ::clear(juce::dsp::AudioBlock<IIRfloat>& ab)
{
    //    const int N = static_cast<int>(ab.getNumSamples()) * IIRfloat_elements;
    //    const int nCh = static_cast<int>(ab.getNumChannels());

    //    for (int ch = 0; ch < nCh; ++ch)
    //        juce::FloatVectorOperations::clear(reinterpret_cast<float*>(ab.getChannelPointer(ch)), N);
}
