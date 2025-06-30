#ifndef PROCESSMONITORSERVICE_H
#define PROCESSMONITORSERVICE_H

#include "DatabaseManager.h"
#include "ProcessLink.h"
#include "ProcessMonitor.h"
#include <QDateTime>
#include <QMap>
#include <QObject>
#include <QTimer>


/**
 * @brief 程序监控服务 - 整合程序监控和数据库记录
 *
 * 此服务负责：
 * 1. 监控已关联的程序运行状态
 * 2. 自动记录程序运行时间到数据库
 * 3. 更新任务的实际时间
 * 4. 发送时间记录通知
 */
class ProcessMonitorService : public QObject {
  Q_OBJECT

public:
  explicit ProcessMonitorService(int userId, QObject *parent = nullptr);

  // 开始/停止监控
  void startMonitoring();
  void stopMonitoring();

  // 设置当前用户
  void setUserId(int userId);

  // 刷新程序关联列表（从数据库重新加载）
  void refreshProcessLinks();

  // 获取活动状态
  bool isMonitoring() const { return m_isMonitoring; }

  // 获取当前监控的程序数量
  int getMonitoredProcessCount() const;

signals:
  // 程序开始运行（自动计时开始）
  void processStarted(const QString &processName, const QString &taskName);

  // 程序停止运行（自动计时结束）
  void processStopped(const QString &processName, const QString &taskName,
                      qint64 durationSeconds);

  // 时间记录已保存到数据库
  void timeRecorded(int taskId, qint64 durationSeconds, bool isAutoRecorded);

  // 监控状态变化
  void monitoringStatusChanged(bool isMonitoring);

private slots:
  void onProcessStarted(const QString &processName);
  void onProcessStopped(const QString &processName, qint64 sessionRuntime);
  void checkAndUpdateActiveProcesses();

private:
  struct ActiveProcessSession {
    int processLinkId;
    int taskId;
    QString processName;
    QString taskName;
    QDateTime startTime;
    bool autoRecordEnabled;
  };

  int m_userId;
  bool m_isMonitoring;

  ProcessMonitor *m_processMonitor;
  QTimer *m_refreshTimer;

  // 当前活动的程序会话
  QMap<QString, ActiveProcessSession> m_activeSessions;

  // 程序关联缓存
  QList<ProcessLink> m_processLinks;

  // 加载用户的程序关联
  void loadProcessLinks();

  // 开始程序会话记录
  void startProcessSession(const ProcessLink &link, const QString &taskName);

  // 结束程序会话记录
  void endProcessSession(const QString &processName);

  // 获取任务名称
  QString getTaskName(int taskId);

  // 更新任务实际时间
  void updateTaskActualTime(int taskId, qint64 durationSeconds);

  static const int REFRESH_INTERVAL =
      60000; // 60秒刷新一次程序关联（从30秒改为60秒）
};

#endif // PROCESSMONITORSERVICE_H
