#ifndef VISRHELPER_H
#define VISRHELPER_H

#undef max
#undef min

#include <QDebug>
#include <QObject>
#include <QTimer>
#include <libefl/denormalised_number_handling.hpp>
#include <libpml/initialise_parameter_library.hpp>

#include <libefl/basic_matrix.hpp>
#include <librrl/audio_signal_flow.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libaudiointerfaces/audio_interface.hpp>
#include <libaudiointerfaces/audio_interface_factory.hpp>
#include <libsignalflows/baseline_renderer.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/parameter_input.hpp>
#include <libvisr/parameter_output.hpp>

#include <libobjectmodel/point_source.hpp>
#include <libobjectmodel/point_source_with_reverb.hpp>
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/scalar_parameter.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/composite_component.hpp>

#include <librcl/add.hpp>
#include <librcl/scene_decoder.hpp>
#include <librcl/udp_receiver.hpp>

#include <libefl/basic_matrix.hpp>

#include <QAbstractListModel>
#include <QUrl>
#include <ciso646>
#include <cstddef>
#include <cstdio> // for getc(), for testing purposes
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <libpml/empty_parameter_config.hpp>
#include <memory>
#include <sstream>

class ObjectRenderer;
class Compressor;
class MetadataExposingRenderer;
class Effect;

class SpeakerModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int enabledCount READ enabledCount NOTIFY enabledCountChanged)
public:
    enum SpeakerModelRole {
        ROLE_SpeakerIndex = Qt::UserRole + 1,
        ROLE_SpeakerEnabled,
        ROLE_SpeakerChecked,
        ROLE_SpeakerName,
    };
    Q_ENUM(SpeakerModelRole)

    explicit SpeakerModel(QObject* parent = Q_NULLPTR)
        : QAbstractListModel(parent)
    {
        m_roleNames[ROLE_SpeakerIndex] = "SpeakerIndex";
        m_roleNames[ROLE_SpeakerEnabled] = "SpeakerEnabled";
        m_roleNames[ROLE_SpeakerChecked] = "SpeakerChecked";
        m_roleNames[ROLE_SpeakerName] = "SpeakerName";
    }
    void init(int maxSpeaker, QString loadSpeakerConfig)
    {

        QUrl url(loadSpeakerConfig);
        boost::filesystem::path const arrayConfigFile(url.toLocalFile().toStdString());
        visr::panning::LoudspeakerArray loudspeakerArray;

        loudspeakerArray.loadXmlFile(arrayConfigFile.string());

        const std::size_t numberOfLoudspeakers = loudspeakerArray.getNumRegularSpeakers();
        const std::size_t numberOfSpeakersAndSubs = numberOfLoudspeakers + loudspeakerArray.getNumSubwoofers();

        Q_EMIT beginResetModel();

        _speakers.clear();
        int counter = 0;
        for (int i = 0; i < numberOfLoudspeakers; i++) {
            _speakers.append(Speaker { counter, QString::fromStdString(loudspeakerArray.loudspeakerId(i)), true, false });
            counter++;
            if (counter >= maxSpeaker) {
                counter = maxSpeaker - 1;
            }
        }
        for (int i = 0; i < loudspeakerArray.getNumSubwoofers(); i++) {
            _speakers.append(Speaker { int(counter), "Subs " + QString::number(i), true, false });
            counter++;
            if (counter >= maxSpeaker) {
                counter = maxSpeaker - 1;
            }
        }
        Q_EMIT endResetModel();
    }
    int isChecked(int index)
    {
        if (index >= 0 && index < _speakers.size()) {
            return _speakers[index].checked;
        }
    }
    int getIndex(int index)
    {
        if (index >= 0 && index < _speakers.size()) {
            return _speakers[index].index;
        }
    }
    Q_INVOKABLE int enabledCount()
    {
        int enabled_counter = 0;
        for (int i = 0; i < _speakers.size(); i++) {
            if (_speakers[i].enabled) {
                enabled_counter++;
            }
        }
        return enabled_counter;
    }

    struct Speaker {
        int index;
        QString name;
        bool enabled;
        bool checked;
    };

private:
    QList<Speaker> _speakers;
    QHash<int, QByteArray> m_roleNames;

signals:
    void enabledCountChanged();

protected:
    bool setData(const QModelIndex& index, const QVariant& value, int role)
    {

        if (_speakers.size() == 0 || index.row() < 0 || index.row() > _speakers.size())
            return false;

        if (index.isValid() && role == ROLE_SpeakerIndex) {
            _speakers[index.row()].index = value.toInt();
        }
        if (index.isValid() && role == ROLE_SpeakerChecked) {

            _speakers[index.row()].checked = value.toBool();

            int checked_counter = 0;
            for (int i = 0; i < _speakers.size(); i++) {
                if (_speakers[i].checked) {
                    checked_counter++;
                }
            }

            if (checked_counter >= 2) {
                for (int i = 0; i < _speakers.size(); i++) {
                    if (_speakers[i].checked == false) {
                        _speakers[i].enabled = false;
                        Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_SpeakerEnabled);
                    }
                }
            } else {
                for (int i = 0; i < _speakers.size(); i++) {
                    _speakers[i].enabled = true;
                    Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_SpeakerEnabled);
                }
            }

            Q_EMIT enabledCountChanged();

            return true;
        }

        return false;
    }
    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }

    virtual int rowCount(const QModelIndex& parent) const
    {
        return _speakers.size();
    }

    virtual QVariant data(const QModelIndex& index, int role) const
    {

        if (!index.isValid())
            return QVariant();

        if (_speakers.size() == 0 || index.row() < 0)
            return QVariant();

        auto value = _speakers[index.row()];

        switch (role) {
        case ROLE_SpeakerIndex:
            return value.index;
        case ROLE_SpeakerEnabled:
            return value.enabled;
        case ROLE_SpeakerChecked:
            return value.checked;
        case ROLE_SpeakerName:
            return value.name;
        }
        return QVariant();
    }
};

class VisrHelper : public QObject {
    Q_OBJECT

    Q_PROPERTY(int speakerCount READ speakerCount NOTIFY speakerCountChanged)

    Q_PROPERTY(bool reverbEnabled READ reverbEnabled WRITE setReverbEnabled NOTIFY reverbEnabledChanged)
public:
    struct EffectParameter {
        qint32 _id;
        QString _title;
        QString _name;
        QString _units;
        qint32 _stepCount;
        double _defaultNormalizedValue;
        QString _value;
        double _normalized;
        bool _active;
        int _index;
    };

    VisrHelper(QSharedPointer<Effect> effect, QObject* parent = Q_NULLPTR);
    ~VisrHelper();

    void process(const visr::SampleType* const* captureSamples, visr::SampleType* const* playbackSamples);

    void loadSpakerConfig(QString speakerConfig, bool frequencyDependentPanning = false);
    void loadSpakerConfigWithReverb(QString speakerConfig, int numReverbObjects, int discreteReflectionsPerObject,
        double lateFilterLengthSeconds, QString lateDiffusionFilters, double maximumDiscreteReflectionDelay,
        bool frequencyDependentPanning = false);

    int speakerCount();

    void setMovePosition(double x, double y, double z);

    QString speakerConfig();

    QVector<EffectParameter> effectParameters() const;

    QSharedPointer<Effect> effect() const;

    QSharedPointer<SpeakerModel> speakerModel() const;

    bool enabled() const;
    void setEnabled(bool newEnabled);

    bool reverbEnabled() const;
    void setReverbEnabled(bool newEnabled);

    void setLateReverbLevels(QVector<float> levels);
    void setDecayCoeffs(QVector<float> levels);
    void setAttackTimes(QVector<float> levels);
    void setOnsetDelay(float onsetDelay);

    QVector<float> getLateReverbLevels() const;
    QVector<float> getDecayCoeffs() const;
    QVector<float> getAttackTimes() const;
    float getOnsetDelay() const;

    void setDiscreteReflectionDelay(int index, float delay);
    void setDiscreteReflectionLevel(int index, float level);
    void setDiscreteReflectionPosition(int index, float x, float y, float z);
    void setDiscreteReflectionFilter(int index, int biquadIdx, float b0, float b1, float b2, float a1, float a2);

    float getDiscreteReflectionDelay(int index);
    float getDiscreteReflectionLevel(int index);
    void getDiscreteReflectionPosition(int index, float &x, float &y, float &z);
    void getDiscreteReflectionFilter(int index, int biquadIdx, float& b0, float& b1, float& b2, float& a1, float& a2);
    void getDiscreteReflectionFilterNumber(int index);

    int discreteReflectionsPerObject() const;
    int numberOfBiquadsos() const;

    bool loadedWithReverb() const;

signals:
    void speakerCountChanged();
    void reverbEnabledChanged();

private:
    bool _loadedWithReverb = false;
    int _numberOfBiquadsos = 1;
    int _discreteReflectionsPerObject = 1;

    bool _reverbEnabled = false;
    bool _enabled = false;
    QSharedPointer<SpeakerModel> _speakerModel;
    QSharedPointer<Effect> m_effect;
    QVector<EffectParameter> _effectParameters;
    // MetadataExposingRenderer* _baselineRender;
    ObjectRenderer* _baselineRender = Q_NULLPTR;
    // Compressor* _baselineRender;
    //  visr::signalflows::BaselineRenderer* _baselineRender;
    visr::rrl::AudioSignalFlow* _audioFlow = Q_NULLPTR;
    visr::SignalFlowContext* _context = Q_NULLPTR;

    QString _loadSpeakerConfig;

    int m_speakerCount = 0;
};

#endif // VISRHELPER_H
