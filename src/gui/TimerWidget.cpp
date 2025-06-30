#include "TimerWidget.h"
#include "core/DatabaseManager.h"
#include "core/ProcessMonitorService.h"
#include "ui_TimerWidget.h"
#include <QApplication> // For qApp->style()
#include <QDebug>
#include <QMessageBox>
#include <QSystemTrayIcon> // For desktop notifications


TimerWidget::TimerWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::TimerWidget), m_ticker(new QTimer(this)),
      m_elapsedSecondsTotal(0), m_currentSegmentSeconds(0), m_isPaused(false),
      m_isPomodoroActive(false),
      m_currentPomodoroCycleType(PomodoroCycleType::None),
      m_pomodoroCycleCount(0), m_processMonitorService(nullptr),
      m_currentTaskProgramTime(0), m_programTimeUpdateTimer(new QTimer(this)) {
  ui->setupUi(this);

  connect(m_ticker, &QTimer::timeout, this, &TimerWidget::updateTimerDisplay);

  // 设置程序时间定时刷新
  connect(m_programTimeUpdateTimer, &QTimer::timeout, this,
          &TimerWidget::updateProgramTimeDisplayPeriodically);
  m_programTimeUpdateTimer->setInterval(2000); // 每2秒刷新一次程序时间
  m_programTimeUpdateTimer->start();

  // Initialize Pomodoro settings from UI defaults
  m_pomodoroWorkDurationMinutes = ui->workDurationSpinBox->value();
  m_pomodoroShortBreakMinutes = ui->shortBreakSpinBox->value();
  m_pomodoroLongBreakMinutes = ui->longBreakSpinBox->value();
  m_pomodoroCyclesBeforeLongBreak = ui->cyclesSpinBox->value();

  connect(ui->pomodoroGroupBox, &QGroupBox::toggled, this,
          &TimerWidget::on_pomodoroGroupBox_toggled);
  // Connect spin boxes to update internal values if they change during
  // non-active timer (For simplicity, we assume they are set before starting a
  // Pomodoro session)

  // Tray Icon for notifications (optional)
  if (QSystemTrayIcon::isSystemTrayAvailable()) {
    m_trayIcon = new QSystemTrayIcon(this);
    // Set an icon for the tray - find a suitable one or use default app icon
    QIcon appIcon =
        qApp->style()->standardIcon(QStyle::SP_ComputerIcon); // Example
    if (!appIcon.isNull())
      m_trayIcon->setIcon(appIcon);
    m_trayIcon->setToolTip(tr("学习计时器"));
    // m_trayIcon->show(); // Show it if you want it always visible, or only
    // when notifying
  } else {
    m_trayIcon = nullptr;
    qDebug() << "System tray not available.";
  }

  resetTimerState(); // Initial UI state
}

TimerWidget::~TimerWidget() { delete ui; }

void TimerWidget::setCurrentUser(const User &user) {
  m_currentUser = user;
  if (m_currentUser.isValid()) {
    loadSubjects();
  } else {
    ui->subjectComboBox->clear();
    ui->taskComboBox->clear();
  }
  resetTimerState(); // Reset timer if user changes
}

void TimerWidget::refreshTargetSelection() {
  if (m_currentUser.isValid()) {
    int currentSubjectId = ui->subjectComboBox->currentData().toInt();
    loadSubjects(); // This will re-populate subjects
    // Try to re-select the previously selected subject if it still exists
    int subjectIdx = ui->subjectComboBox->findData(currentSubjectId);
    if (subjectIdx != -1) {
      ui->subjectComboBox->setCurrentIndex(subjectIdx);
      // loadTasksForSubject will be called by the currentIndexChanged signal
    } else if (ui->subjectComboBox->count() > 0) {
      ui->subjectComboBox->setCurrentIndex(
          0); // Select first if previous is gone
    } else {
      ui->taskComboBox->clear(); // No subjects, so no tasks
    }
  }
}

void TimerWidget::setProcessMonitorService(ProcessMonitorService *service) {
  m_processMonitorService = service;
  if (m_processMonitorService) {
    connect(m_processMonitorService, &ProcessMonitorService::processStarted,
            this, &TimerWidget::onProgramStarted);
    connect(m_processMonitorService, &ProcessMonitorService::processStopped,
            this, &TimerWidget::onProgramStopped);
    connect(m_processMonitorService, &ProcessMonitorService::timeRecorded, this,
            &TimerWidget::onProgramTimeRecorded);
  }
  updateProgramMonitorDisplay();
}

void TimerWidget::onProgramStarted(const QString &processName,
                                   const QString &taskName) {
  qDebug() << "TimerWidget: Program started:" << processName
           << "for task:" << taskName;

  // Check if this program is related to the current selected task
  int currentTaskId = ui->taskComboBox->currentData().toInt();
  if (currentTaskId != -1) {
    Task currentTask = DatabaseManager::instance().getTaskById(
        currentTaskId, m_currentUser.id);
    if (currentTask.isValid() && taskName == currentTask.name) {
      m_currentMonitoredProcess = processName;
      ui->programStatusLabel->setText(QString("正在监控: %1").arg(processName));
      ui->programStatusLabel->setStyleSheet(
          "color: #2E8B57; font-weight: bold;");
      updateProgramTimeDisplay();
    }
  }
}

void TimerWidget::onProgramStopped(const QString &processName,
                                   const QString &taskName,
                                   qint64 durationSeconds) {
  qDebug() << "TimerWidget: Program stopped:" << processName
           << "for task:" << taskName << "duration:" << durationSeconds;

  if (m_currentMonitoredProcess == processName) {
    m_currentMonitoredProcess.clear();
    ui->programStatusLabel->setText("程序已停止");
    ui->programStatusLabel->setStyleSheet("color: #666;");

    // Add the session time to the total
    m_currentTaskProgramTime += durationSeconds;
    updateProgramTimeDisplay();
  }
}

void TimerWidget::onProgramTimeRecorded(int taskId, qint64 durationSeconds,
                                        bool isAutoRecorded) {
  qDebug() << "TimerWidget: Program time recorded: taskId" << taskId
           << durationSeconds << "seconds, auto:" << isAutoRecorded;

  // Check if this is for the current task
  int currentTaskId = ui->taskComboBox->currentData().toInt();
  if (currentTaskId != -1 && currentTaskId == taskId) {
    m_currentTaskProgramTime += durationSeconds;
    updateProgramTimeDisplay();
  }
}

void TimerWidget::updateProgramMonitorDisplay() {
  if (!m_processMonitorService) {
    ui->programStatusLabel->setText("程序监控未启用");
    ui->programStatusLabel->setStyleSheet("color: #666;");
    ui->programTimeLabel->setText("程序计时: --:--:--");
    return;
  }

  if (m_processMonitorService->isMonitoring()) {
    if (m_currentMonitoredProcess.isEmpty()) {
      ui->programStatusLabel->setText("等待程序启动...");
      ui->programStatusLabel->setStyleSheet("color: #666;");
    } else {
      ui->programStatusLabel->setText(
          QString("正在监控: %1").arg(m_currentMonitoredProcess));
      ui->programStatusLabel->setStyleSheet(
          "color: #2E8B57; font-weight: bold;");
    }
  } else {
    ui->programStatusLabel->setText("程序监控已停止");
    ui->programStatusLabel->setStyleSheet("color: #666;");
  }

  updateProgramTimeDisplay();
}

void TimerWidget::updateProgramTimeDisplay() {
  qint64 totalSeconds = m_currentTaskProgramTime;

  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;

  QString timeString = QString("%1:%2:%3")
                           .arg(hours, 2, 10, QChar('0'))
                           .arg(minutes, 2, 10, QChar('0'))
                           .arg(seconds, 2, 10, QChar('0'));

  ui->programTimeLabel->setText(QString("程序计时: %1").arg(timeString));

  // Update progress bar if there's an estimated time for the current task
  int currentTaskId = ui->taskComboBox->currentData().toInt();
  if (currentTaskId != -1) {
    Task currentTask = DatabaseManager::instance().getTaskById(
        currentTaskId, m_currentUser.id);
    if (currentTask.isValid() && currentTask.estimatedTimeMinutes > 0) {
      int estimatedSeconds = currentTask.estimatedTimeMinutes * 60;
      int progressValue = (totalSeconds * 100) / estimatedSeconds;
      progressValue = qMin(100, progressValue); // Cap at 100%

      ui->programTimeProgressBar->setVisible(true);
      ui->programTimeProgressBar->setValue(progressValue);
    } else {
      ui->programTimeProgressBar->setVisible(false);
    }
  } else {
    ui->programTimeProgressBar->setVisible(false);
  }
}

void TimerWidget::updateProgramTimeDisplayPeriodically() {
  // 定时从数据库刷新当前任务的程序时间
  int currentTaskId = ui->taskComboBox->currentData().toInt();
  if (currentTaskId != -1 && m_currentUser.isValid()) {
    QList<ProcessTimeLog> logs =
        DatabaseManager::instance().getProcessTimeLogsByTask(currentTaskId,
                                                             m_currentUser.id);
    qint64 totalProgramTimeSeconds = 0;

    for (const ProcessTimeLog &log : logs) {
      totalProgramTimeSeconds += log.durationSeconds;
    }

    // 只有当时间发生变化时才更新显示
    if (m_currentTaskProgramTime != totalProgramTimeSeconds) {
      m_currentTaskProgramTime = totalProgramTimeSeconds;
      updateProgramTimeDisplay();
    }
  }
}

void TimerWidget::loadSubjects() {
  ui->subjectComboBox->clear();
  ui->taskComboBox->clear(); // Clear tasks when subjects are reloaded
  if (!m_currentUser.isValid())
    return;

  QList<Subject> subjects =
      DatabaseManager::instance().getSubjectsByUser(m_currentUser.id);
  ui->subjectComboBox->addItem(tr("-- 选择科目 --"), -1); // Placeholder
  for (const Subject &subject : subjects) {
    ui->subjectComboBox->addItem(subject.name, subject.id);
  }
}

void TimerWidget::loadTasksForSubject(int subjectId) {
  ui->taskComboBox->clear();
  if (subjectId == -1 || !m_currentUser.isValid())
    return;

  QList<Task> tasks = DatabaseManager::instance().getTasksBySubject(
      subjectId, m_currentUser.id);
  ui->taskComboBox->addItem(tr("-- 选择任务 (可选) --"), -1); // Placeholder
  for (const Task &task : tasks) {
    // Show task name with status
    QString displayName = task.name;
    if (task.status == TaskStatus::Completed) {
      displayName += tr(" (已完成)");
    } else if (task.status == TaskStatus::InProgress) {
      displayName += tr(" (进行中)");
    }
    ui->taskComboBox->addItem(displayName, task.id);
  }
}

void TimerWidget::on_subjectComboBox_currentIndexChanged(int index) {
  Q_UNUSED(index);
  ui->taskComboBox->clear();
  int subjectId = ui->subjectComboBox->currentData().toInt();
  if (subjectId != -1 && m_currentUser.isValid()) {
    loadTasksForSubject(subjectId);
  }

  // Reset current task program time when subject changes
  m_currentTaskProgramTime = 0;
  updateProgramTimeDisplay();
}

void TimerWidget::on_taskComboBox_currentIndexChanged(int index) {
  Q_UNUSED(index);

  // Reset and update program time for the new task
  m_currentTaskProgramTime = 0;

  int taskId = ui->taskComboBox->currentData().toInt();
  if (taskId != -1 && m_currentUser.isValid()) {
    // Get existing program time for this task
    QList<ProcessTimeLog> logs =
        DatabaseManager::instance().getProcessTimeLogsByUser(m_currentUser.id);
    for (const ProcessTimeLog &log : logs) {
      if (log.taskId == taskId) {
        m_currentTaskProgramTime += log.durationSeconds;
      }
    }
  }

  updateProgramTimeDisplay();
}

void TimerWidget::on_startButton_clicked() {
  if (ui->subjectComboBox->currentData().toInt() == -1 &&
      ui->subjectComboBox->currentIndex() == 0) {
    QMessageBox::warning(this, tr("无法开始"),
                         tr("请先选择一个计时目标科目。"));
    return;
  }

  m_isPomodoroActive = ui->pomodoroGroupBox->isChecked();
  if (m_isPomodoroActive) {
    m_pomodoroWorkDurationMinutes = ui->workDurationSpinBox->value();
    m_pomodoroShortBreakMinutes = ui->shortBreakSpinBox->value();
    m_pomodoroLongBreakMinutes = ui->longBreakSpinBox->value();
    m_pomodoroCyclesBeforeLongBreak = ui->cyclesSpinBox->value();
    m_pomodoroCycleCount = 0; // Reset cycle count
    m_currentPomodoroCycleType =
        PomodoroCycleType::None; // Will be set by startNextPomodoroSegment
    startNextPomodoroSegment();
  } else {
    m_currentPomodoroCycleType = PomodoroCycleType::None;
    ui->pomodoroStatusLabel->clear();
    if (m_isPaused) { // Resume
      m_isPaused = false;
    } else { // Start new or restart after reset
      m_sessionStartTime = QDateTime::currentDateTime();
      m_elapsedSecondsTotal = 0;
    }
    m_ticker->start(1000); // Tick every second
  }
  updateButtonStates();
}

void TimerWidget::on_pauseButton_clicked() {
  m_isPaused = true;
  m_ticker->stop();
  updateButtonStates();
}

void TimerWidget::on_stopButton_clicked() {
  m_ticker->stop();
  if (m_elapsedSecondsTotal > 0 ||
      m_currentSegmentSeconds > 0) { // Ensure some time has passed
    saveTimeLog();
  }
  resetTimerState();
}

void TimerWidget::on_resetButton_clicked() {
  m_ticker->stop();
  resetTimerState();
}

void TimerWidget::on_pomodoroGroupBox_toggled(bool checked) {
  // When Pomodoro mode is toggled, reset the timer if it's running.
  // This prevents weird state transitions if toggled mid-session.
  if (m_ticker->isActive() || m_elapsedSecondsTotal > 0) {
    if (QMessageBox::question(this, tr("模式更改"),
                              tr("更改计时模式会重置当前计时会话，确定吗？"),
                              QMessageBox::Yes | QMessageBox::No) ==
        QMessageBox::Yes) {
      resetTimerState(true); // Full reset, clear fields
    } else {
      ui->pomodoroGroupBox->setChecked(!checked); // Revert toggle
      return;
    }
  }
  m_isPomodoroActive = checked;
  if (!m_isPomodoroActive) {
    ui->pomodoroStatusLabel->clear();
    m_currentPomodoroCycleType = PomodoroCycleType::None;
  }
  // Enable/disable Pomodoro settings based on checkbox
  ui->workDurationSpinBox->setEnabled(checked);
  ui->shortBreakSpinBox->setEnabled(checked);
  ui->longBreakSpinBox->setEnabled(checked);
  ui->cyclesSpinBox->setEnabled(checked);
}

void TimerWidget::updateTimerDisplay() {
  if (m_isPaused)
    return;

  if (!m_isPomodoroActive) {
    m_elapsedSecondsTotal++;
    m_currentSegmentSeconds =
        m_elapsedSecondsTotal; // For non-Pomodoro, segment is total
  } else {
    m_currentSegmentSeconds++; // Pomodoro uses segment seconds
  }

  qint64 displaySeconds =
      m_isPomodoroActive
          ? (m_pomodoroWorkDurationMinutes * 60) - m_currentSegmentSeconds
          : // Countdown for work
          ((m_currentPomodoroCycleType == PomodoroCycleType::ShortBreak
                ? m_pomodoroShortBreakMinutes * 60
                : (m_currentPomodoroCycleType == PomodoroCycleType::LongBreak
                       ? m_pomodoroLongBreakMinutes * 60
                       : 0)) -
           m_currentSegmentSeconds); // Countdown for break

  if (m_isPomodoroActive) {
    if (m_currentPomodoroCycleType == PomodoroCycleType::Work) {
      displaySeconds =
          (m_pomodoroWorkDurationMinutes * 60) - m_currentSegmentSeconds;
    } else if (m_currentPomodoroCycleType == PomodoroCycleType::ShortBreak) {
      displaySeconds =
          (m_pomodoroShortBreakMinutes * 60) - m_currentSegmentSeconds;
    } else if (m_currentPomodoroCycleType == PomodoroCycleType::LongBreak) {
      displaySeconds =
          (m_pomodoroLongBreakMinutes * 60) - m_currentSegmentSeconds;
    } else { // Should not happen if pomodoro is active and started
      displaySeconds = 0;
    }

    if (displaySeconds <= 0) {
      displaySeconds = 0; // Don't show negative
      pomodoroSegmentFinished();
    }
  } else { // Normal timer (counts up)
    displaySeconds = m_currentSegmentSeconds;
  }

  qint64 hours = displaySeconds / 3600;
  qint64 minutes = (displaySeconds % 3600) / 60;
  qint64 seconds = displaySeconds % 60;

  ui->timerDisplayLabel->setText(QString("%1:%2:%3")
                                     .arg(hours, 2, 10, QChar('0'))
                                     .arg(minutes, 2, 10, QChar('0'))
                                     .arg(seconds, 2, 10, QChar('0')));
}

void TimerWidget::resetTimerState(bool fullResetLogic) {
  m_ticker->stop();
  m_elapsedSecondsTotal = 0;
  m_currentSegmentSeconds = 0;
  m_isPaused = false;
  ui->timerDisplayLabel->setText("00:00:00");

  if (fullResetLogic) { // Full reset also clears Pomodoro state and notes
    ui->pomodoroGroupBox->setChecked(
        false); // This will trigger on_pomodoroGroupBox_toggled
    m_isPomodoroActive = false;
    m_currentPomodoroCycleType = PomodoroCycleType::None;
    m_pomodoroCycleCount = 0;
    ui->pomodoroStatusLabel->clear();
    ui->notesTextEdit->clear();
    // Don't reset target selection (subject/task) unless user changes
  }
  updateButtonStates();
}

void TimerWidget::updateButtonStates() {
  bool running = m_ticker->isActive() && !m_isPaused;
  bool canStart =
      !m_ticker->isActive() || m_isPaused; // Can start if not running or paused

  ui->startButton->setEnabled(canStart);
  ui->pauseButton->setEnabled(running);
  ui->stopButton->setEnabled(
      m_elapsedSecondsTotal > 0 || m_currentSegmentSeconds > 0 ||
      m_ticker->isActive()); // Can stop if any time elapsed or running
  ui->resetButton->setEnabled(m_elapsedSecondsTotal > 0 ||
                              m_currentSegmentSeconds > 0 ||
                              m_ticker->isActive());

  // Disable target selection and Pomodoro settings while timer is active (not
  // paused)
  bool disableConfig = m_ticker->isActive() && !m_isPaused;
  ui->subjectComboBox->setEnabled(!disableConfig);
  ui->taskComboBox->setEnabled(!disableConfig);
  ui->pomodoroGroupBox->setEnabled(
      !disableConfig ||
      m_isPaused); // Allow toggling if paused, but it will reset.
  if (ui->pomodoroGroupBox->isChecked()) {
    ui->workDurationSpinBox->setEnabled(!disableConfig);
    ui->shortBreakSpinBox->setEnabled(!disableConfig);
    ui->longBreakSpinBox->setEnabled(!disableConfig);
    ui->cyclesSpinBox->setEnabled(!disableConfig);
  }
}

void TimerWidget::saveTimeLog(bool sessionCompletedNormally) {
  if (!m_currentUser.isValid())
    return;
  if (!sessionCompletedNormally &&
      (m_currentSegmentSeconds == 0 && m_elapsedSecondsTotal == 0))
    return; // Nothing to save

  TimeLog log;
  log.userId = m_currentUser.id;
  log.subjectId = ui->subjectComboBox->currentData().toInt();
  log.taskId = ui->taskComboBox->currentData().toInt();

  log.startTime = m_sessionStartTime;
  log.endTime = QDateTime::currentDateTime(); // End time is now

  // For Pomodoro, duration is derived from settings, for normal it's elapsed
  if (m_isPomodoroActive && sessionCompletedNormally) {
    // Use predefined segment duration for Pomodoro logs
    if (m_currentPomodoroCycleType == PomodoroCycleType::Work) {
      log.durationMinutes = m_pomodoroWorkDurationMinutes;
    } else if (m_currentPomodoroCycleType == PomodoroCycleType::ShortBreak) {
      log.durationMinutes = m_pomodoroShortBreakMinutes;
    } else if (m_currentPomodoroCycleType == PomodoroCycleType::LongBreak) {
      log.durationMinutes = m_pomodoroLongBreakMinutes;
    } else { // Should not happen if cycle type is set
      log.durationMinutes = qRound(m_currentSegmentSeconds / 60.0);
    }
    // Adjust start time based on calculated duration for this segment
    log.startTime = log.endTime.addSecs(-log.durationMinutes * 60);

  } else { // Normal timer or Pomodoro stopped prematurely
    log.durationMinutes = qRound(
        (m_isPaused
             ? m_elapsedSecondsTotal
             : m_elapsedSecondsTotal +
                   m_sessionStartTime.secsTo(QDateTime::currentDateTime())) /
        60.0);
    // If stopped prematurely in Pomodoro, log actual elapsed time for the
    // segment
    if (m_isPomodoroActive && !sessionCompletedNormally) {
      log.durationMinutes = qRound(m_currentSegmentSeconds / 60.0);
      log.startTime = log.endTime.addSecs(
          -m_currentSegmentSeconds); // Base start time on actual segment
    }
  }

  if (log.durationMinutes <= 0 &&
      sessionCompletedNormally) { // Don't log zero/negative duration unless
                                  // premature stop with some time
    if (m_isPomodoroActive &&
        log.durationMinutes <=
            0) { /* allow logging break if it was 0 by setting */
    } else {
      qDebug() << "Skipping log for zero or negative duration.";
      return;
    }
  }

  log.notes = ui->notesTextEdit->toPlainText().trimmed();
  log.isPomodoroSession = m_isPomodoroActive;
  log.cycleType =
      m_currentPomodoroCycleType; // Log the type of segment just finished

  if (DatabaseManager::instance().addTimeLog(log)) {
    qDebug() << "TimeLog saved. ID:" << log.id
             << "Duration:" << log.durationMinutes << "min";
    if (log.taskId != -1 &&
        (log.cycleType == PomodoroCycleType::Work ||
         !log.isPomodoroSession)) { // Only add work time to task
      if (DatabaseManager::instance().incrementTaskActualTime(
              log.taskId, log.durationMinutes, m_currentUser.id)) {
        qDebug() << "Task actual time updated for task ID:" << log.taskId;
        // TODO: MainWindow should refresh task view if visible
      }
    }
    emit timerStoppedAndSaved(log); // Emit signal
  } else {
    QMessageBox::critical(this, tr("错误"), tr("无法保存计时记录到数据库。"));
  }
}

// --- Pomodoro Logic ---
void TimerWidget::startNextPomodoroSegment() {
  m_ticker->stop();
  m_currentSegmentSeconds = 0; // Reset segment timer

  if (m_currentPomodoroCycleType ==
      PomodoroCycleType::Work) { // Finished a work cycle
    m_pomodoroCycleCount++;
    if (m_pomodoroCycleCount >= m_pomodoroCyclesBeforeLongBreak) {
      m_currentPomodoroCycleType = PomodoroCycleType::LongBreak;
      ui->pomodoroStatusLabel->setText(
          tr("长时休息 (%1 分钟)").arg(m_pomodoroLongBreakMinutes));
    } else {
      m_currentPomodoroCycleType = PomodoroCycleType::ShortBreak;
      ui->pomodoroStatusLabel->setText(
          tr("短时休息 (%1 分钟)").arg(m_pomodoroShortBreakMinutes));
    }
  } else { // Finished a break or starting fresh
    m_currentPomodoroCycleType = PomodoroCycleType::Work;
    QString statusText = tr("工作中 (周期 %1/%2, %3 分钟)")
                             .arg(m_pomodoroCycleCount + 1)
                             .arg(m_pomodoroCyclesBeforeLongBreak)
                             .arg(m_pomodoroWorkDurationMinutes);
    ui->pomodoroStatusLabel->setText(statusText);
    if (m_pomodoroCycleCount >=
        m_pomodoroCyclesBeforeLongBreak) { // After long break, reset cycle
                                           // count
      m_pomodoroCycleCount = 0;
      ui->pomodoroStatusLabel->setText(tr("工作中 (新一轮周期 1/%1, %2 分钟)")
                                           .arg(m_pomodoroCyclesBeforeLongBreak)
                                           .arg(m_pomodoroWorkDurationMinutes));
    }
  }

  m_sessionStartTime =
      QDateTime::currentDateTime(); // Reset start time for each segment
  m_elapsedSecondsTotal =
      0; // Reset total elapsed for pomodoro segment logic (used if paused)
  m_isPaused = false;

  qDebug() << "Starting Pomodoro segment:"
           << pomodoroCycleTypeToString(m_currentPomodoroCycleType);
  showTrayNotification(
      tr("番茄钟: %1")
          .arg(pomodoroCycleTypeToString(m_currentPomodoroCycleType)),
      tr("新的阶段已开始!"));
  playSoundNotification();
  m_ticker->start(1000);
  updateButtonStates();
}

void TimerWidget::pomodoroSegmentFinished() {
  m_ticker->stop();
  qDebug() << "Pomodoro segment finished:"
           << pomodoroCycleTypeToString(m_currentPomodoroCycleType);

  // Save the log for the completed segment
  // Make a copy of current segment seconds as saveTimeLog might be called later
  // by stop button. For auto-transition, we use the configured duration.
  saveTimeLog(true); // true means segment completed normally as per its defined
                     // duration

  // If it was a long break and it finished, pomodoro session might be
  // considered "done" or it just loops. Current implementation loops.
  if (m_currentPomodoroCycleType == PomodoroCycleType::LongBreak) {
    m_pomodoroCycleCount = 0; // Reset for next set of cycles after a long break
  }

  startNextPomodoroSegment(); // Automatically start the next one
}

void TimerWidget::playSoundNotification() {
  // QSoundEffect is better for short sounds and cross-platform if you package
  // the sound file. For simplicity, QMediaPlayer could also be used. Example:
  // QSoundEffect *effect = new QSoundEffect(this);
  // effect->setSource(QUrl::fromLocalFile(":/sounds/pomodoro_alert.wav")); //
  // Add sound to resources effect->setVolume(0.75f); effect->play();
  // connect(effect, &QSoundEffect::playingChanged, [effect](){
  // if(!effect->isPlaying()) effect->deleteLater(); });
  QApplication::beep(); // Simple system beep
  qDebug() << "Playing sound notification (beep).";
}

void TimerWidget::showTrayNotification(const QString &title,
                                       const QString &message) {
  if (m_trayIcon && m_trayIcon->isVisible()) { // Or just m_trayIcon
    m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information,
                            3000); // Show for 3 seconds
  } else if (m_trayIcon &&
             !m_trayIcon->isVisible()) { // If tray icon exists but isn't shown,
                                         // show it then the message
    m_trayIcon->show();
    m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
  } else {
    qDebug() << "Tray notification:" << title << "-" << message;
  }
}