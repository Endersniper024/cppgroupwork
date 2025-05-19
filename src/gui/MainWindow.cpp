#include "MainWindow.h"
#include "ui_MainWindow.h" // 由 uic 生成
#include "DatabaseManager.h"
#include "LoginDialog.h"    // 需要包含 LoginDialog 以便可以重新显示
#include "ProfileDialog.h"  // (后续添加)
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QDebug>
#include <QApplication> // For qApp

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("协同学习时间管理平台");
    setupMenuBar();
    updateStatusBar(); // 初始状态栏

    // 初始时，主窗口可以隐藏，直到登录成功
    // this->hide();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    QAction *profileAction = fileMenu->addAction("个人信息(&P)..."); // UM-004
    fileMenu->addSeparator();
    QAction *logoutAction = fileMenu->addAction("登出(&L)"); // UM-002
    QAction *exitAction = fileMenu->addAction("退出(&X)");

    connect(profileAction, &QAction::triggered, this, &MainWindow::openProfileDialog);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::handleLogout);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit); // qApp 是 QApplication 的全局指针

    // 可以在这里添加更多菜单，如“任务管理”、“统计报告”等
    QMenu *taskMenu = menuBar()->addMenu("任务(&T)");
    // ... add task related actions ...
}

void MainWindow::setCurrentUser(const User& user) {
    m_currentUser = user;
    if (m_currentUser.isValid()) {
        qDebug() << "MainWindow: Current user set to" << m_currentUser.email;
        ui->welcomeLabel->setText(QString("欢迎您, %1!").arg(m_currentUser.nickname.isEmpty() ? m_currentUser.email : m_currentUser.nickname));
    } else {
        ui->welcomeLabel->setText("未登录");
    }
    updateStatusBar();
}

void MainWindow::updateStatusBar() {
    if (m_currentUser.isValid()) {
        statusBar()->showMessage(QString("当前用户: %1").arg(m_currentUser.email));
    } else {
        statusBar()->showMessage("未登录");
    }
}

void MainWindow::handleLogout() {
    // UM-002 用户登出
    qDebug() << "User" << m_currentUser.email << "logging out.";
    m_currentUser = User(); // 清空当前用户信息
    updateStatusBar();
    ui->welcomeLabel->setText("您已登出。");

    // 清理“记住我”的信息 (如果实现了)
    // QSettings settings("MyCompany", "MyApp");
    // settings.remove("lastUserEmail"); // 或者更复杂的令牌清除

    // 关闭主窗口，重新显示登录对话框
    this->hide();
    LoginDialog loginDialog(nullptr); // 创建一个新的登录对话框实例
    // loginDialog.setWindowModality(Qt::ApplicationModal); // 确保登录对话框是模态的

    if (loginDialog.exec() == QDialog::Accepted) {
        QString userEmail = loginDialog.getEmail();
        User loggedInUser = DatabaseManager::instance().getUserByEmail(userEmail);
        if (loggedInUser.isValid()) {
            setCurrentUser(loggedInUser);
            this->show();
        } else {
            // 这种情况理论上不应该发生，因为 LoginDialog 成功时用户已验证
            QMessageBox::critical(this, "错误", "重新登录失败，用户数据未找到。应用将退出。");
            qApp->quit();
        }
    } else {
        // 用户取消了登录对话框，退出应用
        qApp->quit();
    }
}

void MainWindow::openProfileDialog() {
    // UM-004 个人信息管理
    if (!m_currentUser.isValid()) {
        QMessageBox::warning(this, "未登录", "请先登录后再管理个人信息。");
        return;
    }
    // ProfileDialog profileDlg(m_currentUser, this); // 创建个人信息对话框 (后续实现)
    // if (profileDlg.exec() == QDialog::Accepted) {
    //     m_currentUser = profileDlg.getUpdatedUser(); // 获取更新后的用户信息
    //     DatabaseManager::instance().updateUser(m_currentUser);
    //     setCurrentUser(m_currentUser); // 刷新主窗口显示
    //     QMessageBox::information(this, "成功", "个人信息已更新。");
    // }
    QMessageBox::information(this, "提示", "个人信息管理功能 (UM-004) 待实现。");
}