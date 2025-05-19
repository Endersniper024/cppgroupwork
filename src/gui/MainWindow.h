#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem> // For QListWidget if used


#include "core/User.h" // 引入 User 结构
#include "core/Subject.h" // Add this
#include "core/Task.h" // Task 
#include "core/TimeLog.h"

class QDockWidget;
class QListWidget;
class QPushButton; // For Add/Edit/Delete buttons
class QTableView;
class QStandardItemModel; // For populating the table view simply

class TimerWidget; // Forward declaration

// class QPushButton;

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
    // Subjects
    void onAddSubject();
    void onEditSubject();
    void onDeleteSubject();
    void onSubjectListSelectionChanged(); // When a subject is selected in the list

    // Tasks TM-002
    void onAddTask();
    void onEditTask();
    void onDeleteTask();
    void onTaskTableDoubleClicked(const QModelIndex &index);
    void onChangeTaskStatus(); // Slot for context menu or button


    // ... 其他槽函数 ...

private:
    Ui::MainWindow *ui;
    User m_currentUser;

    void setupMenuBar();
    void updateStatusBar(); // 更新状态栏显示当前用户信息

    // Subjects
    QDockWidget *m_subjectDockWidget;
    QListWidget *m_subjectListWidget;
    QPushButton *m_addSubjectButton;
    QPushButton *m_editSubjectButton;
    QPushButton *m_deleteSubjectButton;

    // Tasks TM-002
    QTableView *m_taskTableView;
    QStandardItemModel *m_taskTableModel;
    
    QPushButton *m_addTaskButton;
    QPushButton *m_editTaskButton;
    QPushButton *m_deleteTaskButton;

    void setupSubjectDockWidget();
    void loadSubjectsForCurrentUser();
    void updateSubjectActionButtons(); // Enable/disable edit/delete based on selection



    // Tasks TM-002

    void setupTaskView(); // To set up the central task display area
    void loadTasksForSubject(int subjectId);
    void updateTaskActionButtons(); // Enable/disable task buttons based on selection
    Subject getCurrentSelectedSubject(); // Helper
    Task getCurrentSelectedTask();       // Helper


    // Time TM-003

    TimerWidget *m_timerWidget;
    QDockWidget *m_timerDockWidget;

    void setupTimerDockWidget();
    void onTimerLoggedNewEntry(const TimeLog& newLog); // Slot to handle new log entries

};

#endif // MAINWINDOW_H