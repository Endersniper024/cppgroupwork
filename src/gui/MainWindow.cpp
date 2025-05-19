#include "MainWindow.h"
#include "ui_MainWindow.h" // 由 uic 生成
#include "DatabaseManager.h"
#include "LoginDialog.h"    // 需要包含 LoginDialog 以便可以重新显示
#include "ProfileDialog.h"  // (后续添加)
#include "gui/SubjectDialog.h" // For opening the subject dialog


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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("协同学习时间管理平台");
    setupMenuBar();
    setupSubjectDockWidget(); // Add this call
    updateStatusBar();
    updateSubjectActionButtons(); // Initial state
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
    if (m_currentUser.isValid()) {
        qDebug() << "MainWindow: Current user set to" << m_currentUser.email;
        ui->welcomeLabel->setText(QString("欢迎您, %1!").arg(m_currentUser.nickname.isEmpty() ? m_currentUser.email : m_currentUser.nickname));
        loadSubjectsForCurrentUser(); // Load subjects when user changes
    } else {
        ui->welcomeLabel->setText("未登录");
        m_subjectListWidget->clear(); // Clear subjects if no user
    }
    updateStatusBar();
    updateSubjectActionButtons();
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
}

void MainWindow::updateSubjectActionButtons() {
    bool itemSelected = m_subjectListWidget->currentItem() != nullptr;
    m_editSubjectButton->setEnabled(itemSelected);
    m_deleteSubjectButton->setEnabled(itemSelected);
    m_addSubjectButton->setEnabled(m_currentUser.isValid()); // Can add if user is logged in
}


void MainWindow::onSubjectListSelectionChanged() {
    updateSubjectActionButtons();
    QListWidgetItem *currentItem = m_subjectListWidget->currentItem();
    if (currentItem) {
        int subjectId = currentItem->data(Qt::UserRole).toInt();
        qDebug() << "Selected Subject ID:" << subjectId << "Name:" << currentItem->text();
        // TODO: Later, load tasks for this subject into a central view
    } else {
        qDebug() << "No subject selected.";
        // TODO: Clear or update task view
    }
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