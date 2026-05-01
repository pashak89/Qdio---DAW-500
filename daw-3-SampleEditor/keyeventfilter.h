#ifndef KEYEVENTFILTER_H
#define KEYEVENTFILTER_H

#include <QObject>

class KeyEventFilter : public QObject {
    Q_OBJECT

public:
    Q_INVOKABLE void listenTo(QObject *object);
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

signals:
    void keyPressed(int modifiers, int key);
    void keyRelease(int modifiers, int key);
    void focusOut();
};
#endif // KEYEVENTFILTER_H
