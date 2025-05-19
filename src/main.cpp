#include <QApplication>
#include "gui/MainWindow.h"
#include "gui/LoginDialog.h"
#include "core/DatabaseManager.h"
#include <QDebug>
#include <QDir> // For QDir::setCurrent

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 设置应用程序信息，用于 QSettings
    QCoreApplication::setOrganizationName("MyOrg");
    QCoreApplication::setApplicationName("CollaborativeLearningTimeManager");

    // 尝试将当前工作目录设置为可执行文件所在的目录
    // 这有助于确保相对路径（如数据库文件）能被正确找到
    QString appDirPath = QCoreApplication::applicationDirPath();
    QDir::setCurrent(appDirPath);
    qDebug() << "Current working directory set to:" << QDir::currentPath();


    // 初始化数据库
    if (!DatabaseManager::instance().openDatabase()) { // 使用默认路径
        qCritical() << "Failed to open database! Application cannot start.";
        // 在实际应用中，这里可能需要一个更友好的错误提示给用户
        return -1;
    }
    qDebug() << "Database initialized.";


    MainWindow w;
    LoginDialog loginDialog(&w); // 将主窗口作为父窗口，登录对话框关闭时不会连带关闭主窗口

    // 尝试从 QSettings 加载 "记住我" 的用户 (可选)
    // QSettings settings;
    // QString lastUserEmail = settings.value("lastUserEmail").toString();
    // if (!lastUserEmail.isEmpty()) {
    //     // 自动填充邮箱，或者尝试自动登录 (如果保存的是令牌)
    //     // loginDialog.setEmail(lastUserEmail);
    //     // 如果是自动登录，这里需要验证令牌，然后直接显示主窗口
    // }

    // 先显示登录对话框
    if (loginDialog.exec() == QDialog::Accepted) {
        QString userEmail = loginDialog.getEmail();
        User currentUser = DatabaseManager::instance().getUserByEmail(userEmail);
        if (currentUser.isValid()) {
            w.setCurrentUser(currentUser);
            w.show();
            return a.exec();
        } else {
            // 理论上 loginDialog.exec() == QDialog::Accepted 就意味着用户已验证
            qCritical() << "Login accepted but user not found in DB. This should not happen.";
            return -2; // 异常退出
        }
    }

    // 如果登录对话框被取消，则应用程序不启动主窗口，直接退出
    DatabaseManager::instance().closeDatabase();
    qDebug() << "Login cancelled or failed. Application will exit.";
    return 0;
}