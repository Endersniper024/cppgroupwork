#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QWebSocket>

class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient();

    void setApiBaseUrl(const QString& baseUrl);
    void setAuthToken(const QString& token); // JWT Token

    // RESTful methods
    QNetworkReply* get(const QString& endpoint, const QUrlQuery& params = QUrlQuery());
    QNetworkReply* post(const QString& endpoint, const QJsonObject& jsonData);
    QNetworkReply* put(const QString& endpoint, const QJsonObject& jsonData);
    QNetworkReply* del(const QString& endpoint); // Qt nemá QNetworkAccessManager::deleteResource, použijeme customRequest

    // WebSocket methods
    void connectWebSocket(const QUrl& url);
    void disconnectWebSocket();
    bool sendWebSocketMessage(const QJsonObject& message);
    bool isWebSocketConnected() const;

signals:
    // RESTful signals
    void authenticationErrorOccurred(); // Emitted on 401/403 for REST calls

    // WebSocket signals
    void webSocketConnected();
    void webSocketDisconnected();
    void webSocketError(QAbstractSocket::SocketError error);
    void webSocketMessageReceived(const QJsonObject& message); // For incoming JSON messages
    void webSocketBinaryMessageReceived(const QByteArray& message);


private slots:
    void onWebSocketConnected();
    void onWebSocketDisconnected();
    void onWebSocketTextMessageReceived(const QString &message);
    void onWebSocketBinaryMessageReceived(const QByteArray &message);
    void onWebSocketSslErrors(const QList<QSslError> &errors);


private:
    QNetworkAccessManager *m_networkManager;
    QWebSocket *m_webSocket;
    QString m_apiBaseUrl;
    QString m_authToken;

    QNetworkRequest createBaseRequest(const QString& endpoint) const;
};

#endif // APICLIENT_H