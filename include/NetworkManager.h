#pragma once

#include <QNetworkAccessManager>
#include <QObject>

class QJsonObject;

class NetworkManager : public QObject {
    Q_OBJECT
public:
    NetworkManager(QObject* parent = nullptr);
    void sendHttpRequest();

signals:
    void jsonReceived(const QJsonObject& json);
    void errorOccurred(const QString& error);

private slots:
    void onRequestFinished();

private:
    QNetworkAccessManager manager;
};
