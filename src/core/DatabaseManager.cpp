#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QCryptographicHash> // 用于密码哈希 (示例)

DatabaseManager& DatabaseManager::instance() {
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

bool DatabaseManager::openDatabase(const QString& path) {
    // 确保data目录存在
    QDir dir;
    QString dataPath = QDir(QCoreApplication::applicationDirPath()).filePath("data"); // 尝试在可执行文件同级目录下创建data
    if (!dir.exists(dataPath)) {
        if (!dir.mkpath(dataPath)) {
             qWarning() << "Could not create data directory:" << dataPath;
             // 尝试在构建目录下创建
             dataPath = "data"; // 回退到 CMake 中指定的相对路径
             if(!dir.exists(dataPath)) dir.mkpath(dataPath);
        }
    }
    QString dbFilePath = QDir(dataPath).filePath(QFileInfo(path).fileName());


    m_db.setDatabaseName(dbFilePath);
    if (!m_db.open()) {
        qWarning() << "Error: connection with database failed:" << m_db.lastError().text();
        return false;
    }
    qDebug() << "Database opened successfully at:" << dbFilePath;
    return initUserTable(); // 打开后立即初始化用户表
}

void DatabaseManager::closeDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

// 简化版的密码哈希 - 生产环境需要更安全的实现！
QString DatabaseManager::hashPassword(const QString& password) {
    // 警告: 这是一个非常基础的哈希示例，不应用于生产环境。
    // 生产中应使用如 Argon2, scrypt, bcrypt 并加盐。
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool DatabaseManager::verifyPassword(const QString& password, const QString& hash) {
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
        qWarning() << "Couldn't create the table 'users':" << query.lastError().text();
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

bool DatabaseManager::addUser(const User& user) {
    if (!m_db.isOpen()) return false;
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (email, password_hash, nickname, avatar_path) "
                  "VALUES (:email, :password_hash, :nickname, :avatar_path)");
    query.bindValue(":email", user.email);
    query.bindValue(":password_hash", user.passwordHash); // 确保传入的是哈希后的密码
    query.bindValue(":nickname", user.nickname);
    query.bindValue(":avatar_path", user.avatarPath);

    if (!query.exec()) {
        qWarning() << "addUser failed:" << query.lastError().text();
        return false;
    }
    return true;
}

User DatabaseManager::getUserByEmail(const QString& email) {
    User user;
    if (!m_db.isOpen()) return user; // 返回空 User 对象

    QSqlQuery query(m_db);
    query.prepare("SELECT id, email, password_hash, nickname, avatar_path FROM users WHERE email = :email");
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

bool DatabaseManager::updateUser(const User& user) {
    if (!m_db.isOpen() || user.id == -1) return false;

    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET email = :email, password_hash = :password_hash, "
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


bool DatabaseManager::validateUser(const QString& email, const QString& password) {
    User user = getUserByEmail(email);
    if (!user.isValid()) {
        qDebug() << "User not found:" << email;
        return false; // 用户不存在
    }
    // 验证密码
    bool ok = verifyPassword(password, user.passwordHash);
    qDebug() << "Password verification for" << email << (ok ? "succeeded" : "failed");
    return ok;
}