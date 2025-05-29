#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem> // For QListWidget if used
#include <QMainWindow>
#include <QPropertyAnimation>
#include <QTimer>
// 系统托盘相关头文件
#include <QAction>
#include <QCloseEvent>
#include <QEvent>
#include <QMenu>
#include <QSystemTrayIcon>

#include "core/ActivityMonitorService.h" // For activity monitoring service
#include "core/Subject.h"                // Add this
#include "core/Task.h"                   // Task
#include "core/TimeLog.h"
#include "core/User.h" // 引入 User 结构

#include "core/reports/Core.h" // For Core::Reports namespace
#include "core/reports/DataAggregator.h"
#include "core/reports/IDataAggregator.h"
#include "core/reports/ILLMCommunicator.h" // For ILLMCommunicator interface
#include "core/reports/LLMCommunicator.h"  // For concrete implementation
#include "core/reports/MetricCalculator.h"
#include "core/reports/ReportDataStructures.h" // For LearningReport
#include "core/reports/ReportEnums.h"          // For ReportPeriod enum
#include "core/reports/ReportGenerator.h"
#include "core/reports/ReportParameters.h"

class QDockWidget;
class QListWidget;
class QPushButton; // For Add/Edit/Delete buttons
class QTableView;
class QStandardItemModel; // For populating the table view simply

class TimerWidget; // Forward declaration

// class DataAggregator;

// Forward declarations for TM-006 components and new dialogs
// namespace Core::Reports { // Assuming your report classes are in this
// namespace
//     class ReportGenerator;
//     class DataAggregator;
//     class MetricCalculator;
//     class ILLMCommunicator; // Interface
//     class LLMCommunicator;  // Concrete implementation (if MainWindow
//     instantiates it directly) struct ReportParameters; struct LearningReport;
// } // namespace Core::Reports

class ReportSettingsDialog; // Forward declare
class ReportDisplayDialog;  // Forward declare

class QAction; // For menu actions

// class QPushButton;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
} // namespace Ui
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void setCurrentUser(const User &user);
  // 重写保护方法
protected:
  void closeEvent(QCloseEvent *event) override;
  void changeEvent(QEvent *event) override;
private slots:
  void handleLogout();
  void openProfileDialog(); // UM-004 个人信息管理入口
  // Subjects
  void onAddSubject();
  void onEditSubject();
  void onDeleteSubject();
  void
  onSubjectListSelectionChanged(); // When a subject is selected in the list

  // Tasks TM-002
  void onAddTask();
  void onEditTask();
  void onDeleteTask();
  void onTaskTableDoubleClicked(const QModelIndex &index);
  void onChangeTaskStatus(); // Slot for context menu or button
  // void onTimerLoggedNewEntry(const Core::TimeLog& newLog); // From manual
  // timer

  // === TM-006: Report Generation Slots ===
  void onGenerateReportActionTriggered();
  void handleReportDataReady(const Core::Reports::LearningReport &report);
  void handleReportDataError(const QString &errorMessage);
  // === End TM-006 Slots ===

  // ... 其他槽函数 ...
  // 悬浮窗口
  void toggleFloatingToolbar();
  void hideFloatingToolbar();
  void showFloatingToolbar();
  void checkMousePositionForFloating();

  // 系统托盘相关槽函数
  void showMainWindow();
  void hideToTray();
  void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
  void quitApplication();
  void toggleMainWindow();
  void onGlobalMouseCheck();

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
  void
  updateSubjectActionButtons(); // Enable/disable edit/delete based on selection

  // Tasks TM-002

  void setupTaskView(); // To set up the central task display area
  void loadTasksForSubject(int subjectId);
  void
  updateTaskActionButtons(); // Enable/disable task buttons based on selection
  Subject getCurrentSelectedSubject(); // Helper
  Task getCurrentSelectedTask();       // Helper

  // Time TM-003

  TimerWidget *m_timerWidget;
  QDockWidget *m_timerDockWidget;

  void setupTimerDockWidget();
  void onTimerLoggedNewEntry(
      const TimeLog &newLog); // Slot to handle new log entries

  // WinAPI TM-005

  ActivityMonitorService *m_activityMonitor;

  void onAutoTimeSegmentLogged(
      const TimeLog &newLog); // Slot to handle auto time logging

  // === TM-006: Report Generation Members ===
  Core::Reports::DataAggregator *m_dataAggregator;
  Core::Reports::MetricCalculator *m_metricCalculator;
  Core::Reports::ILLMCommunicator *m_llmCommunicator; // Use interface
  Core::Reports::ReportGenerator *m_reportGenerator;

  QAction *m_generateReportAction; // To enable/disable during generation
  // === End TM-006 Members ===

  // === TM-006: Helper ===
  void initializeReportComponents();
  void cleanupReportComponents();

  // begin of floating toolbar
  QWidget *m_floatingToolbar;
  QTimer *m_floatingHideTimer;
  QTimer *m_mouseCheckTimer;
  QPropertyAnimation *m_floatingAnimation;
  bool m_floatingToolbarHidden;
  QAction *m_toggleFloatingAction;

  void setupFloatingToolbar();
  void setupFloatingAnimation();
  bool eventFilter(QObject *obj, QEvent *event) override;

  static const int FLOATING_TRIGGER_HEIGHT = 5;
  static const int FLOATING_HIDE_DELAY = 3000;
  int showDialogCentered(QDialog *dialog);
  // end of floating toolbar
  // 系统托盘相关成员变量
  QSystemTrayIcon *m_trayIcon;
  QMenu *m_trayMenu;
  QAction *m_showAction;
  QAction *m_hideAction;
  QAction *m_quitAction;
  QAction *m_addSubjectAction;
  QAction *m_addTaskAction;
  QAction *m_startTimerAction;
  QAction *m_toggleFloatingTrayAction;

  // 状态管理变量
  bool m_isInTrayMode;
  bool m_floatingToolbarEnabled;
  bool m_trayMessageShown;
  QTimer *m_globalMouseTimer;
  // 系统托盘相关方法声明
  void setupSystemTray();
  void createTrayMenu();
  void setupGlobalMouseMonitoring();
  void updateFloatingToolbarForTrayMode();
  bool isSystemTrayAvailable();
  void showTrayMessage(const QString &title, const QString &message);
};

#endif // MAINWINDOW_H