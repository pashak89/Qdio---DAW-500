#ifndef DUMMYEFFECT_H
#define DUMMYEFFECT_H

#include "effect.h"
#include "effectcontrols.h"
#include <QObject>
class EffectControls;

//class DummyEffectControls : public EffectControls {
//public:
//    DummyEffectControls(Effect* _eff)
//        : EffectControls(_eff)
//    {
//    }

//    ~DummyEffectControls() override = default;

//    int controlCount() override
//    {
//        return 0;
//    }

//    // EffectControls interface
//public:
//    QString name()
//    {
//        return "";
//    }
//};

//class DummyEffect : public Effect {
//    Q_OBJECT
//public:
//    DummyEffect(QObject* _parent)
//        : Effect(nullptr, nullptr, _parent)
//        , m_controls(this)

//    {
//    }

//    ~DummyEffect() override = default;

//    bool processAudioBuffer(sampleFrame*) override
//    {
//        return false;
//    }

//    bool initialize() override
//    {
//        return false;
//    }

//private:
//    DummyEffectControls m_controls;
//};

#endif // DUMMYEFFECT_H
