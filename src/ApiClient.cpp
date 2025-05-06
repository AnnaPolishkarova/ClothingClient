#include "../include/apiClient.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    // , m_manager(new QNetworkAccessManager())
    , m_manager(nullptr)
    , m_baseUrl("http://localhost:8080/api")
{
    m_manager = new QNetworkAccessManager(this); // создать менеджера в том же потоке, где ApiClient
    connect(m_manager, &QNetworkAccessManager::finished, this, &ApiClient::onReplyFinished);
}

ApiClient::~ApiClient()
{
    qDebug() << "ApiClient destroyed in thread:" << QThread::currentThread();

    if (m_manager)
    {
        for (auto reply : m_manager->findChildren<QNetworkReply*>())
        {
            reply->abort();
            reply->deleteLater();
        }
        m_manager->deleteLater();
    }
}

void ApiClient::loadItems()
{
    QUrl url(m_baseUrl.toString() + "/items");
    m_manager->get(QNetworkRequest(url));
}

void ApiClient::findMatches(double waist, double hip, double length)
{
    QUrl url(m_baseUrl.toString() + "/match");
    QUrlQuery query;
    query.addQueryItem("waist", QString::number(waist));
    query.addQueryItem("hip", QString::number(hip));
    query.addQueryItem("length", QString::number(length));
    url.setQuery(query);

    m_manager->get(QNetworkRequest(url));
}

void ApiClient::addItem(const ClothingItem &item)
{
    QUrl url(m_baseUrl.toString() + "/items");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["waist"] = item.waist;
    json["hip"] = item.hip;
    json["length"] = item.length;
    json["name"] = item.name;

    m_manager->post(request, QJsonDocument(json).toJson());
}

void ApiClient::editItem(const QString &oldName, const ClothingItem &newItem)
{
    QUrl url(m_baseUrl.toString() + "/items");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["old_name"] = oldName;
    json["waist"] = newItem.waist;
    json["hip"] = newItem.hip;
    json["length"] = newItem.length;
    json["name"] = newItem.name;

    m_manager->put(request, QJsonDocument(json).toJson());
}

void ApiClient::deleteItem(const QString &name)
{
    QUrl url(m_baseUrl.toString() + "/items/" + name);
    m_manager->deleteResource(QNetworkRequest(url));
}


void ApiClient::onReplyFinished(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        handleError(reply);
        reply->deleteLater();
        return;
    }

    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    QString path = reply->url().path();

    if (path.contains("/match")) {
        handleMatchResponse(json);
    }
    else if (path.contains("/items")) {
        if (reply->operation() == QNetworkAccessManager::GetOperation) {
            if (json.isArray()) {
                emit itemsLoaded(json.array());
            }
            else if (json.isObject()) {
                QJsonObject obj = json.object();
                if (obj.contains("items")) {
                    QJsonValue itemsValue = obj["items"];
                    if (itemsValue.isArray()) {
                        emit itemsLoaded(itemsValue.toArray());
                    }
                    else {
                        emit errorOccurred(tr("'items' field is not an array"));
                    }
                }
                else {
                    emit errorOccurred(tr("Response missing 'items' field"));
                }
            }
            else {
                emit errorOccurred(tr("Invalid response format"));
            }
        }
        else {
            // Обработка POST/PUT/DELETE
            if (json.isObject()) {
                QJsonObject obj = json.object();
                bool success = obj.value("status").toString() == "success";
                QString message = obj.value("message").toString();
                emit operationCompleted(success, message);

                if (success) {
                    QMetaObject::invokeMethod(this, "loadItems", Qt::QueuedConnection);
                }
            }
            else {
                emit errorOccurred(tr("Invalid response format"));
            }
        }
    }
    else {
        // Обработка других путей
        if (json.isObject()) {
            QJsonObject obj = json.object();
            bool success = obj.value("status").toString() == "success";
            QString message = obj.value("message").toString();
            emit operationCompleted(success, message);
        }
        else {
            emit errorOccurred(tr("Invalid response format"));
        }
    }

    reply->deleteLater();
}

void ApiClient::handleMatchResponse(const QJsonDocument &json)
{
    QList<QPair<QString, double>> matches;
    QJsonArray items = json.object()["matches"].toArray();

    for (const QJsonValue &item : items) {
        QJsonObject obj = item.toObject();
        matches.append(qMakePair(
            obj["name"].toString(),
            obj["probability"].toDouble()
        ));
    }

    emit matchesFound(matches);
}

void ApiClient::handleError(QNetworkReply *reply)
{
    QString error = reply->errorString();
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    if (!json.isNull() && json.object().contains("message")) {
        error = json.object()["message"].toString();
    }

    emit errorOccurred(error);
    reply->deleteLater();
}
