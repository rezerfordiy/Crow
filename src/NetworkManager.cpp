
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

#include "NetworkManager.h"

NetworkManager::NetworkManager(QObject* parent) : QObject(parent) {}

void NetworkManager::sendHttpRequest() {
    QUrl url("http://127.0.0.1:5000/get_route");
    QNetworkRequest request(url);
    
    QNetworkReply* reply = manager.get(request);
    connect(reply, &QNetworkReply::finished, this, &NetworkManager::onRequestFinished);
}

void NetworkManager::onRequestFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        if (!jsonDoc.isNull()) {
            emit jsonReceived(jsonDoc.object());
        } else {
            emit errorOccurred("Пустой json");
        }
    } else {
        emit errorOccurred(reply->errorString());
    }
    
    reply->deleteLater();
}
