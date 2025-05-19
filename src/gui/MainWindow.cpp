#include "MainWindow.h"
#include "ui_MainWindow.h" // 由 uic 生成
#include "DatabaseManager.h"
#include "LoginDialog.h"    // 需要包含 LoginDialog 以便可以重新显示
#include "ProfileDialog.h"  // (后续添加)
#include "gui/SubjectDialog.h" // For opening the subject dialog
#include "gui/TaskDialog.h" // For task management
#include "gui/TimerWidget.h" // Time 


#include <QDockWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QInputDialog> // For simple input, though SubjectDialog is better
#include <QMessageBox>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QDebug>
#include <QApplication> // For qApp
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView> // For table header properties
#include <QMenu>       // For context menu on tasks


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("协同学习时间管理平台");
    setupMenuBar();
    setupSubjectDockWidget(); // Add this call
    setupTaskView();
    setupTimerDockWidget();


    updateStatusBar();
    updateSubjectActionButtons(); // Initial Subject state
    updateTaskActionButtons(); // Initial state for task buttons
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

// void MainWindow::setCurrentUser(const User& user) {
//     m_currentUser = user;
//     if (m_currentUser.isValid()) {
//         qDebug() << "MainWindow: Current user set to" << m_currentUser.email;
//         ui->welcomeLabel->setText(QString("欢迎您, %1!").arg(m_currentUser.nickname.isEmpty() ? m_currentUser.email : m_currentUser.nickname));
//     } else {
//         ui->welcomeLabel->setText("未登录");
//     }
//     updateStatusBar();
// }

void MainWindow::setCurrentUser(const User& user) {
    m_currentUser = user;

    if (m_timerWidget) m_timerWidget->setCurrentUser(m_currentUser); // Pass user to timer
    
    if (m_currentUser.isValid()) {
        qDebug() << "MainWindow: Current user set to" << m_currentUser.email;
        ui->welcomeLabel->setText(QString("欢迎您, %1!").arg(m_currentUser.nickname.isEmpty() ? m_currentUser.email : m_currentUser.nickname));
        loadSubjectsForCurrentUser(); // Load subjects when user changes
    } else {
        ui->welcomeLabel->setText("未登录");
        m_subjectListWidget->clear(); // Clear subjects if no user
        if (m_taskTableModel) m_taskTableModel->clear();
        if (m_timerWidget) m_timerWidget->refreshTargetSelection();
    }
    updateStatusBar();
    updateSubjectActionButtons();
    updateTaskActionButtons();
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

// void MainWindow::openProfileDialog() {
//     // UM-004 个人信息管理
//     if (!m_currentUser.isValid()) {
//         QMessageBox::warning(this, "未登录", "请先登录后再管理个人信息。");
//         return;
//     }
//     // ProfileDialog profileDlg(m_currentUser, this); // 创建个人信息对话框 (后续实现)
//     // if (profileDlg.exec() == QDialog::Accepted) {
//     //     m_currentUser = profileDlg.getUpdatedUser(); // 获取更新后的用户信息
//     //     DatabaseManager::instance().updateUser(m_currentUser);
//     //     setCurrentUser(m_currentUser); // 刷新主窗口显示
//     //     QMessageBox::information(this, "成功", "个人信息已更新。");
//     // }
//     QMessageBox::information(this, "提示", "个人信息管理功能 (UM-004) 待实现。");
// }


void MainWindow::openProfileDialog() {
    if (!m_currentUser.isValid()) {
        QMessageBox::warning(this, "未登录", "请先登录后再管理个人信息。");
        return;
    }

    ProfileDialog profileDlg(m_currentUser, this);
    // The ProfileDialog::on_buttonBox_accepted now handles calling QDialog::accept() itself.
    // So, the exec() call will return QDialog::Accepted only if our validation passed.
    if (profileDlg.exec() == QDialog::Accepted) {
        User updatedUser = profileDlg.getUpdatedUser();
        
        // Check if actual changes were made that require a DB update
        bool userChanged = (m_currentUser.nickname != updatedUser.nickname ||
                            m_currentUser.avatarPath != updatedUser.avatarPath ||
                            m_currentUser.passwordHash != updatedUser.passwordHash);

        if (userChanged) {
            if (DatabaseManager::instance().updateUser(updatedUser)) {
                m_currentUser = updatedUser; // Update MainWindow's copy
                setCurrentUser(m_currentUser); // Refresh UI elements in MainWindow
                QMessageBox::information(this, "成功", "个人信息已更新。");
            } else {
                QMessageBox::critical(this, "错误", "更新个人信息失败，数据库操作错误。");
                // Optionally, revert m_currentUser if needed or log error
            }
        } else {
             QMessageBox::information(this, "提示", "未检测到任何信息更改。");
        }
    }
}

void MainWindow::setupSubjectDockWidget() {
    m_subjectDockWidget = new QDockWidget(tr("学习科目"), this);
    m_subjectDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget *dockContents = new QWidget(m_subjectDockWidget);
    QVBoxLayout *layout = new QVBoxLayout(dockContents);

    m_subjectListWidget = new QListWidget(dockContents);
    connect(m_subjectListWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::onSubjectListSelectionChanged);
    // We can also connect itemDoubleClicked for editing
    connect(m_subjectListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item){
        if (item) onEditSubject(); // Trigger edit on double click
    });


    layout->addWidget(m_subjectListWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addSubjectButton = new QPushButton(tr("添加"), dockContents);
    m_editSubjectButton = new QPushButton(tr("编辑"), dockContents);
    m_deleteSubjectButton = new QPushButton(tr("删除"), dockContents);

    connect(m_addSubjectButton, &QPushButton::clicked, this, &MainWindow::onAddSubject);
    connect(m_editSubjectButton, &QPushButton::clicked, this, &MainWindow::onEditSubject);
    connect(m_deleteSubjectButton, &QPushButton::clicked, this, &MainWindow::onDeleteSubject);

    buttonLayout->addWidget(m_addSubjectButton);
    buttonLayout->addWidget(m_editSubjectButton);
    buttonLayout->addWidget(m_deleteSubjectButton);
    layout->addLayout(buttonLayout);

    dockContents->setLayout(layout);
    m_subjectDockWidget->setWidget(dockContents);
    addDockWidget(Qt::LeftDockWidgetArea, m_subjectDockWidget);

    // Example: Add a menu item to show/hide this dock widget
    QMenu *viewMenu = menuBar()->addMenu(tr("视图(&V)"));
    viewMenu->addAction(m_subjectDockWidget->toggleViewAction());
}

void MainWindow::loadSubjectsForCurrentUser() {
    m_subjectListWidget->clear();
    if (!m_currentUser.isValid()) {
        updateSubjectActionButtons();
        return;
    }

    QList<Subject> subjects = DatabaseManager::instance().getSubjectsByUser(m_currentUser.id);
    for (const Subject& subject : subjects) {
        QListWidgetItem *item = new QListWidgetItem(subject.name, m_subjectListWidget);
        item->setData(Qt::UserRole, subject.id); // Store subject ID
        if (subject.color.isValid()) {
            item->setForeground(subject.color.darker(120)); // Use color for text, slightly darker for visibility
            // Or set background: item->setBackground(subject.color);
            // Or add a small color indicator icon
        }
    }
    updateSubjectActionButtons();
    if (m_timerWidget) m_timerWidget->refreshTargetSelection();
}

void MainWindow::updateSubjectActionButtons() {
    bool itemSelected = m_subjectListWidget->currentItem() != nullptr;
    m_editSubjectButton->setEnabled(itemSelected);
    m_deleteSubjectButton->setEnabled(itemSelected);
    m_addSubjectButton->setEnabled(m_currentUser.isValid()); // Can add if user is logged in
}


// void MainWindow::onSubjectListSelectionChanged() {
//     updateSubjectActionButtons();
//     QListWidgetItem *currentItem = m_subjectListWidget->currentItem();
//     if (currentItem) {
//         int subjectId = currentItem->data(Qt::UserRole).toInt();
//         qDebug() << "Selected Subject ID:" << subjectId << "Name:" << currentItem->text();
//         // TODO: Later, load tasks for this subject into a central view
//     } else {
//         qDebug() << "No subject selected.";
//         // TODO: Clear or update task view
//     }
// }


void MainWindow::onSubjectListSelectionChanged() {
    updateSubjectActionButtons();
    Subject selectedSubject = getCurrentSelectedSubject();

    if (selectedSubject.isValid()) {
        qDebug() << "Selected Subject ID:" << selectedSubject.id << "Name:" << selectedSubject.name;
        loadTasksForSubject(selectedSubject.id);
    } else {
        qDebug() << "No subject selected or invalid.";
        if (m_taskTableModel) m_taskTableModel->clear(); // Clear task table
        m_taskTableModel->setHorizontalHeaderLabels({
             tr("任务名称"), tr("截止日期"), tr("优先级"), tr("状态"), tr("预计(分钟)"), tr("实际(分钟)")
        }); // Reset headers if cleared
    }
    updateTaskActionButtons(); // Update based on new task list (empty or not)
}

void MainWindow::onAddSubject() {
    if (!m_currentUser.isValid()) return;

    SubjectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Subject newSubject = dialog.getSubjectData();
        newSubject.userId = m_currentUser.id; // Set current user ID

        // The SubjectDialog validation for name emptiness is done.
        // Now, check for uniqueness with DatabaseManager before adding.
        if (DatabaseManager::instance().subjectExists(newSubject.name, newSubject.userId)) {
            QMessageBox::warning(this, tr("添加失败"), tr("科目 '%1' 已存在。请使用其他名称。").arg(newSubject.name));
            return;
        }
        
        if (DatabaseManager::instance().addSubject(newSubject)) { // addSubject now takes ref to get ID
            loadSubjectsForCurrentUser(); // Refresh list
            QMessageBox::information(this, tr("成功"), tr("科目 '%1' 已添加。").arg(newSubject.name));
        } else {
            QMessageBox::critical(this, tr("错误"), tr("无法添加科目到数据库。"));
        }
    }
}

void MainWindow::onEditSubject() {
    if (!m_currentUser.isValid()) return;

    QListWidgetItem *selectedItem = m_subjectListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, tr("编辑科目"), tr("请先选择一个要编辑的科目。"));
        return;
    }

    int subjectId = selectedItem->data(Qt::UserRole).toInt();
    Subject subjectToEdit = DatabaseManager::instance().getSubjectById(subjectId, m_currentUser.id);

    if (!subjectToEdit.isValid()) {
        QMessageBox::critical(this, tr("错误"), tr("无法加载科目信息进行编辑。"));
        return;
    }

    SubjectDialog dialog(subjectToEdit, this);
    if (dialog.exec() == QDialog::Accepted) {
        Subject updatedSubject = dialog.getSubjectData();
        updatedSubject.userId = m_currentUser.id; // Ensure userId is set
        updatedSubject.id = subjectId;         // Ensure id is set

        // Check for name uniqueness if name changed
        if (updatedSubject.name != subjectToEdit.name &&
            DatabaseManager::instance().subjectExists(updatedSubject.name, updatedSubject.userId, updatedSubject.id)) {
             QMessageBox::warning(this, tr("更新失败"), tr("科目 '%1' 已存在。请使用其他名称。").arg(updatedSubject.name));
            return;
        }

        if (DatabaseManager::instance().updateSubject(updatedSubject)) {
            loadSubjectsForCurrentUser(); // Refresh list
            QMessageBox::information(this, tr("成功"), tr("科目 '%1' 已更新。").arg(updatedSubject.name));
        } else {
            QMessageBox::critical(this, tr("错误"), tr("无法更新科目信息。"));
        }
    }
}

void MainWindow::onDeleteSubject() {
    if (!m_currentUser.isValid()) return;

    QListWidgetItem *selectedItem = m_subjectListWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, tr("删除科目"), tr("请先选择一个要删除的科目。"));
        return;
    }

    int subjectId = selectedItem->data(Qt::UserRole).toInt();
    QString subjectName = selectedItem->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("确认删除"),
                                  tr("确定要删除科目 '%1' 吗？\n与此科目关联的所有任务也将被删除（如果已实现）。").arg(subjectName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteSubject(subjectId, m_currentUser.id)) {
            loadSubjectsForCurrentUser(); // Refresh list
            QMessageBox::information(this, tr("成功"), tr("科目 '%1' 已删除。").arg(subjectName));
        } else {
            QMessageBox::critical(this, tr("错误"), tr("无法删除科目 '%1'。").arg(subjectName));
        }
    }
}

// Task management functions TM-002
void MainWindow::setupTaskView() {
    // Assuming mainContentLayout is the QVBoxLayout of centralwidget from the UI
    // If you added a placeholder in UI: m_taskTableView = ui->taskTableViewPlaceholder;
    // else:
    m_taskTableView = new QTableView(this);
    m_taskTableModel = new QStandardItemModel(0, 6, this); // Rows, Cols (Name, Due, Priority, Status, Est. Time, Actual Time)
    
    m_taskTableModel->setHorizontalHeaderLabels({
        tr("任务名称"), tr("截止日期"), tr("优先级"), tr("状态"), tr("预计(分钟)"), tr("实际(分钟)")
    });
    m_taskTableView->setModel(m_taskTableModel);
    m_taskTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_taskTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_taskTableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Read-only table
    m_taskTableView->horizontalHeader()->setStretchLastSection(true);
    m_taskTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_taskTableView, &QTableView::doubleClicked, this, &MainWindow::onTaskTableDoubleClicked);
    connect(m_taskTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateTaskActionButtons);
    connect(m_taskTableView, &QTableView::customContextMenuRequested, this, [this](const QPoint &pos){
        QModelIndex index = m_taskTableView->indexAt(pos);
        if (index.isValid()) {
            QMenu contextMenu(this);
            QAction *editAction = contextMenu.addAction(tr("编辑任务..."));
            QAction *deleteAction = contextMenu.addAction(tr("删除任务"));
            contextMenu.addSeparator();
            QAction *markPendingAction = contextMenu.addAction(tr("标记为 待办"));
            QAction *markInProgressAction = contextMenu.addAction(tr("标记为 进行中"));
            QAction *markCompletedAction = contextMenu.addAction(tr("标记为 已完成"));

            connect(editAction, &QAction::triggered, this, &MainWindow::onEditTask);
            connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteTask);
            
            auto createTaskStatusUpdater = [&](TaskStatus status) {
                return [this, status]() {
                    Task task = getCurrentSelectedTask();
                    if (task.isValid() && DatabaseManager::instance().updateTaskStatus(task.id, status, m_currentUser.id)) {
                        loadTasksForSubject(task.subjectId); // Refresh
                    } else {
                        QMessageBox::warning(this, tr("错误"), tr("无法更新任务状态。"));
                    }
                };
            };
            connect(markPendingAction, &QAction::triggered, createTaskStatusUpdater(TaskStatus::Pending));
            connect(markInProgressAction, &QAction::triggered, createTaskStatusUpdater(TaskStatus::InProgress));
            connect(markCompletedAction, &QAction::triggered, createTaskStatusUpdater(TaskStatus::Completed));
            
            contextMenu.exec(m_taskTableView->viewport()->mapToGlobal(pos));
        }
    });


    // Layout for task buttons
    QHBoxLayout *taskButtonLayout = new QHBoxLayout();
    m_addTaskButton = new QPushButton(tr("添加任务"), this);
    m_editTaskButton = new QPushButton(tr("编辑任务"), this);
    m_deleteTaskButton = new QPushButton(tr("删除任务"), this);
    // m_changeTaskStatusButton = new QPushButton(tr("更改状态"), this); // Example

    connect(m_addTaskButton, &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(m_editTaskButton, &QPushButton::clicked, this, &MainWindow::onEditTask);
    connect(m_deleteTaskButton, &QPushButton::clicked, this, &MainWindow::onDeleteTask);
    // connect(m_changeTaskStatusButton, &QPushButton::clicked, this, &MainWindow::onChangeTaskStatus);


    taskButtonLayout->addWidget(m_addTaskButton);
    taskButtonLayout->addWidget(m_editTaskButton);
    taskButtonLayout->addWidget(m_deleteTaskButton);
    // taskButtonLayout->addWidget(m_changeTaskStatusButton);
    taskButtonLayout->addStretch();

    // Add to the central widget's layout (mainContentLayout from .ui file)
    QVBoxLayout* centralLayout;
    if (ui->centralwidget->layout()) {
        centralLayout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    } else {
        centralLayout = new QVBoxLayout(ui->centralwidget); // Should exist from .ui
    }
    
    if (centralLayout) { // Should be named mainContentLayout in .ui
        QLabel* taskSectionLabel = new QLabel(tr("当前科目任务:"), this);
        QFont font = taskSectionLabel->font();
        font.setPointSize(12);
        font.setBold(true);
        taskSectionLabel->setFont(font);

        centralLayout->addWidget(taskSectionLabel);
        centralLayout->addLayout(taskButtonLayout);
        centralLayout->addWidget(m_taskTableView);
    } else {
        qWarning() << "Central widget layout (mainContentLayout) not found or not QVBoxLayout!";
    }
}

Subject MainWindow::getCurrentSelectedSubject() {
    QListWidgetItem *currentSubjectItem = m_subjectListWidget->currentItem();
    if (currentSubjectItem && m_currentUser.isValid()) {
        int subjectId = currentSubjectItem->data(Qt::UserRole).toInt();
        return DatabaseManager::instance().getSubjectById(subjectId, m_currentUser.id);
    }
    return Subject(); // Invalid subject
}

Task MainWindow::getCurrentSelectedTask() {
    QModelIndexList selectedIndexes = m_taskTableView->selectionModel()->selectedRows();
    if (!selectedIndexes.isEmpty() && m_currentUser.isValid()) {
        int taskId = m_taskTableModel->item(selectedIndexes.first().row(), 0)->data(Qt::UserRole).toInt(); // Assuming ID in UserRole of first col
        return DatabaseManager::instance().getTaskById(taskId, m_currentUser.id);
    }
    return Task(); // Invalid task
}

void MainWindow::loadTasksForSubject(int subjectId) {
    if (!m_taskTableModel) return;
    m_taskTableModel->removeRows(0, m_taskTableModel->rowCount()); // Clear existing rows

    if (!m_currentUser.isValid() || subjectId == -1) {
        updateTaskActionButtons();
        return;
    }

    QList<Task> tasks = DatabaseManager::instance().getTasksBySubject(subjectId, m_currentUser.id, TaskStatus(-1), true /*include completed*/);
    for (const Task& task : tasks) {
        QList<QStandardItem*> rowItems;
        QStandardItem* nameItem = new QStandardItem(task.name);
        nameItem->setData(task.id, Qt::UserRole); // Store task ID
        nameItem->setToolTip(task.description);
        rowItems.append(nameItem);

        rowItems.append(new QStandardItem(task.dueDate.isValid() ? task.dueDate.toString("yyyy-MM-dd HH:mm") : tr("未设置")));
        rowItems.append(new QStandardItem(priorityToString(task.priority)));
        
        QStandardItem* statusItem = new QStandardItem(statusToString(task.status));
        if (task.status == TaskStatus::Completed) {
            QFont font = statusItem->font();
            font.setStrikeOut(true);
            statusItem->setFont(font);
            statusItem->setForeground(Qt::gray);
        }
        rowItems.append(statusItem);

        rowItems.append(new QStandardItem(QString::number(task.estimatedTimeMinutes)));
        rowItems.append(new QStandardItem(QString::number(task.actualTimeMinutes)));
        
        m_taskTableModel->appendRow(rowItems);
    }
    m_taskTableView->resizeColumnsToContents();
    updateTaskActionButtons();
    if (m_timerWidget) m_timerWidget->refreshTargetSelection();
}


void MainWindow::updateTaskActionButtons() {
    bool subjectSelected = m_subjectListWidget->currentItem() != nullptr;
    bool taskSelected = !m_taskTableView->selectionModel()->selectedRows().isEmpty();

    m_addTaskButton->setEnabled(subjectSelected && m_currentUser.isValid());
    m_editTaskButton->setEnabled(taskSelected && m_currentUser.isValid());
    m_deleteTaskButton->setEnabled(taskSelected && m_currentUser.isValid());
    // m_changeTaskStatusButton->setEnabled(taskSelected && m_currentUser.isValid());
}

void MainWindow::onTaskTableDoubleClicked(const QModelIndex &index) {
    if (index.isValid()) {
        onEditTask(); // Edit on double click
    }
}

void MainWindow::onAddTask() {
    Subject currentSubject = getCurrentSelectedSubject();
    if (!currentSubject.isValid()) {
        QMessageBox::information(this, tr("添加任务"), tr("请先选择一个科目。"));
        return;
    }

    QList<Subject> allSubjects = DatabaseManager::instance().getSubjectsByUser(m_currentUser.id);
    TaskDialog dialog(m_currentUser.id, allSubjects, this, currentSubject.id);

    if (dialog.exec() == QDialog::Accepted) {
        Task newTask = dialog.getTaskData();
        // newTask.userId is set in dialog, subjectId is set from combobox
        // creationDate is set in dialog

        if (DatabaseManager::instance().addTask(newTask)) {
            loadTasksForSubject(currentSubject.id); // Refresh list
            QMessageBox::information(this, tr("成功"), tr("任务 '%1' 已添加。").arg(newTask.name));
        } else {
            QMessageBox::critical(this, tr("错误"), tr("无法添加任务到数据库。"));
        }
    }
}

void MainWindow::onEditTask() {
    Task currentTask = getCurrentSelectedTask();
    if (!currentTask.isValid()) {
        QMessageBox::information(this, tr("编辑任务"), tr("请先选择一个任务进行编辑。"));
        return;
    }

    QList<Subject> allSubjects = DatabaseManager::instance().getSubjectsByUser(m_currentUser.id);
    TaskDialog dialog(m_currentUser.id, currentTask, allSubjects, this);

    if (dialog.exec() == QDialog::Accepted) {
        Task updatedTask = dialog.getTaskData();
        // Ensure ID and UserID are preserved from the original task for update
        updatedTask.id = currentTask.id;
        updatedTask.userId = m_currentUser.id;
        // actualTimeMinutes and creationDate are not typically changed by this dialog, ensure they are preserved
        // or handled if the dialog can modify them. TaskDialog currently doesn't modify actualTime.
        // It does re-set creationDate if it's a new task, but for edit, m_task(taskToEdit) preserves it.

        if (DatabaseManager::instance().updateTask(updatedTask)) {
            loadTasksForSubject(updatedTask.subjectId); // Refresh list
            QMessageBox::information(this, tr("成功"), tr("任务 '%1' 已更新。").arg(updatedTask.name));
        } else {
            QMessageBox::critical(this, tr("错误"), tr("无法更新任务。"));
        }
    }
}

void MainWindow::onDeleteTask() {
    Task currentTask = getCurrentSelectedTask();
    if (!currentTask.isValid()) {
        QMessageBox::information(this, tr("删除任务"), tr("请先选择一个任务进行删除。"));
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("确认删除"),
                                  tr("确定要删除任务 '%1' 吗？").arg(currentTask.name),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteTask(currentTask.id, m_currentUser.id)) {
            loadTasksForSubject(currentTask.subjectId); // Refresh list
            QMessageBox::information(this, tr("成功"), tr("任务 '%1' 已删除。").arg(currentTask.name));
        } else {
            QMessageBox::critical(this, tr("错误"), tr("无法删除任务 '%1'。").arg(currentTask.name));
        }
    }
}

void MainWindow::onChangeTaskStatus() {
    // This is now handled by the context menu.
    // If you want a button too, implement similar logic.
    Task currentTask = getCurrentSelectedTask();
    if (!currentTask.isValid()) {
        QMessageBox::information(this, tr("更改状态"), tr("请选择一个任务。"));
        return;
    }
    // Example: cycle status or open a small dialog/menu
    // For now, rely on context menu
    QMessageBox::information(this, tr("提示"), tr("请使用右键菜单更改任务状态。"));
}


// Remember to add calls to updateTaskActionButtons() in appropriate places,
// e.g., after loading tasks, or when subject selection changes to an empty state.
// ... (rest of MainWindow.cpp)


// Time TM-003

void MainWindow::setupTimerDockWidget() {
    m_timerDockWidget = new QDockWidget(tr("学习计时器"), this);
    m_timerDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_timerWidget = new TimerWidget(m_timerDockWidget);
    m_timerDockWidget->setWidget(m_timerWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_timerDockWidget); // Add to right, for example

    // Connect signal from timer widget if needed
    connect(m_timerWidget, &TimerWidget::timerStoppedAndSaved, this, &MainWindow::onTimerLoggedNewEntry);

    // Add to View menu
    QMenu *viewMenu = menuBar()->findChild<QMenu*>("viewMenu"); // Assuming you have a view menu
    if (!viewMenu) { // Or create if it doesn't exist
        viewMenu = menuBar()->addMenu(tr("视图(&V)"));
        viewMenu->setObjectName("viewMenu"); // Give it an object name to find it later
    }
    if (viewMenu) { // Check again
        viewMenu->addSeparator();
        viewMenu->addAction(m_timerDockWidget->toggleViewAction());
    }
}

void MainWindow::onTimerLoggedNewEntry(const TimeLog& newLog) {
    qDebug() << "MainWindow received new TimeLog ID:" << newLog.id;
    // If the log was for a task, the task's actual time has been updated in DB.
    // We need to refresh the task view if the timed task is currently visible.
    Subject currentSubject = getCurrentSelectedSubject();
    if (currentSubject.isValid() && (newLog.subjectId == currentSubject.id || newLog.subjectId == -1 /* general log */)) {
        // If the log is related to the currently selected subject (or is general for the user)
        // and the task is in the current subject view, refresh it.
        // Check if newLog.taskId is in the current view
        bool refreshNeeded = false;
        if (newLog.taskId != -1) {
            for(int i = 0; i < m_taskTableModel->rowCount(); ++i) {
                if (m_taskTableModel->item(i,0)->data(Qt::UserRole).toInt() == newLog.taskId) {
                    refreshNeeded = true;
                    break;
                }
            }
        }
        if (refreshNeeded || newLog.taskId == -1) { // Refresh if task was in view or it was a subject-only log
             loadTasksForSubject(currentSubject.id);
        }
    }
}


// Make sure to call m_timerWidget->refreshTargetSelection(); after any CRUD operation
// on Subjects or Tasks that might affect the comboboxes in TimerWidget.
// For example, in onAddSubject, onEditSubject, onDeleteSubject (after loadSubjectsForCurrentUser):
// if (m_timerWidget) m_timerWidget->refreshTargetSelection();
// And in onAddTask, onEditTask, onDeleteTask (after loadTasksForSubject):
// if (m_timerWidget) m_timerWidget->refreshTargetSelection();
