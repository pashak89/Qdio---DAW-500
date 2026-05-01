#ifndef CURSORPOSITION_H
#define CURSORPOSITION_H

#include <QCursor>
#include <QDebug>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QObject>
#include <QQuickItem>
#include <QScreen>
#include <QWindow>

class CursorPosition : public QObject {
    Q_OBJECT
    Q_PROPERTY(float scale READ getScale NOTIFY scaleChanged)
private:
    QCursor cursor;

    float mousePosX;
    float mousePosY;

public slots:
    Q_INVOKABLE void saveCursorPos(float x, float y)
    {
        mousePosX = x;
        mousePosY = y;
    }
    Q_INVOKABLE void moveCursor(int x, int y)
    {
        cursor.setPos(x, y);
    }
    Q_INVOKABLE void hideCursor()
    {
        QGuiApplication::setOverrideCursor(Qt::BlankCursor);
    }
    Q_INVOKABLE void showCursor()
    {
        cursor.setPos(mousePosX, mousePosY);
        QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
    }

    Q_INVOKABLE void onlyShowCursor()
    {
        QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
    }

    QVector<int> getResolution()
    {
        QScreen* screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int height = screenGeometry.height();
        int width = screenGeometry.width();
        double resScale = screen->logicalDotsPerInch();

        QVector<int> vector;
        vector.append(width);
        vector.append(height);
        vector.append(resScale);
        return vector;
    }

    qreal getScale()
    {
        QScreen* screen1 = QGuiApplication::primaryScreen();
        qreal y = screen1->logicalDotsPerInch() / 96;
        return y;
    }

    Q_INVOKABLE int getDecimal(double value)
    {

        int decimal = 0;

        value = qAbs(value);

        if (value < 10) {
            decimal = 2;
        } else if (value >= 10 && value < 100) {
            decimal = 1;
        } else if (value >= 100 && value < 1000) {
            decimal = 0;
        } else if (value >= 1000 && value < 10000) {
            decimal = 2;
        } else
            decimal = 1;

        return decimal;
    }

protected:
signals:
    void deleteKeyPress();
    void scaleChanged();
};

#endif // CURSORPOSITION_H
