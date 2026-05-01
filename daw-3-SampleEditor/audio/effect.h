#ifndef EFFECT_H
#define EFFECT_H

#include "audioengine.h"
#include "engine.h"
#include "plugin.h"
#include <QAbstractItemModel>
#include <QObject>
#include <core/automation.h>
#include <ui/automationitem.h>
class EffectChain;
class Plugin;
class EffectControls;

class Effect;

class Effect : public Plugin {
    Q_OBJECT
    Q_PROPERTY(EffectControls* controls READ getControls NOTIFY controlsChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)

    Q_PROPERTY(float rightWetLevel READ rightWetLevel WRITE setRightWetLevel NOTIFY rightWetLevelChanged)
    Q_PROPERTY(float leftWetLevel READ leftWetLevel WRITE setLeftWetLevel NOTIFY leftWetLevelChanged)

    Q_PROPERTY(bool rightWetDryEnabled READ rightWetDryEnabled WRITE setRightWetDryEnabled NOTIFY rightWetDryEnabledChanged)
    Q_PROPERTY(bool leftWetDryEnabled READ leftWetDryEnabled WRITE setLeftWetDryEnabled NOTIFY leftWetDryEnabledChanged)

public:
    Effect(const Plugin::Descriptor* _desc,
        const Plugin::Descriptor::SubPluginFeatures::Key* _key,
        QObject* _parent);

    ~Effect() override;

    virtual QString name() const = 0;

    virtual QString type() const = 0;

    virtual QSharedPointer<EffectControls> controls() const = 0;
    virtual EffectControls* getControls() const = 0;

    virtual bool initialize(int trackIndex, QSharedPointer<Effect> effect) = 0;

    virtual bool processAudioBuffer(sampleFrame* _buf)
        = 0;

    Q_INVOKABLE virtual void showControl()
    {
    }
    Q_INVOKABLE virtual void hideControl()
    {
    }

    Q_INVOKABLE void setConfigure(bool configure)
    {
        m_configure = configure;
    }

    Q_INVOKABLE inline bool configure() const
    {
        return m_configure;
    }

    inline ch_cnt_t processorCount() const
    {
        return m_processors;
    }

    inline void setProcessorCount(ch_cnt_t _processors)
    {
        m_processors = _processors;
    }

    inline bool isOkay() const
    {
        return m_okay;
    }

    inline void setOkay(bool _state)
    {
        m_okay = _state;
    }

    inline bool isRunning() const
    {
        return m_running;
    }

    inline void startRunning()
    {
        m_bufferCount = 0;
        m_running = true;
    }

    inline void stopRunning()
    {
        m_running = false;
    }

    inline bool isEnabled() const
    {
        return m_enabled;
    }

    inline f_cnt_t timeout() const
    {
        const float samples = AudioManager::audioEngine()->processingSampleRate() * m_autoQuitModel / 1000.0f;
        return 1 + (static_cast<int>(samples) / AudioManager::audioEngine()->framesPerPeriod());
    }

    inline float leftWetLevel() const
    {
        return m_leftWetLevel;
    }
    inline void setLeftWetLevel(float level)
    {
        int temp = level * 100;
        m_leftWetLevel = temp / 100.0;
        Q_EMIT leftWetLevelChanged();
    }

    inline float leftDryLevel() const
    {
        return 1.0f - m_leftWetLevel;
    }

    inline float rightWetLevel() const
    {
        return m_rightWetLevel;
    }

    inline float rightDryLevel() const
    {
        return 1.0f - m_rightWetLevel;
    }

    inline void setRightWetLevel(float level)
    {
        int temp = level * 100;
        m_rightWetLevel = temp / 100.0;
        Q_EMIT rightWetLevelChanged();
    }

    inline bool leftWetDryEnabled() const
    {
        return m_leftWetDryEnabled;
    }

    inline bool rightWetDryEnabled() const
    {
        return m_rightWetDryEnabled;
    }

    inline void setLeftWetDryEnabled(bool enabled)
    {
        m_leftWetDryEnabled = enabled;
        Q_EMIT leftWetDryEnabledChanged();
    }

    inline void setRightWetDryEnabled(bool enabled)
    {
        m_rightWetDryEnabled = enabled;
        Q_EMIT rightWetDryEnabledChanged();
    }

    inline float gate() const
    {
        const float level = m_gateModel;
        return level * level * m_processors;
    }

    inline f_cnt_t bufferCount() const
    {
        return m_bufferCount;
    }

    inline void resetBufferCount()
    {
        m_bufferCount = 0;
    }

    inline void incrementBufferCount()
    {
        ++m_bufferCount;
    }

    inline bool dontRun() const
    {
        return m_noRun;
    }

    inline void setDontRun(bool _state)
    {
        m_noRun = _state;
    }

    virtual bool initialPlugin(QSharedPointer<Effect> effect)
    {
        return false;
    }

    virtual bool controlVisible() = 0;

    virtual QSharedPointer<Automation> automation(QString name) = 0;

    virtual QSharedPointer<AutomationItem> automationItem(QString name) = 0;

    virtual bool contains(QString name) = 0;

    Q_INVOKABLE virtual bool isAutomated(QString name) = 0;

    virtual void setEnabled(bool newEnabled)
    {
        m_enabled = newEnabled;
    }

    Q_INVOKABLE virtual bool hasExternalControl() = 0;

    virtual QJsonObject getLastStatus()
    {
        return QJsonObject();
    }

    virtual void setLastStatus(QJsonObject status)
    {
        return;
    }

    virtual QJsonObject performOperation(QJsonObject obj)
    {
        return QJsonObject();
    }
    virtual void saveUndoOperation()
    {
        return;
    }
    virtual void saveRedoOperation()
    {
        return;
    }

    int effectIndex() const;
    void setEffectIndex(int newEffectIndex);

protected:
    bool m_controlVisible = false;
    bool m_enabled = true;

private:
    int m_effectIndex = -1;
    bool m_rightWetDryEnabled = true;
    bool m_leftWetDryEnabled = true;

    ch_cnt_t m_processors;

    bool m_okay;
    bool m_noRun;
    bool m_running;
    bool m_configure = false;
    f_cnt_t m_bufferCount;

    float m_leftWetLevel;
    float m_rightWetLevel;

    float m_gateModel;

    bool m_autoQuitDisabled;
    float m_autoQuitModel;

signals:
    void rightWetLevelChanged();
    void leftWetLevelChanged();

    void rightWetDryEnabledChanged();
    void leftWetDryEnabledChanged();

    void sigSaveInUndoItems(QJsonArray array);
    void sigSaveInRedoItems(QJsonArray array);

    void controlsChanged();
    void sigSetCursor(const QCursor& cursor);
    void sigCursorPosition(QPoint point);
    void sigUpdate();
    void nameChanged();
    void typeChanged();
    void sigEnableChanged(int trackIndex, QString menu);

    void sigControlVisibleChanged(int trackIndex, QString menu);
    void sigEffectLoaded(int trackIndex);
    void sigEffectUpdated(int trackIndex, int effectIndex);
};

#endif // EFFECT_H
