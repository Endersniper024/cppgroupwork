#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H

#include "core/ProcessLink.h" // For program monitoring
#include "core/Subject.h"
#include "core/Task.h"
#include "core/TimeLog.h" // For PomodoroCycleType
#include "core/User.h"
#include <QDateTime>
#include <QTimer> // For the ticker
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class TimerWidget;
}
QT_END_NAMESPACE

class QSystemTrayIcon;       // Forward declaration
class ProcessMonitorService; // Forward declaration

class TimerWidget : public QWidget {
  Q_OBJECT

public:
  explicit TimerWidget(QWidget *parent = nullptr);
  ~TimerWidget();

  void setCurrentUser(const User &user); // To load subjects/tasks for this user
  void refreshTargetSelection(); // Call when subjects/tasks might have changed
  void setProcessMonitorService(
      ProcessMonitorService *service); // Set program monitoring service

public slots:
  void onProgramStarted(const QString &processName, const QString &taskName);
  void onProgramStopped(const QString &processName, const QString &taskName,
                        qint64 durationSeconds);
  void onProgramTimeRecorded(int taskId, qint64 durationSeconds,
                             bool isAutoRecorded);

signals:
  void timerStoppedAndSaved(
      const TimeLog &newLog); // Optional: if MainWindow needs to know

private slots:
  void on_startButton_clicked();
  void on_pauseButton_clicked();
  void on_stopButton_clicked();
  void on_resetButton_clicked();
  void on_pomodoroGroupBox_toggled(bool checked);
  void on_subjectComboBox_currentIndexChanged(int index);
  void on_taskComboBox_currentIndexChanged(int index);

  void updateTimerDisplay();                   // Slot for QTimer's timeout
  void updateProgramTimeDisplayPeriodically(); // 定时更新程序时间显示

private:
  Ui::TimerWidget *ui;
  User m_currentUser;
  QTimer *m_ticker; // Main timer for counting seconds
  QDateTime m_sessionStartTime;
  qint64 m_elapsedSecondsTotal;   // Total seconds elapsed in current session
                                  // (handles pauses)
  qint64 m_currentSegmentSeconds; // Seconds for the current segment (work/break
                                  // in Pomodoro)
  bool m_isPaused;

  // Pomodoro state
  bool m_isPomodoroActive;
  PomodoroCycleType m_currentPomodoroCycleType;
  int m_pomodoroWorkDurationMinutes;
  int m_pomodoroShortBreakMinutes;
  int m_pomodoroLongBreakMinutes;
  int m_pomodoroCyclesBeforeLongBreak;
  int m_pomodoroCycleCount; // Current number of work cycles completed

  QSystemTrayIcon *m_trayIcon; // Optional: for notifications

  // Program monitoring
  ProcessMonitorService *m_processMonitorService;
  QMap<QString, qint64> m_programTimes; // processName -> total seconds
  QString m_currentMonitoredProcess;    // Currently running monitored process
  qint64 m_currentTaskProgramTime;      // Program time for current task
  QTimer *m_programTimeUpdateTimer;     // 定时刷新程序时间显示

  void loadSubjects();
  void loadTasksForSubject(int subjectId);

  void resetTimerState(bool fullResetLogic = true);
  void updateButtonStates();
  void saveTimeLog(bool sessionCompletedNormally = true);

  // Pomodoro logic
  void startNextPomodoroSegment();
  void pomodoroSegmentFinished();
  void playSoundNotification(); // Placeholder for sound
  void showTrayNotification(const QString &title, const QString &message);

  // Program monitoring UI updates
  void updateProgramMonitorDisplay();
  void updateProgramTimeDisplay();
};

#endif // TIMERWIDGET_H