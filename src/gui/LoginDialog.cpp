#include "LoginDialog.h"
#include "ui_LoginDialog.h" // 由 uic 生成
#include "DatabaseManager.h"
#include "RegisterDialog.h"

#include <QMessageBox>
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    setWindowTitle("用户登录");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password); // 确保密码隐藏
    ui->statusLabel->setText(""); // 清空状态标签
}

LoginDialog::~LoginDialog() {
    delete ui;
}

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
        accept(); // 关闭对话框并返回 QDialog::Accepted
    } else {
        qDebug() << "Login failed for" << email;
        ui->statusLabel->setText("邮箱或密码错误！");
        // QMessageBox::warning(this, "登录失败", "邮箱或密码错误！");
    }
}

void LoginDialog::on_registerButton_clicked() {
    // UM-003 用户注册功能，这里暂时留空
    // 可以弹出一个新的注册对话框
    // QMessageBox::information(this, "注册", "注册功能尚未实现。");
    RegisterDialog regDialog(this);
    if (regDialog.exec() == QDialog::Accepted) {
        // Registration was successful
        ui->emailLineEdit->setText(regDialog.getRegisteredEmail()); // Pre-fill email
        ui->passwordLineEdit->clear();                              // Clear password field
        ui->statusLabel->setText(tr("注册成功！请输入密码登录。"));
        ui->statusLabel->setStyleSheet("color: green;"); // Optional: success color
    }
    // If rejected, do nothing, LoginDialog remains.
}

bool LoginDialog::setEmail(const QString& email) {
    if (email.isEmpty()) return false;
    ui->emailLineEdit->setText(email);
    return true;
}

bool LoginDialog::setRememberMeChecked(bool checked) {
    ui->rememberMeCheckBox->setChecked(checked);
    return true;
}