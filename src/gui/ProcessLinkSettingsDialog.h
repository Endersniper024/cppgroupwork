#ifndef PROCESSLINKSETTINGSDIALOG_H
#define PROCESSLINKSETTINGSDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>


#include "core/ProcessMonitor.h"
#include "core/Task.h"


/**
 * @brief 程序关联设置对话框
 *
 * 004-关联程序功能的用户界面
 * 允许用户设置任务与特定程序的关联，实现自动监控程序运行时间
 */
class ProcessLinkSettingsDialog : public QDialog {
  Q_OBJECT

public:
  explicit ProcessLinkSettingsDialog(QWidget *parent = nullptr);

  // 设置当前任务（用于关联）
  void setCurrentTask(const Task &task);

  // 获取选中的程序名称
  QString getSelectedProcess() const;

  // 获取关联设置
  struct ProcessLinkSettings {
    int taskId;
    QString processName;
    QString displayName;
    bool autoStart; // 是否自动开始计时
    bool autoStop;  // 是否自动停止计时
  };

  ProcessLinkSettings getLinkSettings() const;

private slots:
  void onRefreshProcesses();
  void onBrowseExecutable();
  void onAddCustomProcess();
  void onRemoveProcess();
  void onProcessSelectionChanged();
  void onTestProcess();
  void onSaveLinkSettings();
  void onProcessDetected(const QString &processName);

private:
  void setupUI();
  void setupConnections();
  void updateProcessList();
  void updateRunningProcesses();

  // UI 组件
  QVBoxLayout *m_mainLayout;

  // 任务信息区域
  QGroupBox *m_taskInfoGroup;
  QLabel *m_taskNameLabel;
  QLabel *m_taskDescLabel;

  // 程序选择区域
  QGroupBox *m_processSelectGroup;
  QComboBox *m_processCombo;
  QPushButton *m_refreshBtn;
  QPushButton *m_browseBtn;
  QLineEdit *m_customProcessEdit;
  QPushButton *m_addCustomBtn;

  // 当前运行的程序列表
  QGroupBox *m_runningProcessGroup;
  QListWidget *m_runningProcessList;
  QPushButton *m_testBtn;

  // 关联设置
  QGroupBox *m_settingsGroup;
  QLineEdit *m_displayNameEdit;
  QComboBox *m_autoStartCombo;
  QComboBox *m_autoStopCombo;

  // 已关联程序管理
  QGroupBox *m_linkedProcessGroup;
  QTableWidget *m_linkedProcessTable;
  QPushButton *m_removeBtn;

  // 按钮区域
  QHBoxLayout *m_buttonLayout;
  QPushButton *m_saveBtn;
  QPushButton *m_cancelBtn;

  // 数据
  Task m_currentTask;
  ProcessMonitor *m_processMonitor;
  QTimer *m_refreshTimer;
};

#endif // PROCESSLINKSETTINGSDIALOG_H
