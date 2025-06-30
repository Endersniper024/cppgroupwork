#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QDateTime>
#include <QMap>
#include <QObject>
#include <QProcess>
#include <QSet>
#include <QString>
#include <QTimer>


/**
 * @brief 程序监控器 - 监控特定程序的运行时间
 *
 * 004-关联程序功能：把任务和某个电脑程序关联起来，直接监控程序运行来监控时间
 */
class ProcessMonitor : public QObject {
  Q_OBJECT

public:
  explicit ProcessMonitor(QObject *parent = nullptr);

  struct ProcessInfo {
    QString processName; // 进程名称
    QString displayName; // 显示名称
    QDateTime startTime; // 开始监控时间
    qint64 totalRuntime; // 总运行时间（毫秒）
    bool isRunning;      // 是否正在运行
  };

  // 添加要监控的程序
  void addMonitoredProcess(const QString &processName,
                           const QString &displayName = "");

  // 移除监控的程序
  void removeMonitoredProcess(const QString &processName);

  // 获取监控的程序列表
  QStringList getMonitoredProcesses() const;

  // 获取程序信息
  ProcessInfo getProcessInfo(const QString &processName) const;

  // 获取程序运行时间（毫秒）
  qint64 getProcessRuntime(const QString &processName) const;

  // 检查程序是否正在运行
  bool isProcessRunning(const QString &processName) const;

  // 开始/停止监控
  void startMonitoring();
  void stopMonitoring();

  // 重置程序统计
  void resetProcessStats(const QString &processName);

  // 获取当前运行的进程列表（用于测试和UI显示）
  QStringList getCurrentProcesses();

  // 检查特定进程是否运行
  bool checkProcessExists(const QString &processName);

signals:
  // 程序开始运行
  void processStarted(const QString &processName);

  // 程序停止运行
  void processStopped(const QString &processName, qint64 sessionRuntime);

  // 运行时间更新
  void runtimeUpdated(const QString &processName, qint64 totalRuntime);

private slots:
  void checkProcesses();

private:
  QTimer *m_checkTimer;
  QMap<QString, ProcessInfo> m_monitoredProcesses;

  // 更新进程状态
  void updateProcessStatus(const QString &processName, bool isRunning);

  static const int CHECK_INTERVAL =
      3000; // 检查间隔（毫秒）- 从1秒改为3秒以减少卡顿
};

#endif // PROCESSMONITOR_H
