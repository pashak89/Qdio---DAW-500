#ifndef MULTIEQ_H
#define MULTIEQ_H

#include "JuceLibraryCode/JuceHeader.h"
#include <QDebug>
#include <QJsonObject>
#include <audio/plugin.h>

class MultiEqEffect;
// using namespace juce::dsp;

class Coefficients {

public:
    Coefficients(juce::dsp::IIR::Coefficients<double>::Ptr ptr)
    {
        _ptr = ptr;
    }
    void getMagnitudeForFrequencyArray(const double* frequencies, double* magnitudes,
        size_t numSamples, double sampleRate)
    {
        _ptr->getMagnitudeForFrequencyArray(frequencies, magnitudes, numSamples, sampleRate);
    }
    void getPhaseForFrequencyArray(double* frequencies, double* phases,
        size_t numSamples, double sampleRate)
    {
        _ptr->getPhaseForFrequencyArray(frequencies, phases, numSamples, sampleRate);
    }

private:
    juce::dsp::IIR::Coefficients<double>::Ptr _ptr;
};

template <typename type>
class FilterVisualizerHelper {
public:
    static juce::Array<type> cascadeSecondOrderCoefficients(juce::Array<type>& c0, juce::Array<type>& c1)
    {
        juce::Array<type> c12;
        c12.resize(9);
        const int o = 2;

        c12.setUnchecked(0, c0[0] * c1[0]);
        c12.setUnchecked(1, c0[0] * c1[1] + c0[1] * c1[0]);
        c12.setUnchecked(2, c0[0] * c1[2] + c0[1] * c1[1] + c0[2] * c1[0]);
        c12.setUnchecked(3, c0[1] * c1[2] + c0[2] * c1[1]);
        c12.setUnchecked(4, c0[2] * c1[2]);

        c12.setUnchecked(5, c1[1 + o] + c0[1 + o]);
        c12.setUnchecked(6, c1[2 + o] + c0[1 + o] * c1[1 + o] + c0[2 + o]);
        c12.setUnchecked(7, c0[1 + o] * c1[2 + o] + c0[2 + o] * c1[1 + o]);
        c12.setUnchecked(8, c0[2 + o] * c1[2 + o]);

        return c12;
    }

    static juce::Array<type> cascadeFirstAndSecondOrderCoefficients(juce::Array<type>& firstOrder, juce::Array<type>& secondOrder)
    {
        juce::Array<type>& c1 = firstOrder;
        juce::Array<type>& c2 = secondOrder;

        juce::Array<type> c12;
        c12.resize(7);

        // b
        c12.setUnchecked(0, c1[0] * c2[0]);
        c12.setUnchecked(1, c1[0] * c2[1] + c1[1] * c2[0]);
        c12.setUnchecked(2, c1[0] * c2[2] + c1[1] * c2[1]);
        c12.setUnchecked(3, c1[1] * c2[2]);

        // a
        c12.setUnchecked(4, c1[2] + c2[3]);
        c12.setUnchecked(5, c1[2] * c2[3] + c2[4]);
        c12.setUnchecked(6, c1[2] * c2[4]);

        return c12;
    }
};

#if JUCE_USE_SIMD
using IIRfloat = juce::dsp::SIMDRegister<float>;
static constexpr int IIRfloat_elements = juce::dsp::SIMDRegister<float>::size();
#else /* !JUCE_USE_SIMD */
using IIRfloat = float;
static constexpr int IIRfloat_elements = 1;
#endif /* JUCE_USE_SIMD */

#define numFilterBands 4

class MultiEQ : public QObject {
    Q_OBJECT
public:
    struct Parameters {
        // list of used audio parameters
        bool filterEnabled[numFilterBands];
        int filterType[numFilterBands];
        float filterFrequency[numFilterBands];
        float filterQ[numFilterBands];
        float filterGain[numFilterBands];
    };

    struct EffectParameter {
        uint32 _id;
        QString _title;
        QString _name;
        QString _units;
        int32 _stepCount;
        double _defaultNormalizedValue;
        QString _value;
        double _normalized;
        bool _active;
        int _index;
    };

    enum SpecialFilterType {
        SecondOrderHighPass,
        BellFilter,
        HighShelf,
        SecondOrderLowPass,
        LinkwitzRileyHighPass,
        PeakFilter,
        LowShelf,
        LinkwitzRileyLowPass,

    };
    Q_ENUM(SpecialFilterType)

    explicit MultiEQ(QSharedPointer<MultiEqEffect> multiEqEffect = nullptr, QObject* parent = nullptr);

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer);

    void copyFilterCoefficientsToProcessor();

    // Coefficients* getCoefficientsForGui(const int filterIndex) { return _guiCoefficients[filterIndex]; };
    void updateGuiCoefficients();

    double getSampleRate() const
    {
        return _sampleRate;
    }

    QSharedPointer<Parameters> parameters() const;

    void getMagnitudeForFrequencyArray(int index, const double* frequencies, double* magnitudes,
        size_t numSamples, double sampleRate)
    {
        guiCoefficients[index]->getMagnitudeForFrequencyArray(frequencies, magnitudes, numSamples, sampleRate);
    }

    void getPhaseForFrequencyArray(int index, double* frequencies, double* phases,
        size_t numSamples, double sampleRate)
    {
        guiCoefficients[index]->getPhaseForFrequencyArray(frequencies, phases, numSamples, sampleRate);
    }

    QString getParamValue(int index, double normialized);
    double getParamNormalized(int index, double value);

    QVector<EffectParameter> effectParameters() const;

    void updateParameter(int index, QString title, double normalized);
    void parameterChanged();

    void parameterAutomated(int index, bool automated);
    void parameterActive(int index, bool active);
    void parameterUpdated(int index);

    void automatedEnabled(int index);

    void deviceOnActive(bool active);
    void deviceOnAutomated(bool automated);
    void deviceOnValue(bool value);
    void deviceOnAutomatedEnabled();
    void deviceOnEnabled(bool enabled);

    void setActiveIndex(int index);
    void updateStatus(QJsonObject status);
    void saveUndoOperation();
    void saveRedoOperation();

    QSharedPointer<MultiEqEffect> multiEqEffect() const;

    int activeIndex() const;

    QJsonObject status() const;

Q_SIGNALS:

    void sigSaveRedoOperation();
    void sigSaveUndoOperation();

    void sigAutomatedEnabled(int index);
    void sigUpdateParams(int index, QString type, double value);
    void sigPerformEdit(uint32 index, double normalized);

    void sigParameterAutomated(int index, bool automated);
    void sigParameterActiveChanged(int index, bool active);
    void sigParameterUpdated(int index);
    void sigParameterChanged();

    void sigDeviceOnActiveChanged(bool active);
    void sigDeviceOnAutomatedChanged(bool automated);
    void sigDeviceOnValueChanged(bool value);
    void sigDeviceOnAutomatedEnabled();

    void sigDeviceOnEnabledChanged(bool enabled);

    void sigUpdateStatus(QJsonObject status);

    void sigUpdateUI();

private:
    QJsonObject _status;
    int m_activeIndex;
    QSharedPointer<MultiEqEffect> _multiEqEffect;
    double _sampleRate = 48000;
    QSharedPointer<Parameters> _parameters;
    QVector<EffectParameter> _effectParameters;

    // filter dummy for GUI
    juce::dsp::IIR::Coefficients<double>::Ptr guiCoefficients[numFilterBands];
    Coefficients* _guiCoefficients[numFilterBands];

    juce::dsp::IIR::Coefficients<float>::Ptr processorCoefficients[numFilterBands];
    juce::dsp::IIR::Coefficients<float>::Ptr additionalProcessorCoefficients[2];

    juce::dsp::IIR::Coefficients<float>::Ptr tempCoefficients[numFilterBands];
    juce::dsp::IIR::Coefficients<float>::Ptr additionalTempCoefficients[2];

    // data for interleaving audio
    juce::HeapBlock<char> interleavedBlockData[16], zeroData; // todo: dynamically?
    juce::OwnedArray<juce::dsp::AudioBlock<IIRfloat>> interleavedData;
    juce::dsp::AudioBlock<float> zero;

    // filters for processing
    juce::OwnedArray<juce::dsp::IIR::Filter<IIRfloat>> filterArrays[numFilterBands];
    juce::OwnedArray<juce::dsp::IIR::Filter<IIRfloat>> additionalFilterArrays[2];

    juce::Atomic<bool> userHasChangedFilterSettings = true;

    void createLinkwitzRileyFilter(const bool isUpperBand);
    void createFilterCoefficients(const int filterIndex, const double sampleRate);

    inline void clear(juce::dsp::AudioBlock<IIRfloat>& ab);

    inline juce::dsp::IIR::Coefficients<float>::Ptr createFilterCoefficients(const SpecialFilterType type, const double sampleRate, const float frequency, const float Q, const float gain);

    inline juce::dsp::IIR::Coefficients<double>::Ptr createFilterCoefficientsForGui(const SpecialFilterType type, const double sampleRate, const float frequency, const float Q, const float gain);
};

#endif // MULTIEQ_H
