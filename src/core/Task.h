#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>

// Enum for Task Priority
enum class TaskPriority {
    Low,
    Medium,
    High
};

// Enum for Task Status
enum class TaskStatus {
    Pending,
    InProgress,
    Completed
    // Archived (optional for later)
};

// Helper functions to convert enums to string for display/DB if needed
// and from string/int if storing them differently.
// For DB, storing as INTEGER is often best for enums.

inline QString priorityToString(TaskPriority priority) {
    switch (priority) {
        case TaskPriority::Low: return QStringLiteral("低");
        case TaskPriority::Medium: return QStringLiteral("中");
        case TaskPriority::High: return QStringLiteral("高");
        default: return QStringLiteral("未知");
    }
}

inline QString statusToString(TaskStatus status) {
    switch (status) {
        case TaskStatus::Pending: return QStringLiteral("待办");
        case TaskStatus::InProgress: return QStringLiteral("进行中");
        case TaskStatus::Completed: return QStringLiteral("已完成");
        default: return QStringLiteral("未知");
    }
}


struct Task {
    int id = -1;
    int userId = -1;
    int subjectId = -1; // Foreign key to Subject

    QString name;
    QString description;
    TaskPriority priority = TaskPriority::Medium;
    QDateTime dueDate;
    int estimatedTimeMinutes = 0; // Estimated time in minutes
    int actualTimeMinutes = 0;    // Actual time spent, updated by timer

    TaskStatus status = TaskStatus::Pending;
    QDateTime creationDate;
    QDateTime completionDate; // Null if not completed

    Task() : creationDate(QDateTime::currentDateTime()) {} // Default creation date

    bool isValid() const {
        return id != -1 && userId != -1 && subjectId != -1 && !name.isEmpty();
    }

    bool isCompleted() const {
        return status == TaskStatus::Completed;
    }
};

#endif // TASK_H