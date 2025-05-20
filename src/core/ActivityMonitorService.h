#ifndef ACTIVITYMONITORSERVICE_H
#define ACTIVITYMONITORSERVICE_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QDateTime>
#include <QList>
#include "core/TimeLog.h" // For TimeLog struct
#include "core/User.h"    // For current user context

// Forward declare the conceptual rule struct, or include if it becomes a proper header
struct ApplicationRule {
    int ruleId = -1;
    QString processNamePattern;
    QString windowTitlePattern; // Supports simple wildcard '*' at start/end for contains, or exact match
    int subjectId = -1;
    int taskId = -1;
    int userId = -1; // Ensure rule is for current user
    // Add other properties like minimum duration to log, etc. if needed
};


class ActivityMonitorService : public QObject {
    Q_OBJECT

public:
    explicit ActivityMonitorService(QObject *parent = nullptr);
    ~ActivityMonitorService();

    void startMonitoring(int userId, const QList<ApplicationRule>& rules);
    void stopMonitoring();
    void updateRules(const QList<ApplicationRule>& rules); // To update rules while running
    bool isMonitoring() const;

signals:
    void currentActivityUpdate(const QString& appName, const QString& windowTitle, int matchedRuleId, qint64 currentSegmentDurationSeconds);
    void noMatchedActivity(const QString& appName, const QString& windowTitle);
    void timedSegmentLogged(const TimeLog& newLogEntry); // Emitted when a segment is saved

private slots:
    void pollForegroundWindow();

private:
    QTimer *m_pollTimer;
    User m_currentUser; // Not strictly needed if userId is passed, but good for context
    int m_currentUserId;
    QList<ApplicationRule> m_activeRules;

    QString m_lastProcessName;
    QString m_lastWindowTitle;
    int m_lastMatchedRuleId;    // ID of the ApplicationRule
    QDateTime m_currentSegmentStartTime;
    bool m_isCurrentlyTracking; // True if the current foreground app matches a rule

    // Windows specific helper
    bool getForegroundAppInfo(QString& processName, QString& windowTitle);
    int findMatchingRule(const QString& processName, const QString& windowTitle) const;
    void finalizeCurrentSegment(const QDateTime& endTime);
};

#endif // ACTIVITYMONITORSERVICE_H