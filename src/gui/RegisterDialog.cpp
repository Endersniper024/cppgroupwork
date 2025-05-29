#include "RegisterDialog.h"
#include "DatabaseManager.h"
#include <QRegularExpression>
#include <QDebug>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent),
    m_emailLineEdit(nullptr),
    m_nicknameLineEdit(nullptr),
    m_passwordLineEdit(nullptr),
    m_confirmPasswordLineEdit(nullptr),
    m_statusLabel(nullptr),
    m_passwordStrengthLabel(nullptr),
    m_registerButton(nullptr),
    m_cancelButton(nullptr),
    m_agreeTermsCheckBox(nullptr)
{
    setupUI();
    connectSignals();

    setWindowTitle("用户注册");
    setModal(true);
    // 移除 setFixedSize 改为设置最小大小
    setMinimumSize(400, 350);
    setMaximumSize(450, 400);  // 添加最大大小限制防止过度拉伸

    // 设置初始状态
    m_registerButton->setEnabled(false);
    setStatusMessage("请填写完整的注册信息", false);
}

RegisterDialog::~RegisterDialog()
{
    // Qt 对象会自动清理，无需手动删除
}

void RegisterDialog::setupUI()
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 创建标题
    QLabel *titleLabel = new QLabel("创建新账户", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // 创建表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(8);

    // 邮箱输入
    m_emailLineEdit = new QLineEdit(this);
    m_emailLineEdit->setPlaceholderText("请输入邮箱地址");
    m_emailLineEdit->setStyleSheet("padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px;");
    formLayout->addRow("邮箱地址 *:", m_emailLineEdit);

    // 昵称输入
    m_nicknameLineEdit = new QLineEdit(this);
    m_nicknameLineEdit->setPlaceholderText("请输入昵称（可选）");
    m_nicknameLineEdit->setStyleSheet("padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px;");
    formLayout->addRow("昵称:", m_nicknameLineEdit);

    // 密码输入
    m_passwordLineEdit = new QLineEdit(this);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_passwordLineEdit->setPlaceholderText("请输入密码（至少6位）");
    m_passwordLineEdit->setStyleSheet("padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px;");
    formLayout->addRow("密码 *:", m_passwordLineEdit);

    // 密码强度标签
    m_passwordStrengthLabel = new QLabel(this);
    m_passwordStrengthLabel->setStyleSheet("font-size: 12px; margin-left: 5px;");
    formLayout->addRow("", m_passwordStrengthLabel);

    // 确认密码输入
    m_confirmPasswordLineEdit = new QLineEdit(this);
    m_confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordLineEdit->setPlaceholderText("请再次输入密码");
    m_confirmPasswordLineEdit->setStyleSheet("padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px;");
    formLayout->addRow("确认密码 *:", m_confirmPasswordLineEdit);

    mainLayout->addLayout(formLayout);

    // 同意条款复选框
    m_agreeTermsCheckBox = new QCheckBox("我已阅读并同意用户协议和隐私政策", this);
    m_agreeTermsCheckBox->setStyleSheet("margin: 10px 0;");
    mainLayout->addWidget(m_agreeTermsCheckBox);

    // 状态标签
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet("color: #e74c3c; font-size: 12px; margin: 5px 0;");
    mainLayout->addWidget(m_statusLabel);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_cancelButton = new QPushButton("取消", this);
    m_cancelButton->setStyleSheet(R"(
        QPushButton {
            background-color: #95a5a6;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 4px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #7f8c8d;
        }
        QPushButton:pressed {
            background-color: #6c7b7d;
        }
    )");

    m_registerButton = new QPushButton("注册", this);
    m_registerButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 4px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #21618c;
        }
        QPushButton:disabled {
            background-color: #bdc3c7;
            color: #7f8c8d;
        }
    )");

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_registerButton);

    mainLayout->addLayout(buttonLayout);

    // 修改对话框样式表 - 添加背景固定
    setStyleSheet(R"(
        RegisterDialog {
            background-color: #ecf0f1;
        }
        QLabel {
            color: #2c3e50;
        }
    )");

    // 添加布局大小约束策略
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
}

void RegisterDialog::connectSignals()
{
    // 连接按钮信号
    connect(m_registerButton, &QPushButton::clicked, this, &RegisterDialog::onRegisterButtonClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &RegisterDialog::onCancelButtonClicked);

    // 连接输入验证信号
    connect(m_emailLineEdit, &QLineEdit::textChanged, this, &RegisterDialog::validateInput);
    connect(m_passwordLineEdit, &QLineEdit::textChanged, this, &RegisterDialog::onPasswordChanged);
    connect(m_confirmPasswordLineEdit, &QLineEdit::textChanged, this, &RegisterDialog::validateInput);
    connect(m_agreeTermsCheckBox, &QCheckBox::toggled, this, &RegisterDialog::validateInput);

    // 回车键处理
    connect(m_emailLineEdit, &QLineEdit::returnPressed, m_nicknameLineEdit, QOverload<>::of(&QLineEdit::setFocus));
    connect(m_nicknameLineEdit, &QLineEdit::returnPressed, m_passwordLineEdit, QOverload<>::of(&QLineEdit::setFocus));
    connect(m_passwordLineEdit, &QLineEdit::returnPressed, m_confirmPasswordLineEdit, QOverload<>::of(&QLineEdit::setFocus));
    connect(m_confirmPasswordLineEdit, &QLineEdit::returnPressed, this, &RegisterDialog::onRegisterButtonClicked);
}

void RegisterDialog::onRegisterButtonClicked()
{
    if (!validateInputFields()) {
        return;
    }

    QString email = m_emailLineEdit->text().trimmed();
    QString nickname = m_nicknameLineEdit->text().trimmed();
    QString password = m_passwordLineEdit->text();

    // 如果昵称为空，使用邮箱前缀作为默认昵称
    if (nickname.isEmpty()) {
        nickname = email.split("@").first();
    }

    // 尝试注册用户
    bool success = DatabaseManager::instance().registerUser(email, password, nickname);

    if (success) {
        m_registeredEmail = email;

        QMessageBox::information(this, "注册成功",
                                 QString("注册成功！\n\n邮箱: %1\n昵称: %2\n\n请使用新账号登录。")
                                     .arg(email)
                                     .arg(nickname));

        accept(); // 关闭对话框并返回 QDialog::Accepted
    } else {
        setStatusMessage("注册失败！该邮箱可能已被注册，请尝试其他邮箱地址。");
    }
}

void RegisterDialog::onCancelButtonClicked()
{
    reject(); // 关闭对话框并返回 QDialog::Rejected
}

void RegisterDialog::onPasswordChanged()
{
    QString password = m_passwordLineEdit->text();
    showPasswordStrength(password);
    validateInput();
}

void RegisterDialog::validateInput()
{
    bool isValid = validateInputFields();
    m_registerButton->setEnabled(isValid);

    if (isValid) {
        setStatusMessage("信息填写完整，可以注册", false);
    }
}

bool RegisterDialog::validateInputFields()
{
    QString email = m_emailLineEdit->text().trimmed();
    QString password = m_passwordLineEdit->text();
    QString confirmPassword = m_confirmPasswordLineEdit->text();

    // 检查邮箱
    if (email.isEmpty()) {
        setStatusMessage("请输入邮箱地址");
        return false;
    }

    if (!isValidEmail(email)) {
        setStatusMessage("请输入有效的邮箱地址");
        return false;
    }

    // 检查密码
    if (password.isEmpty()) {
        setStatusMessage("请输入密码");
        return false;
    }

    if (password.length() < 6) {
        setStatusMessage("密码长度至少需要6位");
        return false;
    }

    // 检查确认密码
    if (confirmPassword.isEmpty()) {
        setStatusMessage("请确认密码");
        return false;
    }

    if (password != confirmPassword) {
        setStatusMessage("两次输入的密码不一致");
        return false;
    }

    // 检查同意条款
    if (!m_agreeTermsCheckBox->isChecked()) {
        setStatusMessage("请阅读并同意用户协议");
        return false;
    }

    return true;
}

bool RegisterDialog::isValidEmail(const QString &email) const
{
    // 简单的邮箱格式验证
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return emailRegex.match(email).hasMatch();
}

QString RegisterDialog::checkPasswordStrength(const QString &password) const
{
    if (password.length() < 6) {
        return "太短";
    }

    int score = 0;

    // 长度加分
    if (password.length() >= 8) score++;
    if (password.length() >= 12) score++;

    // 字符类型加分
    if (password.contains(QRegularExpression("[a-z]"))) score++; // 小写字母
    if (password.contains(QRegularExpression("[A-Z]"))) score++; // 大写字母
    if (password.contains(QRegularExpression("[0-9]"))) score++; // 数字
    if (password.contains(QRegularExpression("[^a-zA-Z0-9]"))) score++; // 特殊字符

    if (score <= 2) return "弱";
    if (score <= 4) return "中等";
    return "强";
}

void RegisterDialog::showPasswordStrength(const QString &password)
{
    if (password.isEmpty()) {
        m_passwordStrengthLabel->setText("");
        return;
    }

    QString strength = checkPasswordStrength(password);
    QString color;

    if (strength == "太短" || strength == "弱") {
        color = "#e74c3c"; // 红色
    } else if (strength == "中等") {
        color = "#f39c12"; // 橙色
    } else {
        color = "#27ae60"; // 绿色
    }

    m_passwordStrengthLabel->setText(QString("密码强度: <span style='color: %1; font-weight: bold;'>%2</span>")
                                         .arg(color)
                                         .arg(strength));
}

void RegisterDialog::setStatusMessage(const QString &message, bool isError)
{
    if (isError) {
        m_statusLabel->setStyleSheet("color: #e74c3c; font-size: 12px; margin: 5px 0;");
    } else {
        m_statusLabel->setStyleSheet("color: #27ae60; font-size: 12px; margin: 5px 0;");
    }
    m_statusLabel->setText(message);
}

QString RegisterDialog::getRegisteredEmail() const
{
    return m_registeredEmail;
}
