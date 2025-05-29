#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H

#include <QWidget>
#include <QTimer>   // For the ticker
#include <QDateTime>
#include "core/User.h"
#include "core/Subject.h"
#include "core/Task.h"
#include "core/TimeLog.h" // For PomodoroCycleType

QT_BEGIN_NAMESPACE
namespace Ui { class TimerWidget; }
QT_END_NAMESPACE

class QSystemTrayIcon; // Forward declaration

class TimerWidget : public QWidget {
    Q_OBJECT

public:
    explicit TimerWidget(QWidget *parent = nullptr);
    ~TimerWidget();

    void setCurrentUser(const User& user); // To load subjects/tasks for this user
    void refreshTargetSelection(); // Call when subjects/tasks might have changed

signals:
    void timerStoppedAndSaved(const TimeLog& newLog); // Optional: if MainWindow needs to know

private slots:
    void on_startButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_resetButton_clicked();
    void on_pomodoroGroupBox_toggled(bool checked);
    void on_subjectComboBox_currentIndexChanged(int index);

    void updateTimerDisplay(); // Slot for QTimer's timeout

private:
    Ui::TimerWidget *ui;
    User m_currentUser;
    QTimer *m_ticker; // Main timer for counting seconds
    QDateTime m_sessionStartTime;
    qint64 m_elapsedSecondsTotal; // Total seconds elapsed in current session (handles pauses)
    qint64 m_currentSegmentSeconds; // Seconds for the current segment (work/break in Pomodoro)
    bool m_isPaused;

    // Pomodoro state
    bool m_isPomodoroActive;
    PomodoroCycleType m_currentPomodoroCycleType;
    int m_pomodoroWorkDurationMinutes;
    int m_pomodoroShortBreakMinutes;
    int m_pomodoroLongBreakMinutes;
    int m_pomodoroCyclesBeforeLongBreak;
    int m_pomodoroCycleCount; // Current number of work cycles completed

    QSystemTrayIcon* m_trayIcon; // Optional: for notifications

    void loadSubjects();
    void loadTasksForSubject(int subjectId);

    void resetTimerState(bool fullResetLogic = true);
    void updateButtonStates();
    void saveTimeLog(bool sessionCompletedNormally = true);

    // Pomodoro logic
    void startNextPomodoroSegment();
    void pomodoroSegmentFinished();
    void playSoundNotification(); // Placeholder for sound
    void showTrayNotification(const QString& title, const QString& message);
};

#endif // TIMERWIDGET_H