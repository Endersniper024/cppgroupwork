#include "ActivityMonitorService.h"
#include "core/DatabaseManager.h" // For saving TimeLog and updating tasks
#include <QDebug>
#include <QFileInfo> // For QFileInfo

// Windows specific includes
#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h> // For GetModuleFileNameEx
// For QueryFullProcessImageName, uncomment if MinGW supports it well or if using MSVC
// #include <winbase.h> // Might be needed for QueryFullProcessImageName
#endif


ActivityMonitorService::ActivityMonitorService(QObject *parent) :
    QObject(parent),
    m_pollTimer(new QTimer(this)),
    m_currentUserId(-1),
    m_lastMatchedRuleId(-1),
    m_isCurrentlyTracking(false)
{
    connect(m_pollTimer, &QTimer::timeout, this, &ActivityMonitorService::pollForegroundWindow);
}

ActivityMonitorService::~ActivityMonitorService() {
    stopMonitoring(); // Ensure timer is stopped
}

void ActivityMonitorService::startMonitoring(int userId, const QList<ApplicationRule>& rules) {
    if (m_pollTimer->isActive()) {
        qDebug() << "Activity monitor is already running.";
        return;
    }
#ifndef Q_OS_WIN
    qWarning() << "Activity monitoring is only supported on Windows. Service will not start.";
    // Optionally emit a signal or show a message to the user via main application
    return;
#endif

    m_currentUserId = userId;
    m_activeRules = rules; // Store a copy of the rules for the current user
    m_lastProcessName.clear();
    m_lastWindowTitle.clear();
    m_lastMatchedRuleId = -1;
    m_isCurrentlyTracking = false;

    m_pollTimer->start(5000); // Poll every 5 seconds (adjustable)
    qDebug() << "Activity monitor started for user ID:" << m_currentUserId;
}

void ActivityMonitorService::stopMonitoring() {
    if (m_pollTimer->isActive()) {
        m_pollTimer->stop();
        if (m_isCurrentlyTracking) {
            finalizeCurrentSegment(QDateTime::currentDateTime()); // Log any pending time
            m_isCurrentlyTracking = false;
        }
        qDebug() << "Activity monitor stopped.";
    }
}

void ActivityMonitorService::updateRules(const QList<ApplicationRule>& rules) {
    // Filter rules for the current user if not already done
    m_activeRules.clear();
    for(const auto& rule : rules) {
        if(rule.userId == m_currentUserId) { // Or assume rules passed are already filtered
            m_activeRules.append(rule);
        }
    }
    qDebug() << "Activity monitor rules updated. Count:" << m_activeRules.size();
    // If tracking, current match might change, so re-evaluate immediately
    if(m_pollTimer->isActive()) {
        pollForegroundWindow();
    }
}

bool ActivityMonitorService::isMonitoring() const {
    return m_pollTimer->isActive();
}

void ActivityMonitorService::pollForegroundWindow() {
#ifndef Q_OS_WIN
    return; // Should not be called if not Windows, but as a safeguard
#endif

    QString currentProcessName, currentWindowTitle;
    if (!getForegroundAppInfo(currentProcessName, currentWindowTitle)) {
        // Could not get info, maybe no window focused or an error
        if (m_isCurrentlyTracking) { // If we were tracking something, it's no longer active
            finalizeCurrentSegment(QDateTime::currentDateTime());
            m_isCurrentlyTracking = false;
            m_lastMatchedRuleId = -1;
        }
        emit noMatchedActivity("", " (Error retrieving foreground window info)");
        return;
    }

    // Normalize for comparison if needed (e.g. toLower)
    // currentProcessName = currentProcessName.toLower();

    if (currentProcessName == m_lastProcessName && currentWindowTitle == m_lastWindowTitle && m_isCurrentlyTracking) {
        // Same app and rule still active, update UI with current duration
        if (m_lastMatchedRuleId != -1) {
            qint64 durationSeconds = m_currentSegmentStartTime.secsTo(QDateTime::currentDateTime());
            emit currentActivityUpdate(currentProcessName, currentWindowTitle, m_lastMatchedRuleId, durationSeconds);
        }
        return; // No change in focused app that we are tracking
    }
    
    // App or title has changed, or we weren't tracking
    int matchedRuleId = findMatchingRule(currentProcessName, currentWindowTitle);

    if (m_isCurrentlyTracking) { // If we were tracking something before
        // Check if the new situation still matches the *same* rule or if rule changed/no longer matches
        if (matchedRuleId != m_lastMatchedRuleId) {
            finalizeCurrentSegment(QDateTime::currentDateTime()); // Stop previous tracking
            m_isCurrentlyTracking = false; // Assume no tracking until new rule is confirmed
        }
    }
    
    // Update last known foreground app, regardless of match
    m_lastProcessName = currentProcessName;
    m_lastWindowTitle = currentWindowTitle;

    if (matchedRuleId != -1) { // New activity matches a rule
        if (!m_isCurrentlyTracking || matchedRuleId != m_lastMatchedRuleId) { // Start new tracking if not tracking or rule changed
            m_currentSegmentStartTime = QDateTime::currentDateTime();
            m_isCurrentlyTracking = true;
            m_lastMatchedRuleId = matchedRuleId;
            qDebug() << "Started tracking for rule ID:" << matchedRuleId << "App:" << currentProcessName << "Title:" << currentWindowTitle;
        }
        // Emit current activity, even if it's a continuation of the same rule (handled by initial check)
        // but this ensures UI updates if it just became tracked.
        qint64 durationSeconds = m_currentSegmentStartTime.secsTo(QDateTime::currentDateTime());
        emit currentActivityUpdate(currentProcessName, currentWindowTitle, matchedRuleId, durationSeconds);

    } else { // No rule matched
        // If we were tracking (because m_isCurrentlyTracking was true and now matchedRuleId is -1),
        // finalizeCurrentSegment was already called.
        m_isCurrentlyTracking = false; // Ensure this is false
        m_lastMatchedRuleId = -1;
        emit noMatchedActivity(currentProcessName, currentWindowTitle);
        qDebug() << "No rule matched for App:" << currentProcessName << "Title:" << currentWindowTitle;
    }
}

bool ActivityMonitorService::getForegroundAppInfo(QString& processName, QString& windowTitle) {
#ifdef Q_OS_WIN
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) {
        return false;
    }

    // Get Window Title
    const int bufferSize = 256;
    WCHAR titleBuffer[bufferSize];
    if (GetWindowTextW(hwnd, titleBuffer, bufferSize) > 0) {
        windowTitle = QString::fromWCharArray(titleBuffer);
    } else {
        windowTitle = ""; // Or "Unknown Title"
    }

    // Get Process ID and Name
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess != NULL) {
        WCHAR pathBuffer[MAX_PATH];
        // Using QueryFullProcessImageName for more robust name retrieval (requires Vista+)
        // If compatibility with XP is needed, GetModuleFileNameEx would be an alternative,
        // but QueryFullProcessImageName is generally better if available.
        // DWORD pathLen = QueryFullProcessImageNameW(hProcess, 0, pathBuffer, MAX_PATH); // MSVC
        // For MinGW, GetModuleFileNameEx might be more readily available or easier to link
        DWORD pathLen = GetModuleFileNameExW(hProcess, NULL, pathBuffer, MAX_PATH);

        if (pathLen > 0) {
            QString fullPath = QString::fromWCharArray(pathBuffer);
            processName = QFileInfo(fullPath).fileName(); // Extract just the exe name
        } else {
            // Fallback or error
            processName = "UnknownApp.exe";
            qDebug() << "Failed to get process name. Error:" << GetLastError();
        }
        CloseHandle(hProcess);
    } else {
        processName = "UnknownApp.exe";
         qDebug() << "Failed to open process. Error:" << GetLastError();
        return false; // Could not get process info
    }
    return true;
#else
    processName = "NotOnWindows.app";
    windowTitle = "Not on Windows";
    return false; // Should not be called on non-Windows
#endif
}

int ActivityMonitorService::findMatchingRule(const QString& processName, const QString& windowTitle) const {
    for (const ApplicationRule& rule : m_activeRules) {
        if (rule.userId != m_currentUserId) continue; // Should be pre-filtered, but double check

        bool processMatch = false;
        if (rule.processNamePattern.isEmpty() || // Empty pattern matches any process
            (rule.processNamePattern.startsWith('*') && rule.processNamePattern.endsWith('*') && processName.contains(rule.processNamePattern.mid(1, rule.processNamePattern.length() - 2), Qt::CaseInsensitive)) ||
            (rule.processNamePattern.startsWith('*') && processName.endsWith(rule.processNamePattern.mid(1), Qt::CaseInsensitive)) ||
            (rule.processNamePattern.endsWith('*') && processName.startsWith(rule.processNamePattern.left(rule.processNamePattern.length() - 1), Qt::CaseInsensitive)) ||
            (processName.compare(rule.processNamePattern, Qt::CaseInsensitive) == 0) )
        {
            processMatch = true;
        }

        if (!processMatch) continue;

        bool titleMatch = false;
        if (rule.windowTitlePattern.isEmpty() || // Empty pattern matches any title
            (rule.windowTitlePattern.startsWith('*') && rule.windowTitlePattern.endsWith('*') && windowTitle.contains(rule.windowTitlePattern.mid(1, rule.windowTitlePattern.length() - 2), Qt::CaseInsensitive)) ||
            (rule.windowTitlePattern.startsWith('*') && windowTitle.endsWith(rule.windowTitlePattern.mid(1), Qt::CaseInsensitive)) ||
            (rule.windowTitlePattern.endsWith('*') && windowTitle.startsWith(rule.windowTitlePattern.left(rule.windowTitlePattern.length() - 1), Qt::CaseInsensitive)) ||
            (windowTitle.compare(rule.windowTitlePattern, Qt::CaseInsensitive) == 0) )
        {
            titleMatch = true;
        }
        
        if (titleMatch) { // Process matched and title matched
            return rule.ruleId;
        }
    }
    return -1; // No rule matched
}

void ActivityMonitorService::finalizeCurrentSegment(const QDateTime& endTime) {
    if (!m_isCurrentlyTracking || m_lastMatchedRuleId == -1 || m_currentUserId == -1) {
        return;
    }

    qint64 durationSeconds = m_currentSegmentStartTime.secsTo(endTime);
    if (durationSeconds <= 0) { // Don't log empty or negative segments
        qDebug() << "Skipping zero/negative duration segment for rule ID:" << m_lastMatchedRuleId;
        return;
    }

    // Find the rule details
    const ApplicationRule* foundRule = nullptr;
    for(const auto& rule : m_activeRules) {
        if(rule.ruleId == m_lastMatchedRuleId) {
            foundRule = &rule;
            break;
        }
    }

    if (!foundRule) {
        qWarning() << "Could not find rule details for ID:" << m_lastMatchedRuleId << "to log time.";
        return;
    }

    TimeLog log;
    log.userId = m_currentUserId;
    log.subjectId = foundRule->subjectId;
    log.taskId = foundRule->taskId; // Task ID takes precedence if UI logic wants that
    log.startTime = m_currentSegmentStartTime;
    log.endTime = endTime;
    log.durationMinutes = qRound(durationSeconds / 60.0);
    log.notes = tr("自动追踪: %1 (%2)").arg(m_lastProcessName).arg(m_lastWindowTitle.left(50)); // Truncate title if too long
    log.isPomodoroSession = false; // This is not a manual Pomodoro session
    log.cycleType = PomodoroCycleType::None;

    if (DatabaseManager::instance().addTimeLog(log)) {
        qDebug() << "Automatic TimeLog saved for rule ID:" << m_lastMatchedRuleId << "Duration:" << log.durationMinutes << "min.";
        if (log.taskId != -1) {
            DatabaseManager::instance().incrementTaskActualTime(log.taskId, log.durationMinutes, m_currentUserId);
        }
        emit timedSegmentLogged(log);
    } else {
        qWarning() << "Failed to save automatic TimeLog for rule ID:" << m_lastMatchedRuleId;
    }
    m_lastMatchedRuleId = -1; // Reset last matched rule after logging
}