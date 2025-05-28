/********************************************************************************
** Form generated from reading UI file 'LoginDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QFormLayout *formLayout;
    QLabel *emailLabel;
    QLineEdit *emailLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QHBoxLayout *optionsLayout;
    QCheckBox *rememberMeCheckBox;
    QSpacerItem *optionsSpacer;
    QLabel *statusLabel;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *buttonLayout;
    QSpacerItem *horizontalSpacer_L;
    QPushButton *registerButton;
    QPushButton *loginButton;
    QSpacerItem *horizontalSpacer_R;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(350, 250);
        LoginDialog->setModal(true);
        verticalLayout = new QVBoxLayout(LoginDialog);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(LoginDialog);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        emailLabel = new QLabel(LoginDialog);
        emailLabel->setObjectName("emailLabel");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, emailLabel);

        emailLineEdit = new QLineEdit(LoginDialog);
        emailLineEdit->setObjectName("emailLineEdit");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, emailLineEdit);

        passwordLabel = new QLabel(LoginDialog);
        passwordLabel->setObjectName("passwordLabel");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, passwordLabel);

        passwordLineEdit = new QLineEdit(LoginDialog);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, passwordLineEdit);


        verticalLayout->addLayout(formLayout);

        optionsLayout = new QHBoxLayout();
        optionsLayout->setObjectName("optionsLayout");
        rememberMeCheckBox = new QCheckBox(LoginDialog);
        rememberMeCheckBox->setObjectName("rememberMeCheckBox");

        optionsLayout->addWidget(rememberMeCheckBox);

        optionsSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        optionsLayout->addItem(optionsSpacer);


        verticalLayout->addLayout(optionsLayout);

        statusLabel = new QLabel(LoginDialog);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setWordWrap(true);

        verticalLayout->addWidget(statusLabel);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName("buttonLayout");
        horizontalSpacer_L = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        buttonLayout->addItem(horizontalSpacer_L);

        registerButton = new QPushButton(LoginDialog);
        registerButton->setObjectName("registerButton");

        buttonLayout->addWidget(registerButton);

        loginButton = new QPushButton(LoginDialog);
        loginButton->setObjectName("loginButton");

        buttonLayout->addWidget(loginButton);

        horizontalSpacer_R = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        buttonLayout->addItem(horizontalSpacer_R);


        verticalLayout->addLayout(buttonLayout);

#if QT_CONFIG(shortcut)
        emailLabel->setBuddy(emailLineEdit);
        passwordLabel->setBuddy(passwordLineEdit);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(emailLineEdit, passwordLineEdit);
        QWidget::setTabOrder(passwordLineEdit, rememberMeCheckBox);
        QWidget::setTabOrder(rememberMeCheckBox, loginButton);
        QWidget::setTabOrder(loginButton, registerButton);

        retranslateUi(LoginDialog);

        loginButton->setDefault(true);


        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "\347\224\250\346\210\267\347\231\273\345\275\225", nullptr));
        titleLabel->setText(QCoreApplication::translate("LoginDialog", "\346\254\242\350\277\216\345\233\236\346\235\245", nullptr));
        emailLabel->setText(QCoreApplication::translate("LoginDialog", "\351\202\256\347\256\261(&E):", nullptr));
        emailLineEdit->setPlaceholderText(QCoreApplication::translate("LoginDialog", "\350\257\267\350\276\223\345\205\245\351\202\256\347\256\261\345\234\260\345\235\200", nullptr));
        passwordLabel->setText(QCoreApplication::translate("LoginDialog", "\345\257\206\347\240\201(&P):", nullptr));
        passwordLineEdit->setPlaceholderText(QCoreApplication::translate("LoginDialog", "\350\257\267\350\276\223\345\205\245\345\257\206\347\240\201", nullptr));
        rememberMeCheckBox->setText(QCoreApplication::translate("LoginDialog", "\350\256\260\344\275\217\346\210\221(&R)", nullptr));
        statusLabel->setText(QString());
        registerButton->setText(QCoreApplication::translate("LoginDialog", "\346\263\250\345\206\214(&G)", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginDialog", "\347\231\273\345\275\225(&L)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
