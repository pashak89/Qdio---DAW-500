#ifndef INTERFACE_H
#define INTERFACE_H

#include <QGuiApplication>
#include <QObject>
#include <QScreen>

class ScreenInterface : public QObject {
    Q_PROPERTY(qreal resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)

    Q_OBJECT
public:
    explicit ScreenInterface(QObject* parent = nullptr);

    qreal resolution() const;

    qreal scale() const;

    static ScreenInterface* instance();

    int freeResolution(int pixel);

    static qreal getScale()
    {
        QScreen* screen1 = QGuiApplication::primaryScreen();
        qreal y = screen1->logicalDotsPerInch() / 96;
        return y;
    }

    static int scaleSize(int value, float refSize = 1)
    {
        auto scale = getScale();
        auto result = qRound(value * scale * refSize / 2.0);
        if (result % 2 != 0)
            return result;
        else
            return result - 1;
    }

    static int scaleSize2(int value, float refSize = 1)
    {
        auto scale = getScale();
        return qRound(value * scale * refSize / 2.0);
    }

signals:
    void resolutionChanged(qreal resolution);
    void scaleChanged(qreal scale);

public slots:

    void setResolution(qreal resolution);
    void setScale(qreal scale);

private:
    QScreen* m_screen;

    qreal m_resolution;

    qreal m_scale;

    static ScreenInterface* m_instance;

private slots:
    void onLogicalDotsPerInchChanged(qreal val);
    void onPhysicalDotsPerInchChanged(qreal val);
};

#endif // INTERFACE_H
