#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

// 前向声明，避免循环依赖 (如果将来 User 需要知道 MainWindow)
// class MainWindow;

struct User {
    int id = -1; // -1 表示尚未保存到数据库或无效用户
    QString email;
    QString passwordHash; // 重要：存储密码的哈希值，而不是明文密码
    QString nickname;
    QString avatarPath; // 头像文件路径

    QDateTime registrationDate;
    QDateTime lastLoginDate; // Updated by login logic
    int accountStatus = 0;   // 0: Active, 1: PendingVerification, etc.

    User() : accountStatus(0) {}

    bool isValid() const {
        return id != -1 && !email.isEmpty();
    }
    bool isActive() const { return accountStatus == 0; }
};

#endif // USER_H