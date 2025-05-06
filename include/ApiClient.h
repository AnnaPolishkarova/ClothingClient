#pragma once

#include "ClothingItemModel.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <memory>
#include <qcoreevent.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

class ApiClient : public QObject
{
    Q_OBJECT
    friend class TestApiClient;

public:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient() override;

    Q_INVOKABLE void findMatches(double waist, double hip, double length);
    Q_INVOKABLE void addItem(const ClothingItem &item);
    Q_INVOKABLE void editItem(const QString &oldName, const ClothingItem &newItem);
    Q_INVOKABLE void deleteItem(const QString &name);
    Q_INVOKABLE void loadItems();

signals:
    void matchesFound(const QList<QPair<QString, double>> &matches);
    void operationCompleted(bool success, const QString &message);
    void errorOccurred(const QString &error);
    void itemsLoaded(const QJsonArray &items);

private slots:
    void onReplyFinished(QNetworkReply *reply);

protected:
    void customEvent(QEvent *event) override
    {
        if (event-> type() == QEvent::DeferredDelete)
        {
            qDebug() << "Deferred delete in thread:" << QThread::currentThreadId();
        }
        QObject::customEvent(event);
    }

private:
    void handleMatchResponse(const QJsonDocument &json);
    void handleError(QNetworkReply *reply);

    // Доступ к этим полям для тестов
#ifdef UNIT_TESTS
public:
#else
private:
#endif
    QNetworkAccessManager* m_manager;
    QUrl m_baseUrl;
};
