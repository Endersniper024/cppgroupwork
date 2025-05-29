#include "DatabaseManager.h"

#include <QCoreApplication>
#include <QCryptographicHash> // 用于密码哈希 (示例)
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <Qstring>

DatabaseManager &DatabaseManager::instance() {
  static DatabaseManager dmInstance;
  return dmInstance;
}

DatabaseManager::DatabaseManager() {
  // 构造函数中可以进行 Qt 数据库驱动的检查
  if (QSqlDatabase::isDriverAvailable("QSQLITE")) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    qDebug() << "QSQLITE driver available.";
  } else {
    qWarning() << "QSQLITE driver not available!";
  }
}

DatabaseManager::~DatabaseManager() {
  if (m_db.isOpen()) {
    m_db.close();
  }
}

bool DatabaseManager::openDatabase(const QString &path) {
  // 确保data目录存在
  QDir dir;
  QString dataPath =
      QDir(QCoreApplication::applicationDirPath())
          .filePath("data"); // 尝试在可执行文件同级目录下创建data
  if (!dir.exists(dataPath)) {
    if (!dir.mkpath(dataPath)) {
      qWarning() << "Could not create data directory:" << dataPath;
      // 尝试在构建目录下创建
      dataPath = "data"; // 回退到 CMake 中指定的相对路径
      if (!dir.exists(dataPath))
        dir.mkpath(dataPath);
    }
  }
  QString dbFilePath = QDir(dataPath).filePath(QFileInfo(path).fileName());

  m_db.setDatabaseName(dbFilePath);
  if (!m_db.open()) {
    qWarning() << "Error: connection with database failed:"
               << m_db.lastError().text();
    return false;
  }
  qDebug() << "Database opened successfully at:" << dbFilePath;
  return initUserTable() && initSubjectTable() && initTaskTable() &&
         initTimeLogTable();
  // return initUserTable(); // 打开后立即初始化用户表
}

void DatabaseManager::closeDatabase() {
  if (m_db.isOpen()) {
    m_db.close();
  }
}

// 简化版的密码哈希 - 生产环境需要更安全的实现！
QString DatabaseManager::hashPassword(const QString &password) {
  // 警告: 这是一个非常基础的哈希示例，不应用于生产环境。
  // 生产中应使用如 Argon2, scrypt, bcrypt 并加盐。
  return QString(
      QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256)
          .toHex());
}

bool DatabaseManager::verifyPassword(const QString &password,
                                     const QString &hash) {
  return hashPassword(password) == hash;
}

bool DatabaseManager::initUserTable() {
  if (!m_db.isOpen()) {
    qWarning() << "Database not open. Cannot init user table.";
    return false;
  }
  QSqlQuery query(m_db);
  QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            nickname TEXT,
            avatar_path TEXT
        );
    )";
  if (!query.exec(createTableQuery)) {
    qWarning() << "Couldn't create the table 'users':"
               << query.lastError().text();
    return false;
  }
  qDebug() << "User table initialized or already exists.";

  // For testing: Add a default user if no users exist
  query.exec("SELECT COUNT(*) FROM users");
  if (query.next() && query.value(0).toInt() == 0) {
    User defaultUser;
    defaultUser.email = "test@example.com";
    defaultUser.passwordHash = hashPassword("password123"); // Hash the password
    defaultUser.nickname = "Test User";
    addUser(defaultUser); // Use the addUser function
    qDebug() << "Added default user: test@example.com / password123";
  }
  return true;
}

bool DatabaseManager::addUser(const User &user) {
  if (!m_db.isOpen())
    return false;
  QSqlQuery query(m_db);
  query.prepare(
      "INSERT INTO users (email, password_hash, nickname, avatar_path) "
      "VALUES (:email, :password_hash, :nickname, :avatar_path)");
  query.bindValue(":email", user.email);
  query.bindValue(":password_hash",
                  user.passwordHash); // 确保传入的是哈希后的密码
  query.bindValue(":nickname", user.nickname);
  query.bindValue(":avatar_path", user.avatarPath);

  if (!query.exec()) {
    qWarning() << "addUser failed:" << query.lastError().text();
    return false;
  }
  return true;
}

User DatabaseManager::getUserByEmail(const QString &email) {
  User user;
  if (!m_db.isOpen())
    return user; // 返回空 User 对象

  QSqlQuery query(m_db);
  query.prepare("SELECT id, email, password_hash, nickname, avatar_path FROM "
                "users WHERE email = :email");
  query.bindValue(":email", email);

  if (query.exec() && query.next()) {
    user.id = query.value("id").toInt();
    user.email = query.value("email").toString();
    user.passwordHash = query.value("password_hash").toString();
    user.nickname = query.value("nickname").toString();
    user.avatarPath = query.value("avatar_path").toString();
  } else if (!query.isActive()) {
    qWarning() << "getUserByEmail query failed:" << query.lastError().text();
  }
  return user;
}

bool DatabaseManager::updateUser(const User &user) {
  if (!m_db.isOpen() || user.id == -1)
    return false;

  QSqlQuery query(m_db);
  query.prepare(
      "UPDATE users SET email = :email, password_hash = :password_hash, "
      "nickname = :nickname, avatar_path = :avatar_path WHERE id = :id");
  query.bindValue(":email", user.email);
  query.bindValue(":password_hash", user.passwordHash);
  query.bindValue(":nickname", user.nickname);
  query.bindValue(":avatar_path", user.avatarPath);
  query.bindValue(":id", user.id);

  if (!query.exec()) {
    qWarning() << "updateUser failed:" << query.lastError().text();
    return false;
  }
  return query.numRowsAffected() > 0;
}

bool DatabaseManager::validateUser(const QString &email,
                                   const QString &password) {
  User user = getUserByEmail(email);
  if (!user.isValid()) {
    qDebug() << "User not found:" << email;
    return false; // 用户不存在
  }
  // 验证密码
  bool ok = verifyPassword(password, user.passwordHash);
  qDebug() << "Password verification for" << email
           << (ok ? "succeeded" : "failed");
  return ok;
}

// QString DatabaseManager::hashPassword(const QString& password) const {
//     return QString(QCryptographicHash::hash(password.toUtf8(),
//     QCryptographicHash::Sha256).toHex());
// }

// In DatabaseManager::openDatabase(), after initUserTable(), add:
// return initUserTable() && initSubjectTable(); // Modified line
// And ensure the definition in openDatabase:
// bool DatabaseManager::openDatabase(const QString& path) {
// ...
//     qDebug() << "Database opened successfully at:" << dbFilePath;
//     return initUserTable() && initSubjectTable(); // Ensure both tables are
//     initialized
// }

bool DatabaseManager::initSubjectTable() {
  if (!m_db.isOpen()) {
    qWarning() << "Database not open. Cannot init subject table.";
    return false;
  }
  QSqlQuery query(m_db);
  QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS subjects (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            name TEXT NOT NULL,
            description TEXT,
            color_hex TEXT,
            tags TEXT,
            FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE,
            UNIQUE (user_id, name) -- A user cannot have two subjects with the same name
        );
    )";
  if (!query.exec(createTableQuery)) {
    qWarning() << "Couldn't create the table 'subjects':"
               << query.lastError().text();
    return false;
  }
  qDebug() << "Subject table initialized or already exists.";
  return true;
}

bool DatabaseManager::addSubject(Subject &subject) {
  if (!m_db.isOpen() || subject.userId == -1)
    return false;

  if (subjectExists(subject.name, subject.userId)) {
    qWarning() << "Subject with name" << subject.name
               << "already exists for this user.";
    // Optionally set an error message here for the UI to pick up
    return false;
  }

  QSqlQuery query(m_db);
  query.prepare(
      "INSERT INTO subjects (user_id, name, description, color_hex, tags) "
      "VALUES (:user_id, :name, :description, :color_hex, :tags)");
  query.bindValue(":user_id", subject.userId);
  query.bindValue(":name", subject.name);
  query.bindValue(":description", subject.description);
  query.bindValue(":color_hex", subject.colorToHexString());
  query.bindValue(":tags", subject.tags);

  if (!query.exec()) {
    qWarning() << "addSubject failed:" << query.lastError().text();
    return false;
  }
  subject.id =
      query.lastInsertId().toInt(); // Get the ID of the newly inserted subject
  return true;
}

Subject DatabaseManager::getSubjectById(int subjectId, int userId) {
  Subject subject;
  if (!m_db.isOpen())
    return subject;

  QSqlQuery query(m_db);
  query.prepare(
      "SELECT id, user_id, name, description, color_hex, tags FROM subjects "
      "WHERE id = :id AND user_id = :user_id");
  query.bindValue(":id", subjectId);
  query.bindValue(":user_id", userId);

  if (query.exec() && query.next()) {
    subject.id = query.value("id").toInt();
    subject.userId = query.value("user_id").toInt();
    subject.name = query.value("name").toString();
    subject.description = query.value("description").toString();
    subject.setColorFromHexString(query.value("color_hex").toString());
    subject.tags = query.value("tags").toString();
  } else if (!query.isActive()) {
    qWarning() << "getSubjectById query failed:" << query.lastError().text();
  }
  return subject;
}

QList<Subject> DatabaseManager::getSubjectsByUser(int userId) {
  QList<Subject> subjects;
  if (!m_db.isOpen() || userId == -1)
    return subjects;

  QSqlQuery query(m_db);
  query.prepare(
      "SELECT id, user_id, name, description, color_hex, tags FROM subjects "
      "WHERE user_id = :user_id ORDER BY name ASC");
  query.bindValue(":user_id", userId);

  if (query.exec()) {
    while (query.next()) {
      Subject subject;
      subject.id = query.value("id").toInt();
      subject.userId = query.value("user_id").toInt();
      subject.name = query.value("name").toString();
      subject.description = query.value("description").toString();
      subject.setColorFromHexString(query.value("color_hex").toString());
      subject.tags = query.value("tags").toString();
      subjects.append(subject);
    }
  } else {
    qWarning() << "getSubjectsByUser query failed:" << query.lastError().text();
  }
  return subjects;
}

bool DatabaseManager::updateSubject(const Subject &subject) {
  if (!m_db.isOpen() || !subject.isValid())
    return false;

  if (subjectExists(subject.name, subject.userId, subject.id)) {
    qWarning() << "Another subject with name" << subject.name
               << "already exists for this user.";
    return false;
  }

  QSqlQuery query(m_db);
  query.prepare("UPDATE subjects SET name = :name, description = :description, "
                "color_hex = :color_hex, tags = :tags "
                "WHERE id = :id AND user_id = :user_id");
  query.bindValue(":name", subject.name);
  query.bindValue(":description", subject.description);
  query.bindValue(":color_hex", subject.colorToHexString());
  query.bindValue(":tags", subject.tags);
  query.bindValue(":id", subject.id);
  query.bindValue(":user_id", subject.userId); // Ensure user owns this subject

  if (!query.exec()) {
    qWarning() << "updateSubject failed:" << query.lastError().text();
    return false;
  }
  return query.numRowsAffected() > 0;
}

bool DatabaseManager::deleteSubject(int subjectId, int userId) {
  if (!m_db.isOpen())
    return false;

  QSqlQuery query(m_db);
  // Consider impact on tasks associated with this subject.
  // For now, simple delete. Later, might need to handle/delete associated tasks
  // or disassociate them. The ON DELETE CASCADE for tasks referencing subjects
  // (when task table is added) will be important.
  query.prepare("DELETE FROM subjects WHERE id = :id AND user_id = :user_id");
  query.bindValue(":id", subjectId);
  query.bindValue(":user_id", userId);

  if (!query.exec()) {
    qWarning() << "deleteSubject failed:" << query.lastError().text();
    return false;
  }
  bool success = query.numRowsAffected() > 0;
  if (success) {
    qDebug() << "Subject with ID" << subjectId << "deleted for user" << userId;
    // TODO LATER: When tasks are implemented, ensure they are also deleted if
    // they belonged to this subject (via FK ON DELETE CASCADE) Or handle them
    // in a different way (e.g., set task's subject_id to null if allowed, or
    // prompt user).
  }
  return success;
}

bool DatabaseManager::subjectExists(const QString &name, int userId,
                                    int excludeSubjectId) {
  if (!m_db.isOpen())
    return true; // Fail safe, assume it exists if DB is not open

  QSqlQuery query(m_db);
  QString queryString =
      "SELECT COUNT(*) FROM subjects WHERE name = :name AND user_id = :user_id";
  if (excludeSubjectId != -1) {
    queryString += " AND id != :exclude_id";
  }
  query.prepare(queryString);
  query.bindValue(":name", name);
  query.bindValue(":user_id", userId);
  if (excludeSubjectId != -1) {
    query.bindValue(":exclude_id", excludeSubjectId);
  }

  if (query.exec() && query.next()) {
    return query.value(0).toInt() > 0;
  }
  qWarning() << "subjectExists query failed:" << query.lastError().text();
  return true; // Fail safe
}

// IMPORTANT: Make sure to call initSubjectTable() when the database is opened.
// In DatabaseManager::openDatabase:
// modify the return line:
// return initUserTable() && initSubjectTable();

// 学科任务 TM-002

bool DatabaseManager::initTaskTable() {
  if (!m_db.isOpen()) {
    qWarning() << "Database not open. Cannot init task table.";
    return false;
  }
  QSqlQuery query(m_db);
  // Note: ON DELETE CASCADE for subject_id will delete tasks if their parent
  // subject is deleted.
  QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            subject_id INTEGER NOT NULL,
            name TEXT NOT NULL,
            description TEXT,
            priority INTEGER DEFAULT 1, -- 0:Low, 1:Medium, 2:High
            due_date TEXT,              -- ISO8601 format (YYYY-MM-DD HH:MM:SS)
            estimated_time_minutes INTEGER DEFAULT 0,
            actual_time_minutes INTEGER DEFAULT 0,
            status INTEGER DEFAULT 0,   -- 0:Pending, 1:InProgress, 2:Completed
            creation_date TEXT NOT NULL,
            completion_date TEXT,       -- ISO8601 format or NULL
            FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE,
            FOREIGN KEY (subject_id) REFERENCES subjects (id) ON DELETE CASCADE
        );
    )";
  if (!query.exec(createTableQuery)) {
    qWarning() << "Couldn't create the table 'tasks':"
               << query.lastError().text();
    return false;
  }
  qDebug() << "Task table initialized or already exists.";
  return true;
}

bool DatabaseManager::addTask(Task &task) {
  if (!m_db.isOpen() || task.userId == -1 || task.subjectId == -1)
    return false;

  QSqlQuery query(m_db);
  query.prepare("INSERT INTO tasks (user_id, subject_id, name, description, "
                "priority, due_date, "
                "estimated_time_minutes, actual_time_minutes, status, "
                "creation_date, completion_date) "
                "VALUES (:user_id, :subject_id, :name, :description, "
                ":priority, :due_date, "
                ":estimated_time_minutes, :actual_time_minutes, :status, "
                ":creation_date, :completion_date)");

  query.bindValue(":user_id", task.userId);
  query.bindValue(":subject_id", task.subjectId);
  query.bindValue(":name", task.name);
  query.bindValue(":description", task.description);
  query.bindValue(":priority", static_cast<int>(task.priority));
  query.bindValue(":due_date", task.dueDate.isValid()
                                   ? task.dueDate.toString(Qt::ISODate)
                                   : QVariant(QMetaType(QMetaType::QString)));
  query.bindValue(":estimated_time_minutes", task.estimatedTimeMinutes);
  query.bindValue(":actual_time_minutes", task.actualTimeMinutes);
  query.bindValue(":status", static_cast<int>(task.status));
  query.bindValue(":creation_date", task.creationDate.toString(Qt::ISODate));
  query.bindValue(":completion_date",
                  task.completionDate.isValid()
                      ? task.completionDate.toString(Qt::ISODate)
                      : QVariant(QMetaType(QMetaType::QString)));

  if (!query.exec()) {
    qWarning() << "addTask failed:" << query.lastError().text();
    return false;
  }
  task.id = query.lastInsertId().toInt();
  return true;
}

Task DatabaseManager::getTaskById(int taskId, int userId) {
  Task task;
  if (!m_db.isOpen())
    return task;

  QSqlQuery query(m_db);
  query.prepare(
      "SELECT id, user_id, subject_id, name, description, priority, due_date, "
      "estimated_time_minutes, actual_time_minutes, status, creation_date, "
      "completion_date "
      "FROM tasks WHERE id = :id AND user_id = :user_id");
  query.bindValue(":id", taskId);
  query.bindValue(":user_id", userId);

  if (query.exec() && query.next()) {
    task.id = query.value("id").toInt();
    task.userId = query.value("user_id").toInt();
    task.subjectId = query.value("subject_id").toInt();
    task.name = query.value("name").toString();
    task.description = query.value("description").toString();
    task.priority = static_cast<TaskPriority>(query.value("priority").toInt());
    task.dueDate =
        QDateTime::fromString(query.value("due_date").toString(), Qt::ISODate);
    task.estimatedTimeMinutes = query.value("estimated_time_minutes").toInt();
    task.actualTimeMinutes = query.value("actual_time_minutes").toInt();
    task.status = static_cast<TaskStatus>(query.value("status").toInt());
    task.creationDate = QDateTime::fromString(
        query.value("creation_date").toString(), Qt::ISODate);
    task.completionDate = QDateTime::fromString(
        query.value("completion_date").toString(), Qt::ISODate);
  } else if (!query.isActive()) {
    qWarning() << "getTaskById query failed:" << query.lastError().text();
  }
  return task;
}

QList<Task> DatabaseManager::getTasksBySubject(int subjectId, int userId,
                                               TaskStatus filterStatus,
                                               bool includeCompleted) {
  QList<Task> tasks;
  if (!m_db.isOpen() || userId == -1 || subjectId == -1)
    return tasks;

  QString queryString =
      "SELECT id, user_id, subject_id, name, description, priority, due_date, "
      "estimated_time_minutes, actual_time_minutes, status, creation_date, "
      "completion_date "
      "FROM tasks WHERE subject_id = :subject_id AND user_id = :user_id";

  if (static_cast<int>(filterStatus) !=
      -1) { // If a specific status filter is applied
    queryString += " AND status = :status_filter";
  } else if (!includeCompleted) { // If no specific status, and we don't want
                                  // completed
    queryString += QString(" AND status != %1")
                       .arg(static_cast<int>(TaskStatus::Completed));
  }
  queryString += " ORDER BY due_date ASC, priority DESC, name ASC";

  QSqlQuery query(m_db);
  query.prepare(queryString);
  query.bindValue(":subject_id", subjectId);
  query.bindValue(":user_id", userId);
  if (static_cast<int>(filterStatus) != -1) {
    query.bindValue(":status_filter", static_cast<int>(filterStatus));
  }

  if (query.exec()) {
    while (query.next()) {
      Task task;
      task.id = query.value("id").toInt();
      task.userId = query.value("user_id").toInt();
      task.subjectId = query.value("subject_id").toInt();
      task.name = query.value("name").toString();
      task.description = query.value("description").toString();
      task.priority =
          static_cast<TaskPriority>(query.value("priority").toInt());
      task.dueDate = QDateTime::fromString(query.value("due_date").toString(),
                                           Qt::ISODate);
      task.estimatedTimeMinutes = query.value("estimated_time_minutes").toInt();
      task.actualTimeMinutes = query.value("actual_time_minutes").toInt();
      task.status = static_cast<TaskStatus>(query.value("status").toInt());
      task.creationDate = QDateTime::fromString(
          query.value("creation_date").toString(), Qt::ISODate);
      task.completionDate = QDateTime::fromString(
          query.value("completion_date").toString(), Qt::ISODate);
      tasks.append(task);
    }
  } else {
    qWarning() << "getTasksBySubject query failed:" << query.lastError().text();
  }
  return tasks;
}

// Implementation for getTasksByUser can be similar, just without subject_id
// filter.
QList<Task> DatabaseManager::getTasksByUser(int userId, TaskStatus filterStatus,
                                            bool includeCompleted) {
  QList<Task> tasks;
  if (!m_db.isOpen() || userId == -1)
    return tasks;

  QString queryString =
      "SELECT id, user_id, subject_id, name, description, priority, due_date, "
      "estimated_time_minutes, actual_time_minutes, status, creation_date, "
      "completion_date "
      "FROM tasks WHERE user_id = :user_id";

  if (static_cast<int>(filterStatus) != -1) {
    queryString += " AND status = :status_filter";
  } else if (!includeCompleted) {
    queryString += QString(" AND status != %1")
                       .arg(static_cast<int>(TaskStatus::Completed));
  }
  queryString += " ORDER BY due_date ASC, priority DESC, name ASC";

  QSqlQuery query(m_db);
  query.prepare(queryString);
  query.bindValue(":user_id", userId);
  if (static_cast<int>(filterStatus) != -1) {
    query.bindValue(":status_filter", static_cast<int>(filterStatus));
  }

  if (query.exec()) {
    while (query.next()) {
      // ... (same data extraction logic as in getTasksBySubject)
      Task task;
      task.id = query.value("id").toInt();
      task.userId = query.value("user_id").toInt();
      task.subjectId = query.value("subject_id").toInt();
      task.name = query.value("name").toString();
      task.description = query.value("description").toString();
      task.priority =
          static_cast<TaskPriority>(query.value("priority").toInt());
      task.dueDate = QDateTime::fromString(query.value("due_date").toString(),
                                           Qt::ISODate);
      task.estimatedTimeMinutes = query.value("estimated_time_minutes").toInt();
      task.actualTimeMinutes = query.value("actual_time_minutes").toInt();
      task.status = static_cast<TaskStatus>(query.value("status").toInt());
      task.creationDate = QDateTime::fromString(
          query.value("creation_date").toString(), Qt::ISODate);
      task.completionDate = QDateTime::fromString(
          query.value("completion_date").toString(), Qt::ISODate);
      tasks.append(task);
    }
  } else {
    qWarning() << "getTasksByUser query failed:" << query.lastError().text();
  }
  return tasks;
}

bool DatabaseManager::updateTask(const Task &task) {
  if (!m_db.isOpen() || !task.isValid())
    return false;

  QSqlQuery query(m_db);
  query.prepare("UPDATE tasks SET subject_id = :subject_id, name = :name, "
                "description = :description, "
                "priority = :priority, due_date = :due_date, "
                "estimated_time_minutes = :estimated_time_minutes, "
                "actual_time_minutes = :actual_time_minutes, status = :status, "
                "completion_date = :completion_date "
                "WHERE id = :id AND user_id = :user_id");

  query.bindValue(":subject_id", task.subjectId);
  query.bindValue(":name", task.name);
  query.bindValue(":description", task.description);
  query.bindValue(":priority", static_cast<int>(task.priority));
  query.bindValue(":due_date", task.dueDate.isValid()
                                   ? task.dueDate.toString(Qt::ISODate)
                                   : QVariant(QVariant::String));
  query.bindValue(":estimated_time_minutes", task.estimatedTimeMinutes);
  query.bindValue(":actual_time_minutes", task.actualTimeMinutes);
  query.bindValue(":status", static_cast<int>(task.status));
  query.bindValue(
      ":completion_date",
      (task.status == TaskStatus::Completed && task.completionDate.isValid())
          ? task.completionDate.toString(Qt::ISODate)
          : QVariant(QMetaType(QMetaType::QString)));
  query.bindValue(":id", task.id);
  query.bindValue(":user_id", task.userId);

  if (!query.exec()) {
    qWarning() << "updateTask failed:" << query.lastError().text();
    return false;
  }
  return query.numRowsAffected() > 0;
}

bool DatabaseManager::deleteTask(int taskId, int userId) {
  if (!m_db.isOpen())
    return false;

  QSqlQuery query(m_db);
  query.prepare("DELETE FROM tasks WHERE id = :id AND user_id = :user_id");
  query.bindValue(":id", taskId);
  query.bindValue(":user_id", userId);

  if (!query.exec()) {
    qWarning() << "deleteTask failed:" << query.lastError().text();
    return false;
  }
  return query.numRowsAffected() > 0;
}

bool DatabaseManager::updateTaskStatus(int taskId, TaskStatus newStatus,
                                       int userId) {
  if (!m_db.isOpen())
    return false;

  QSqlQuery query(m_db);
  QString queryString = "UPDATE tasks SET status = :status";
  if (newStatus == TaskStatus::Completed) {
    queryString += ", completion_date = :completion_date";
  } else {
    // If moving away from completed, clear completion date
    queryString += ", completion_date = NULL";
  }
  queryString += " WHERE id = :id AND user_id = :user_id";

  query.prepare(queryString);
  query.bindValue(":status", static_cast<int>(newStatus));
  if (newStatus == TaskStatus::Completed) {
    query.bindValue(":completion_date",
                    QDateTime::currentDateTime().toString(Qt::ISODate));
  }
  query.bindValue(":id", taskId);
  query.bindValue(":user_id", userId);

  if (!query.exec()) {
    qWarning() << "updateTaskStatus failed:" << query.lastError().text();
    return false;
  }
  return query.numRowsAffected() > 0;
}

bool DatabaseManager::incrementTaskActualTime(int taskId, int minutesToAdd,
                                              int userId) {
  if (!m_db.isOpen())
    return false;

  QSqlQuery query(m_db);
  query.prepare("UPDATE tasks SET actual_time_minutes = actual_time_minutes + "
                ":minutes_to_add "
                "WHERE id = :id AND user_id = :user_id");
  query.bindValue(":minutes_to_add", minutesToAdd);
  query.bindValue(":id", taskId);
  query.bindValue(":user_id", userId);

  if (!query.exec()) {
    qWarning() << "incrementTaskActualTime failed:" << query.lastError().text();
    return false;
  }
  return query.numRowsAffected() > 0;
}

// IMPORTANT: Make sure to call initTaskTable() when the database is opened.
// In DatabaseManager::openDatabase, ensure the return line is:
// return initUserTable() && initSubjectTable() && initTaskTable();

bool DatabaseManager::initTimeLogTable() {
  if (!m_db.isOpen()) {
    qWarning() << "Database not open. Cannot init timelog table.";
    return false;
  }
  QSqlQuery query(m_db);
  QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS timelogs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            task_id INTEGER,          -- Nullable
            subject_id INTEGER,       -- Nullable
            start_time TEXT NOT NULL, -- ISO8601
            end_time TEXT NOT NULL,   -- ISO8601
            duration_minutes INTEGER NOT NULL,
            notes TEXT,
            is_pomodoro_session INTEGER DEFAULT 0, -- 0 for false, 1 for true
            cycle_type INTEGER DEFAULT 0,          -- Corresponds to PomodoroCycleType enum
            FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE,
            FOREIGN KEY (task_id) REFERENCES tasks (id) ON DELETE SET NULL, -- If task deleted, keep log but unlink
            FOREIGN KEY (subject_id) REFERENCES subjects (id) ON DELETE SET NULL -- If subject deleted, keep log but unlink
        );
    )";
  // Using ON DELETE SET NULL for task_id and subject_id means if a task/subject
  // is deleted, the time log entry remains but is no longer associated with
  // that specific task/subject. This preserves the historical time logged, even
  // if the item it was logged against is removed. If you want to delete time
  // logs when tasks/subjects are deleted, use ON DELETE CASCADE.

  if (!query.exec(createTableQuery)) {
    qWarning() << "Couldn't create the table 'timelogs':"
               << query.lastError().text();
    return false;
  }
  qDebug() << "TimeLog table initialized or already exists.";
  return true;
}

bool DatabaseManager::addTimeLog(TimeLog &timeLog) {
  if (!m_db.isOpen() || timeLog.userId == -1)
    return false;

  QSqlQuery query(m_db);
  query.prepare(
      "INSERT INTO timelogs (user_id, task_id, subject_id, start_time, "
      "end_time, "
      "duration_minutes, notes, is_pomodoro_session, cycle_type) "
      "VALUES (:user_id, :task_id, :subject_id, :start_time, :end_time, "
      ":duration_minutes, :notes, :is_pomodoro_session, :cycle_type)");

  query.bindValue(":user_id", timeLog.userId);
  query.bindValue(":task_id", timeLog.taskId == -1
                                  ? QVariant(QVariant::Int)
                                  : timeLog.taskId); // Handle -1 as NULL
  query.bindValue(":subject_id", timeLog.subjectId == -1
                                     ? QVariant(QVariant::Int)
                                     : timeLog.subjectId); // Handle -1 as NULL
  query.bindValue(":start_time", timeLog.startTime.toString(Qt::ISODate));
  query.bindValue(":end_time", timeLog.endTime.toString(Qt::ISODate));
  query.bindValue(":duration_minutes", timeLog.durationMinutes);
  query.bindValue(":notes", timeLog.notes);
  query.bindValue(":is_pomodoro_session",
                  static_cast<int>(timeLog.isPomodoroSession));
  query.bindValue(":cycle_type", static_cast<int>(timeLog.cycleType));

  if (!query.exec()) {
    qWarning() << "addTimeLog failed:" << query.lastError().text();
    return false;
  }
  timeLog.id = query.lastInsertId().toInt();
  return true;
}

// 任务时间记录 TM-003 TM-004
// Implementations for getTimeLogsByUser, getTimeLogsByTask,
// getTimeLogsBySubject (for TM-004) These will be similar to other get...
// methods, selecting from 'timelogs' table. For now, let's add a basic
// getTimeLogsByUser for potential immediate use or testing.

QList<TimeLog> DatabaseManager::getTimeLogsByUser(int userId,
                                                  const QDateTime &fromDate,
                                                  const QDateTime &toDate) {
  QList<TimeLog> logs;
  if (!m_db.isOpen() || userId == -1)
    return logs;

  QString queryString =
      "SELECT id, user_id, task_id, subject_id, start_time, end_time, "
      "duration_minutes, notes, is_pomodoro_session, cycle_type "
      "FROM timelogs WHERE user_id = :user_id";
  if (fromDate.isValid()) {
    queryString += " AND end_time >= :from_date";
  }
  if (toDate.isValid()) {
    queryString += " AND start_time <= :to_date";
  }
  queryString += " ORDER BY start_time DESC";

  QSqlQuery query(m_db);
  query.prepare(queryString);
  query.bindValue(":user_id", userId);
  if (fromDate.isValid()) {
    query.bindValue(":from_date", fromDate.toString(Qt::ISODate));
  }
  if (toDate.isValid()) {
    // For "to date", we usually want to include the whole day, so adjust if
    // only date is given
    QDateTime effectiveToDate = toDate;
    if (toDate.time() == QTime(0, 0, 0)) { // If it's just a date (midnight)
      effectiveToDate = QDateTime(toDate.date(), QTime(23, 59, 59));
    }
    query.bindValue(":to_date", effectiveToDate.toString(Qt::ISODate));
  }

  if (query.exec()) {
    while (query.next()) {
      TimeLog log;
      log.id = query.value("id").toInt();
      log.userId = query.value("user_id").toInt();
      log.taskId =
          query.value("task_id").isNull() ? -1 : query.value("task_id").toInt();
      log.subjectId = query.value("subject_id").isNull()
                          ? -1
                          : query.value("subject_id").toInt();
      log.startTime = QDateTime::fromString(
          query.value("start_time").toString(), Qt::ISODate);
      log.endTime = QDateTime::fromString(query.value("end_time").toString(),
                                          Qt::ISODate);
      log.durationMinutes = query.value("duration_minutes").toInt();
      log.notes = query.value("notes").toString();
      log.isPomodoroSession = query.value("is_pomodoro_session").toBool();
      log.cycleType =
          static_cast<PomodoroCycleType>(query.value("cycle_type").toInt());
      logs.append(log);
    }
  } else {
    qWarning() << "getTimeLogsByUser query failed:" << query.lastError().text();
  }
  return logs;
}

// Minimal stubs for other getTimeLogs... for TM-004, to be fully implemented
// later
QList<TimeLog> DatabaseManager::getTimeLogsByTask(int taskId, int userId) {
  QList<TimeLog> logs; // TODO: Implement for TM-004
  Q_UNUSED(taskId);
  Q_UNUSED(userId);
  return logs;
}
QList<TimeLog> DatabaseManager::getTimeLogsBySubject(int subjectId,
                                                     int userId) {
  QList<TimeLog> logs; // TODO: Implement for TM-004
  Q_UNUSED(subjectId);
  Q_UNUSED(userId);
  return logs;
}

// IMPORTANT: Call initTimeLogTable() in DatabaseManager::openDatabase()

// 注册功能实现
bool DatabaseManager::registerUser(const QString &email,
                                   const QString &password,
                                   const QString &nickname) {
  qDebug() << "Attempting to register user:" << email;

  // 检查数据库连接是否有效
  if (!m_db.isOpen()) {
    qDebug() << "Database is not open!";
    return false;
  }

  // 检查邮箱是否已存在
  if (isEmailExists(email)) {
    qDebug() << "Email already exists:" << email;
    return false;
  }

  // 修改SQL语句，匹配实际的表结构
  QSqlQuery query(m_db); // 使用数据库连接
  query.prepare("INSERT INTO users (email, password_hash, nickname, "
                "avatar_path) VALUES (?, ?, ?, ?)");
  query.addBindValue(email);
  query.addBindValue(hashPassword(password));
  query.addBindValue(nickname.isEmpty() ? email.split("@").first() : nickname);
  query.addBindValue(""); // avatar_path 设为空字符串

  if (query.exec()) {
    qDebug() << "User registered successfully:" << email;
    return true;
  } else {
    qDebug() << "Failed to register user:" << email
             << "Error:" << query.lastError().text();
    qDebug() << "SQL Query:" << query.lastQuery();
    return false;
  }
}

bool DatabaseManager::isEmailExists(const QString &email) const {
  if (!m_db.isOpen()) {
    qDebug() << "Database is not open for email check!";
    return false;
  }

  QSqlQuery query(m_db); // 使用数据库连接
  query.prepare("SELECT COUNT(*) FROM users WHERE email = ?");
  query.addBindValue(email);

  if (query.exec() && query.next()) {
    int count = query.value(0).toInt();
    qDebug() << "Email check for" << email << "found" << count << "records";
    return count > 0;
  } else {
    qDebug() << "Email check query failed:" << query.lastError().text();
    return false;
  }
}