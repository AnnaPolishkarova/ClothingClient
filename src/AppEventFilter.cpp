#include "../include/AppEventFilter.h"
#include <QKeyEvent>
#include <QDebug>

AppEventFilter::AppEventFilter(QObject* parent)
    : QObject(parent) {}

bool AppEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type()) {
    case QEvent::KeyPress: {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            // Глобальные горячие клавиши
            if (keyEvent->modifiers() == Qt::ControlModifier) {
                switch (keyEvent->key()) {
                case Qt::Key_N:
                    qDebug() << "Ctrl+N pressed - new item";
                    return true;
                case Qt::Key_F:
                    qDebug() << "Ctrl+F pressed - find matches";
                    return true;
                }
            }
            break;
    }
    case QEvent::MouseButtonPress:
        // обработка кликов мыши
            break;
    case QEvent::Close:
        // Обработка попытки закрытия приложения
            break;
    default:
        break;
    }

    return QObject::eventFilter(watched, event);
}
