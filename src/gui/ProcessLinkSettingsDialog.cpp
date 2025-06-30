#include "ProcessLinkSettingsDialog.h"
#include <QDebug>

#ifdef Q_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tlhelp32.h>
#endif

// 移除了假的 COMMON_PROCESSES 列表，改为显示真实的用户进程

ProcessLinkSettingsDialog::ProcessLinkSettingsDialog(QWidget *parent)
    : QDialog(parent), m_processMonitor(new ProcessMonitor(this)),
      m_refreshTimer(new QTimer(this)) {
  setWindowTitle(tr("程序关联设置"));
  setMinimumSize(800, 600);

  setupUI();
  setupConnections();

  // 定时刷新运行中的程序列表
  m_refreshTimer->setInterval(2000); // 每2秒刷新一次
  connect(m_refreshTimer, &QTimer::timeout, this,
          &ProcessLinkSettingsDialog::updateRunningProcesses);
  m_refreshTimer->start();

  // 初始化数据
  updateProcessList();
  updateRunningProcesses();
}

void ProcessLinkSettingsDialog::setCurrentTask(const Task &task) {
  m_currentTask = task;
  m_taskNameLabel->setText(QString("<b>%1</b>").arg(task.name));
  m_taskDescLabel->setText(task.description.isEmpty() ? tr("无描述")
                                                      : task.description);
}

QString ProcessLinkSettingsDialog::getSelectedProcess() const {
  return m_processCombo->currentText();
}

ProcessLinkSettingsDialog::ProcessLinkSettings
ProcessLinkSettingsDialog::getLinkSettings() const {
  ProcessLinkSettings settings;
  settings.taskId = m_currentTask.id;
  settings.processName = m_processCombo->currentText();
  settings.displayName = m_displayNameEdit->text().isEmpty()
                             ? m_processCombo->currentText()
                             : m_displayNameEdit->text();
  settings.autoStart = m_autoStartCombo->currentIndex() == 0; // 是/否
  settings.autoStop = m_autoStopCombo->currentIndex() == 0;   // 是/否

  return settings;
}

void ProcessLinkSettingsDialog::setupUI() {
  m_mainLayout = new QVBoxLayout(this);

  // 任务信息区域
  m_taskInfoGroup = new QGroupBox(tr("当前任务"), this);
  QVBoxLayout *taskLayout = new QVBoxLayout(m_taskInfoGroup);

  m_taskNameLabel = new QLabel(this);
  m_taskDescLabel = new QLabel(this);
  m_taskDescLabel->setWordWrap(true);
  m_taskDescLabel->setStyleSheet("color: gray;");

  taskLayout->addWidget(m_taskNameLabel);
  taskLayout->addWidget(m_taskDescLabel);

  // 程序选择区域
  m_processSelectGroup = new QGroupBox(tr("选择要关联的程序"), this);
  QVBoxLayout *selectLayout = new QVBoxLayout(m_processSelectGroup);

  // 程序下拉框和刷新按钮
  QHBoxLayout *comboLayout = new QHBoxLayout();
  m_processCombo = new QComboBox(this);
  m_processCombo->setEditable(true);
  m_refreshBtn = new QPushButton(tr("刷新列表"), this);
  m_browseBtn = new QPushButton(tr("浏览文件..."), this);

  comboLayout->addWidget(new QLabel(tr("程序名称:")));
  comboLayout->addWidget(m_processCombo);
  comboLayout->addWidget(m_refreshBtn);
  comboLayout->addWidget(m_browseBtn);

  // 自定义程序输入
  QHBoxLayout *customLayout = new QHBoxLayout();
  m_customProcessEdit = new QLineEdit(this);
  m_customProcessEdit->setPlaceholderText(tr("输入自定义程序名称..."));
  m_addCustomBtn = new QPushButton(tr("添加"), this);

  customLayout->addWidget(new QLabel(tr("自定义程序:")));
  customLayout->addWidget(m_customProcessEdit);
  customLayout->addWidget(m_addCustomBtn);

  selectLayout->addLayout(comboLayout);
  selectLayout->addLayout(customLayout);

  // 当前运行的程序列表
  m_runningProcessGroup = new QGroupBox(tr("当前运行的程序"), this);
  QVBoxLayout *runningLayout = new QVBoxLayout(m_runningProcessGroup);

  m_runningProcessList = new QListWidget(this);
  m_runningProcessList->setMaximumHeight(120);
  m_testBtn = new QPushButton(tr("测试选中程序"), this);

  QHBoxLayout *testLayout = new QHBoxLayout();
  testLayout->addStretch();
  testLayout->addWidget(m_testBtn);

  runningLayout->addWidget(m_runningProcessList);
  runningLayout->addLayout(testLayout);

  // 关联设置
  m_settingsGroup = new QGroupBox(tr("关联设置"), this);
  QFormLayout *settingsLayout = new QFormLayout(m_settingsGroup);

  m_displayNameEdit = new QLineEdit(this);
  m_displayNameEdit->setPlaceholderText(tr("显示名称（可选）"));

  m_autoStartCombo = new QComboBox(this);
  m_autoStartCombo->addItems({tr("是"), tr("否")});

  m_autoStopCombo = new QComboBox(this);
  m_autoStopCombo->addItems({tr("是"), tr("否")});

  settingsLayout->addRow(tr("显示名称:"), m_displayNameEdit);
  settingsLayout->addRow(tr("程序启动时自动开始计时:"), m_autoStartCombo);
  settingsLayout->addRow(tr("程序关闭时自动停止计时:"), m_autoStopCombo);

  // 已关联程序管理
  m_linkedProcessGroup = new QGroupBox(tr("已关联的程序"), this);
  QVBoxLayout *linkedLayout = new QVBoxLayout(m_linkedProcessGroup);

  m_linkedProcessTable = new QTableWidget(this);
  m_linkedProcessTable->setColumnCount(4);
  m_linkedProcessTable->setHorizontalHeaderLabels(
      {tr("程序名称"), tr("显示名称"), tr("自动开始"), tr("自动停止")});
  m_linkedProcessTable->horizontalHeader()->setStretchLastSection(true);
  m_linkedProcessTable->setMaximumHeight(150);

  m_removeBtn = new QPushButton(tr("移除选中"), this);

  QHBoxLayout *removeLayout = new QHBoxLayout();
  removeLayout->addStretch();
  removeLayout->addWidget(m_removeBtn);

  linkedLayout->addWidget(m_linkedProcessTable);
  linkedLayout->addLayout(removeLayout);

  // 按钮区域
  m_buttonLayout = new QHBoxLayout();
  m_saveBtn = new QPushButton(tr("保存设置"), this);
  m_cancelBtn = new QPushButton(tr("取消"), this);

  m_saveBtn->setDefault(true);

  m_buttonLayout->addStretch();
  m_buttonLayout->addWidget(m_saveBtn);
  m_buttonLayout->addWidget(m_cancelBtn);

  // 主布局
  m_mainLayout->addWidget(m_taskInfoGroup);
  m_mainLayout->addWidget(m_processSelectGroup);
  m_mainLayout->addWidget(m_runningProcessGroup);
  m_mainLayout->addWidget(m_settingsGroup);
  m_mainLayout->addWidget(m_linkedProcessGroup);
  m_mainLayout->addLayout(m_buttonLayout);
}

void ProcessLinkSettingsDialog::setupConnections() {
  connect(m_refreshBtn, &QPushButton::clicked, this,
          &ProcessLinkSettingsDialog::onRefreshProcesses);
  connect(m_browseBtn, &QPushButton::clicked, this,
          &ProcessLinkSettingsDialog::onBrowseExecutable);
  connect(m_addCustomBtn, &QPushButton::clicked, this,
          &ProcessLinkSettingsDialog::onAddCustomProcess);
  connect(m_removeBtn, &QPushButton::clicked, this,
          &ProcessLinkSettingsDialog::onRemoveProcess);
  connect(m_testBtn, &QPushButton::clicked, this,
          &ProcessLinkSettingsDialog::onTestProcess);
  connect(m_saveBtn, &QPushButton::clicked, this,
          &ProcessLinkSettingsDialog::onSaveLinkSettings);
  connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

  connect(m_runningProcessList, &QListWidget::itemDoubleClicked,
          [this](QListWidgetItem *item) {
            if (item) {
              m_processCombo->setCurrentText(item->text());
            }
          });

  connect(m_processCombo,
          QOverload<const QString &>::of(&QComboBox::currentTextChanged), this,
          &ProcessLinkSettingsDialog::onProcessSelectionChanged);
}

void ProcessLinkSettingsDialog::updateProcessList() {
  m_processCombo->clear();

  // 获取当前运行的用户进程并填充到下拉框
  QStringList allProcesses = m_processMonitor->getCurrentProcesses();

  // 简单过滤，只保留常见的用户程序
  QStringList userProcesses;
  QStringList commonUserApps = {
      "notepad", "code",       "chrome",    "firefox", "edge",   "word",
      "excel",   "powerpoint", "photoshop", "matlab",  "python", "java",
      "idea",    "studio",     "qq",        "wechat",  "tim",    "music",
      "player",  "steam",      "discord",   "slack",   "zoom",   "teams",
      "obs",     "vlc",        "winrar"};

  for (const QString &process : allProcesses) {
    QString lowerProcess = process.toLower();

    // 检查是否包含常见用户程序关键词
    bool isUserApp = false;
    for (const QString &keyword : commonUserApps) {
      if (lowerProcess.contains(keyword)) {
        isUserApp = true;
        break;
      }
    }

    // 或者是 .exe 结尾且不是明显的系统进程
    if (!isUserApp && process.endsWith(".exe", Qt::CaseInsensitive)) {
      QString baseName = process.left(process.length() - 4).toLower();
      if (!baseName.startsWith("system") && !baseName.contains("svchost") &&
          !baseName.contains("runtime") && baseName.length() > 3) {
        isUserApp = true;
      }
    }

    if (isUserApp && !userProcesses.contains(process, Qt::CaseInsensitive)) {
      userProcesses.append(process);
    }
  }

  // 排序并添加到下拉框
  userProcesses.sort(Qt::CaseInsensitive);
  m_processCombo->addItems(userProcesses);

  // 如果列表为空，添加一个提示
  if (userProcesses.isEmpty()) {
    m_processCombo->addItem(tr("请手动输入程序名称"));
  }
}

void ProcessLinkSettingsDialog::updateRunningProcesses() {
  m_runningProcessList->clear();

  // 获取真实的运行进程列表
  QStringList allProcesses = m_processMonitor->getCurrentProcesses();

  // 定义系统进程列表（需要过滤掉的）
  QStringList systemProcesses = {"System",
                                 "Registry",
                                 "smss.exe",
                                 "csrss.exe",
                                 "wininit.exe",
                                 "winlogon.exe",
                                 "services.exe",
                                 "lsass.exe",
                                 "lsaiso.exe",
                                 "fontdrvhost.exe",
                                 "WUDFHost.exe",
                                 "dwm.exe",
                                 "audiodg.exe",
                                 "WmiPrvSE.exe",
                                 "spoolsv.exe",
                                 "svchost.exe",
                                 "taskhost.exe",
                                 "taskhostw.exe",
                                 "RuntimeBroker.exe",
                                 "dllhost.exe",
                                 "conhost.exe",
                                 "winlogon.exe",
                                 "LogonUI.exe",
                                 "userinit.exe",
                                 "ctfmon.exe",
                                 "SearchHost.exe",
                                 "StartMenuExperienceHost.exe",
                                 "ShellExperienceHost.exe",
                                 "ApplicationFrameHost.exe",
                                 "SystemSettings.exe",
                                 "PickerHost.exe",
                                 "LockApp.exe",
                                 "SecurityHealthSystray.exe",
                                 "SecurityHealthService.exe",
                                 "MsMpEng.exe",
                                 "NisSrv.exe",
                                 "smartscreen.exe"};

  // 过滤用户进程
  QStringList userProcesses;
  for (const QString &process : allProcesses) {
    QString lowerProcess = process.toLower();
    bool isSystemProcess = false;

    // 检查是否为系统进程
    for (const QString &sysProc : systemProcesses) {
      if (lowerProcess == sysProc.toLower()) {
        isSystemProcess = true;
        break;
      }
    }

    // 过滤掉明显的系统进程模式
    if (!isSystemProcess) {
      // 排除某些系统进程模式
      if (lowerProcess.contains("svchost") ||
          lowerProcess.contains("wudfhost") ||
          lowerProcess.contains("runtimebroker") ||
          lowerProcess.startsWith("system") || process.length() < 3) {
        continue;
      }

      userProcesses.append(process);
    }
  }

  // 去重并排序
  userProcesses.removeDuplicates();
  userProcesses.sort(Qt::CaseInsensitive);

  // 显示所有用户进程
  qDebug() << "Found" << allProcesses.size() << "total processes,"
           << userProcesses.size() << "user processes";

  for (const QString &process : userProcesses) {
    QListWidgetItem *item = new QListWidgetItem(process);

    // 为常见程序添加不同图标
    if (process.toLower().contains("code") ||
        process.toLower().contains("notepad") ||
        process.toLower().contains("devenv")) {
      item->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    } else if (process.toLower().contains("chrome") ||
               process.toLower().contains("firefox") ||
               process.toLower().contains("edge")) {
      item->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    } else {
      item->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    }

    m_runningProcessList->addItem(item);
  }

  // 更新状态信息
  if (userProcesses.isEmpty()) {
    QListWidgetItem *item = new QListWidgetItem(tr("未找到用户进程"));
    item->setFlags(Qt::NoItemFlags); // 不可选择
    item->setForeground(QBrush(Qt::gray));
    m_runningProcessList->addItem(item);
  }
}

void ProcessLinkSettingsDialog::onRefreshProcesses() {
  updateRunningProcesses();
  QMessageBox::information(this, tr("刷新完成"),
                           tr("已更新当前运行的程序列表。"));
}

void ProcessLinkSettingsDialog::onBrowseExecutable() {
  QString filePath =
      QFileDialog::getOpenFileName(this, tr("选择程序文件"), QString(),
                                   tr("可执行文件 (*.exe);;所有文件 (*.*)"));

  if (!filePath.isEmpty()) {
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();
    m_processCombo->setCurrentText(fileName);
    m_displayNameEdit->setText(fileInfo.baseName());
  }
}

void ProcessLinkSettingsDialog::onAddCustomProcess() {
  QString customProcess = m_customProcessEdit->text().trimmed();
  if (customProcess.isEmpty()) {
    QMessageBox::warning(this, tr("输入错误"), tr("请输入有效的程序名称。"));
    return;
  }

  // 添加到下拉框
  if (m_processCombo->findText(customProcess) == -1) {
    m_processCombo->addItem(customProcess);
  }
  m_processCombo->setCurrentText(customProcess);
  m_customProcessEdit->clear();
}

void ProcessLinkSettingsDialog::onRemoveProcess() {
  int currentRow = m_linkedProcessTable->currentRow();
  if (currentRow >= 0) {
    m_linkedProcessTable->removeRow(currentRow);
  }
}

void ProcessLinkSettingsDialog::onProcessSelectionChanged() {
  QString processName = m_processCombo->currentText();
  if (m_displayNameEdit->text().isEmpty()) {
    // 自动设置显示名称
    if (processName.endsWith(".exe", Qt::CaseInsensitive)) {
      m_displayNameEdit->setText(processName.left(processName.length() - 4));
    } else {
      m_displayNameEdit->setText(processName);
    }
  }
}

void ProcessLinkSettingsDialog::onTestProcess() {
  QListWidgetItem *currentItem = m_runningProcessList->currentItem();
  if (currentItem) {
    QString processName = currentItem->text();
    QMessageBox::information(this, tr("程序测试"),
                             tr("检测到程序: %1\n"
                                "该程序目前正在运行。")
                                 .arg(processName));
  } else {
    QMessageBox::warning(this, tr("未选中程序"),
                         tr("请先选择一个运行中的程序进行测试。"));
  }
}

void ProcessLinkSettingsDialog::onSaveLinkSettings() {
  QString processName = m_processCombo->currentText().trimmed();
  if (processName.isEmpty()) {
    QMessageBox::warning(this, tr("设置错误"),
                         tr("请选择或输入一个程序名称。"));
    return;
  }

  // 添加到已关联程序表格
  int row = m_linkedProcessTable->rowCount();
  m_linkedProcessTable->insertRow(row);

  ProcessLinkSettings settings = getLinkSettings();

  m_linkedProcessTable->setItem(row, 0,
                                new QTableWidgetItem(settings.processName));
  m_linkedProcessTable->setItem(row, 1,
                                new QTableWidgetItem(settings.displayName));
  m_linkedProcessTable->setItem(
      row, 2, new QTableWidgetItem(settings.autoStart ? tr("是") : tr("否")));
  m_linkedProcessTable->setItem(
      row, 3, new QTableWidgetItem(settings.autoStop ? tr("是") : tr("否")));

  QMessageBox::information(this, tr("设置已保存"),
                           tr("已成功关联程序 %1 到任务 %2。")
                               .arg(settings.displayName)
                               .arg(m_currentTask.name));

  accept(); // 关闭对话框
}

void ProcessLinkSettingsDialog::onProcessDetected(const QString &processName) {
  // 当检测到程序运行时的处理
  qDebug() << "Process detected:" << processName;
}
