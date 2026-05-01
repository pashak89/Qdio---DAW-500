#include "visrhelper.h"
#include "audioengine.h"
#include "audioport.h"
#include "engine.h"
#include "mixer.h"
#include "mixhelpers.h"
#include "sampletrack.h"
#include "song.h"
#include <QDebug>
#include <filesystem>
#include <libefl/cartesian_spherical_conversion.hpp>
#include <libefl/degree_radian_conversion.hpp>
#include <qtimer.h>

/**
 * Audio signal graph object for the VISR baseline renderer.
 */
class MetadataExposingRenderer : public visr::CompositeComponent {
public:
    explicit MetadataExposingRenderer(visr::SignalFlowContext const& context,
        char const* name,
        CompositeComponent* parent,
        visr::panning::LoudspeakerArray const& loudspeakerConfiguration,
        std::size_t numberOfInputs,
        std::size_t numberOfOutputs,
        std::size_t interpolationPeriod,
        visr::efl::BasicMatrix<visr::SampleType> const& diffusionFilters,
        std::string const& trackingConfiguration,
        std::size_t sceneReceiverPort,
        std::size_t numberOfObjectEqSections,
        std::string const& reverbConfig,
        bool frequencyDependentPanning)
        : CompositeComponent(context, name, parent)
        , mCoreRenderer(context, "CoreRenderer", this, loudspeakerConfiguration, numberOfInputs, numberOfOutputs,
              interpolationPeriod, diffusionFilters, trackingConfiguration, numberOfObjectEqSections,
              reverbConfig, frequencyDependentPanning)
        , mObjectVectorInput("objectVector", *this, visr::pml::EmptyParameterConfig())
        , mInput("input", *this, numberOfInputs)
        , mOutput("output", *this, numberOfOutputs)
    {
        audioConnection(mInput, mCoreRenderer.audioPort("audioIn"));
        audioConnection(mCoreRenderer.audioPort("audioOut"), mOutput);
        parameterConnection(mObjectVectorInput, mCoreRenderer.parameterPort("objectDataInput"));
        if (not trackingConfiguration.empty()) {
            mTrackingReceiver.reset(new visr::rcl::UdpReceiver(context, "TrackingReceiver", nullptr, 8888, visr::rcl::UdpReceiver::Mode::Synchronous));
            mTrackingPositionDecoder.reset(new visr::rcl::PositionDecoder(context, "TrackingPositionDecoder", nullptr, visr::panning::XYZ(0.0f, 0.0f, 0.0f)));
        }
    }

    /**
     * Simplified constructor using default values for several parameters.
     */
    explicit MetadataExposingRenderer(visr::SignalFlowContext const& context,
        char const* name,
        CompositeComponent* parent,
        visr::panning::LoudspeakerArray const& loudspeakerConfiguration,
        std::size_t numberOfInputs,
        std::size_t numberOfOutputs)
        : MetadataExposingRenderer(context, name, parent, loudspeakerConfiguration, numberOfInputs, numberOfOutputs,
            4096ul,
            visr::efl::BasicMatrix<visr::SampleType>(numberOfOutputs, 2, visr::cVectorAlignmentSamples),
            std::string(), 4242, 0 /*number of EQ sections*/, std::string(), false)
    {
    }

private:
    /**
     * Tracking-related members
     */
    //@{
    /**
     * UDP port for receiving listener position updates
     */
    std::unique_ptr<visr::rcl::UdpReceiver> mTrackingReceiver;

    /**
     * Component that transforms JSON strings into the internal listener position format
     */
    std::unique_ptr<visr::rcl::PositionDecoder> mTrackingPositionDecoder;

    visr::signalflows::CoreRenderer mCoreRenderer;

    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ObjectVector> mObjectVectorInput;

    visr::AudioInput mInput;
    visr::AudioOutput mOutput;
};

class CoreRenderer2 : public visr::CompositeComponent {
public:
    explicit CoreRenderer2(visr::SignalFlowContext const& context,
        char const* name,
        CompositeComponent* parent,
        visr::panning::LoudspeakerArray const& loudspeakerConfiguration,
        std::size_t numberOfInputs,
        std::size_t numberOfOutputs,
        std::size_t interpolationPeriod,
        visr::efl::BasicMatrix<visr::SampleType> const& diffusionFilters,
        std::string const& trackingConfiguration,
        std::size_t numberOfObjectEqSections,
        std::string const& reverbConfig,
        bool frequencyDependentPanning)
        : CompositeComponent(context, name, parent)
        , mObjectSignalInput("audioIn", *this, numberOfInputs)
        , mLoudspeakerOutput("audioOut", *this, numberOfOutputs)
        , mObjectVectorInput("objectDataInput", *this, visr::pml::EmptyParameterConfig())
        , mObjectInputGainEqCalculator(context, "ObjectGainEqCalculator", this, numberOfInputs, numberOfObjectEqSections)
        , mObjectGain(context, "ObjectGain", this)
        , mObjectEq(context, "ObjectEq", this)
        , mOutputAdjustment(context, "OutputAdjustment", this)
        , mNullSource(context, "NullSource", this, 2)
        , mGainCalculator(context, "VbapGainCalculator", this, numberOfInputs, loudspeakerConfiguration, not trackingConfiguration.empty(),
              frequencyDependentPanning ? visr::rcl::PanningCalculator::PanningMode::All : (visr::rcl::PanningCalculator::PanningMode::LF | visr::rcl::PanningCalculator::PanningMode::Diffuse))
    {
        //        std::size_t const numberOfLoudspeakers = loudspeakerConfiguration.getNumRegularSpeakers();
        //        std::size_t const numberOfSubwoofers = loudspeakerConfiguration.getNumSubwoofers();
        //        std::size_t const numberOfOutputSignals = numberOfLoudspeakers + numberOfSubwoofers;

        //        bool const trackingEnabled = not trackingConfiguration.empty();

        //        parameterConnection(mObjectVectorInput, mGainCalculator.parameterPort("objectVectorInput"));

        //        parameterConnection(mObjectVectorInput, mObjectInputGainEqCalculator.parameterPort("objectIn"));
        //        mObjectGain.setup(numberOfInputs, interpolationPeriod, true /* controlInputs */);
        //        audioConnection(mObjectSignalInput, mObjectGain.audioPort("in"));
        //        parameterConnection(mObjectInputGainEqCalculator.parameterPort("gainOut"), mObjectGain.parameterPort("gainInput"));

        //        mObjectEq.setup(numberOfInputs, numberOfObjectEqSections, true /* Enable control input */);
        //        audioConnection(mObjectGain.audioPort("out"), mObjectEq.audioPort("in"));
        //        parameterConnection(mObjectInputGainEqCalculator.parameterPort("eqOut"), mObjectEq.parameterPort("eqInput"));

        //        mVbapMatrix.setup(numberOfInputs, numberOfLoudspeakers, interpolationPeriod, 0.0f);
        //        audioConnection(mVbapMatrix.audioPort("out"), mDirectDiffuseMix.audioPort("in0"));

        //        audioConnection(mObjectEq.audioPort("out"), mVbapMatrix.audioPort("in"));

        //        //////////////////////////////////////////////////////////////////////////////////////

        //        mDiffuseMatrix.setup(numberOfInputs, numberOfLoudspeakers, interpolationPeriod, 0.0f);
        //        //  parameterConnection( mObjectVectorInput,  mDiffusionGainCalculator.parameterPort("objectInput") );
        //        //  parameterConnection( "DiffusionCalculator", "gainOutput", "DiffusePartMatrix", "gainInput" );
        //        parameterConnection(mGainCalculator.parameterPort("diffuseGains"), mDiffuseMatrix.parameterPort("gainInput"));
        //        audioConnection(mObjectEq.audioPort("out"), mDiffuseMatrix.audioPort("in"));

        //        visr::efl::BasicVector<SampleType> const& outputGains = loudspeakerConfiguration.getGainAdjustment();
        //        visr::efl::BasicVector<SampleType> const& outputDelays = loudspeakerConfiguration.getDelayAdjustment();

        //        Afloat const* const maxEl = std::max_element(outputDelays.data(),
        //            outputDelays.data() + outputDelays.size());
        //        Afloat const maxDelay = std::ceil(*maxEl); // Sufficient for nearestSample even if there is no particular compensation for the interpolation method's delay inside.

        //        mOutputAdjustment.setup(numberOfOutputSignals, period(), maxDelay,
        //            "lagrangeOrder0",
        //            rcl::DelayVector::MethodDelayPolicy::Limit,
        //            rcl::DelayVector::ControlPortConfig::None,
        //            outputDelays, outputGains);

        //        // Note: This assumes that the type 'Afloat' used in libpanning is
        //        // identical to SampleType (at the moment, both are floats).
        //        visr::efl::BasicMatrix<SampleType> const& subwooferMixGains = loudspeakerConfiguration.getSubwooferGains();
        //        mSubwooferMix.setup(numberOfLoudspeakers, numberOfSubwoofers, 0 /*interpolation steps*/, subwooferMixGains, false /*controlInput*/);

        //        audioConnection(mDiffuseMatrix.audioPort("out"), mDecorrelator.audioPort("in"));
        //        audioConnection(mDecorrelator.audioPort("out"),
        //            mDirectDiffuseMix.audioPort(frequencyDependentPanning ? "in2" : "in1"));

        //        audioConnection(mDirectDiffuseMix.audioPort("out"), ChannelRange(0, numberOfLoudspeakers),
        //            mOutputAdjustment.audioPort("in"), ChannelRange(0, numberOfLoudspeakers));
        //        audioConnection(mSubwooferMix.audioPort("out"), ChannelRange(0, numberOfSubwoofers),
        //            mOutputAdjustment.audioPort("in"), ChannelRange(numberOfLoudspeakers, numberOfLoudspeakers + numberOfSubwoofers));

        //        // Connect to the external playback channels, including the silencing of unused channels.
        //        if (numberOfLoudspeakers + numberOfSubwoofers > numberOfOutputs) // Otherwise the computation below would cause an immense memory allocation.
        //        {
        //            throw std::invalid_argument("The number of loudspeakers plus subwoofers exceeds the number of output channels.");
        //        }
        //        constexpr ChannelList::IndexType invalidIdx = std::numeric_limits<ChannelList::IndexType>::max();
        //        std::vector<ChannelList::IndexType> activePlaybackChannels(numberOfLoudspeakers + numberOfSubwoofers, invalidIdx);
        //        for (std::size_t idx(0); idx < numberOfLoudspeakers; ++idx) {
        //            panning::LoudspeakerArray::ChannelIndex const chIdx = loudspeakerConfiguration.channelIndex(idx) - 1;
        //            if (chIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs)) {
        //                throw std::invalid_argument("The loudspeakers channel index exceeds the admissible range.");
        //            }
        //            // This does not check whether an index is used multiple times.
        //            activePlaybackChannels[idx] = chIdx;
        //        }
        //        for (std::size_t idx(0); idx < numberOfSubwoofers; ++idx) {
        //            visr::panning::LoudspeakerArray::ChannelIndex const chIdx = loudspeakerConfiguration.getSubwooferChannels()[idx] - 1;
        //            if (chIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs)) {
        //                throw std::invalid_argument("The subwoofer channel index exceeds the admissible range.");
        //            }
        //            // This does not check whether an index is used multiple times.
        //            activePlaybackChannels[numberOfLoudspeakers + idx] = chIdx;
        //        }
        //        if (std::find(activePlaybackChannels.begin(), activePlaybackChannels.end(), invalidIdx) != activePlaybackChannels.end()) {
        //            throw std::invalid_argument("Not all active output channels are assigned.");
        //        }
        //        std::vector<ChannelList::IndexType> sortedPlaybackChannels(activePlaybackChannels);
        //        std::sort(sortedPlaybackChannels.begin(), sortedPlaybackChannels.end());
        //        if (std::unique(sortedPlaybackChannels.begin(), sortedPlaybackChannels.end()) != sortedPlaybackChannels.end()) {
        //            throw std::invalid_argument("The loudspeaker array contains a duplicated output channel index.");
        //        }
        //        audioConnection(mOutputAdjustment.audioPort("out"), ChannelRange(0, numberOfLoudspeakers + numberOfSubwoofers),
        //            mLoudspeakerOutput, ChannelList(activePlaybackChannels));
    }

private:
    visr::AudioInput mObjectSignalInput;

    visr::AudioOutput mLoudspeakerOutput;

    visr::rcl::ObjectGainEqCalculator mObjectInputGainEqCalculator;
    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ObjectVector> mObjectVectorInput;
    visr::rcl::GainVector mObjectGain;
    visr::rcl::PanningCalculator mGainCalculator;
    visr::rcl::NullSource mNullSource;

    visr::rcl::BiquadIirFilter mObjectEq;
    visr::rcl::DelayVector mOutputAdjustment;
};

class ObjectVectorEncoder : public visr::AtomicComponent {
public:
    /**
     * Constructor.
     * @param context Configuration object containing basic execution parameters.
     * @param name The name of the component. Must be unique within the containing composite component (if there is one).
     * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
     */
    explicit ObjectVectorEncoder(visr::SignalFlowContext const& context,
        char const* name,
        visr::CompositeComponent* parent = nullptr);

    /**
     * Disabled (deleted) copy constructor
     */
    ObjectVectorEncoder(ObjectVectorEncoder const&) = delete;

    /**
     * Destructor.
     */
    ~ObjectVectorEncoder();

    /**
     * Method to initialise the component.
     * At the moment, there are arguments to customize the component, but this might change in the future.
     */
    void setup();

    void setupReverb();

    void updatePosition(double x, double y, double z);
    void setLateReverbLevels(QVector<float> levels);
    void setDecayCoeffs(QVector<float> levels);
    void setAttackTimes(QVector<float> levels);
    void setOnsetDelay(float delay);

    void setNumberOfDiscreteReflections(int number);
    void setDiscreteReflectionDelay(int index, float delay);
    void setDiscreteReflectionLevel(int index, float level);
    void setDiscreteReflectionPosition(int index, float x, float y, float z);
    void setDiscreteReflectionFilter(int index, int biquadIdx, float b0, float b1, float b2, float a1, float a2);

    float getDiscreteReflectionDelay(int index);
    float getDiscreteReflectionLevel(int index);
    void getDiscreteReflectionPosition(int index, float& x, float& y, float& z);
    void getDiscreteReflectionFilter(int index, int biquadIdx, float& b0, float& b1, float& b2, float& a1, float& a2);
    int getDiscreteReflectionFilterNumber(int index);

    QVector<float> getLateReverbLevels() const;
    QVector<float> getDecayCoeffs() const;
    QVector<float> getAttackTimes() const;
    float getOnsetDelay() const;

protected:
    /**
     * The process function. Decodes all messages arriving through the "messageInput" parameter input port and sends
     * them through the parameter output "positionOutput".
     */
    void process() override;

private:
    bool _reverbEnabled = false;
    std::unique_ptr<visr::objectmodel::PointSource> mPs;
    std::unique_ptr<visr::objectmodel::PointSourceWithReverb> mPsReverb;

    std::unique_ptr<visr::pml::ObjectVector> scene;
    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ScalarParameter<float>> azPosInput;
    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ScalarParameter<float>> elPosInput;
    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ScalarParameter<float>> radiusPosInput;
    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ScalarParameter<int>> objectIDInput;
    float phi = 0.5;
    float theta = 0.5;
    float radius = 1.0;
    visr::ParameterOutput<visr::pml::DoubleBufferingProtocol, visr::pml::ObjectVector> mObjectVectorOut;
};

class ObjectRenderer : public visr::CompositeComponent {
public:
    explicit ObjectRenderer(visr::SignalFlowContext const& context,
        char const* name,
        CompositeComponent* parent,
        visr::panning::LoudspeakerArray const& loudspeakerConfiguration,
        std::size_t numberOfInputs,
        std::size_t numberOfOutputs,
        std::size_t interpolationPeriod,
        visr::efl::BasicMatrix<visr::SampleType> const& diffusionFilters,
        std::string const& trackingConfiguration,
        std::size_t numberOfObjectEqSections,
        std::string const& reverbConfig,
        bool frequencyDependentPanning);

    /**
     * Simplified constructor using default values for several parameters.
     */
    explicit ObjectRenderer(visr::SignalFlowContext const& context,
        char const* name,
        CompositeComponent* parent,
        visr::panning::LoudspeakerArray const& loudspeakerConfiguration,
        std::size_t numberOfInputs,
        std::size_t numberOfOutputs);

    ~ObjectRenderer();

    void updatePosition(double x, double y, double z);
    void reverbEnabled(bool enabled);
    void setLateReverbLevels(QVector<float> levels);

    void setNumberOfDiscreteReflections(int number);

    ObjectVectorEncoder* objectVectorEncoder() const;

private:
    bool _reverbEnabled = false;
    visr::AudioInput mInput;
    visr::AudioOutput mOutput;

    // CoreRenderer2 mCoreRenderer;
    visr::signalflows::CoreRenderer mCoreRenderer;
    std::unique_ptr<ObjectVectorEncoder> mObjectVectorEncoder;
    //    //    visr::rcl::UdpReceiver mSceneReceiver;
    //    //    visr::rcl::SceneDecoder mSceneDecoder;

    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ScalarParameter<float>> azPosInput;
    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ScalarParameter<float>> elPosInput;
    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ScalarParameter<float>> radiusPosInput;
    visr::ParameterInput<visr::pml::DoubleBufferingProtocol, visr::pml::ScalarParameter<int>> objectIDInput;
};

class Compressor : public visr::AtomicComponent {
public:
    using SampleType = visr::SampleType;

    /**
     * Constructor.
     * @param context Object containing standard signal processing parameters
     * (sampling rate, block size).
     * @param name
     * @param parent The containing component, or nullptr if this is the top-level component.
     * @param numberOfChannels The umber of channels that are processed
     * simultaneously.
     * @param compressorThresholdDB The level [in dB] where the compression sets in.
     * @param compressorSlope The slope of the compressor, in dB/octave.
     * @param averagingTimeSeconds,
     * @param attackTimeSeconds,
     * @param releaseTimeSeconds
     */
    Compressor(visr::SignalFlowContext& context,
        char const* name,
        visr::CompositeComponent* parent,
        std::size_t numberOfChannels, SampleType compressorThresholdDB,
        SampleType compressorSlope,
        SampleType averagingTimeSeconds,
        SampleType attackTimeSeconds,
        SampleType releaseTimeSeconds)
        : visr::AtomicComponent(context, name, parent)
        , mInput("in", *this, numberOfChannels)
        , mOutput("out", *this, numberOfChannels)
        , mAveragingCoefficient(timeConstantToCoefficient(averagingTimeSeconds))
        , mAttackCoefficient(timeConstantToCoefficient(attackTimeSeconds))
        , mReleaseCoefficient(timeConstantToCoefficient(releaseTimeSeconds))
        , mCompressorThreshold(compressorThresholdDB)
        , mCompressorSlope(compressorSlope)
        , mControlValues(context.period(), visr::cVectorAlignmentSamples)
        , mGainValues(context.period(), visr::cVectorAlignmentSamples)
        , mPastPeakValues(numberOfChannels, visr::cVectorAlignmentSamples)
        , mPastRmsValues(numberOfChannels, visr::cVectorAlignmentSamples)
    {
    }

    Compressor::SampleType timeConstantToCoefficient(SampleType timeConstant) const
    {
        SampleType const arg { -2.2f / timeConstant / static_cast<SampleType>(samplingFrequency()) };
        return static_cast<SampleType>(1.0) - std::exp(arg);
    }

    void
    peakFilterInplace(SampleType* const values,
        SampleType attackCoefficient,
        SampleType releaseCoefficient,
        std::size_t numberOfSamples,
        SampleType& state)
    {
        SampleType xp = state;
        for (std::size_t sampleIdx { 0 }; sampleIdx < numberOfSamples; ++sampleIdx) {
            SampleType const xc { values[sampleIdx] };
            SampleType const coeff { xc > xp ? attackCoefficient : releaseCoefficient };
            xp = (static_cast<SampleType>(1.0) - coeff) * xp + +coeff * xc;
            values[sampleIdx] = xp;
        }
        state = xp;
    }

    void
    averagingFilter(SampleType const* const input,
        SampleType* const output,
        SampleType filterCoeff,
        std::size_t numberOfSamples,
        SampleType& state)
    {
        SampleType const oneMinusC { static_cast<SampleType>(1.0) - filterCoeff };
        for (std::size_t sampleIdx { 0 }; sampleIdx < numberOfSamples; ++sampleIdx) {
            SampleType const xPwr = input[sampleIdx] * input[sampleIdx];
            state = oneMinusC * state + filterCoeff * xPwr;
            output[sampleIdx] = state;
        }
    }

    void process()
    {
        std::size_t const numberOfChannels { mInput.width() };
        std::size_t const numberOfSamples { period() };
        for (std::size_t chIdx { 0 }; chIdx < numberOfChannels; ++chIdx) {
            averagingFilter(mInput[chIdx], mControlValues.data(),
                mAveragingCoefficient,
                numberOfSamples, mPastRmsValues[chIdx]);
            // Linear -> log conversion
            // should be a vectorised library call
            // Note: The averaging filter returns the averaged signal power, that means we have to use factor 10 in the lin->log conversion.
            std::for_each(mControlValues.data(), mControlValues.data() + numberOfSamples,
                [](SampleType& val) { val = static_cast<SampleType>(10.0) * std::log10(val); });
            for (std::size_t sampleIdx { 0 }; sampleIdx < numberOfSamples; ++sampleIdx) {
                if (mControlValues[sampleIdx] > mCompressorThreshold) {
                    mControlValues[sampleIdx] = (mCompressorThreshold - mControlValues[sampleIdx]) * mCompressorSlope;
                } else {
                    mControlValues[sampleIdx] = static_cast<SampleType>(0.0);
                }
            }
            // Log-> linear conversion
            // This should be a vectorised function call (e.g., from libefl)
            // Note: these are amplitude values, hence the constant 0.05 = 1/20 in the conversion.
            std::for_each(mControlValues.data(), mControlValues.data() + numberOfSamples,
                [](SampleType& val) { val = std::pow(static_cast<SampleType>(10.0),
                                          static_cast<SampleType>(0.05) * val); });
            peakFilterInplace(mControlValues.data(), mAttackCoefficient, mReleaseCoefficient,
                numberOfSamples, mPastPeakValues[chIdx]);

#if 0
            // Output the linear control gain instead of the compressed signal
            visr::efl::ErrorCode const err = visr::efl::vectorCopy( mControlValues.data(),
                                                                   mOutput[chIdx],
                                                                   numberOfSamples,
                                                                   visr::cVectorAlignmentSamples );
#else
            visr::efl::ErrorCode const err = visr::efl::vectorMultiply(mInput[chIdx],
                mControlValues.data(),
                mOutput[chIdx],
                numberOfSamples,
                visr::cVectorAlignmentSamples);
#endif
            if (err != visr::efl::noError) {
                status(visr::StatusMessage::Error, "Error while multiplying control gain.",
                    visr::efl::errorMessage(err));
            }
        }
    }

private:
    visr::AudioInput mInput;

    visr::AudioOutput mOutput;

    SampleType mAveragingCoefficient;

    SampleType mAttackCoefficient;

    SampleType mReleaseCoefficient;

    SampleType mCompressorThreshold;

    SampleType mCompressorSlope;

    visr::efl::BasicVector<SampleType> mControlValues;

    visr::efl::BasicVector<SampleType> mGainValues;

    visr::efl::BasicVector<SampleType> mPastRmsValues;

    visr::efl::BasicVector<SampleType> mPastPeakValues;
};

VisrHelper::VisrHelper(QSharedPointer<Effect> effect, QObject* parent)
    : m_effect(effect)
    , QObject(parent)
{

    try {

        visr::pml::initialiseParameterLibrary();

        visr::efl::DenormalisedNumbers::State const oldDenormNumbersState
            = visr::efl::DenormalisedNumbers::setDenormHandling();

        auto periodSize = AudioManager::audioEngine()->framesPerPeriod();
        auto samplingFrequency = AudioManager::audioEngine()->processingSampleRate();

        _context = new visr::SignalFlowContext(periodSize, samplingFrequency);

        qDebug() << "LOADED0;";
    } catch (std::exception const& ex) {
        std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    }

    _effectParameters.append(EffectParameter {
        0, "PosX", "PosX", "", 1, 0.5, "0", 0.5, true, 0 });
    _effectParameters.append(EffectParameter {
        1, "PosY", "PosY", "", 1, 0.5, "0", 0.5, true, 1 });
    _effectParameters.append(EffectParameter {
        2, "PosZ", "PosZ", "", 1, 0.5, "0", 0.5, true, 2 });

    _speakerModel = QSharedPointer<SpeakerModel>(new SpeakerModel());

    QString speakerConfig = "stereo.xml";
    loadSpakerConfig(QUrl::fromLocalFile(speakerConfig).toString());
}

int VisrHelper::discreteReflectionsPerObject() const
{
    return _discreteReflectionsPerObject;
}

int VisrHelper::numberOfBiquadsos() const
{
    return _numberOfBiquadsos;
}

bool VisrHelper::loadedWithReverb() const
{
    return _loadedWithReverb;
}

VisrHelper::~VisrHelper()
{
    qDebug() << "VisrHelper::~VisrHelper()";
    _speakerModel.reset();
    m_speakerCount = 0;

}

void VisrHelper::process(visr::SampleType const* const* captureSamples,
    visr::SampleType* const* playbackSamples)
{

    _audioFlow->process(captureSamples, playbackSamples);
}

void VisrHelper::loadSpakerConfig(QString speakerConfig, bool frequencyDependentPanning)
{
    QUrl url(speakerConfig);

    qDebug() << url.toLocalFile();
    _loadSpeakerConfig = url.toLocalFile();
    boost::filesystem::path const arrayConfigFile(url.toLocalFile().toStdString());
    visr::panning::LoudspeakerArray loudspeakerArray;
    loudspeakerArray.loadXmlFile(arrayConfigFile.string());

    if (_baselineRender) {
        delete _baselineRender;
        _baselineRender = Q_NULLPTR;
    }
    if (_audioFlow) {
        delete _audioFlow;
        _audioFlow = Q_NULLPTR;
    }

    const std::size_t numberOfLoudspeakers = loudspeakerArray.getNumRegularSpeakers();
    const std::size_t numberOfSpeakersAndSubs = numberOfLoudspeakers + loudspeakerArray.getNumSubwoofers();
    m_speakerCount = numberOfSpeakersAndSubs;

    std::stringstream reverbConfig;

    std::string const trackingConfig("");
    std::size_t const period = 128;
    std::size_t const interpolationPeriod = 16 * period;
    std::size_t const diffusionFilterLength = 512;
    std::size_t const numInputEqSections = 0;
    visr::efl::BasicMatrix<visr::SampleType> const diffusionFilters(numberOfLoudspeakers, diffusionFilterLength);

    _baselineRender = new ObjectRenderer(*_context, "", nullptr, loudspeakerArray, 1, numberOfSpeakersAndSubs, interpolationPeriod,
        diffusionFilters, trackingConfig, numInputEqSections, reverbConfig.str(), frequencyDependentPanning);
    _audioFlow = new visr::rrl::AudioSignalFlow(*_baselineRender);

    _speakerModel->init(AudioManager::audioEngine()->channels(), speakerConfig);

    Q_EMIT speakerCountChanged();

    _loadedWithReverb = false;
}

void VisrHelper::loadSpakerConfigWithReverb(QString speakerConfig, int numReverbObjects, int discreteReflectionsPerObject,
    double lateFilterLengthSeconds, QString lateDiffusionFilters, double maximumDiscreteReflectionDelay, bool frequencyDependentPanning)
{
    QUrl url(speakerConfig);

    qDebug() << url.toLocalFile();
    _loadSpeakerConfig = url.toLocalFile();
    boost::filesystem::path const arrayConfigFile(url.toLocalFile().toStdString());
    visr::panning::LoudspeakerArray loudspeakerArray;
    loudspeakerArray.loadXmlFile(arrayConfigFile.string());

    if (_baselineRender) {
        delete _baselineRender;
        _baselineRender = Q_NULLPTR;
    }
    if (_audioFlow) {
        delete _audioFlow;
        _audioFlow = Q_NULLPTR;
    }

    const std::size_t numberOfLoudspeakers = loudspeakerArray.getNumRegularSpeakers();
    const std::size_t numberOfSpeakersAndSubs = numberOfLoudspeakers + loudspeakerArray.getNumSubwoofers();
    m_speakerCount = numberOfSpeakersAndSubs;
    qDebug() << "numberOfSpeakersAndSubs" << numberOfSpeakersAndSubs;

    std::stringstream reverbConfig;
    reverbConfig << "{ \"numReverbObjects\": " << numReverbObjects
                 << ", \"discreteReflectionsPerObject\": " << discreteReflectionsPerObject
                 << ", \"lateReverbFilterLength\": " << lateFilterLengthSeconds
                 << ", \"maxDiscreteReflectionDelay\": " << maximumDiscreteReflectionDelay
                 << ", \"lateReverbDecorrelationFilters\": \"" << lateDiffusionFilters.toStdString()
                 << "\" }";

    _discreteReflectionsPerObject = maximumDiscreteReflectionDelay;
    std::string const trackingConfig("");
    std::size_t const period = 128;
    std::size_t const interpolationPeriod = 16 * period;
    std::size_t const diffusionFilterLength = 512;
    std::size_t const numInputEqSections = 0;
    visr::efl::BasicMatrix<visr::SampleType> const diffusionFilters(numberOfLoudspeakers, diffusionFilterLength);

    _baselineRender = new ObjectRenderer(*_context, "", nullptr, loudspeakerArray, 1, numberOfSpeakersAndSubs, interpolationPeriod,
        diffusionFilters, trackingConfig, numInputEqSections, reverbConfig.str(), frequencyDependentPanning);

    setReverbEnabled(true);
    _audioFlow = new visr::rrl::AudioSignalFlow(*_baselineRender);

    _speakerModel->init(AudioManager::audioEngine()->channels(), speakerConfig);

    _baselineRender->setNumberOfDiscreteReflections(1);
    QVector<float> levels = { 0.02522, 0.01052, 0.01657, 0.02744, 0.02058, 0.01679, 0.01698, 0.01433, 0.00041 };
    setLateReverbLevels(levels);

    QVector<float> decay = { -4.50698, -5.02028, -5.75817, -5.36509, -5.42654, -5.62316, -5.75298, -6.41075, -11.13465 };
    setDecayCoeffs(decay);

    QVector<float> attactime = { 0.01321, 0.01321, 0.01321, 0.01321, 0.01321, 0.01321, 0.01321, 0.01321, 0.01321 };
    setAttackTimes(attactime);

    setOnsetDelay(0.00931);

    Q_EMIT speakerCountChanged();

    _loadedWithReverb = true;
}

int VisrHelper::speakerCount()
{
    return m_speakerCount;
}

void VisrHelper::setMovePosition(double x, double y, double z)
{

    _baselineRender->updatePosition(x, y, z);
}

QString VisrHelper::speakerConfig()
{
    return _loadSpeakerConfig;
}

QVector<VisrHelper::EffectParameter> VisrHelper::effectParameters() const
{
    return _effectParameters;
}

QSharedPointer<Effect> VisrHelper::effect() const
{
    return m_effect;
}

QSharedPointer<SpeakerModel> VisrHelper::speakerModel() const
{
    return _speakerModel;
}

bool VisrHelper::enabled() const
{
    return _enabled;
}

void VisrHelper::setEnabled(bool newEnabled)
{
    _enabled = newEnabled;
}

bool VisrHelper::reverbEnabled() const
{
    return _reverbEnabled;
}

void VisrHelper::setReverbEnabled(bool newEnabled)
{
    _reverbEnabled = newEnabled;
    _baselineRender->reverbEnabled(newEnabled);
    Q_EMIT reverbEnabledChanged();
}

void VisrHelper::setLateReverbLevels(QVector<float> levels)
{
    _baselineRender->setLateReverbLevels(levels);
}

void VisrHelper::setDecayCoeffs(QVector<float> levels)
{
    _baselineRender->objectVectorEncoder()->setDecayCoeffs(levels);
}

void VisrHelper::setAttackTimes(QVector<float> levels)
{
    _baselineRender->objectVectorEncoder()->setAttackTimes(levels);
}

void VisrHelper::setOnsetDelay(float onsetDelay)
{
    _baselineRender->objectVectorEncoder()->setOnsetDelay(onsetDelay);
}

QVector<float> VisrHelper::getLateReverbLevels() const
{
    return _baselineRender->objectVectorEncoder()->getLateReverbLevels();
}

QVector<float> VisrHelper::getDecayCoeffs() const
{
    return _baselineRender->objectVectorEncoder()->getDecayCoeffs();
}

QVector<float> VisrHelper::getAttackTimes() const
{
    return _baselineRender->objectVectorEncoder()->getAttackTimes();
}

float VisrHelper::getOnsetDelay() const
{
    return _baselineRender->objectVectorEncoder()->getOnsetDelay();
}

void VisrHelper::setDiscreteReflectionDelay(int index, float delay)
{
    _baselineRender->objectVectorEncoder()->setDiscreteReflectionDelay(index, delay);
}

void VisrHelper::setDiscreteReflectionLevel(int index, float level)
{
    _baselineRender->objectVectorEncoder()->setDiscreteReflectionLevel(index, level);
}

void VisrHelper::setDiscreteReflectionPosition(int index, float x, float y, float z)
{
    _baselineRender->objectVectorEncoder()->setDiscreteReflectionPosition(index, x, y, z);
}

void VisrHelper::setDiscreteReflectionFilter(int index, int biquadIdx, float b0, float b1, float b2, float a1, float a2)
{
    _baselineRender->objectVectorEncoder()->setDiscreteReflectionFilter(index, biquadIdx, b0, b1, b2, a1, a2);
}

float VisrHelper::getDiscreteReflectionDelay(int index)
{
    return _baselineRender->objectVectorEncoder()->getDiscreteReflectionLevel(index);
}

float VisrHelper::getDiscreteReflectionLevel(int index)
{
    return _baselineRender->objectVectorEncoder()->getDiscreteReflectionLevel(index);
}

void VisrHelper::getDiscreteReflectionPosition(int index, float& x, float& y, float& z)
{
    _baselineRender->objectVectorEncoder()->getDiscreteReflectionPosition(index, x, y, z);
}

void VisrHelper::getDiscreteReflectionFilter(int index, int biquadIdx, float& b0, float& b1, float& b2, float& a1, float& a2)
{
    _baselineRender->objectVectorEncoder()->getDiscreteReflectionFilter(index, biquadIdx, b0, b1, b2, a1, a2);
}

void VisrHelper::getDiscreteReflectionFilterNumber(int index)
{
}

ObjectRenderer::ObjectRenderer(visr::SignalFlowContext const& context,
    char const* name,
    CompositeComponent* parent,
    visr::panning::LoudspeakerArray const& loudspeakerConfiguration,
    std::size_t numberOfInputs,
    std::size_t numberOfOutputs)
    : ObjectRenderer(context, name, parent, loudspeakerConfiguration, numberOfInputs, numberOfOutputs,
        4096ul,
        visr::efl::BasicMatrix<visr::SampleType>(numberOfOutputs, 512, visr::cVectorAlignmentSamples),
        std::string(), 1 /*number of EQ sections*/, std::string(), false)
{
}

ObjectRenderer::ObjectRenderer(visr::SignalFlowContext const& context,
    char const* name,
    CompositeComponent* parent,
    visr::panning::LoudspeakerArray const& loudspeakerConfiguration,
    std::size_t numberOfInputs,
    std::size_t numberOfOutputs,
    std::size_t interpolationPeriod,
    visr::efl::BasicMatrix<visr::SampleType> const& diffusionFilters,
    std::string const& trackingConfiguration,
    std::size_t numberOfObjectEqSections,
    std::string const& reverbConfig,
    bool frequencyDependentPanning)
    : CompositeComponent(context, name, parent)
    , mCoreRenderer(context, "CoreRenderer", this, loudspeakerConfiguration, numberOfInputs, numberOfOutputs,
          interpolationPeriod, diffusionFilters, trackingConfiguration, numberOfObjectEqSections,
          reverbConfig, frequencyDependentPanning)
    , mInput("input", *this, numberOfInputs)
    , mOutput("output", *this, numberOfOutputs)
    , azPosInput("azPositionInput", *this, visr::pml::EmptyParameterConfig())
    , elPosInput("elPositionInput", *this, visr::pml::EmptyParameterConfig())
    , radiusPosInput("radiusPositionInput", *this, visr::pml::EmptyParameterConfig())
    , objectIDInput("objID", *this, visr::pml::EmptyParameterConfig())
{
    audioConnection(mInput, mCoreRenderer.audioPort("audioIn"));
    audioConnection(mCoreRenderer.audioPort("audioOut"), mOutput);

    mObjectVectorEncoder.reset(new ObjectVectorEncoder(context, "ObjectVectorEncoder", this));
    mObjectVectorEncoder->setup();
    parameterConnection(azPosInput, mObjectVectorEncoder->parameterPort("azPosInput"));
    parameterConnection(elPosInput, mObjectVectorEncoder->parameterPort("elPosInput"));
    parameterConnection(radiusPosInput, mObjectVectorEncoder->parameterPort("radiusPosInput"));
    parameterConnection(objectIDInput, mObjectVectorEncoder->parameterPort("objectIDInput"));
    parameterConnection(mObjectVectorEncoder->parameterPort("objectVectorOut"), mCoreRenderer.parameterPort("objectDataInput"));
}

ObjectRenderer::~ObjectRenderer()
{
}

void ObjectRenderer::updatePosition(double x, double y, double z)
{
    mObjectVectorEncoder->updatePosition(x, y, z);
}

void ObjectRenderer::reverbEnabled(bool enabled)
{
    _reverbEnabled = enabled;
    if (enabled) {
        mObjectVectorEncoder->setupReverb();
    } else {
        mObjectVectorEncoder->setup();
    }
}

void ObjectRenderer::setLateReverbLevels(QVector<float> levels)
{
    mObjectVectorEncoder->setLateReverbLevels(levels);
}

void ObjectRenderer::setNumberOfDiscreteReflections(int number)
{
    mObjectVectorEncoder->setNumberOfDiscreteReflections(number);
}

ObjectVectorEncoder* ObjectRenderer::objectVectorEncoder() const
{
    return mObjectVectorEncoder.get();
}

ObjectVectorEncoder::ObjectVectorEncoder(visr::SignalFlowContext const& context,
    char const* name,
    visr::CompositeComponent* parent /*= nullptr*/)
    : AtomicComponent(context, name, parent)
    , azPosInput("azPosInput", *this, visr::pml::EmptyParameterConfig())
    , elPosInput("elPosInput", *this, visr::pml::EmptyParameterConfig())
    , radiusPosInput("radiusPosInput", *this, visr::pml::EmptyParameterConfig())
    , objectIDInput("objectIDInput", *this, visr::pml::EmptyParameterConfig())
    , mObjectVectorOut("objectVectorOut", *this, visr::pml::EmptyParameterConfig())
{
}

ObjectVectorEncoder::~ObjectVectorEncoder()
{
}

void ObjectVectorEncoder::setup()
{

    _reverbEnabled = false;
    mPs.reset(new visr::objectmodel::PointSource(0));
    mPs->resetNumberOfChannels(1);

    mPs->setChannelIndex(0, mPs->id());

    // mPs->setChannelIndex(1, mPs->id());
    mPs->setX(0.f);
    mPs->setY(0.f);
    mPs->setZ(0.f);
    mPs->setLevel(0.5);

    //    visr::rbbl::ParametricIirCoefficientList<visr::objectmodel::Object::Coordinate> d;
    //    d.resize(1);
    //    d[0] = (visr::objectmodel::Object::Coordinate)1;
    //    mPs->setEqCoefficients(d);

    mPs->setGroupId(5);
    mPs->setPriority(5);

    scene.reset(new visr::pml::ObjectVector());
    scene->clear();
    scene->insert(*mPs);
}

void ObjectVectorEncoder::setupReverb()
{
    _reverbEnabled = true;
    mPsReverb.reset(new visr::objectmodel::PointSourceWithReverb(0));
    mPsReverb->resetNumberOfChannels(1);

    mPsReverb->setChannelIndex(0, mPs->id());

    // mPs->setChannelIndex(1, mPs->id());
    mPsReverb->setX(0.f);
    mPsReverb->setY(0.f);
    mPsReverb->setZ(0.f);
    mPsReverb->setLevel(0.5);

    //    visr::rbbl::ParametricIirCoefficientList<visr::objectmodel::Object::Coordinate> d;
    //    d.resize(1);
    //    d[0] = (visr::objectmodel::Object::Coordinate)1;
    //    mPs->setEqCoefficients(d);

    mPsReverb->setGroupId(5);
    mPsReverb->setPriority(5);

    scene.reset(new visr::pml::ObjectVector());
    scene->clear();
    scene->insert(*mPs);
}

void ObjectVectorEncoder::updatePosition(double x, double y, double z)
{

    if (_reverbEnabled) {

        mPsReverb->setX(x);
        mPsReverb->setY(y);
        mPsReverb->setZ(z);

        scene->clear();
        scene->insert(*mPsReverb);

        mObjectVectorOut.data() = *scene.get();
        mObjectVectorOut.swapBuffers();
    } else {

        mPs->setX(x);
        mPs->setY(y);
        mPs->setZ(z);

        scene->clear();
        scene->insert(*mPs);

        mObjectVectorOut.data() = *scene.get();
        mObjectVectorOut.swapBuffers();
    }
}

void ObjectVectorEncoder::setLateReverbLevels(QVector<float> levels)
{

    visr::objectmodel::PointSourceWithReverb::LateReverbCoeffs new_levels;
    auto vector = levels.toStdVector();
    std::copy(vector.begin(), vector.end(), new_levels.begin());

    mPsReverb->lateReverb().setLevels(new_levels);
}

void ObjectVectorEncoder::setOnsetDelay(float delay)
{
    mPsReverb->lateReverb().setOnsetDelay(delay);
}

void ObjectVectorEncoder::setNumberOfDiscreteReflections(int number)
{
    mPsReverb->setNumberOfDiscreteReflections(number);
}

void ObjectVectorEncoder::setDiscreteReflectionDelay(int index, float delay)
{
    mPsReverb->discreteReflection(index).setDelay(delay);
}

void ObjectVectorEncoder::setDiscreteReflectionLevel(int index, float level)
{

    mPsReverb->discreteReflection(index).setLevel(level);
}

void ObjectVectorEncoder::setDiscreteReflectionPosition(int index, float x, float y, float z)
{
    mPsReverb->discreteReflection(index).setPosition(x, y, z);
}

void ObjectVectorEncoder::setDiscreteReflectionFilter(int index, int biquadIdx, float b0, float b1, float b2, float a1, float a2)
{
    visr::rbbl::BiquadCoefficient<float> new_filter(b0, b1, b2, a1, a2);

    mPsReverb->discreteReflection(index).setReflectionFilter(biquadIdx, new_filter);
}

float ObjectVectorEncoder::getDiscreteReflectionDelay(int index)
{
    return mPsReverb->discreteReflection(index).delay();
}

float ObjectVectorEncoder::getDiscreteReflectionLevel(int index)
{

    return mPsReverb->discreteReflection(index).level();
}

void ObjectVectorEncoder::getDiscreteReflectionPosition(int index, float& x, float& y, float& z)
{
    auto discreteReflection = mPsReverb->discreteReflection(index);
    x = discreteReflection.positionX();
    y = discreteReflection.positionY();
    z = discreteReflection.positionZ();
}

void ObjectVectorEncoder::getDiscreteReflectionFilter(int index, int biquadIdx, float& b0, float& b1, float& b2, float& a1, float& a2)
{
    auto discreteReflection = mPsReverb->discreteReflection(index).reflectionFilter(biquadIdx);
    b0 = discreteReflection.b0();
    b1 = discreteReflection.b1();
    b2 = discreteReflection.b2();
    a1 = discreteReflection.a1();
    a2 = discreteReflection.a2();
}

int ObjectVectorEncoder::getDiscreteReflectionFilterNumber(int index)
{
    return mPsReverb->discreteReflection(index).reflectionFilters().size();
}

QVector<float> ObjectVectorEncoder::getLateReverbLevels() const
{
    std::vector<float> myvector { mPsReverb->lateReverb().levels().begin(), mPsReverb->lateReverb().levels().end() };
    return QVector<float>::fromStdVector(myvector);
}

QVector<float> ObjectVectorEncoder::getDecayCoeffs() const
{
    std::vector<float> myvector { mPsReverb->lateReverb().decayCoeffs().begin(), mPsReverb->lateReverb().decayCoeffs().end() };
    return QVector<float>::fromStdVector(myvector);
}

QVector<float> ObjectVectorEncoder::getAttackTimes() const
{

    std::vector<float> myvector { mPsReverb->lateReverb().attackTimes().begin(), mPsReverb->lateReverb().attackTimes().end() };
    return QVector<float>::fromStdVector(myvector);
}

float ObjectVectorEncoder::getOnsetDelay() const
{
    return mPsReverb->lateReverb().onsetDelay();
}

void ObjectVectorEncoder::setDecayCoeffs(QVector<float> levels)
{
    visr::objectmodel::PointSourceWithReverb::LateReverbCoeffs new_levels;

    auto vector = levels.toStdVector();
    std::copy(vector.begin(), vector.end(), new_levels.begin());

    mPsReverb->lateReverb().setDecayCoeffs(new_levels);
}

void ObjectVectorEncoder::setAttackTimes(QVector<float> levels)
{
    visr::objectmodel::PointSourceWithReverb::LateReverbCoeffs new_levels;
    auto vector = levels.toStdVector();
    std::copy(vector.begin(), vector.end(), new_levels.begin());

    mPsReverb->lateReverb().setAttackTimes(new_levels);
}

void ObjectVectorEncoder::process()
{
    if (azPosInput.changed() || elPosInput.changed() || radiusPosInput.changed() || objectIDInput.changed()) {
        if (azPosInput.changed()) {
            phi = visr::efl::degree2radian(azPosInput.data().value());
        }

        if (elPosInput.changed()) {
            theta = visr::efl::degree2radian(elPosInput.data().value());
        }

        if (radiusPosInput.changed()) {
            radius = (radiusPosInput.data().value());
        }
        if (objectIDInput.changed()) {
            int newIDandChan = objectIDInput.data().value() - 1;

            if (_reverbEnabled) {
                mPsReverb.reset(new visr::objectmodel::PointSourceWithReverb(newIDandChan));
                mPsReverb->resetNumberOfChannels(1);
                mPsReverb->setChannelIndex(0, newIDandChan);

                mPsReverb->setLevel(0.5);
                mPsReverb->setGroupId(5);
                mPsReverb->setPriority(5);
            } else {
                mPs.reset(new visr::objectmodel::PointSource(newIDandChan));
                mPs->resetNumberOfChannels(1);
                mPs->setChannelIndex(0, newIDandChan);

                mPs->setLevel(0.5);
                mPs->setGroupId(5);
                mPs->setPriority(5);
            }
        }
        try {
            visr::objectmodel::Object::Coordinate xPos, yPos, zPos;
            std::tie(xPos, yPos, zPos) = visr::efl::spherical2cartesian(phi, theta, radius);

            if (_reverbEnabled) {

                mPsReverb->setX(xPos);
                mPsReverb->setY(yPos);
                mPsReverb->setZ(zPos);
                scene->clear();
                scene->insert(*mPsReverb);
                azPosInput.resetChanged();
                elPosInput.resetChanged();
                radiusPosInput.resetChanged();
                objectIDInput.resetChanged();
            } else {

                mPs->setX(xPos);
                mPs->setY(yPos);
                mPs->setZ(zPos);
                scene->clear();
                scene->insert(*mPs);
                azPosInput.resetChanged();
                elPosInput.resetChanged();
                radiusPosInput.resetChanged();
                objectIDInput.resetChanged();
            }

        } catch (std::exception const& ex) {
            // Don't abort the program when receiving a corrupted message.
            std::cerr << "ObjectVectorEncoder: Error while decoding a position message: " << ex.what() << std::endl;
        }

        mObjectVectorOut.data() = *scene.get();
        mObjectVectorOut.swapBuffers();
    }
}
