/********************************************************************************
** Form generated from reading UI file 'RegisterDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERDIALOG_H
#define UI_REGISTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RegisterDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QFormLayout *formLayout;
    QLabel *emailLabel;
    QLineEdit *emailLineEdit;
    QLabel *nicknameLabel;
    QLineEdit *nicknameLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QLabel *passwordStrengthLabel;
    QHBoxLayout *horizontalLayout;
    QProgressBar *passwordStrengthBar;
    QLabel *passwordStrengthTextLabel;
    QLabel *confirmPasswordLabel;
    QLineEdit *confirmPasswordLineEdit;
    QCheckBox *termsCheckBox;
    QLabel *statusLabel;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *RegisterDialog)
    {
        if (RegisterDialog->objectName().isEmpty())
            RegisterDialog->setObjectName(QString::fromUtf8("RegisterDialog"));
        RegisterDialog->resize(450, 420);
        RegisterDialog->setModal(true);
        verticalLayout = new QVBoxLayout(RegisterDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(RegisterDialog);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(QFont::Bold);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        emailLabel = new QLabel(RegisterDialog);
        emailLabel->setObjectName(QString::fromUtf8("emailLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, emailLabel);

        emailLineEdit = new QLineEdit(RegisterDialog);
        emailLineEdit->setObjectName(QString::fromUtf8("emailLineEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, emailLineEdit);

        nicknameLabel = new QLabel(RegisterDialog);
        nicknameLabel->setObjectName(QString::fromUtf8("nicknameLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, nicknameLabel);

        nicknameLineEdit = new QLineEdit(RegisterDialog);
        nicknameLineEdit->setObjectName(QString::fromUtf8("nicknameLineEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, nicknameLineEdit);

        passwordLabel = new QLabel(RegisterDialog);
        passwordLabel->setObjectName(QString::fromUtf8("passwordLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, passwordLabel);

        passwordLineEdit = new QLineEdit(RegisterDialog);
        passwordLineEdit->setObjectName(QString::fromUtf8("passwordLineEdit"));
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(2, QFormLayout::FieldRole, passwordLineEdit);

        passwordStrengthLabel = new QLabel(RegisterDialog);
        passwordStrengthLabel->setObjectName(QString::fromUtf8("passwordStrengthLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, passwordStrengthLabel);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        passwordStrengthBar = new QProgressBar(RegisterDialog);
        passwordStrengthBar->setObjectName(QString::fromUtf8("passwordStrengthBar"));
        passwordStrengthBar->setMaximum(100);
        passwordStrengthBar->setValue(0);
        passwordStrengthBar->setTextVisible(false);

        horizontalLayout->addWidget(passwordStrengthBar);

        passwordStrengthTextLabel = new QLabel(RegisterDialog);
        passwordStrengthTextLabel->setObjectName(QString::fromUtf8("passwordStrengthTextLabel"));
        passwordStrengthTextLabel->setMinimumSize(QSize(40, 0));
        passwordStrengthTextLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(passwordStrengthTextLabel);


        formLayout->setLayout(3, QFormLayout::FieldRole, horizontalLayout);

        confirmPasswordLabel = new QLabel(RegisterDialog);
        confirmPasswordLabel->setObjectName(QString::fromUtf8("confirmPasswordLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, confirmPasswordLabel);

        confirmPasswordLineEdit = new QLineEdit(RegisterDialog);
        confirmPasswordLineEdit->setObjectName(QString::fromUtf8("confirmPasswordLineEdit"));
        confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(4, QFormLayout::FieldRole, confirmPasswordLineEdit);


        verticalLayout->addLayout(formLayout);

        termsCheckBox = new QCheckBox(RegisterDialog);
        termsCheckBox->setObjectName(QString::fromUtf8("termsCheckBox"));

        verticalLayout->addWidget(termsCheckBox);

        statusLabel = new QLabel(RegisterDialog);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setWordWrap(true);
        statusLabel->setStyleSheet(QString::fromUtf8("color: red;"));

        verticalLayout->addWidget(statusLabel);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(RegisterDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#if QT_CONFIG(shortcut)
        emailLabel->setBuddy(emailLineEdit);
        nicknameLabel->setBuddy(nicknameLineEdit);
        passwordLabel->setBuddy(passwordLineEdit);
        confirmPasswordLabel->setBuddy(confirmPasswordLineEdit);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(emailLineEdit, nicknameLineEdit);
        QWidget::setTabOrder(nicknameLineEdit, passwordLineEdit);
        QWidget::setTabOrder(passwordLineEdit, confirmPasswordLineEdit);
        QWidget::setTabOrder(confirmPasswordLineEdit, termsCheckBox);
        QWidget::setTabOrder(termsCheckBox, buttonBox);

        retranslateUi(RegisterDialog);

        QMetaObject::connectSlotsByName(RegisterDialog);
    } // setupUi

    void retranslateUi(QDialog *RegisterDialog)
    {
        RegisterDialog->setWindowTitle(QCoreApplication::translate("RegisterDialog", "\345\210\233\345\273\272\350\264\246\346\210\267", nullptr));
        titleLabel->setText(QCoreApplication::translate("RegisterDialog", "\345\210\233\345\273\272\346\202\250\347\232\204\346\226\260\350\264\246\346\210\267", nullptr));
        emailLabel->setText(QCoreApplication::translate("RegisterDialog", "\351\202\256\347\256\261\345\234\260\345\235\200(&E):", nullptr));
        emailLineEdit->setPlaceholderText(QCoreApplication::translate("RegisterDialog", "\344\276\213\345\246\202: user@example.com", nullptr));
        nicknameLabel->setText(QCoreApplication::translate("RegisterDialog", "\346\230\265\347\247\260(&N):", nullptr));
        nicknameLineEdit->setPlaceholderText(QCoreApplication::translate("RegisterDialog", "\345\217\257\351\200\211", nullptr));
        passwordLabel->setText(QCoreApplication::translate("RegisterDialog", "\345\257\206\347\240\201(&P):", nullptr));
        passwordLineEdit->setPlaceholderText(QCoreApplication::translate("RegisterDialog", "\350\207\263\345\260\2218\344\275\215\357\274\214\345\214\205\345\220\253\345\255\227\346\257\215\345\222\214\346\225\260\345\255\227", nullptr));
        passwordStrengthLabel->setText(QCoreApplication::translate("RegisterDialog", "\345\257\206\347\240\201\345\274\272\345\272\246:", nullptr));
        passwordStrengthTextLabel->setText(QCoreApplication::translate("RegisterDialog", "\345\274\261", nullptr));
        confirmPasswordLabel->setText(QCoreApplication::translate("RegisterDialog", "\347\241\256\350\256\244\345\257\206\347\240\201(&C):", nullptr));
        termsCheckBox->setText(QCoreApplication::translate("RegisterDialog", "\346\210\221\345\267\262\351\230\205\350\257\273\345\271\266\345\220\214\346\204\217\346\234\215\345\212\241\346\235\241\346\254\276\345\222\214\351\232\220\347\247\201\346\224\277\347\255\226\343\200\202(&A)", nullptr));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class RegisterDialog: public Ui_RegisterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERDIALOG_H
