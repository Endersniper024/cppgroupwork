#ifndef TIMELOG_H
#define TIMELOG_H

#include <QString>
#include <QDateTime>

enum class PomodoroCycleType {
    None,
    Work,
    ShortBreak,
    LongBreak
};

inline QString pomodoroCycleTypeToString(PomodoroCycleType type) {
    switch (type) {
        case PomodoroCycleType::Work: return QStringLiteral("工作");
        case PomodoroCycleType::ShortBreak: return QStringLiteral("短时休息");
        case PomodoroCycleType::LongBreak: return QStringLiteral("长时休息");
        case PomodoroCycleType::None:
        default:
            return QStringLiteral("常规计时");
    }
}

struct TimeLog {
    int id = -1;
    int userId = -1;
    int taskId = -1;      // Optional: Associated task ID
    int subjectId = -1;   // Optional: Associated subject ID (can be set even if taskId is set)

    QDateTime startTime;
    QDateTime endTime;
    int durationMinutes = 0;

    QString notes;        // Optional notes for the session
    bool isPomodoroSession = false;
    PomodoroCycleType cycleType = PomodoroCycleType::None;

    TimeLog() = default;

    bool isValid() const {
        return userId != -1 && startTime.isValid() && endTime.isValid() && endTime >= startTime && durationMinutes >= 0;
    }
};

#endif // TIMELOG_H