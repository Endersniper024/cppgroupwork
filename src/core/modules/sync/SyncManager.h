#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QList>
#include "src/core/network/ApiClient.h"    // 正确的相对路径或通过INCLUDEPATH设置
#include "src/core/database/DatabaseManager.h" // 正确的路径
#include "OfflineOperation.h"
#include "SyncEnums.h"

class SyncManager : public QObject {
    Q_OBJECT
public:
    explicit SyncManager(QObject *parent = nullptr);
    void initialize(const QString& currentUserIdGlobal, ApiClient* apiClient, DatabaseManager* dbManager);
    void scheduleSync(bool immediate = false);
    void onUserLoggedIn(const QString& userGlobalId);
    void onUserLoggedOut();
    void networkStatusChanged(bool isOnline);

signals:
    void syncCompleted(const QString& summary);
    void syncProgress(int percentage, const QString& message);
    void syncError(const QString& error);
    void requestUIRefresh(const QString& entityType); // 通知UI特定类型数据已更新

private slots:
    void performSyncCycle();
    void handlePushChangesReply(QNetworkReply* reply);
    void handlePullUpdatesReply(QNetworkReply* reply);

private:
    QString m_currentUserIdGlobal;
    ApiClient* m_apiClient;             // Non-owning pointer, managed externally
    DatabaseManager* m_dbManager;       // Non-owning pointer
    QTimer* m_syncTimer;
    QList<OfflineOperation> m_offlineQueue; // Or manage via DB table
    qint64 m_lastSuccessfulSyncTimestamp; // Global or per-entity type
    bool m_isSyncing;

    void pushLocalChanges();
    void pullRemoteChanges();
    void processPulledUpdates(const QJsonArray& updates);
    void queueOfflineOperation(EntityType type, OperationType op, const QJsonObject& data, const QString& globalId = QString(), int localId = -1);
    void loadOfflineQueueFromDB();
    void clearOfflineQueueInDB(const QList<qint64>& processedOpIds); // op_id from offline_operations table
};

#endif // SYNCMANAGER_H