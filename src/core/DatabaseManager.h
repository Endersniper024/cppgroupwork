#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QVariant> // For QVariantMap
#include "User.h"   // 包含用户结构体定义

class DatabaseManager {
public:
    static DatabaseManager& instance(); // 单例模式

    bool openDatabase(const QString& path = "data/learning_platform.db");
    void closeDatabase();

    // 用户管理
    bool initUserTable();
    bool addUser(const User& user); // 初始注册时使用
    User getUserByEmail(const QString& email);
    bool updateUser(const User& user);
    bool validateUser(const QString& email, const QString& password); // 登录验证

    // 将来添加其他模块的数据库操作
    // bool addSubject(...);
    // bool addTask(...);

private:
    DatabaseManager(); // 私有构造函数
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;

    // 辅助函数
    QString hashPassword(const QString& password); // 应该使用安全的哈希算法
    bool verifyPassword(const QString& password, const QString& hash);
};

#endif // DATABASEMANAGER_H