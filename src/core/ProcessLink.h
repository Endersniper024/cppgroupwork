#ifndef PROCESSLINK_H
#define PROCESSLINK_H

#include <QDateTime>
#include <QString>


struct ProcessLink {
  int id = -1;
  int taskId = -1;
  int userId = -1;
  QString processName;
  QString displayName;
  bool autoStart = false;
  bool autoStop = false;
  QDateTime creationDate;
  bool isActive = true;

  bool isValid() const {
    return id != -1 && taskId != -1 && userId != -1 && !processName.isEmpty();
  }
};

// 程序时间记录结构体
struct ProcessTimeLog {
  int id = -1;
  int processLinkId = -1;
  int taskId = -1;
  int userId = -1;
  QString processName;
  QDateTime startTime;
  QDateTime endTime;
  qint64 durationSeconds = 0;
  bool isAutoRecorded = true; // 是否为自动记录

  bool isValid() const {
    return id != -1 && processLinkId != -1 && taskId != -1 && userId != -1;
  }
};

#endif // PROCESSLINK_H
