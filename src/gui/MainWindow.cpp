#include "MainWindow.h"
#include "DatabaseManager.h"
#include "LoginDialog.h"   // 需要包含 LoginDialog 以便可以重新显示
#include "ProfileDialog.h" // (后续添加)
#include "gui/SubjectDialog.h" // For opening the subject dialog
#include "gui/TaskDialog.h"    // For task management
#include "gui/TimerWidget.h"   // Time
#include "ui_MainWindow.h"     // 由 uic 生成

// === TM-006: Includes ===
#include "core/reports/DataAggregator.h" // This must provide the full class definition, not just a forward declaration
#include "core/reports/IDataAggregator.h" // Ensure this header contains the full class definition
#include "core/reports/LLMCommunicator.h" // Concrete implementation for instantiation
#include "core/reports/ReportDataStructures.h"
#include "core/reports/ReportGenerator.h"
#include "core/reports/ReportParameters.h"
#include "gui/report/ReportDisplayDialog.h"  // Include for ReportDisplayDialog
#include "gui/report/ReportSettingsDialog.h" // You'll need to create this dialog

// #include "core/reports/*" // Remove or comment out this line; it's not a
// valid header and may cause confusion
// === End TM-006 Includes ===

#include <QAction>
#include <QApplication> // For qApp
#include <QCursor>
#include <QDebug>
#include <QDockWidget>
#include <QEnterEvent>
#include <QHBoxLayout>
#include <QHeaderView>  // For table header properties
#include <QInputDialog> // For simple input, though SubjectDialog is better
#include <QListWidget>
#include <QMenu> // For context menu on tasks
#include <QMenuBar>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScreen>
#include <QScreen> // For QScreen pointer usage
#include <QSettings> // For application settings (fixes QSettings not declared error)
#include <QStandardItemModel>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_dataAggregator(nullptr),
      m_metricCalculator(nullptr), m_llmCommunicator(nullptr),
      m_reportGenerator(nullptr), m_generateReportAction(nullptr),

      m_floatingToolbar(nullptr),
      m_floatingHideTimer(new QTimer(this)), // 浮动栏初始化
      m_mouseCheckTimer(new QTimer(this)),
      m_floatingAnimation(new QPropertyAnimation(this)),
      m_floatingToolbarHidden(false),
      m_toggleFloatingAction(nullptr), // 托盘相关初始化
      m_trayIcon(nullptr), m_trayMenu(nullptr), m_showAction(nullptr),
      m_hideAction(nullptr), m_quitAction(nullptr), m_addSubjectAction(nullptr),
      m_addTaskAction(nullptr), m_startTimerAction(nullptr),
      m_toggleFloatingTrayAction(nullptr), m_isInTrayMode(false),
      m_floatingToolbarEnabled(true), m_trayMessageShown(false),
      m_globalMouseTimer(new QTimer(this)) {
  // qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" ;

  // 设置全局字体变大，自适应分辨率
  QFont appFont = qApp->font();
  // 你可以根据屏幕分辨率动态调整字号
  QScreen *screen = QGuiApplication::primaryScreen();
  int dpi = screen ? int(screen->logicalDotsPerInch()) : 96;
  int basePointSize = 13;
  if (dpi > 120) {
    basePointSize = 17;
  } else if (dpi > 100) {
    basePointSize = 15;
  }
  appFont.setPointSize(basePointSize);
  qApp->setFont(appFont);

  ui->setupUi(this);
  qDebug() << "1";
  setWindowTitle("协同学习时间管理平台");
  qDebug() << "2";
  setupMenuBar();
  qDebug() << "3";
  setupSubjectDockWidget(); // Add this call
  qDebug() << "4";
  setupTaskView();
  qDebug() << "5";
  setupTimerDockWidget();
  qDebug() << "6";
  initializeReportComponents(); // Initialize TM-006 components
  m_activityMonitor = new ActivityMonitorService(this);
  connect(m_activityMonitor, &ActivityMonitorService::timedSegmentLogged, this,
          &MainWindow::onAutoTimeSegmentLogged);
  // connect(m_activityMonitor, &ActivityMonitorService::currentActivityUpdate,
  // this, &MainWindow::onAutoActivityUpdate);

  qDebug() << "7";

  updateStatusBar();
  updateSubjectActionButtons(); // Initial Subject state
  updateTaskActionButtons();    // Initial state for task buttons
  // 初始时，主窗口可以隐藏，直到登录成功
  // this->hide();

  if (m_generateReportAction) {
    m_generateReportAction->setEnabled(false);
  }

  // 设置系统托盘
  setupSystemTray();
  // 设置全局鼠标监控
  setupGlobalMouseMonitoring();
  // begin of floating toolbar

  // 设置浮动工具栏
  setupFloatingToolbar();

  // 连接浮动工具栏相关信号
  connect(m_floatingHideTimer, &QTimer::timeout, this,
          &MainWindow::hideFloatingToolbar);
  connect(m_mouseCheckTimer, &QTimer::timeout, this,
          &MainWindow::checkMousePositionForFloating);

  // 启动鼠标位置检测
  m_mouseCheckTimer->start(50);

  // 安装事件过滤器
  qApp->installEventFilter(this);
  // end of floating toolbar
}

MainWindow::~MainWindow() {
  // begin of 托盘、浮动工具栏相关
  // 停止所有计时器
  if (m_globalMouseTimer)
    m_globalMouseTimer->stop();
  if (m_mouseCheckTimer)
    m_mouseCheckTimer->stop();
  if (m_floatingHideTimer)
    m_floatingHideTimer->stop();

  // 隐藏托盘图标
  if (m_trayIcon) {
    m_trayIcon->hide();
  }

  // 隐藏浮动工具栏
  if (m_floatingToolbar) {
    m_floatingToolbar->hide();
  }
  // end
  cleanupReportComponents();     // Cleanup TM-006 components
  qApp->removeEventFilter(this); // floating toolbar
  delete ui;
}

void MainWindow::setupMenuBar() {
  QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
  QAction *profileAction = fileMenu->addAction("个人信息(&P)..."); // UM-004
  fileMenu->addSeparator();
  QAction *logoutAction = fileMenu->addAction("登出(&L)"); // UM-002
  QAction *exitAction = fileMenu->addAction("退出(&X)");

  connect(profileAction, &QAction::triggered, this,
          &MainWindow::openProfileDialog);
  connect(logoutAction, &QAction::triggered, this, &MainWindow::handleLogout);
  connect(exitAction, &QAction::triggered, qApp,
          &QApplication::quit); // qApp 是 QApplication 的全局指针

  // 可以在这里添加更多菜单，如“任务管理”、“统计报告”等
  QMenu *taskMenu = menuBar()->addMenu("任务(&T)");
  // ... add task related actions ...

  // === TM-006: Add Reports Menu ===
  QMenu *reportsMenu = menuBar()->addMenu(tr("报告(&R)"));
  m_generateReportAction = reportsMenu->addAction(tr("生成学习报告(&G)..."));
  connect(m_generateReportAction, &QAction::triggered, this,
          &MainWindow::onGenerateReportActionTriggered);
  // === End TM-006 Menu ===

  // QMenu *viewMenu = menuBar()->findChild<QMenu*>("viewMenu"); // Find
  // existing or create if (!viewMenu) {
  //     viewMenu = menuBar()->addMenu(tr("视图(&V)"));
  //     viewMenu->setObjectName("viewMenu");
  // }
  // if (m_subjectDockWidget)
  // viewMenu->addAction(m_subjectDockWidget->toggleViewAction()); if
  // (m_timerDockWidget)
  // viewMenu->addAction(m_timerDockWidget->toggleViewAction()); Add toggle for
  // WinAPI timer dock if it exists

  // begin of floating toolbar
  // 添加到视图菜单
  QMenu *viewMenu = menuBar()->findChild<QMenu *>("viewMenu");
  if (!viewMenu) {
    viewMenu = menuBar()->addMenu(tr("视图(&V)"));
    viewMenu->setObjectName("viewMenu");
  }

  // 添加浮动工具栏切换选项
  m_toggleFloatingAction = viewMenu->addAction(tr("浮动工具栏(&F)"));
  m_toggleFloatingAction->setCheckable(true);
  m_toggleFloatingAction->setChecked(true);
  connect(m_toggleFloatingAction, &QAction::triggered, this,
          &MainWindow::toggleFloatingToolbar);
  // end of floating toolbar
  // begin of 托盘相关
  // 在方法最后添加窗口菜单
  QMenu *windowMenu = menuBar()->addMenu(tr("窗口(&W)"));

  QAction *minimizeAction = new QAction(tr("最小化"), this);
  // 修复：使用正确的快捷键设置方式
  minimizeAction->setShortcut(
      QKeySequence("Ctrl+M")); // 或者使用其他合适的快捷键
  connect(minimizeAction, &QAction::triggered, this, &QWidget::showMinimized);

  QAction *hideToTrayAction = new QAction(tr("隐藏到托盘"), this);
  hideToTrayAction->setShortcut(QKeySequence("Ctrl+H"));
  connect(hideToTrayAction, &QAction::triggered, this, &MainWindow::hideToTray);

  windowMenu->addAction(minimizeAction);
  windowMenu->addAction(hideToTrayAction);
  windowMenu->addSeparator();

  // 现有的浮动工具栏切换
  if (!m_toggleFloatingAction) {
    m_toggleFloatingAction = new QAction(tr("浮动工具栏(&F)"), this);
    m_toggleFloatingAction->setCheckable(true);
    m_toggleFloatingAction->setChecked(true);
    connect(m_toggleFloatingAction, &QAction::triggered, this,
            &MainWindow::toggleFloatingToolbar);
  }
  windowMenu->addAction(m_toggleFloatingAction);
  // end of 托盘相关
}

// void MainWindow::setCurrentUser(const User& user) {
//     m_currentUser = user;
//     if (m_currentUser.isValid()) {
//         qDebug() << "MainWindow: Current user set to" << m_currentUser.email;
//         ui->welcomeLabel->setText(QString("欢迎您,
//         %1!").arg(m_currentUser.nickname.isEmpty() ? m_currentUser.email :
//         m_currentUser.nickname));
//     } else {
//         ui->welcomeLabel->setText("未登录");
//     }
//     updateStatusBar();
// }

void MainWindow::setCurrentUser(const User &user) {
  m_currentUser = user;

  if (m_timerWidget)
    m_timerWidget->setCurrentUser(m_currentUser); // Pass user to timer

  if (m_currentUser.isValid()) {
    qDebug() << "MainWindow: Current user set to" << m_currentUser.email;
    ui->welcomeLabel->setText(QString("欢迎您, %1!")
                                  .arg(m_currentUser.nickname.isEmpty()
                                           ? m_currentUser.email
                                           : m_currentUser.nickname));
    loadSubjectsForCurrentUser(); // Load subjects when user changes
    if (m_generateReportAction)
      m_generateReportAction->setEnabled(true);
  } else {
    ui->welcomeLabel->setText("未登录");
    m_subjectListWidget->clear(); // Clear subjects if no user
    if (m_generateReportAction)
      m_generateReportAction->setEnabled(false);
    if (m_taskTableModel)
      m_taskTableModel->clear();
    if (m_timerWidget)
      m_timerWidget->refreshTargetSelection();
  }
  // begin of floating toolbar
  if (m_floatingToolbar) {
    QLabel *userLabel =
        m_floatingToolbar->findChild<QLabel *>("floatingUserLabel");
    if (userLabel) {
      if (m_currentUser.isValid()) {
        userLabel->setText(QString("用户: %1")
                               .arg(m_currentUser.nickname.isEmpty()
                                        ? m_currentUser.email
                                        : m_currentUser.nickname));
      } else {
        userLabel->setText("未登录");
      }
    }
  }
  // end of floating toolbar

  // begin of 托盘
  // 在方法最后的浮动工具栏更新部分修改：
  if (m_floatingToolbar) {
    QLabel *userLabel =
        m_floatingToolbar->findChild<QLabel *>("floatingUserLabel");
    if (userLabel) {
      if (m_currentUser.isValid()) {
        userLabel->setText(QString("用户: %1")
                               .arg(m_currentUser.nickname.isEmpty()
                                        ? m_currentUser.email
                                        : m_currentUser.nickname));
      } else {
        userLabel->setText("未登录");
      }
    }

    // 更新托盘模式状态
    updateFloatingToolbarForTrayMode();
  }

  // 更新托盘图标提示
  if (m_trayIcon) {
    QString tooltip = tr("学习时间管理器");
    if (m_currentUser.isValid()) {
      tooltip +=
          tr("\n用户: %1")
              .arg(m_currentUser.nickname.isEmpty() ? m_currentUser.email
                                                    : m_currentUser.nickname);
    }
    m_trayIcon->setToolTip(tooltip);
  }
  // end of 托盘
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
  // loginDialog.setWindowModality(Qt::ApplicationModal); //
  // 确保登录对话框是模态的

  if (loginDialog.exec() == QDialog::Accepted) {
    QString userEmail = loginDialog.getEmail();
    User loggedInUser = DatabaseManager::instance().getUserByEmail(userEmail);
    if (loggedInUser.isValid()) {
      setCurrentUser(loggedInUser);
      this->show();
    } else {
      // 这种情况理论上不应该发生，因为 LoginDialog 成功时用户已验证
      QMessageBox::critical(this, "错误",
                            "重新登录失败，用户数据未找到。应用将退出。");
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
//     // ProfileDialog profileDlg(m_currentUser, this); // 创建个人信息对话框
//     (后续实现)
//     // if (profileDlg.exec() == QDialog::Accepted) {
//     //     m_currentUser = profileDlg.getUpdatedUser(); //
//     获取更新后的用户信息
//     //     DatabaseManager::instance().updateUser(m_currentUser);
//     //     setCurrentUser(m_currentUser); // 刷新主窗口显示
//     //     QMessageBox::information(this, "成功", "个人信息已更新。");
//     // }
//     QMessageBox::information(this, "提示", "个人信息管理功能 (UM-004)
//     待实现。");
// }

void MainWindow::openProfileDialog() {
  if (!m_currentUser.isValid()) {
    QMessageBox::warning(this, "未登录", "请先登录后再管理个人信息。");
    return;
  }

  ProfileDialog profileDlg(m_currentUser, this);
  // The ProfileDialog::on_buttonBox_accepted now handles calling
  // QDialog::accept() itself. So, the exec() call will return QDialog::Accepted
  // only if our validation passed.
  if (profileDlg.exec() == QDialog::Accepted) {
    User updatedUser = profileDlg.getUpdatedUser();

    // Check if actual changes were made that require a DB update
    bool userChanged = (m_currentUser.nickname != updatedUser.nickname ||
                        m_currentUser.avatarPath != updatedUser.avatarPath ||
                        m_currentUser.passwordHash != updatedUser.passwordHash);

    if (userChanged) {
      if (DatabaseManager::instance().updateUser(updatedUser)) {
        m_currentUser = updatedUser;   // Update MainWindow's copy
        setCurrentUser(m_currentUser); // Refresh UI elements in MainWindow
        QMessageBox::information(this, "成功", "个人信息已更新。");
      } else {
        QMessageBox::critical(this, "错误",
                              "更新个人信息失败，数据库操作错误。");
        // Optionally, revert m_currentUser if needed or log error
      }
    } else {
      QMessageBox::information(this, "提示", "未检测到任何信息更改。");
    }
  }
}

void MainWindow::setupSubjectDockWidget() {
  m_subjectDockWidget = new QDockWidget(tr("学习科目"), this);
  m_subjectDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea |
                                       Qt::RightDockWidgetArea);

  QWidget *dockContents = new QWidget(m_subjectDockWidget);
  QVBoxLayout *layout = new QVBoxLayout(dockContents);

  m_subjectListWidget = new QListWidget(dockContents);
  connect(m_subjectListWidget, &QListWidget::itemSelectionChanged, this,
          &MainWindow::onSubjectListSelectionChanged);
  // We can also connect itemDoubleClicked for editing
  connect(m_subjectListWidget, &QListWidget::itemDoubleClicked, this,
          [this](QListWidgetItem *item) {
            if (item)
              onEditSubject(); // Trigger edit on double click
          });

  QFont listFont = m_subjectListWidget->font();
  listFont.setPointSize(listFont.pointSize() + 10);
  m_subjectListWidget->setFont(listFont);
  layout->addWidget(m_subjectListWidget);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  m_addSubjectButton = new QPushButton(tr("添加"), dockContents);
  m_editSubjectButton = new QPushButton(tr("编辑"), dockContents);
  m_deleteSubjectButton = new QPushButton(tr("删除"), dockContents);

  connect(m_addSubjectButton, &QPushButton::clicked, this,
          &MainWindow::onAddSubject);
  connect(m_editSubjectButton, &QPushButton::clicked, this,
          &MainWindow::onEditSubject);
  connect(m_deleteSubjectButton, &QPushButton::clicked, this,
          &MainWindow::onDeleteSubject);

  buttonLayout->addWidget(m_addSubjectButton);
  buttonLayout->addWidget(m_editSubjectButton);
  buttonLayout->addWidget(m_deleteSubjectButton);
  layout->addLayout(buttonLayout);

  dockContents->setLayout(layout);
  m_subjectDockWidget->setWidget(dockContents);
  addDockWidget(Qt::LeftDockWidgetArea, m_subjectDockWidget);

  // Example: Add a menu item to show/hide this dock widget
  QMenu *viewMenu = menuBar()->findChild<QMenu *>("viewMenu");
  if (!viewMenu) {
    viewMenu = menuBar()->addMenu(tr("视图(&V)"));
    viewMenu->setObjectName("viewMenu");
  }
  // else {
  viewMenu->addAction(m_subjectDockWidget->toggleViewAction());
  // }
  // QMenu *viewMenu = menuBar()->addMenu(tr("视图(&V)"));
}

void MainWindow::loadSubjectsForCurrentUser() {
  m_subjectListWidget->clear();
  if (!m_currentUser.isValid()) {
    updateSubjectActionButtons();
    return;
  }

  QList<Subject> subjects =
      DatabaseManager::instance().getSubjectsByUser(m_currentUser.id);
  for (const Subject &subject : subjects) {
    QListWidgetItem *item =
        new QListWidgetItem(subject.name, m_subjectListWidget);
    item->setData(Qt::UserRole, subject.id); // Store subject ID
    if (subject.color.isValid()) {
      item->setForeground(subject.color.darker(
          120)); // Use color for text, slightly darker for visibility
                 // Or set background: item->setBackground(subject.color);
                 // Or add a small color indicator icon
    }
  }
  updateSubjectActionButtons();
  if (m_timerWidget)
    m_timerWidget->refreshTargetSelection();
}

void MainWindow::updateSubjectActionButtons() {
  bool itemSelected = m_subjectListWidget->currentItem() != nullptr;
  m_editSubjectButton->setEnabled(itemSelected);
  m_deleteSubjectButton->setEnabled(itemSelected);
  m_addSubjectButton->setEnabled(
      m_currentUser.isValid()); // Can add if user is logged in
}

// void MainWindow::onSubjectListSelectionChanged() {
//     updateSubjectActionButtons();
//     QListWidgetItem *currentItem = m_subjectListWidget->currentItem();
//     if (currentItem) {
//         int subjectId = currentItem->data(Qt::UserRole).toInt();
//         qDebug() << "Selected Subject ID:" << subjectId << "Name:" <<
//         currentItem->text();
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
    qDebug() << "Selected Subject ID:" << selectedSubject.id
             << "Name:" << selectedSubject.name;
    loadTasksForSubject(selectedSubject.id);
  } else {
    qDebug() << "No subject selected or invalid.";
    if (m_taskTableModel)
      m_taskTableModel->clear(); // Clear task table
    m_taskTableModel->setHorizontalHeaderLabels(
        {tr("任务名称"), tr("截止日期"), tr("优先级"), tr("状态"),
         tr("预计(分钟)"), tr("实际(分钟)")}); // Reset headers if cleared
  }
  updateTaskActionButtons(); // Update based on new task list (empty or not)
}

void MainWindow::onAddSubject() {
  if (!m_currentUser.isValid())
    return;

  SubjectDialog dialog(this);

  if (showDialogCentered(&dialog) == QDialog::Accepted) {
    Subject newSubject = dialog.getSubjectData();
    newSubject.userId = m_currentUser.id; // Set current user ID

    // The SubjectDialog validation for name emptiness is done.
    // Now, check for uniqueness with DatabaseManager before adding.
    if (DatabaseManager::instance().subjectExists(newSubject.name,
                                                  newSubject.userId)) {
      QMessageBox::warning(
          this, tr("添加失败"),
          tr("科目 '%1' 已存在。请使用其他名称。").arg(newSubject.name));
      return;
    }

    if (DatabaseManager::instance().addSubject(
            newSubject)) {          // addSubject now takes ref to get ID
      loadSubjectsForCurrentUser(); // Refresh list
      QMessageBox::information(this, tr("成功"),
                               tr("科目 '%1' 已添加。").arg(newSubject.name));
    } else {
      QMessageBox::critical(this, tr("错误"), tr("无法添加科目到数据库。"));
    }
  }
}

void MainWindow::onEditSubject() {
  if (!m_currentUser.isValid())
    return;

  QListWidgetItem *selectedItem = m_subjectListWidget->currentItem();
  if (!selectedItem) {
    QMessageBox::information(this, tr("编辑科目"),
                             tr("请先选择一个要编辑的科目。"));
    return;
  }

  int subjectId = selectedItem->data(Qt::UserRole).toInt();
  Subject subjectToEdit =
      DatabaseManager::instance().getSubjectById(subjectId, m_currentUser.id);

  if (!subjectToEdit.isValid()) {
    QMessageBox::critical(this, tr("错误"), tr("无法加载科目信息进行编辑。"));
    return;
  }

  SubjectDialog dialog(subjectToEdit, this);
  if (dialog.exec() == QDialog::Accepted) {
    Subject updatedSubject = dialog.getSubjectData();
    updatedSubject.userId = m_currentUser.id; // Ensure userId is set
    updatedSubject.id = subjectId;            // Ensure id is set

    // Check for name uniqueness if name changed
    if (updatedSubject.name != subjectToEdit.name &&
        DatabaseManager::instance().subjectExists(
            updatedSubject.name, updatedSubject.userId, updatedSubject.id)) {
      QMessageBox::warning(
          this, tr("更新失败"),
          tr("科目 '%1' 已存在。请使用其他名称。").arg(updatedSubject.name));
      return;
    }

    if (DatabaseManager::instance().updateSubject(updatedSubject)) {
      loadSubjectsForCurrentUser(); // Refresh list
      QMessageBox::information(
          this, tr("成功"), tr("科目 '%1' 已更新。").arg(updatedSubject.name));
    } else {
      QMessageBox::critical(this, tr("错误"), tr("无法更新科目信息。"));
    }
  }
}

void MainWindow::onDeleteSubject() {
  if (!m_currentUser.isValid())
    return;

  QListWidgetItem *selectedItem = m_subjectListWidget->currentItem();
  if (!selectedItem) {
    QMessageBox::information(this, tr("删除科目"),
                             tr("请先选择一个要删除的科目。"));
    return;
  }

  int subjectId = selectedItem->data(Qt::UserRole).toInt();
  QString subjectName = selectedItem->text();

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
      this, tr("确认删除"),
      tr("确定要删除科目 '%1' "
         "吗？\n与此科目关联的所有任务也将被删除（如果已实现）。")
          .arg(subjectName),
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    if (DatabaseManager::instance().deleteSubject(subjectId,
                                                  m_currentUser.id)) {
      loadSubjectsForCurrentUser(); // Refresh list
      QMessageBox::information(this, tr("成功"),
                               tr("科目 '%1' 已删除。").arg(subjectName));
    } else {
      QMessageBox::critical(this, tr("错误"),
                            tr("无法删除科目 '%1'。").arg(subjectName));
    }
  }
}

// Task management functions TM-002
void MainWindow::setupTaskView() {
  // Assuming mainContentLayout is the QVBoxLayout of centralwidget from the UI
  // If you added a placeholder in UI: m_taskTableView =
  // ui->taskTableViewPlaceholder; else:
  m_taskTableView = new QTableView(this);
  m_taskTableModel = new QStandardItemModel(
      0, 6,
      this); // Rows, Cols (Name, Due, Priority, Status, Est. Time, Actual Time)

  m_taskTableModel->setHorizontalHeaderLabels(
      {tr("任务名称"), tr("截止日期"), tr("优先级"), tr("状态"),
       tr("预计(分钟)"), tr("实际(分钟)")});
  m_taskTableView->setModel(m_taskTableModel);
  m_taskTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_taskTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  m_taskTableView->setEditTriggers(
      QAbstractItemView::NoEditTriggers); // Read-only table
  m_taskTableView->horizontalHeader()->setStretchLastSection(true);
  m_taskTableView->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(m_taskTableView, &QTableView::doubleClicked, this,
          &MainWindow::onTaskTableDoubleClicked);
  connect(m_taskTableView->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &MainWindow::updateTaskActionButtons);
  connect(m_taskTableView, &QTableView::customContextMenuRequested, this,
          [this](const QPoint &pos) {
            QModelIndex index = m_taskTableView->indexAt(pos);
            if (index.isValid()) {
              QMenu contextMenu(this);
              QAction *editAction = contextMenu.addAction(tr("编辑任务..."));
              QAction *deleteAction = contextMenu.addAction(tr("删除任务"));
              contextMenu.addSeparator();
              QAction *markPendingAction =
                  contextMenu.addAction(tr("标记为 待办"));
              QAction *markInProgressAction =
                  contextMenu.addAction(tr("标记为 进行中"));
              QAction *markCompletedAction =
                  contextMenu.addAction(tr("标记为 已完成"));

              connect(editAction, &QAction::triggered, this,
                      &MainWindow::onEditTask);
              connect(deleteAction, &QAction::triggered, this,
                      &MainWindow::onDeleteTask);

              auto createTaskStatusUpdater = [&](TaskStatus status) {
                return [this, status]() {
                  Task task = getCurrentSelectedTask();
                  if (task.isValid() &&
                      DatabaseManager::instance().updateTaskStatus(
                          task.id, status, m_currentUser.id)) {
                    loadTasksForSubject(task.subjectId); // Refresh
                  } else {
                    QMessageBox::warning(this, tr("错误"),
                                         tr("无法更新任务状态。"));
                  }
                };
              };
              connect(markPendingAction, &QAction::triggered,
                      createTaskStatusUpdater(TaskStatus::Pending));
              connect(markInProgressAction, &QAction::triggered,
                      createTaskStatusUpdater(TaskStatus::InProgress));
              connect(markCompletedAction, &QAction::triggered,
                      createTaskStatusUpdater(TaskStatus::Completed));

              contextMenu.exec(m_taskTableView->viewport()->mapToGlobal(pos));
            }
          });

  // Layout for task buttons
  QHBoxLayout *taskButtonLayout = new QHBoxLayout();
  m_addTaskButton = new QPushButton(tr("添加任务"), this);
  m_editTaskButton = new QPushButton(tr("编辑任务"), this);
  m_deleteTaskButton = new QPushButton(tr("删除任务"), this);
  // m_changeTaskStatusButton = new QPushButton(tr("更改状态"), this); //
  // Example

  connect(m_addTaskButton, &QPushButton::clicked, this, &MainWindow::onAddTask);
  connect(m_editTaskButton, &QPushButton::clicked, this,
          &MainWindow::onEditTask);
  connect(m_deleteTaskButton, &QPushButton::clicked, this,
          &MainWindow::onDeleteTask);
  // connect(m_changeTaskStatusButton, &QPushButton::clicked, this,
  // &MainWindow::onChangeTaskStatus);

  taskButtonLayout->addWidget(m_addTaskButton);
  taskButtonLayout->addWidget(m_editTaskButton);
  taskButtonLayout->addWidget(m_deleteTaskButton);
  // taskButtonLayout->addWidget(m_changeTaskStatusButton);
  taskButtonLayout->addStretch();

  // Add to the central widget's layout (mainContentLayout from .ui file)
  QVBoxLayout *centralLayout;
  if (ui->centralwidget->layout()) {
    centralLayout = qobject_cast<QVBoxLayout *>(ui->centralwidget->layout());
  } else {
    centralLayout = new QVBoxLayout(ui->centralwidget); // Should exist from .ui
  }

  if (centralLayout) { // Should be named mainContentLayout in .ui
    QLabel *taskSectionLabel = new QLabel(tr("当前科目任务:"), this);
    QFont font = taskSectionLabel->font();
    font.setPointSize(12);
    font.setBold(true);
    taskSectionLabel->setFont(font);

    centralLayout->addWidget(taskSectionLabel);
    centralLayout->addLayout(taskButtonLayout);
    centralLayout->addWidget(m_taskTableView);
  } else {
    qWarning() << "Central widget layout (mainContentLayout) not found or not "
                  "QVBoxLayout!";
  }
}

Subject MainWindow::getCurrentSelectedSubject() {
  QListWidgetItem *currentSubjectItem = m_subjectListWidget->currentItem();
  if (currentSubjectItem && m_currentUser.isValid()) {
    int subjectId = currentSubjectItem->data(Qt::UserRole).toInt();
    return DatabaseManager::instance().getSubjectById(subjectId,
                                                      m_currentUser.id);
  }
  return Subject(); // Invalid subject
}

Task MainWindow::getCurrentSelectedTask() {
  QModelIndexList selectedIndexes =
      m_taskTableView->selectionModel()->selectedRows();
  if (!selectedIndexes.isEmpty() && m_currentUser.isValid()) {
    int taskId = m_taskTableModel->item(selectedIndexes.first().row(), 0)
                     ->data(Qt::UserRole)
                     .toInt(); // Assuming ID in UserRole of first col
    return DatabaseManager::instance().getTaskById(taskId, m_currentUser.id);
  }
  return Task(); // Invalid task
}

void MainWindow::loadTasksForSubject(int subjectId) {
  if (!m_taskTableModel)
    return;
  m_taskTableModel->removeRows(
      0, m_taskTableModel->rowCount()); // Clear existing rows

  if (!m_currentUser.isValid() || subjectId == -1) {
    updateTaskActionButtons();
    return;
  }

  QList<Task> tasks = DatabaseManager::instance().getTasksBySubject(
      subjectId, m_currentUser.id, TaskStatus(-1), true /*include completed*/);
  for (const Task &task : tasks) {
    QList<QStandardItem *> rowItems;
    QStandardItem *nameItem = new QStandardItem(task.name);
    nameItem->setData(task.id, Qt::UserRole); // Store task ID
    nameItem->setToolTip(task.description);
    rowItems.append(nameItem);

    rowItems.append(new QStandardItem(
        task.dueDate.isValid() ? task.dueDate.toString("yyyy-MM-dd HH:mm")
                               : tr("未设置")));
    rowItems.append(new QStandardItem(priorityToString(task.priority)));

    QStandardItem *statusItem = new QStandardItem(statusToString(task.status));
    if (task.status == TaskStatus::Completed) {
      QFont font = statusItem->font();
      font.setStrikeOut(true);
      statusItem->setFont(font);
      statusItem->setForeground(Qt::gray);
    }
    rowItems.append(statusItem);

    rowItems.append(
        new QStandardItem(QString::number(task.estimatedTimeMinutes)));
    rowItems.append(new QStandardItem(QString::number(task.actualTimeMinutes)));

    m_taskTableModel->appendRow(rowItems);
  }
  m_taskTableView->resizeColumnsToContents();
  updateTaskActionButtons();
  if (m_timerWidget)
    m_timerWidget->refreshTargetSelection();
}

void MainWindow::updateTaskActionButtons() {
  bool subjectSelected = m_subjectListWidget->currentItem() != nullptr;
  bool taskSelected =
      !m_taskTableView->selectionModel()->selectedRows().isEmpty();

  m_addTaskButton->setEnabled(subjectSelected && m_currentUser.isValid());
  m_editTaskButton->setEnabled(taskSelected && m_currentUser.isValid());
  m_deleteTaskButton->setEnabled(taskSelected && m_currentUser.isValid());
  // m_changeTaskStatusButton->setEnabled(taskSelected &&
  // m_currentUser.isValid());
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

  QList<Subject> allSubjects =
      DatabaseManager::instance().getSubjectsByUser(m_currentUser.id);
  TaskDialog dialog(m_currentUser.id, allSubjects, this, currentSubject.id);

  if (dialog.exec() == QDialog::Accepted) {
    Task newTask = dialog.getTaskData();
    // newTask.userId is set in dialog, subjectId is set from combobox
    // creationDate is set in dialog

    if (DatabaseManager::instance().addTask(newTask)) {
      loadTasksForSubject(currentSubject.id); // Refresh list
      QMessageBox::information(this, tr("成功"),
                               tr("任务 '%1' 已添加。").arg(newTask.name));
    } else {
      QMessageBox::critical(this, tr("错误"), tr("无法添加任务到数据库。"));
    }
  }
}

void MainWindow::onEditTask() {
  Task currentTask = getCurrentSelectedTask();
  if (!currentTask.isValid()) {
    QMessageBox::information(this, tr("编辑任务"),
                             tr("请先选择一个任务进行编辑。"));
    return;
  }

  QList<Subject> allSubjects =
      DatabaseManager::instance().getSubjectsByUser(m_currentUser.id);
  TaskDialog dialog(m_currentUser.id, currentTask, allSubjects, this);

  if (dialog.exec() == QDialog::Accepted) {
    Task updatedTask = dialog.getTaskData();
    // Ensure ID and UserID are preserved from the original task for update
    updatedTask.id = currentTask.id;
    updatedTask.userId = m_currentUser.id;
    // actualTimeMinutes and creationDate are not typically changed by this
    // dialog, ensure they are preserved or handled if the dialog can modify
    // them. TaskDialog currently doesn't modify actualTime. It does re-set
    // creationDate if it's a new task, but for edit, m_task(taskToEdit)
    // preserves it.

    if (DatabaseManager::instance().updateTask(updatedTask)) {
      loadTasksForSubject(updatedTask.subjectId); // Refresh list
      QMessageBox::information(this, tr("成功"),
                               tr("任务 '%1' 已更新。").arg(updatedTask.name));
    } else {
      QMessageBox::critical(this, tr("错误"), tr("无法更新任务。"));
    }
  }
}

void MainWindow::onDeleteTask() {
  Task currentTask = getCurrentSelectedTask();
  if (!currentTask.isValid()) {
    QMessageBox::information(this, tr("删除任务"),
                             tr("请先选择一个任务进行删除。"));
    return;
  }

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
      this, tr("确认删除"),
      tr("确定要删除任务 '%1' 吗？").arg(currentTask.name),
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    if (DatabaseManager::instance().deleteTask(currentTask.id,
                                               m_currentUser.id)) {
      loadTasksForSubject(currentTask.subjectId); // Refresh list
      QMessageBox::information(this, tr("成功"),
                               tr("任务 '%1' 已删除。").arg(currentTask.name));
    } else {
      QMessageBox::critical(this, tr("错误"),
                            tr("无法删除任务 '%1'。").arg(currentTask.name));
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
  QMessageBox::information(this, tr("提示"),
                           tr("请使用右键菜单更改任务状态。"));
}

// Remember to add calls to updateTaskActionButtons() in appropriate places,
// e.g., after loading tasks, or when subject selection changes to an empty
// state.
// ... (rest of MainWindow.cpp)

// Time TM-003

void MainWindow::setupTimerDockWidget() {
  m_timerDockWidget = new QDockWidget(tr("学习计时器"), this);
  m_timerDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea |
                                     Qt::RightDockWidgetArea);

  m_timerWidget = new TimerWidget(m_timerDockWidget);
  m_timerDockWidget->setWidget(m_timerWidget);
  addDockWidget(Qt::RightDockWidgetArea,
                m_timerDockWidget); // Add to right, for example

  // Connect signal from timer widget if needed
  connect(m_timerWidget, &TimerWidget::timerStoppedAndSaved, this,
          &MainWindow::onTimerLoggedNewEntry);

  // Add to View menu
  QMenu *viewMenu = menuBar()->findChild<QMenu *>(
      "viewMenu"); // Assuming you have a view menu
  if (!viewMenu) { // Or create if it doesn't exist
    viewMenu = menuBar()->addMenu(tr("视图(&V)"));
    viewMenu->setObjectName(
        "viewMenu"); // Give it an object name to find it later
  }
  if (viewMenu) { // Check again
    viewMenu->addSeparator();
    viewMenu->addAction(m_timerDockWidget->toggleViewAction());
  }
}

void MainWindow::onTimerLoggedNewEntry(const TimeLog &newLog) {
  qDebug() << "MainWindow received new TimeLog ID:" << newLog.id;
  // If the log was for a task, the task's actual time has been updated in DB.
  // We need to refresh the task view if the timed task is currently visible.
  Subject currentSubject = getCurrentSelectedSubject();
  if (currentSubject.isValid() && (newLog.subjectId == currentSubject.id ||
                                   newLog.subjectId == -1 /* general log */)) {
    // If the log is related to the currently selected subject (or is general
    // for the user) and the task is in the current subject view, refresh it.
    // Check if newLog.taskId is in the current view
    bool refreshNeeded = false;
    if (newLog.taskId != -1) {
      for (int i = 0; i < m_taskTableModel->rowCount(); ++i) {
        if (m_taskTableModel->item(i, 0)->data(Qt::UserRole).toInt() ==
            newLog.taskId) {
          refreshNeeded = true;
          break;
        }
      }
    }
    if (refreshNeeded ||
        newLog.taskId ==
            -1) { // Refresh if task was in view or it was a subject-only log
      loadTasksForSubject(currentSubject.id);
    }
  }
}

// Make sure to call m_timerWidget->refreshTargetSelection(); after any CRUD
// operation on Subjects or Tasks that might affect the comboboxes in
// TimerWidget. For example, in onAddSubject, onEditSubject, onDeleteSubject
// (after loadSubjectsForCurrentUser): if (m_timerWidget)
// m_timerWidget->refreshTargetSelection(); And in onAddTask, onEditTask,
// onDeleteTask (after loadTasksForSubject): if (m_timerWidget)
// m_timerWidget->refreshTargetSelection();

// Implementation of the missing slot for
// ActivityMonitorService::timedSegmentLogged
void MainWindow::onAutoTimeSegmentLogged(const TimeLog &log) {
  // You can refresh the UI or log the event as needed.
  qDebug() << "Auto time segment logged:" << log.id;
  // Optionally, refresh task/subject views if needed
  // Example: reload tasks if log.taskId is valid and matches current subject
  Subject currentSubject = getCurrentSelectedSubject();
  if (currentSubject.isValid() && log.subjectId == currentSubject.id) {
    loadTasksForSubject(currentSubject.id);
  }
}

void MainWindow::initializeReportComponents() {
  // Ensure DatabaseManager is accessible
  DatabaseManager &dbManager =
      DatabaseManager::instance(); // Or this->m_dbManager

  m_dataAggregator = new Core::Reports::DataAggregator();
  m_metricCalculator = new Core::Reports::MetricCalculator();

  // Configure and instantiate LLMCommunicator
  // For a real app, use a more secure way or a dedicated settings dialog for
  // API key
  QSettings *settings =
      new QSettings(QStringLiteral("data/set.ini"),
                    QSettings::IniFormat); // Uses organizationName and
                                           // applicationName set in main.cpp

  // 基于当前用户读取 LLM 配置
  QString userPrefix = QString("user_%1_").arg(m_currentUser.id);
  QString apiKey =
      settings->value(QString("%1llm_Qwen/apiKey").arg(userPrefix)).toString();
  QString apiUrl =
      settings->value(QString("%1llm_Qwen/apiUrl").arg(userPrefix)).toString();
  QString modelName =
      settings->value(QString("%1llm_Qwen/modelName").arg(userPrefix))
          .toString();

  // 如果用户专用配置不存在，使用默认配置
  if (apiUrl.isEmpty()) {
    apiUrl = settings->value("llm_Qwen/apiUrl").toString();
    modelName = settings->value("llm_Qwen/modelName").toString();
  }

  // 不在程序启动时强制要求输入 API Key
  // 用户会在实际选择和使用模型时输入相应的 API Key

  auto concreteLlmComm = new Core::Reports::LLMCommunicator();
  concreteLlmComm->setUserId(m_currentUser.id); // 设置当前用户ID
  concreteLlmComm->configure(apiKey, apiUrl, modelName);
  m_llmCommunicator = concreteLlmComm; // Store as interface type

  m_reportGenerator = new Core::Reports::ReportGenerator(
      dbManager, *m_dataAggregator, *m_metricCalculator, *m_llmCommunicator,
      this);

  connect(m_reportGenerator, &Core::Reports::ReportGenerator::reportReady, this,
          &MainWindow::handleReportDataReady);
  connect(m_reportGenerator, &Core::Reports::ReportGenerator::reportFailed,
          this, &MainWindow::handleReportDataError);

  delete settings; // Clean up settings if not needed anymore
}

void MainWindow::cleanupReportComponents() {
  // Disconnect signals first if m_reportGenerator is deleted before this
  // (though parent 'this' should handle it) if (m_reportGenerator) {
  //    disconnect(m_reportGenerator, nullptr, this, nullptr);
  // }
  delete m_reportGenerator; // QObject parent system will delete children if
                            // ReportGenerator is parented
  delete m_llmCommunicator; // Or if concreteLlmComm was parented to this, it's
                            // handled.
  delete m_metricCalculator;
  delete m_dataAggregator;
}

// === TM-006: Report Generation Slot Implementations ===

void MainWindow::onGenerateReportActionTriggered() {
  if (!m_currentUser.isValid()) {
    QMessageBox::warning(this, tr("未登录"), tr("请先登录以生成报告。"));
    return;
  }
  if (!m_reportGenerator) {
    QMessageBox::critical(this, tr("错误"), tr("报告生成组件未初始化。"));
    return;
  }

  ReportSettingsDialog settingsDialog(
      m_currentUser, this); // You need to implement this dialog
  // Pass current user's subjects to settingsDialog if it allows subject
  // filtering
  // settingsDialog.setSubjects(DatabaseManager::instance().getSubjectsByUser(m_currentUser.id));

  if (settingsDialog.exec() == QDialog::Accepted) {
    Core::Reports::ReportParameters params =
        settingsDialog.getReportParameters();
    params.userId = m_currentUser.id; // Ensure correct user ID

    if (params.selectedLLMModel == "Gemini") {
      QSettings *settings =
          new QSettings(QStringLiteral("data/set.ini"), QSettings::IniFormat);

      QString userPrefix = QString("user_%1_").arg(m_currentUser.id);
      QString apiKey =
          settings->value(QString("%1llm_Gemini/apiKey").arg(userPrefix))
              .toString();
      QString apiUrl =
          settings->value(QString("%1llm_Gemini/apiUrl").arg(userPrefix))
              .toString();
      QString modelName =
          settings->value(QString("%1llm_Gemini/modelName").arg(userPrefix))
              .toString();

      // 如果用户专用配置不存在，使用默认配置
      if (apiUrl.isEmpty()) {
        apiUrl = settings->value("llm_Gemini/apiUrl").toString();
        modelName = settings->value("llm_Gemini/modelName").toString();
      }

      m_llmCommunicator->configure(apiKey, apiUrl, modelName);
      delete settings;
    } else if (params.selectedLLMModel == "Qwen") {
      QSettings *settings =
          new QSettings(QStringLiteral("data/set.ini"), QSettings::IniFormat);

      QString userPrefix = QString("user_%1_").arg(m_currentUser.id);
      QString apiKey =
          settings->value(QString("%1llm_Qwen/apiKey").arg(userPrefix))
              .toString();
      QString apiUrl =
          settings->value(QString("%1llm_Qwen/apiUrl").arg(userPrefix))
              .toString();
      QString modelName =
          settings->value(QString("%1llm_Qwen/modelName").arg(userPrefix))
              .toString();

      // 如果用户专用配置不存在，使用默认配置
      if (apiUrl.isEmpty()) {
        apiUrl = settings->value("llm_Qwen/apiUrl").toString();
        modelName = settings->value("llm_Qwen/modelName").toString();
      }

      m_llmCommunicator->configure(apiKey, apiUrl, modelName);
      delete settings;
    }

    statusBar()->showMessage(tr("正在生成报告，请稍候..."));
    if (m_generateReportAction)
      m_generateReportAction->setEnabled(false); // Prevent multiple clicks

    m_reportGenerator->generateReportAsync(params);
  }
}

void MainWindow::handleReportDataReady(
    const Core::Reports::LearningReport &report) {
  statusBar()->clearMessage();
  if (m_generateReportAction)
    m_generateReportAction->setEnabled(true);

  if (report.hasError) {
    QMessageBox::warning(this, tr("报告通知"),
                         report.errorMessage.isEmpty()
                             ? tr("报告已生成，但包含一些警告或无数据显示。")
                             : report.errorMessage);
  } else {
    QMessageBox::information(this, tr("报告已生成"),
                             tr("学习报告已成功生成！"));
  }

  ReportDisplayDialog displayDialog(report, this);
  displayDialog.exec();
}

void MainWindow::handleReportDataError(const QString &errorMessage) {
  statusBar()->clearMessage();
  if (m_generateReportAction)
    m_generateReportAction->setEnabled(true);
  QMessageBox::critical(this, tr("报告生成失败"), errorMessage);
}
// === End TM-006 Slot Implementations ===

// ... (Rest of MainWindow.cpp: onAddSubject, onEditTask, etc.)
// 设置浮动工具栏
void MainWindow::setupFloatingToolbar() {
  // 创建独立的悬浮工具栏（不依赖主窗口）
  m_floatingToolbar = new QWidget(
      nullptr,
      Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |
          Qt::Tool |
          Qt::WindowDoesNotAcceptFocus); // 不接受焦点，避免影响其他窗口

  // 设置工具栏属性
  m_floatingToolbar->setAttribute(Qt::WA_ShowWithoutActivating, true);
  m_floatingToolbar->setAttribute(Qt::WA_TranslucentBackground, true);

  // 设置工具栏大小和位置
  QScreen *screen = QApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();

  int toolbarHeight = 80;
  m_floatingToolbar->setFixedSize(screenGeometry.width(), toolbarHeight);
  m_floatingToolbar->move(0, 0);

  // 创建工具栏内容
  QHBoxLayout *layout = new QHBoxLayout(m_floatingToolbar);
  layout->setContentsMargins(10, 5, 10, 5);

  // 添加快速操作按钮
  QPushButton *quickAddSubjectBtn =
      new QPushButton(tr("快速添加科目"), m_floatingToolbar);
  QPushButton *quickAddTaskBtn =
      new QPushButton(tr("快速添加任务"), m_floatingToolbar);
  QPushButton *timerBtn = new QPushButton(tr("开始计时"), m_floatingToolbar);
  QPushButton *reportBtn = new QPushButton(tr("生成报告"), m_floatingToolbar);

  // 连接按钮信号
  connect(quickAddSubjectBtn, &QPushButton::clicked, this,
          &MainWindow::onAddSubject);
  connect(quickAddTaskBtn, &QPushButton::clicked, this, &MainWindow::onAddTask);
  connect(timerBtn, &QPushButton::clicked, [this]() {
    if (m_isInTrayMode) {
      showMainWindow(); // 先显示主窗口
    }
    if (m_timerWidget) {
      m_timerDockWidget->raise();
      m_timerDockWidget->show();
    }
  });
  connect(reportBtn, &QPushButton::clicked, this,
          &MainWindow::onGenerateReportActionTriggered);

  // 添加用户信息标签
  QLabel *userLabel = new QLabel(m_floatingToolbar);
  userLabel->setObjectName("floatingUserLabel");

  // 布局
  layout->addWidget(quickAddSubjectBtn);
  layout->addWidget(quickAddTaskBtn);
  layout->addWidget(timerBtn);
  layout->addWidget(reportBtn);
  layout->addStretch();
  layout->addWidget(userLabel);

  // 设置样式（增强视觉效果）
  m_floatingToolbar->setStyleSheet(R"(
        QWidget {
            background-color: rgba(30, 30, 30, 220);
            border-bottom: 3px solid #4CAF50;
            border-radius: 0px 0px 8px 8px;
        }
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 8px 16px;
            margin: 2px;
            border-radius: 4px;
            font-weight: bold;
            min-width: 80px;
        }
        QPushButton:hover {
            background-color: #45a049;
            transform: translateY(-1px);
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        QLabel {
            color: white;
            font-weight: bold;
            margin: 0px 8px;
        }
    )");

  setupFloatingAnimation();

  // 连接鼠标事件
  m_floatingToolbar->installEventFilter(this);

  // 显示工具栏
  m_floatingToolbar->show();

  // 启动自动隐藏定时器
  m_floatingHideTimer->setSingleShot(true);
  m_floatingHideTimer->start(FLOATING_HIDE_DELAY);
}
void MainWindow::setupFloatingAnimation() {
  m_floatingAnimation->setTargetObject(m_floatingToolbar);
  m_floatingAnimation->setPropertyName("geometry");
  m_floatingAnimation->setDuration(300);
  m_floatingAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  if (obj == m_floatingToolbar) {
    if (event->type() == QEvent::Enter) {
      showFloatingToolbar();
      return true;
    } else if (event->type() == QEvent::Leave) {
      m_floatingHideTimer->start(FLOATING_HIDE_DELAY);
      return true;
    }
  }

  if (event->type() == QEvent::MouseMove && !m_floatingToolbarHidden) {
    m_floatingHideTimer->start(FLOATING_HIDE_DELAY);
  }

  return QMainWindow::eventFilter(obj, event);
}

void MainWindow::toggleFloatingToolbar() {
  if (m_floatingToolbar) {
    if (m_floatingToolbar->isVisible()) {
      m_floatingToolbar->hide();
      m_mouseCheckTimer->stop();
    } else {
      m_floatingToolbar->show();
      m_mouseCheckTimer->start(50);
      showFloatingToolbar();
    }
  }
}

void MainWindow::hideFloatingToolbar() {
  if (!m_floatingToolbar || m_floatingToolbarHidden)
    return;

  m_floatingToolbarHidden = true;

  QRect currentGeometry = m_floatingToolbar->geometry();
  QRect targetGeometry = currentGeometry;
  targetGeometry.moveTop(-currentGeometry.height() + FLOATING_TRIGGER_HEIGHT);

  m_floatingAnimation->setStartValue(currentGeometry);
  m_floatingAnimation->setEndValue(targetGeometry);
  m_floatingAnimation->start();
}

void MainWindow::showFloatingToolbar() {
  if (!m_floatingToolbar || !m_floatingToolbarHidden) {
    if (!m_floatingToolbarHidden) {
      m_floatingHideTimer->start(FLOATING_HIDE_DELAY);
    }
    return;
  }

  m_floatingToolbarHidden = false;

  QRect currentGeometry = m_floatingToolbar->geometry();
  QRect targetGeometry = currentGeometry;
  targetGeometry.moveTop(0);

  m_floatingAnimation->setStartValue(currentGeometry);
  m_floatingAnimation->setEndValue(targetGeometry);
  m_floatingAnimation->start();

  m_floatingHideTimer->start(FLOATING_HIDE_DELAY);
}

void MainWindow::checkMousePositionForFloating() {
  if (!m_floatingToolbar || !m_floatingToolbar->isVisible() ||
      !m_floatingToolbarHidden)
    return;

  QPoint globalMousePos = QCursor::pos();
  if (globalMousePos.y() <= FLOATING_TRIGGER_HEIGHT) {
    showFloatingToolbar();
  }
}
// end of floating window

int MainWindow::showDialogCentered(QDialog *dialog) {
  if (!dialog)
    return QDialog::Rejected;

  // 临时隐藏浮动工具栏
  bool wasFloatingVisible = false;
  if (m_floatingToolbar && m_floatingToolbar->isVisible() &&
      !m_floatingToolbarHidden) {
    wasFloatingVisible = true;
    hideFloatingToolbar();
  }

  // 设置对话框属性
  dialog->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);

  // 确保对话框大小合适
  dialog->adjustSize();

  // 居中显示
  QScreen *screen = QApplication::primaryScreen();
  if (screen) {
    QRect screenGeometry = screen->geometry();
    QSize dialogSize = dialog->size();
    int x = (screenGeometry.width() - dialogSize.width()) / 2;
    int y = (screenGeometry.height() - dialogSize.height()) / 2;
    dialog->move(x, y);
  }

  // 执行对话框并获取结果
  int result = dialog->exec();

  // 恢复浮动工具栏
  if (wasFloatingVisible && m_floatingToolbar) {
    showFloatingToolbar();
  }

  return result; // 返回执行结果
}
// begin of 托盘相关
void MainWindow::setupSystemTray() {
  if (!isSystemTrayAvailable()) {
    QMessageBox::critical(this, tr("系统托盘"),
                          tr("系统托盘不可用，应用将无法最小化到托盘。"));
    return;
  }

  // 创建托盘图标
  m_trayIcon = new QSystemTrayIcon(this);

  // 设置托盘图标
  QIcon trayIconImage = this->style()->standardIcon(QStyle::SP_ComputerIcon);
  m_trayIcon->setIcon(trayIconImage);

  // 设置工具提示
  m_trayIcon->setToolTip(tr("学习时间管理器"));

  // 创建托盘菜单
  createTrayMenu();

  // 连接托盘信号
  connect(m_trayIcon, &QSystemTrayIcon::activated, this,
          &MainWindow::onTrayIconActivated);

  // 显示托盘图标
  m_trayIcon->show();
}

// 创建托盘菜单
void MainWindow::createTrayMenu() {
  m_trayMenu = new QMenu(this);

  // 显示/隐藏主窗口
  m_showAction = new QAction(tr("显示主窗口"), this);
  m_showAction->setIcon(
      this->style()->standardIcon(QStyle::SP_DialogOpenButton));
  connect(m_showAction, &QAction::triggered, this, &MainWindow::showMainWindow);

  m_hideAction = new QAction(tr("隐藏到托盘"), this);
  m_hideAction->setIcon(
      this->style()->standardIcon(QStyle::SP_DialogCloseButton));
  connect(m_hideAction, &QAction::triggered, this, &MainWindow::hideToTray);

  m_trayMenu->addAction(m_showAction);
  m_trayMenu->addAction(m_hideAction);
  m_trayMenu->addSeparator();

  // 悬浮工具栏控制
  m_toggleFloatingTrayAction = new QAction(tr("显示/隐藏悬浮工具栏"), this);
  m_toggleFloatingTrayAction->setCheckable(true);
  m_toggleFloatingTrayAction->setChecked(m_floatingToolbarEnabled);
  connect(m_toggleFloatingTrayAction, &QAction::triggered, this,
          [this](bool checked) {
            m_floatingToolbarEnabled = checked;
            if (checked) {
              if (m_floatingToolbar) {
                m_floatingToolbar->show();
                if (!m_globalMouseTimer->isActive()) {
                  m_globalMouseTimer->start();
                }
              }
            } else {
              if (m_floatingToolbar) {
                m_floatingToolbar->hide();
                m_globalMouseTimer->stop();
              }
            }
          });

  m_trayMenu->addAction(m_toggleFloatingTrayAction);
  m_trayMenu->addSeparator();

  // 快速操作
  m_addSubjectAction = new QAction(tr("快速添加科目"), this);
  m_addSubjectAction->setIcon(
      this->style()->standardIcon(QStyle::SP_FileDialogNewFolder));
  connect(m_addSubjectAction, &QAction::triggered, this,
          &MainWindow::onAddSubject);

  m_addTaskAction = new QAction(tr("快速添加任务"), this);
  m_addTaskAction->setIcon(
      this->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  connect(m_addTaskAction, &QAction::triggered, this, &MainWindow::onAddTask);

  m_startTimerAction = new QAction(tr("开始计时"), this);
  m_startTimerAction->setIcon(
      this->style()->standardIcon(QStyle::SP_MediaPlay));
  connect(m_startTimerAction, &QAction::triggered, this, [this]() {
    showMainWindow();
    if (m_timerWidget) {
      m_timerDockWidget->raise();
      m_timerDockWidget->show();
    }
  });

  m_trayMenu->addAction(m_addSubjectAction);
  m_trayMenu->addAction(m_addTaskAction);
  m_trayMenu->addAction(m_startTimerAction);
  m_trayMenu->addSeparator();

  // 退出应用
  m_quitAction = new QAction(tr("完全退出"), this);
  m_quitAction->setIcon(
      this->style()->standardIcon(QStyle::SP_DialogCancelButton));
  connect(m_quitAction, &QAction::triggered, this,
          &MainWindow::quitApplication);

  m_trayMenu->addAction(m_quitAction);

  // 设置托盘菜单
  m_trayIcon->setContextMenu(m_trayMenu);
}

// 设置全局鼠标监控
void MainWindow::setupGlobalMouseMonitoring() {
  m_globalMouseTimer->setInterval(100); // 100ms检查间隔
  connect(m_globalMouseTimer, &QTimer::timeout, this,
          &MainWindow::onGlobalMouseCheck);

  // 启动全局监控
  m_globalMouseTimer->start();
}

// 检查系统托盘是否可用
bool MainWindow::isSystemTrayAvailable() {
  return QSystemTrayIcon::isSystemTrayAvailable();
}

// 全局鼠标检查
void MainWindow::onGlobalMouseCheck() {
  if (!m_floatingToolbar || !m_floatingToolbarEnabled) {
    return;
  }

  // 全局鼠标位置检测
  QPoint globalMousePos = QCursor::pos();

  // 检查是否在触发区域
  if (globalMousePos.y() <= FLOATING_TRIGGER_HEIGHT) {
    if (m_floatingToolbarHidden) {
      showFloatingToolbar();
    }
  }

  // 检查是否在工具栏区域内
  if (m_floatingToolbar->isVisible()) {
    QRect toolbarRect = m_floatingToolbar->geometry();
    bool mouseInToolbar = toolbarRect.contains(globalMousePos);

    if (mouseInToolbar) {
      // 鼠标在工具栏内，停止隐藏计时器
      m_floatingHideTimer->stop();
    } else if (!m_floatingToolbarHidden &&
               globalMousePos.y() > toolbarRect.bottom() + 50) {
      // 鼠标离开工具栏较远，启动隐藏计时器
      if (!m_floatingHideTimer->isActive()) {
        m_floatingHideTimer->start(FLOATING_HIDE_DELAY);
      }
    }
  }
}

// 重写关闭事件
void MainWindow::closeEvent(QCloseEvent *event) {
  // 如果系统托盘可用，询问用户意图
  if (m_trayIcon && m_trayIcon->isVisible()) {
    // 如果之前已经显示过托盘消息，直接隐藏到托盘
    if (m_trayMessageShown) {
      hideToTray();
      event->ignore();
      return;
    }

    // 第一次关闭时询问用户
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("关闭选项"));
    msgBox.setText(tr("您想要如何关闭应用程序？"));
    msgBox.setInformativeText(
        tr("您可以选择完全退出，或者最小化到系统托盘继续在后台运行。"));

    QPushButton *exitButton =
        msgBox.addButton(tr("完全退出"), QMessageBox::AcceptRole);
    QPushButton *trayButton =
        msgBox.addButton(tr("最小化到托盘"), QMessageBox::RejectRole);
    QPushButton *cancelButton =
        msgBox.addButton(tr("取消"), QMessageBox::DestructiveRole);

    msgBox.setDefaultButton(trayButton);
    msgBox.exec();

    if (msgBox.clickedButton() == exitButton) {
      // 用户选择完全退出
      m_isInTrayMode = false;
      m_floatingToolbarEnabled = false;

      // 停止所有计时器
      if (m_globalMouseTimer)
        m_globalMouseTimer->stop();
      if (m_mouseCheckTimer)
        m_mouseCheckTimer->stop();
      if (m_floatingHideTimer)
        m_floatingHideTimer->stop();

      // 隐藏所有窗口
      if (m_floatingToolbar) {
        m_floatingToolbar->hide();
      }
      if (m_trayIcon) {
        m_trayIcon->hide();
      }

      event->accept();
    } else if (msgBox.clickedButton() == trayButton) {
      // 用户选择最小化到托盘
      showTrayMessage(
          tr("最小化到托盘"),
          tr("应用已最小化到系统托盘。\n悬浮工具栏仍然可用，双击托盘图标可重新"
             "打开主窗口。\n\n下次点击关闭按钮将直接最小化到托盘。"));
      m_trayMessageShown = true;
      hideToTray();
      event->ignore();
    } else {
      // 用户取消
      event->ignore();
    }
  } else {
    // 如果没有托盘，直接询问是否退出
    int ret = QMessageBox::question(
        this, tr("退出确认"),
        tr("确定要退出应用程序吗？\n悬浮工具栏也将关闭。"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
      if (m_floatingToolbar) {
        m_floatingToolbar->hide();
      }
      event->accept();
    } else {
      event->ignore();
    }
  }
}

// 处理窗口状态变化
void MainWindow::changeEvent(QEvent *event) {
  QMainWindow::changeEvent(event);

  if (event->type() == QEvent::WindowStateChange) {
    if (isMinimized() && m_trayIcon && m_trayIcon->isVisible()) {
      // 最小化时隐藏到托盘
      QTimer::singleShot(0, this, &MainWindow::hideToTray);
    }
  }
}

// 托盘图标激活处理
void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
  case QSystemTrayIcon::Trigger:
  case QSystemTrayIcon::DoubleClick:
    toggleMainWindow();
    break;
  case QSystemTrayIcon::MiddleClick:
    showTrayMessage(tr("学习时间管理器"), tr("应用正在后台运行..."));
    break;
  default:
    break;
  }
}

// 显示主窗口
void MainWindow::showMainWindow() {
  show();
  raise();
  activateWindow();
  setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);

  m_isInTrayMode = false;

  // 更新悬浮工具栏状态
  updateFloatingToolbarForTrayMode();

  // 更新菜单状态
  if (m_showAction)
    m_showAction->setEnabled(false);
  if (m_hideAction)
    m_hideAction->setEnabled(true);

  // 如果浮动工具栏被隐藏，重新显示
  if (m_floatingToolbar && m_floatingToolbarHidden) {
    showFloatingToolbar();
  }
}

// 隐藏到托盘
void MainWindow::hideToTray() {
  if (m_trayIcon && m_trayIcon->isVisible()) {
    hide();
    m_isInTrayMode = true;

    // 更新悬浮工具栏状态
    updateFloatingToolbarForTrayMode();

    // 确保悬浮工具栏在托盘模式下仍然工作
    if (m_floatingToolbar && m_floatingToolbarEnabled) {
      m_floatingToolbar->show();
      if (!m_globalMouseTimer->isActive()) {
        m_globalMouseTimer->start();
      }
    }

    // 更新菜单状态
    if (m_showAction)
      m_showAction->setEnabled(true);
    if (m_hideAction)
      m_hideAction->setEnabled(false);
  }
}

// 切换主窗口显示状态
void MainWindow::toggleMainWindow() {
  if (isVisible() && !isMinimized()) {
    hideToTray();
  } else {
    showMainWindow();
  }
}

// 退出应用
void MainWindow::quitApplication() {
  int ret = QMessageBox::question(this, tr("退出确认"),
                                  tr("确定要完全退出应用程序吗？\n\n正在进行的"
                                     "计时将会停止，悬浮工具栏也将关闭。"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);

  if (ret == QMessageBox::Yes) {
    m_isInTrayMode = false;
    m_floatingToolbarEnabled = false;

    // 停止所有计时器
    if (m_globalMouseTimer)
      m_globalMouseTimer->stop();
    if (m_mouseCheckTimer)
      m_mouseCheckTimer->stop();
    if (m_floatingHideTimer)
      m_floatingHideTimer->stop();

    // 隐藏所有窗口
    if (m_floatingToolbar) {
      m_floatingToolbar->hide();
    }
    if (m_trayIcon) {
      m_trayIcon->hide();
    }

    qApp->quit();
  }
}

// 显示托盘消息
void MainWindow::showTrayMessage(const QString &title, const QString &message) {
  if (m_trayIcon && m_trayIcon->isVisible()) {
    m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
  }
}

// 更新托盘模式下的悬浮工具栏
void MainWindow::updateFloatingToolbarForTrayMode() {
  if (!m_floatingToolbar)
    return;

  // 添加或更新状态标签
  QLabel *statusLabel =
      m_floatingToolbar->findChild<QLabel *>("floatingStatusLabel");
  if (!statusLabel) {
    statusLabel = new QLabel(m_floatingToolbar);
    statusLabel->setObjectName("floatingStatusLabel");

    // 添加到布局
    QHBoxLayout *layout =
        qobject_cast<QHBoxLayout *>(m_floatingToolbar->layout());
    if (layout) {
      layout->addWidget(statusLabel);
    }
  }

  if (statusLabel) {
    if (m_isInTrayMode) {
      statusLabel->setText(tr("后台运行"));
      statusLabel->setStyleSheet(
          "color: #FFA500; font-weight: bold;"); // 橙色表示后台
    } else {
      statusLabel->setText(tr("运行中"));
      statusLabel->setStyleSheet("color: white; font-weight: bold;");
    }
  }

  // 在托盘模式下，添加"显示主窗口"按钮或更新现有按钮
  QPushButton *showMainBtn =
      m_floatingToolbar->findChild<QPushButton *>("showMainButton");
  if (!showMainBtn && m_isInTrayMode) {
    showMainBtn = new QPushButton(tr("显示主窗口"), m_floatingToolbar);
    showMainBtn->setObjectName("showMainButton");
    connect(showMainBtn, &QPushButton::clicked, this,
            &MainWindow::showMainWindow);

    // 添加到布局的开始位置
    QHBoxLayout *layout =
        qobject_cast<QHBoxLayout *>(m_floatingToolbar->layout());
    if (layout) {
      layout->insertWidget(0, showMainBtn);
    }
  }

  if (showMainBtn) {
    if (m_isInTrayMode) {
      showMainBtn->show();
      showMainBtn->setStyleSheet(R"(
                background-color: #FF9800;
                color: white;
                border: 2px solid #FFC107;
                padding: 8px 16px;
                margin: 2px;
                border-radius: 4px;
                font-weight: bold;
                min-width: 100px;
            )");
    } else {
      showMainBtn->hide();
    }
  }
} // end of 托盘相关
