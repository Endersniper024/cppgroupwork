#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>


QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class RegisterDialog : public QDialog {
  Q_OBJECT

public:
  explicit RegisterDialog(QWidget *parent = nullptr);
  ~RegisterDialog();

  // 获取注册成功的用户邮箱
  QString getRegisteredEmail() const;

private slots:
  void onRegisterButtonClicked();
  void onCancelButtonClicked();
  void onPasswordChanged();
  void validateInput();

private:
  // UI 组件
  QLineEdit *m_emailLineEdit;
  QLineEdit *m_nicknameLineEdit;
  QLineEdit *m_passwordLineEdit;
  QLineEdit *m_confirmPasswordLineEdit;
  QLabel *m_statusLabel;
  QLabel *m_passwordStrengthLabel;
  QPushButton *m_registerButton;
  QPushButton *m_cancelButton;
  QCheckBox *m_agreeTermsCheckBox;

  // 数据
  QString m_registeredEmail;

  // 方法
  void setupUI();
  void connectSignals();
  bool validateInputFields();
  bool isValidEmail(const QString &email) const;
  QString checkPasswordStrength(const QString &password) const;
  void showPasswordStrength(const QString &password);
  void setStatusMessage(const QString &message, bool isError = true);
  bool registerUser(const QString &email, const QString &password,
                    const QString &nickname = "");
};
#endif // REGISTERDIALOG_H
