#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem> // For QListWidget if used
#include "core/User.h" // 引入 User 结构
#include "core/Subject.h" // Add this

class QDockWidget;
class QListWidget;
class QPushButton; // For Add/Edit/Delete buttons

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
    void onAddSubject();
    void onEditSubject();
    void onDeleteSubject();
    void onSubjectListSelectionChanged(); // When a subject is selected in the list
    // ... 其他槽函数 ...

private:
    Ui::MainWindow *ui;
    User m_currentUser;

    void setupMenuBar();
    void updateStatusBar(); // 更新状态栏显示当前用户信息

    QDockWidget *m_subjectDockWidget;
    QListWidget *m_subjectListWidget;
    QPushButton *m_addSubjectButton;
    QPushButton *m_editSubjectButton;
    QPushButton *m_deleteSubjectButton;

    void setupSubjectDockWidget();
    void loadSubjectsForCurrentUser();
    void updateSubjectActionButtons(); // Enable/disable edit/delete based on selection

};

#endif // MAINWINDOW_H