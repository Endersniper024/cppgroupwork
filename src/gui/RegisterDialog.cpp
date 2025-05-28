#include "RegisterDialog.h"
#include "ui_RegisterDialog.h" // Generated from RegisterDialog.ui
#include "core/DatabaseManager.h"
#include "core/User.h"

#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QDebug>
#include <QDateTime>
#include <QPushButton> // For changing button text

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("创建新账户"));

    setupValidators();

    connect(ui->passwordLineEdit, &QLineEdit::textChanged,
            this, &RegisterDialog::on_passwordLineEdit_textChanged);

    // Crucial: Disconnect default accept, connect to our custom slot for validation first.
    // This gives us control over whether the dialog actually closes.
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &RegisterDialog::on_buttonBox_accepted);
    // The rejected() signal can keep its default connection to QDialog::reject().

    // Change "OK" button text to "Register"
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (okButton) {
        okButton->setText(tr("注册"));
    }
    
    // Initialize password strength indicator
    updatePasswordStrengthIndicator("");
    ui->statusLabel->clear();
}

RegisterDialog::~RegisterDialog() {
    delete ui;
}

QString RegisterDialog::getRegisteredEmail() const {
    return m_registeredEmail;
}

void RegisterDialog::setupValidators() {
    // Basic email format validation (more comprehensive regex can be used)
    // This regex is a common simple one: allows most valid emails but not all edge cases.
    // For truly robust validation, often a library or more complex regex is used.
    QRegularExpression emailRegex(R"(\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}\b)", QRegularExpression::CaseInsensitiveOption);
    ui->emailLineEdit->setValidator(new QRegularExpressionValidator(emailRegex, this));
}

void RegisterDialog::on_passwordLineEdit_textChanged(const QString &text) {
    updatePasswordStrengthIndicator(text);
}

int RegisterDialog::calculatePasswordStrength(const QString& password) const {
    int score = 0;
    if (password.isEmpty()) return 0;

    // 1. Length (max 2 points for length)
    if (password.length() >= 8) score++;
    if (password.length() >= 12) score++;

    // 2. Character types (1 point each, max 4 points for variety)
    bool hasLower = password.contains(QRegularExpression("[a-z]"));
    bool hasUpper = password.contains(QRegularExpression("[A-Z]"));
    bool hasDigit = password.contains(QRegularExpression("[0-9]"));
    bool hasSpecial = password.contains(QRegularExpression("[^a-zA-Z0-9]")); // Non-alphanumeric

    if (hasLower) score++;
    if (hasUpper) score++;
    if (hasDigit) score++;
    if (hasSpecial) score++;
    
    // Max possible score for this logic: 2 (length) + 4 (char types) = 6
    return score;
}

void RegisterDialog::updatePasswordStrengthIndicator(const QString& password) {
    int score = calculatePasswordStrength(password);
    int maxScore = 6; // Based on current calculatePasswordStrength logic

    ui->passwordStrengthBar->setValue(static_cast<int>((static_cast<double>(score) / maxScore) * 100.0));

    if (password.isEmpty()) {
        ui->passwordStrengthTextLabel->setText("");
        ui->passwordStrengthBar->setValue(0);
    } else if (score < 2) {
        ui->passwordStrengthTextLabel->setText(tr("太短"));
        ui->passwordStrengthTextLabel->setStyleSheet("color: red;");
    } else if (score < 4) {
        ui->passwordStrengthTextLabel->setText(tr("弱"));
        ui->passwordStrengthTextLabel->setStyleSheet("color: orange;");
    } else if (score < 6) {
        ui->passwordStrengthTextLabel->setText(tr("中"));
        ui->passwordStrengthTextLabel->setStyleSheet("color: blue;");
    } else {
        ui->passwordStrengthTextLabel->setText(tr("强"));
        ui->passwordStrengthTextLabel->setStyleSheet("color: green;");
    }
}


bool RegisterDialog::validateInput(QString& errorMessages) {
    errorMessages.clear();
    QList<QString> errors;

    QString email = ui->emailLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();

    // Email
    if (email.isEmpty()) {
        errors.append(tr("邮箱地址不能为空。"));
    } else if (!ui->emailLineEdit->hasAcceptableInput()) {
        errors.append(tr("请输入有效的邮箱地址格式。"));
    }

    // Nickname (optional, so no validation if empty unless specific rules apply)
    if (!ui->nicknameLineEdit->text().trimmed().isEmpty() && ui->nicknameLineEdit->text().trimmed().length() > 50) {
        errors.append(tr("昵称过长 (最多50字符)。"));
    }


    // Password
    if (password.isEmpty()) {
        errors.append(tr("密码不能为空。"));
    } else {
        int strengthScore = calculatePasswordStrength(password);
        // Define minimum acceptable score, e.g., 3 for "Weak" but not "Too short"
        if (strengthScore < 3) { // Example: Score 0-2 is too weak
            errors.append(tr("密码强度不足，请尝试更复杂的密码 (建议至少8位，包含大小写字母、数字或符号)。"));
        }
    }

    // Confirm Password
    if (confirmPassword.isEmpty()) {
        errors.append(tr("确认密码不能为空。"));
    } else if (password != confirmPassword) {
        errors.append(tr("两次输入的密码不匹配。"));
    }

    // Terms
    if (!ui->termsCheckBox->isChecked()) {
        errors.append(tr("请阅读并同意服务条款和隐私政策。"));
    }

    if (!errors.isEmpty()) {
        errorMessages = errors.join("<br>"); // Join with HTML line break for QLabel
        return false;
    }
    return true;
}

void RegisterDialog::on_buttonBox_accepted() {
    QString errorMessages;
    if (!validateInput(errorMessages)) {
        ui->statusLabel->setText(errorMessages);
        return;
    }
    ui->statusLabel->clear(); // Clear previous errors

    QString email = ui->emailLineEdit->text().trimmed();
    QString nickname = ui->nicknameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    // Check if email already exists
    if (DatabaseManager::instance().getUserByEmail(email).isValid()) {
        ui->statusLabel->setText(tr("此邮箱地址已被注册。请尝试其他邮箱或登录。"));
        return;
    }

    // Proceed with registration
    User newUser;
    newUser.email = email;
    newUser.nickname = nickname.isEmpty() ? tr("用户%1").arg(QDateTime::currentMSecsSinceEpoch() % 10000) : nickname; // Default nickname if empty
    newUser.passwordHash = DatabaseManager::instance().hashPassword(password);
    newUser.registrationDate = QDateTime::currentDateTime();
    newUser.accountStatus = 0; // 0 for Active (assuming no email verification for now)
    // avatarPath will be empty/default

    if (DatabaseManager::instance().addUser(newUser)) {
        m_registeredEmail = newUser.email; // Store for pre-filling login
        QMessageBox::information(this, tr("注册成功"), tr("您的账户已成功创建！您现在可以使用邮箱和密码登录了。"));
        QDialog::accept(); // This will close the dialog with QDialog::Accepted state
    } else {
        ui->statusLabel->setText(tr("注册失败：无法将用户信息保存到数据库。请稍后再试。"));
        // Log detailed error internally using qDebug() or a proper logger
        qWarning() << "Failed to add user to database for email:" << email;
    }
}