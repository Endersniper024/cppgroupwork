#include "LoginDialog.h"
#include "DatabaseManager.h"
#include "RegisterDialog.h"
#include "ui_LoginDialog.h" // 由 uic 生成
#include <QDebug>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog) {
  ui->setupUi(this);
  setWindowTitle("用户登录");
  ui->passwordLineEdit->setEchoMode(QLineEdit::Password); // 确保密码隐藏
  ui->statusLabel->setText("");                           // 清空状态标签
}

LoginDialog::~LoginDialog() { delete ui; }

QString LoginDialog::getEmail() const {
  return ui->emailLineEdit->text().trimmed();
}

QString LoginDialog::getPassword() const {
  return ui->passwordLineEdit->text();
}

bool LoginDialog::isRememberMeChecked() const {
  return ui->rememberMeCheckBox->isChecked();
}

void LoginDialog::on_loginButton_clicked() {
  QString email = getEmail();
  QString password = getPassword();

  if (email.isEmpty() || password.isEmpty()) {
    ui->statusLabel->setText("邮箱和密码不能为空！");
    // QMessageBox::warning(this, "登录失败", "邮箱和密码不能为空！");
    return;
  }

  if (DatabaseManager::instance().validateUser(email, password)) {
    qDebug() << "Login successful for" << email;
    // 如果有 "记住我" 逻辑，在这里处理
    // QSettings settings("MyCompany", "MyApp");
    // if (isRememberMeChecked()) {
    //     settings.setValue("lastUserEmail", email);
    //     // 警告: 不应直接存储密码。通常是存储一个长期令牌。
    // } else {
    //     settings.remove("lastUserEmail");
    // }
    emit loginSuccessful(email); // 发射成功信号
    accept();                    // 关闭对话框并返回 QDialog::Accepted
  } else {
    qDebug() << "Login failed for" << email;
    ui->statusLabel->setText("邮箱或密码错误！");
    // QMessageBox::warning(this, "登录失败", "邮箱或密码错误！");
  }
}

// 同时添加一个设置状态消息的辅助方法
void LoginDialog::setStatusMessage(const QString &message, bool isError) {
  if (isError) {
    ui->statusLabel->setStyleSheet("color: red;");
  } else {
    ui->statusLabel->setStyleSheet("color: green;");
  }
  ui->statusLabel->setText(message);
}
void LoginDialog::on_registerButton_clicked() {
  // UM-003 用户注册功能
  RegisterDialog registerDialog(this);

  if (registerDialog.exec() == QDialog::Accepted) {
    // 注册成功，自动填充邮箱到登录界面
    QString registeredEmail = registerDialog.getRegisteredEmail();
    if (!registeredEmail.isEmpty()) {
      setEmail(registeredEmail);
      ui->passwordLineEdit->setFocus(); // 焦点移到密码输入框
      setStatusMessage("注册成功！请输入密码登录。", false);
    }
  }
}

bool LoginDialog::setEmail(const QString &email) {
  if (email.isEmpty())
    return false;
  ui->emailLineEdit->setText(email);
  return true;
}

bool LoginDialog::setRememberMeChecked(bool checked) {
  ui->rememberMeCheckBox->setChecked(checked);
  return true;
}