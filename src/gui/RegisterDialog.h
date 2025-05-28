#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "core/User.h" // Assuming User struct is here

QT_BEGIN_NAMESPACE
namespace Ui { class RegisterDialog; }
QT_END_NAMESPACE

class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

    QString getRegisteredEmail() const; // To allow LoginDialog to prefill email

private slots:
    void on_passwordLineEdit_textChanged(const QString &text);
    void on_buttonBox_accepted(); // Custom slot for OK/Register button logic

private:
    Ui::RegisterDialog *ui;
    QString m_registeredEmail; // Stores email upon successful registration

    // Helper methods
    bool validateInput(QString& errorMessages);
    int calculatePasswordStrength(const QString& password) const;
    void updatePasswordStrengthIndicator(const QString& password);
    void setupValidators();
};

#endif // REGISTERDIALOG_H