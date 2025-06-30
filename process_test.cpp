#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <iostream>


#ifdef Q_OS_WIN
#include <tlhelp32.h>
#include <windows.h>

#endif

QStringList getCurrentProcesses() {
  QStringList processes;

#ifdef Q_OS_WIN
  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcessSnap == INVALID_HANDLE_VALUE) {
    qWarning() << "Failed to create process snapshot";
    return processes;
  }

  PROCESSENTRY32 pe32;
  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (Process32First(hProcessSnap, &pe32)) {
    do {
      QString processName = QString::fromWCharArray(pe32.szExeFile);
      processes.append(processName);
    } while (Process32Next(hProcessSnap, &pe32));
  }

  CloseHandle(hProcessSnap);
#endif

  return processes;
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  QStringList processes = getCurrentProcesses();

  qDebug() << "Found" << processes.size() << "processes total";

  // 过滤用户进程
  QStringList userProcesses;
  QStringList systemProcesses = {
      "System",           "Registry",     "smss.exe",     "csrss.exe",
      "wininit.exe",      "winlogon.exe", "services.exe", "lsass.exe",
      "svchost.exe",      "taskhost.exe", "dwm.exe",      "audiodg.exe",
      "RuntimeBroker.exe"};

  for (const QString &process : processes) {
    if (!systemProcesses.contains(process, Qt::CaseInsensitive) &&
        !process.toLower().contains("svchost") &&
        !process.toLower().contains("runtimebroker") && process.length() > 3) {
      userProcesses.append(process);
    }
  }

  userProcesses.removeDuplicates();
  userProcesses.sort(Qt::CaseInsensitive);

  qDebug() << "User processes:" << userProcesses.size();
  for (int i = 0; i < qMin(20, userProcesses.size()); ++i) {
    qDebug() << (i + 1) << ":" << userProcesses[i];
  }

  return 0;
}
