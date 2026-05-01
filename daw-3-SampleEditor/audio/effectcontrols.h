#ifndef EFFECTCONTROLS_H
#define EFFECTCONTROLS_H

#include "effect.h"

#include <QObject>

class EffectControls : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(int controlCount READ controlCount NOTIFY shortTitleListChanged)
    Q_PROPERTY(QString name READ name NOTIFY shortTitleListChanged)

public:
    EffectControls(QSharedPointer<Effect> _eff, QObject* parent = 0)
        : QAbstractListModel(parent)
        , m_effect(_eff)
        , m_viewVisible(false)
    {
    }

    ~EffectControls() override
    {
        qDebug() << "EffectControls::~EffectControls()";
    }

    virtual int controlCount() = 0;

    void setViewVisible(bool _visible)
    {
        m_viewVisible = _visible;
    }

    bool isViewVisible() const
    {
        return m_viewVisible;
    }

    QSharedPointer<Effect> effect()
    {
        return m_effect;
    }

    virtual QString name() = 0;
    virtual QStringList shortTitleList() = 0;

signals:
    void shortTitleListChanged();

private:
    QSharedPointer<Effect> m_effect;
    bool m_viewVisible;

protected:
    QHash<int, QByteArray> m_roleNames;

protected:
    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }
};

#endif // EFFECTCONTROLS_H
