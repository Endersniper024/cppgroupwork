#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

// 前向声明，避免包含整个头文件
QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getEmail() const;
    QString getPassword() const;
    bool isRememberMeChecked() const;
    bool setEmail(const QString& email) ;
    bool setRememberMeChecked(bool checked) ;


signals:
    void loginSuccessful(const QString& userEmail); // 登录成功信号

private slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked(); // 可选的注册按钮槽

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H