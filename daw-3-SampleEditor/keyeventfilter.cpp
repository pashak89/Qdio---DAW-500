#include "keyeventfilter.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

bool KeyEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        emit keyPressed(keyEvent->modifiers(), keyEvent->key());
    }
    if (event->type() == QEvent::KeyRelease) {

        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        emit keyRelease(keyEvent->modifiers(), keyEvent->key());
    }
    if (event->type() == QEvent::FocusOut) {

        emit focusOut();
    }
    QObject::eventFilter(obj, event);
    return false;
}

void KeyEventFilter::listenTo(QObject* object)
{
    if (!object)
        return;

    object->installEventFilter(this);
}
