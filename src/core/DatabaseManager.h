#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "Subject.h" // 包含学科结构体定义
#include "Task.h"    // 包含学科任务
#include "TimeLog.h" // 包含任务时间
#include "User.h"    // 包含用户结构体定义
#include <QList>
#include <QSqlDatabase>
#include <QString>
#include <QVariant> // For QVariantMap

class DatabaseManager {
public:
  static DatabaseManager &instance(); // 单例模式

  bool openDatabase(const QString &path = "data/learning_platform.db");
  void closeDatabase();

  // 用户管理
  bool initUserTable();
  bool addUser(const User &user); // 初始注册时使用
  User getUserByEmail(const QString &email);
  bool updateUser(const User &user);
  bool validateUser(const QString &email, const QString &password); // 登录验证

  // 学科管理
  // 这里的学科表是用户自定义的，可能会有多个学科
  bool initSubjectTable();
  bool addSubject(Subject &subject); // Pass by reference to get ID back
  Subject getSubjectById(int subjectId, int userId);
  QList<Subject> getSubjectsByUser(int userId);
  bool updateSubject(const Subject &subject);
  bool deleteSubject(int subjectId, int userId);
  bool subjectExists(const QString &name, int userId,
                     int excludeSubjectId = -1); // For validation

  // 学科任务 TM-002
  bool initTaskTable();
  bool addTask(Task &task); // Pass by reference to get ID back
  Task getTaskById(int taskId, int userId);
  QList<Task>
  getTasksBySubject(int subjectId, int userId,
                    TaskStatus filterStatus = TaskStatus(-1) /* -1 means all */,
                    bool includeCompleted = false);
  QList<Task>
  getTasksByUser(int userId, TaskStatus filterStatus = TaskStatus(-1),
                 bool includeCompleted = false); // For dashboard/all tasks view
  bool updateTask(const Task &task);
  bool deleteTask(int taskId, int userId);
  bool updateTaskStatus(int taskId, TaskStatus newStatus, int userId);
  bool incrementTaskActualTime(int taskId, int minutesToAdd,
                               int userId); // For TM-003

  // 任务时间记录 TimeLog Management (TM-003, TM-004)
  bool initTimeLogTable();
  bool addTimeLog(TimeLog &timeLog); // Pass by reference to get ID back
  QList<TimeLog> getTimeLogsByUser(int userId,
                                   const QDateTime &fromDate = QDateTime(),
                                   const QDateTime &toDate = QDateTime());
  QList<TimeLog> getTimeLogsByTask(int taskId, int userId);
  QList<TimeLog> getTimeLogsBySubject(int subjectId, int userId);

  // 注册功能
  bool registerUser(const QString &email, const QString &password,
                    const QString &nickname = "");
  bool isEmailExists(const QString &email) const;

  // QString hashPassword(const QString& password) const;

  // 将来添加其他模块的数据库操作
  // bool addSubject(...);
  // bool addTask(...);

private:
  DatabaseManager(); // 私有构造函数
  ~DatabaseManager();
  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager &operator=(const DatabaseManager &) = delete;

  QSqlDatabase m_db;

public:
  // 辅助函数
  QString hashPassword(const QString &password); // 应该使用安全的哈希算法
  bool verifyPassword(const QString &password, const QString &hash);
};

#endif // DATABASEMANAGER_H