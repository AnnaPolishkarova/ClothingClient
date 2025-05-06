#pragma once

#include <QObject>

class AppEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit AppEventFilter(QObject* parent = nullptr);
    bool eventFilter(QObject* watched, QEvent* event) override;
};