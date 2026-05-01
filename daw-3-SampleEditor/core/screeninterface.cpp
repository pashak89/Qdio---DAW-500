#include "screeninterface.h"
#include <QDebug>
#include <QGuiApplication>
ScreenInterface* ScreenInterface::m_instance = Q_NULLPTR;
ScreenInterface::ScreenInterface(QObject* parent)
    : QObject(parent)
{
    m_screen = QGuiApplication::primaryScreen();
    setScale(m_screen->logicalDotsPerInch());

    connect(m_screen, SIGNAL(logicalDotsPerInchChanged(qreal)), this, SLOT(onLogicalDotsPerInchChanged(qreal)));
    connect(m_screen, SIGNAL(physicalDotsPerInchChanged(qreal)), this, SLOT(onPhysicalDotsPerInchChanged(qreal)));

    m_instance = this;
}

qreal ScreenInterface::resolution() const
{
    return m_resolution;
}

qreal ScreenInterface::scale() const
{
    return m_scale;
}

ScreenInterface* ScreenInterface::instance()
{
    if (m_instance == Q_NULLPTR) {
        m_instance = new ScreenInterface;
    }

    return m_instance;
}

int ScreenInterface::freeResolution(int pixel)
{
    return m_scale / 120.0 * pixel;
}

void ScreenInterface::setResolution(qreal resolution)
{
    qWarning("Floating point comparison needs context sanity check");
    if (qFuzzyCompare(m_resolution, resolution))
        return;

    m_resolution = resolution;
    emit resolutionChanged(m_resolution);
}

void ScreenInterface::setScale(qreal scale)
{
    qWarning("Floating point comparison needs context sanity check");
    if (qFuzzyCompare(m_scale, scale))
        return;

    m_scale = scale;

    qDebug() << "QT onScaleChanged= " << scale;

    emit scaleChanged(m_scale);
}

void ScreenInterface::onLogicalDotsPerInchChanged(qreal val)
{
    setScale(val);
    qDebug() << val;
}

void ScreenInterface::onPhysicalDotsPerInchChanged(qreal val)
{
    setResolution(val);
}
