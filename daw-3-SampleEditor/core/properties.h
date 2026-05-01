
#ifndef PRPPERTIES_H
#define PRPPERTIES_H

#include <QObject>
#include <QDebug>

class Properties : public QObject
{
    Q_OBJECT
public:
    explicit Properties(QObject *parent = nullptr);

    Q_PROPERTY(bool masterOnTop READ masterOnTop WRITE setMasterOnTop NOTIFY masterOnTopChanged FINAL)

    bool masterOnTop() const;
    void setMasterOnTop(bool newMasterIsTop);

    Q_PROPERTY(bool tracksOnLeft READ tracksOnLeft WRITE setTracksOnLeft NOTIFY tracksOnLeftChanged FINAL)

    bool tracksOnLeft() const;
    // void setTracksOnLeft(bool newTracksOnLeft);


    Q_PROPERTY(float brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged FINAL)

    float brightness() const;
    void setBrightness(float newBrightness);

    Q_PROPERTY(bool holdCtrl READ holdCtrl WRITE setHoldCtrl NOTIFY holdCtrlChanged)

    bool holdCtrl() const;
    // void setHoldCtrl(bool newHoldCtrl);

    Q_PROPERTY(bool holdShift READ holdShift WRITE setHoldShift NOTIFY holdShiftChanged FINAL)

    bool holdShift() const;
    void setHoldShift(bool newHoldShift);

public slots:

    bool shiftPressed();
    bool shiftReleased();

    bool ctrl(bool b);
void setHoldCtrl(bool newHoldCtrl);
void setTracksOnLeft(bool newTracksOnLeft);

signals:
    void masterOnTopChanged();
    void tracksOnLeftChanged();

    void brightnessChanged();

    void holdCtrlChanged();

    void holdShiftChanged();

    void emitCtrl(bool b);
private:
    bool m_masterOnTop;
    bool m_tracksOnLeft;
    float m_brightness;

    bool res = false;
    bool m_holdCtrl;
    bool m_holdShift;
};

#endif // PRPPERTIES_H
