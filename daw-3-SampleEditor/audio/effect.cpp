#include "effect.h"
#include "effectchain.h"
Effect::Effect(const Plugin::Descriptor* _desc,
    const Plugin::Descriptor::SubPluginFeatures::Key* _key, QObject* _parent)
    : Plugin(_desc, _key, _parent)

    , m_processors(1)
    , m_okay(true)
    , m_noRun(false)
    , m_running(false)
    , m_bufferCount(0)
    , m_enabled(true)
    , m_rightWetLevel(1.0f)
    , m_leftWetLevel(1.0f)
    , m_gateModel(0.0f)
    , m_autoQuitModel(1.0f)
    , m_rightWetDryEnabled(true)
    , m_leftWetDryEnabled(true)
    , m_autoQuitDisabled(false)
{
}

Effect::~Effect()
{
    qDebug() << "!EFFECT";
    // delete descriptor();
}

int Effect::effectIndex() const
{
    return m_effectIndex;
}

void Effect::setEffectIndex(int newEffectIndex)
{
    m_effectIndex = newEffectIndex;
}
