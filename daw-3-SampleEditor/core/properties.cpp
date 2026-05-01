#include "properties.h"

Properties::Properties(QObject* parent)
    : QObject { parent }
{
    setTracksOnLeft(true);

    setHoldCtrl(false);
    setHoldShift(false);

    setMasterOnTop(false);

    connect(this, SIGNAL(emitCtrl(bool)), this, SLOT(setHoldCtrl(bool)));
}

bool Properties::masterOnTop() const
{
    return m_masterOnTop;
}

void Properties::setMasterOnTop(bool newMasterOnTop)
{
    if (m_masterOnTop == newMasterOnTop)
        return;
    m_masterOnTop = newMasterOnTop;
    emit masterOnTopChanged();
}

bool Properties::tracksOnLeft() const
{
    return m_tracksOnLeft;
}

void Properties::setTracksOnLeft(bool newTracksOnLeft)
{ // qDebug() << newTracksOnLeft;
    if (m_tracksOnLeft == newTracksOnLeft)
        return;
    m_tracksOnLeft = newTracksOnLeft;
    emit tracksOnLeftChanged();
}

float Properties::brightness() const
{
    return m_brightness;
}

void Properties::setBrightness(float newBrightness)
{
    if (qFuzzyCompare(m_brightness, newBrightness))
        return;
    m_brightness = newBrightness;
    emit brightnessChanged();
}

bool Properties::holdCtrl() const
{ // qDebug() << "first "<< m_holdCtrl;
    return m_holdCtrl;
}

void Properties::setHoldCtrl(bool newHoldCtrl)
{
    // qDebug() << "hold0 " << newHoldCtrl;
    if (m_holdCtrl == newHoldCtrl)
        return;
    m_holdCtrl = newHoldCtrl;
    emit holdCtrlChanged(); // qDebug() << "hold1 " << newHoldCtrl;
}

bool Properties::holdShift() const
{
    return m_holdShift;
}

void Properties::setHoldShift(bool newHoldShift)
{
    if (m_holdShift == newHoldShift)
        return;
    m_holdShift = newHoldShift;
    emit holdShiftChanged();
}

bool Properties::shiftPressed()
{
    setHoldShift(true);
    return true;
}

bool Properties::shiftReleased()
{
    setHoldShift(false);
    return false;
}

bool Properties::ctrl(bool b)
{
    // qDebug() << "b: " << b;
    emit emitCtrl(b);
    return b;
}

