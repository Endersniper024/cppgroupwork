#ifndef GROUPMANAGER_H
#define GROUPMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include "src/core/network/ApiClient.h" // 正确的路径
#include "src/core/Group.h"            // 正确的路径
#include "src/core/GroupMember.h"
#include "src/core/GroupTask.h"
#include "src/core/ChatMessage.h"
#include "GroupEnums.h"

class GroupManager : public QObject {
    Q_OBJECT
public:
    explicit GroupManager(QObject *parent = nullptr);
    void initialize(const QString& currentUserIdGlobal, ApiClient* apiClient);

    // Public methods to be called by UI
    void fetchMyGroups();
    void createGroup(const QString& name, const QString& description);
    void fetchGroupDetails(const QString& groupGlobalId); // Includes members and tasks
    void generateInviteCode(const QString& groupGlobalId);
    void joinGroupWithCode(const QString& inviteCode);
    void postGroupChatMessage(const QString& groupGlobalId, const QString& messageContent);
    void createGroupTask(const QString& groupGlobalId, const QString& name, const QString& description, TaskPriority priority, const QDateTime& dueDate);
    void updateGroupTask(const GroupTask& task); // Pass whole task object
    void assignUsersToGroupTask(const QString& groupTaskGlobalId, const QList<QString>& userGlobalIds);
    void updateGroupTaskStatus(const QString& groupTaskGlobalId, TaskStatus status);


signals:
    void myGroupsListUpdated(const QList<Group>& groups);
    void groupDetailsUpdated(const Group& group); // Might emit separate signals for members/tasks
    void groupMembersUpdated(const QString& groupGlobalId, const QList<GroupMember>& members);
    void groupTasksUpdated(const QString& groupGlobalId, const QList<GroupTask>& tasks);
    void newChatMessageReceived(const ChatMessage& message);
    void groupOperationError(const QString& operation, const QString& error);
    void inviteCodeGenerated(const QString& groupGlobalId, const QString& code);

private slots:
    // Slots to handle replies from ApiClient
    void handleMyGroupsReply(QNetworkReply* reply);
    void handleCreateGroupReply(QNetworkReply* reply);
    void handleGroupDetailsReply(QNetworkReply* reply);
    void handleInviteCodeReply(QNetworkReply* reply);
    void handleJoinGroupReply(QNetworkReply* reply);
    void handlePostChatReply(QNetworkReply* reply);
    void handleCreateGroupTaskReply(QNetworkReply* reply);
    // ... other reply handlers ...

    // Slot to handle incoming WebSocket messages relevant to groups
    void onWebSocketMessageReceived(const QJsonObject& message);


private:
    QString m_currentUserIdGlobal;
    ApiClient* m_apiClient; // Non-owning pointer
    // Potentially local caches for groups, members, tasks if complex offline viewing for groups is needed
    // QMap<QString, Group> m_myGroupsCache;
};

#endif // GROUPMANAGER_H