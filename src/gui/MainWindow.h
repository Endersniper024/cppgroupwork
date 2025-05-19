#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "core/User.h" // 引入 User 结构

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setCurrentUser(const User& user);

private slots:
    void handleLogout();
    void openProfileDialog(); // UM-004 个人信息管理入口
    // ... 其他槽函数 ...

private:
    Ui::MainWindow *ui;
    User m_currentUser;

    void setupMenuBar();
    void updateStatusBar(); // 更新状态栏显示当前用户信息
};

#endif // MAINWINDOW_H