#include "../src/core/ProcessMonitor.h"
#include <QCoreApplication>
#include <QDebug>
#include <iostream>


int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  ProcessMonitor monitor;
  QStringList processes = monitor.getCurrentProcesses();

  qDebug() << "Found" << processes.size() << "processes:";

  // 显示前20个进程作为示例
  for (int i = 0; i < qMin(20, processes.size()); ++i) {
    qDebug() << (i + 1) << ":" << processes[i];
  }

  return 0;
}
