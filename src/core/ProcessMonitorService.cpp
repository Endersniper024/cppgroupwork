#include "ProcessMonitorService.h"
#include "Task.h"
#include <QDebug>

ProcessMonitorService::ProcessMonitorService(int userId, QObject *parent)
    : QObject(parent), m_userId(userId), m_isMonitoring(false),
      m_processMonitor(new ProcessMonitor(this)),
      m_refreshTimer(new QTimer(this)) {
  // 连接程序监控信号
  connect(m_processMonitor, &ProcessMonitor::processStarted, this,
          &ProcessMonitorService::onProcessStarted);
  connect(m_processMonitor, &ProcessMonitor::processStopped, this,
          &ProcessMonitorService::onProcessStopped);

  // 设置定时刷新
  m_refreshTimer->setInterval(REFRESH_INTERVAL);
  connect(m_refreshTimer, &QTimer::timeout, this,
          &ProcessMonitorService::checkAndUpdateActiveProcesses);
}

void ProcessMonitorService::startMonitoring() {
  if (m_isMonitoring)
    return;

  qDebug() << "ProcessMonitorService: Starting monitoring for user" << m_userId;

  // 加载程序关联
  loadProcessLinks();

  // 开始监控
  m_processMonitor->startMonitoring();
  m_refreshTimer->start();

  m_isMonitoring = true;
  emit monitoringStatusChanged(true);
}

void ProcessMonitorService::stopMonitoring() {
  if (!m_isMonitoring)
    return;

  qDebug() << "ProcessMonitorService: Stopping monitoring";

  // 结束所有活动会话
  for (auto it = m_activeSessions.begin(); it != m_activeSessions.end(); ++it) {
    endProcessSession(it.key());
  }
  m_activeSessions.clear();

  // 停止监控
  m_processMonitor->stopMonitoring();
  m_refreshTimer->stop();

  m_isMonitoring = false;
  emit monitoringStatusChanged(false);
}

void ProcessMonitorService::setUserId(int userId) {
  if (m_userId != userId) {
    bool wasMonitoring = m_isMonitoring;

    if (wasMonitoring) {
      stopMonitoring();
    }

    m_userId = userId;

    if (wasMonitoring) {
      startMonitoring();
    }
  }
}

void ProcessMonitorService::refreshProcessLinks() { loadProcessLinks(); }

int ProcessMonitorService::getMonitoredProcessCount() const {
  return m_processLinks.size();
}

void ProcessMonitorService::onProcessStarted(const QString &processName) {
  qDebug() << "Process started:" << processName;

  // 查找匹配的程序关联
  for (const ProcessLink &link : m_processLinks) {
    if (link.processName.compare(processName, Qt::CaseInsensitive) == 0 &&
        link.autoStart) {
      QString taskName = getTaskName(link.taskId);
      startProcessSession(link, taskName);
      emit processStarted(processName, taskName);
      break;
    }
  }
}

void ProcessMonitorService::onProcessStopped(const QString &processName,
                                             qint64 sessionRuntime) {
  Q_UNUSED(sessionRuntime)
  qDebug() << "Process stopped:" << processName;

  if (m_activeSessions.contains(processName)) {
    const ActiveProcessSession &session = m_activeSessions[processName];
    if (session.autoRecordEnabled) {
      endProcessSession(processName);
      emit processStopped(
          processName, session.taskName,
          session.startTime.msecsTo(QDateTime::currentDateTime()) / 1000);
    }
  }
}

void ProcessMonitorService::checkAndUpdateActiveProcesses() {
  // 刷新程序关联（可能有新的关联被添加）
  refreshProcessLinks();

  // 检查当前活动会话的状态
  QStringList processesToRemove;
  for (auto it = m_activeSessions.begin(); it != m_activeSessions.end(); ++it) {
    if (!m_processMonitor->isProcessRunning(it.key())) {
      processesToRemove.append(it.key());
    }
  }

  // 清理已停止的进程会话
  for (const QString &processName : processesToRemove) {
    endProcessSession(processName);
  }
}

void ProcessMonitorService::loadProcessLinks() {
  m_processLinks = DatabaseManager::instance().getProcessLinksByUser(m_userId);

  // 将程序添加到监控器
  for (const ProcessLink &link : m_processLinks) {
    m_processMonitor->addMonitoredProcess(link.processName, link.displayName);
  }

  qDebug() << "Loaded" << m_processLinks.size() << "process links for user"
           << m_userId;
}

void ProcessMonitorService::startProcessSession(const ProcessLink &link,
                                                const QString &taskName) {
  if (m_activeSessions.contains(link.processName)) {
    // 已经有活动会话，不重复开始
    return;
  }

  ActiveProcessSession session;
  session.processLinkId = link.id;
  session.taskId = link.taskId;
  session.processName = link.processName;
  session.taskName = taskName;
  session.startTime = QDateTime::currentDateTime();
  session.autoRecordEnabled = link.autoStop;

  m_activeSessions[link.processName] = session;

  qDebug() << "Started process session for" << link.processName << "-> task"
           << taskName;
}

void ProcessMonitorService::endProcessSession(const QString &processName) {
  if (!m_activeSessions.contains(processName)) {
    return;
  }

  ActiveProcessSession session = m_activeSessions.take(processName);
  QDateTime endTime = QDateTime::currentDateTime();
  qint64 durationSeconds = session.startTime.secsTo(endTime);

  if (durationSeconds > 0) {
    // 保存到数据库
    ProcessTimeLog timeLog;
    timeLog.processLinkId = session.processLinkId;
    timeLog.taskId = session.taskId;
    timeLog.userId = m_userId;
    timeLog.processName = session.processName;
    timeLog.startTime = session.startTime;
    timeLog.endTime = endTime;
    timeLog.durationSeconds = durationSeconds;
    timeLog.isAutoRecorded = true;

    if (DatabaseManager::instance().addProcessTimeLog(timeLog)) {
      // 更新任务实际时间
      updateTaskActualTime(session.taskId, durationSeconds);
      emit timeRecorded(session.taskId, durationSeconds, true);
      qDebug() << "Recorded" << durationSeconds << "seconds for task"
               << session.taskName;
    }
  }
}

QString ProcessMonitorService::getTaskName(int taskId) {
  Task task = DatabaseManager::instance().getTaskById(taskId, m_userId);
  return task.isValid() ? task.name : tr("未知任务");
}

void ProcessMonitorService::updateTaskActualTime(int taskId,
                                                 qint64 durationSeconds) {
  // 将秒转换为分钟（向上取整）
  int durationMinutes = (durationSeconds + 59) / 60;
  DatabaseManager::instance().incrementTaskActualTime(taskId, durationMinutes,
                                                      m_userId);
}
