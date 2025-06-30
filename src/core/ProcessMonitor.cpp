#include "ProcessMonitor.h"
#include <QDebug>

#ifdef Q_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#endif

ProcessMonitor::ProcessMonitor(QObject *parent)
    : QObject(parent), m_checkTimer(new QTimer(this)) {
  m_checkTimer->setInterval(CHECK_INTERVAL);
  connect(m_checkTimer, &QTimer::timeout, this,
          &ProcessMonitor::checkProcesses);
}

void ProcessMonitor::addMonitoredProcess(const QString &processName,
                                         const QString &displayName) {
  ProcessInfo info;
  info.processName = processName;
  info.displayName = displayName.isEmpty() ? processName : displayName;
  info.startTime = QDateTime::currentDateTime();
  info.totalRuntime = 0;
  info.isRunning = false;

  m_monitoredProcesses[processName] = info;

  qDebug() << "Added process to monitor:" << processName << "("
           << info.displayName << ")";
}

void ProcessMonitor::removeMonitoredProcess(const QString &processName) {
  if (m_monitoredProcesses.contains(processName)) {
    m_monitoredProcesses.remove(processName);
    qDebug() << "Removed process from monitor:" << processName;
  }
}

QStringList ProcessMonitor::getMonitoredProcesses() const {
  return m_monitoredProcesses.keys();
}

ProcessMonitor::ProcessInfo
ProcessMonitor::getProcessInfo(const QString &processName) const {
  return m_monitoredProcesses.value(processName);
}

qint64 ProcessMonitor::getProcessRuntime(const QString &processName) const {
  return m_monitoredProcesses.value(processName).totalRuntime;
}

bool ProcessMonitor::isProcessRunning(const QString &processName) const {
  return m_monitoredProcesses.value(processName).isRunning;
}

void ProcessMonitor::startMonitoring() {
  qDebug() << "Starting process monitoring...";
  m_checkTimer->start();
}

void ProcessMonitor::stopMonitoring() {
  qDebug() << "Stopping process monitoring...";
  m_checkTimer->stop();
}

void ProcessMonitor::resetProcessStats(const QString &processName) {
  if (m_monitoredProcesses.contains(processName)) {
    m_monitoredProcesses[processName].totalRuntime = 0;
    m_monitoredProcesses[processName].startTime = QDateTime::currentDateTime();
    qDebug() << "Reset stats for process:" << processName;
  }
}

void ProcessMonitor::checkProcesses() {
  QStringList currentProcesses = getCurrentProcesses();

  for (auto it = m_monitoredProcesses.begin(); it != m_monitoredProcesses.end();
       ++it) {
    const QString &processName = it.key();
    bool isCurrentlyRunning =
        currentProcesses.contains(processName, Qt::CaseInsensitive);

    updateProcessStatus(processName, isCurrentlyRunning);
  }
}

QStringList ProcessMonitor::getCurrentProcesses() {
  QStringList processes;

#ifdef Q_OS_WIN
  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcessSnap == INVALID_HANDLE_VALUE) {
    qWarning() << "Failed to create process snapshot";
    return processes;
  }

  PROCESSENTRY32W pe32;
  pe32.dwSize = sizeof(PROCESSENTRY32W);

  if (Process32FirstW(hProcessSnap, &pe32)) {
    do {
      QString processName = QString::fromWCharArray(pe32.szExeFile);
      processes.append(processName);
    } while (Process32NextW(hProcessSnap, &pe32));
  }

  CloseHandle(hProcessSnap);
#else
  // Linux/macOS 实现
  QProcess process;
  process.start("ps", QStringList() << "-eo" << "comm");
  process.waitForFinished();

  QString output = process.readAllStandardOutput();
  QStringList lines = output.split('\n', Qt::SkipEmptyParts);

  for (const QString &line : lines) {
    QString processName = line.trimmed();
    if (!processName.isEmpty() && processName != "COMMAND") {
      processes.append(processName);
    }
  }
#endif

  return processes;
}

bool ProcessMonitor::checkProcessExists(const QString &processName) {
  QStringList processes = getCurrentProcesses();
  return processes.contains(processName, Qt::CaseInsensitive);
}

void ProcessMonitor::updateProcessStatus(const QString &processName,
                                         bool isRunning) {
  if (!m_monitoredProcesses.contains(processName)) {
    return;
  }

  ProcessInfo &info = m_monitoredProcesses[processName];
  bool wasRunning = info.isRunning;

  if (isRunning && !wasRunning) {
    // 程序刚开始运行
    info.isRunning = true;
    info.startTime = QDateTime::currentDateTime();
    emit processStarted(processName);
    qDebug() << "Process started:" << processName;

  } else if (!isRunning && wasRunning) {
    // 程序刚停止运行
    info.isRunning = false;
    qint64 sessionRuntime =
        info.startTime.msecsTo(QDateTime::currentDateTime());
    info.totalRuntime += sessionRuntime;

    emit processStopped(processName, sessionRuntime);
    emit runtimeUpdated(processName, info.totalRuntime);
    qDebug() << "Process stopped:" << processName
             << "Session runtime:" << sessionRuntime << "ms";

  } else if (isRunning && wasRunning) {
    // 程序持续运行，更新总时间
    qint64 currentSessionTime =
        info.startTime.msecsTo(QDateTime::currentDateTime());
    qint64 currentTotalTime = info.totalRuntime + currentSessionTime;

    emit runtimeUpdated(processName, currentTotalTime);
  }
}
