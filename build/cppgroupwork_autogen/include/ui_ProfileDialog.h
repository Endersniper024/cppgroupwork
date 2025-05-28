/********************************************************************************
** Form generated from reading UI file 'ProfileDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILEDIALOG_H
#define UI_PROFILEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ProfileDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *avatarGroupBox;
    QHBoxLayout *horizontalLayout_Avatar;
    QLabel *avatarDisplayLabel;
    QVBoxLayout *verticalLayout_AvatarButton;
    QPushButton *changeAvatarButton;
    QSpacerItem *verticalSpacer_Avatar;
    QGroupBox *infoGroupBox;
    QFormLayout *formLayout;
    QLabel *emailLabel;
    QLineEdit *emailLineEdit;
    QLabel *nicknameLabel;
    QLineEdit *nicknameLineEdit;
    QGroupBox *passwordGroupBox;
    QFormLayout *formLayout_2;
    QLabel *oldPasswordLabel;
    QLineEdit *oldPasswordLineEdit;
    QLabel *newPasswordLabel;
    QLineEdit *newPasswordLineEdit;
    QLabel *confirmPasswordLabel;
    QLineEdit *confirmPasswordLineEdit;
    QLabel *statusLabel;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ProfileDialog)
    {
        if (ProfileDialog->objectName().isEmpty())
            ProfileDialog->setObjectName("ProfileDialog");
        ProfileDialog->resize(450, 480);
        ProfileDialog->setModal(true);
        verticalLayout = new QVBoxLayout(ProfileDialog);
        verticalLayout->setObjectName("verticalLayout");
        avatarGroupBox = new QGroupBox(ProfileDialog);
        avatarGroupBox->setObjectName("avatarGroupBox");
        horizontalLayout_Avatar = new QHBoxLayout(avatarGroupBox);
        horizontalLayout_Avatar->setObjectName("horizontalLayout_Avatar");
        avatarDisplayLabel = new QLabel(avatarGroupBox);
        avatarDisplayLabel->setObjectName("avatarDisplayLabel");
        avatarDisplayLabel->setMinimumSize(QSize(100, 100));
        avatarDisplayLabel->setMaximumSize(QSize(100, 100));
        avatarDisplayLabel->setStyleSheet(QString::fromUtf8("border: 1px solid gray; background-color: white;"));
        avatarDisplayLabel->setAlignment(Qt::AlignCenter);
        avatarDisplayLabel->setScaledContents(true);

        horizontalLayout_Avatar->addWidget(avatarDisplayLabel);

        verticalLayout_AvatarButton = new QVBoxLayout();
        verticalLayout_AvatarButton->setObjectName("verticalLayout_AvatarButton");
        changeAvatarButton = new QPushButton(avatarGroupBox);
        changeAvatarButton->setObjectName("changeAvatarButton");

        verticalLayout_AvatarButton->addWidget(changeAvatarButton);

        verticalSpacer_Avatar = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_AvatarButton->addItem(verticalSpacer_Avatar);


        horizontalLayout_Avatar->addLayout(verticalLayout_AvatarButton);


        verticalLayout->addWidget(avatarGroupBox);

        infoGroupBox = new QGroupBox(ProfileDialog);
        infoGroupBox->setObjectName("infoGroupBox");
        formLayout = new QFormLayout(infoGroupBox);
        formLayout->setObjectName("formLayout");
        emailLabel = new QLabel(infoGroupBox);
        emailLabel->setObjectName("emailLabel");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, emailLabel);

        emailLineEdit = new QLineEdit(infoGroupBox);
        emailLineEdit->setObjectName("emailLineEdit");
        emailLineEdit->setReadOnly(true);

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, emailLineEdit);

        nicknameLabel = new QLabel(infoGroupBox);
        nicknameLabel->setObjectName("nicknameLabel");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, nicknameLabel);

        nicknameLineEdit = new QLineEdit(infoGroupBox);
        nicknameLineEdit->setObjectName("nicknameLineEdit");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, nicknameLineEdit);


        verticalLayout->addWidget(infoGroupBox);

        passwordGroupBox = new QGroupBox(ProfileDialog);
        passwordGroupBox->setObjectName("passwordGroupBox");
        formLayout_2 = new QFormLayout(passwordGroupBox);
        formLayout_2->setObjectName("formLayout_2");
        oldPasswordLabel = new QLabel(passwordGroupBox);
        oldPasswordLabel->setObjectName("oldPasswordLabel");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::LabelRole, oldPasswordLabel);

        oldPasswordLineEdit = new QLineEdit(passwordGroupBox);
        oldPasswordLineEdit->setObjectName("oldPasswordLineEdit");
        oldPasswordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout_2->setWidget(0, QFormLayout::ItemRole::FieldRole, oldPasswordLineEdit);

        newPasswordLabel = new QLabel(passwordGroupBox);
        newPasswordLabel->setObjectName("newPasswordLabel");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::LabelRole, newPasswordLabel);

        newPasswordLineEdit = new QLineEdit(passwordGroupBox);
        newPasswordLineEdit->setObjectName("newPasswordLineEdit");
        newPasswordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout_2->setWidget(1, QFormLayout::ItemRole::FieldRole, newPasswordLineEdit);

        confirmPasswordLabel = new QLabel(passwordGroupBox);
        confirmPasswordLabel->setObjectName("confirmPasswordLabel");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::LabelRole, confirmPasswordLabel);

        confirmPasswordLineEdit = new QLineEdit(passwordGroupBox);
        confirmPasswordLineEdit->setObjectName("confirmPasswordLineEdit");
        confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout_2->setWidget(2, QFormLayout::ItemRole::FieldRole, confirmPasswordLineEdit);


        verticalLayout->addWidget(passwordGroupBox);

        statusLabel = new QLabel(ProfileDialog);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setWordWrap(true);

        verticalLayout->addWidget(statusLabel);

        verticalSpacer = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(ProfileDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);
        buttonBox->setCenterButtons(false);

        verticalLayout->addWidget(buttonBox);

#if QT_CONFIG(shortcut)
        nicknameLabel->setBuddy(nicknameLineEdit);
        oldPasswordLabel->setBuddy(oldPasswordLineEdit);
        newPasswordLabel->setBuddy(newPasswordLineEdit);
        confirmPasswordLabel->setBuddy(confirmPasswordLineEdit);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(changeAvatarButton, nicknameLineEdit);
        QWidget::setTabOrder(nicknameLineEdit, oldPasswordLineEdit);
        QWidget::setTabOrder(oldPasswordLineEdit, newPasswordLineEdit);
        QWidget::setTabOrder(newPasswordLineEdit, confirmPasswordLineEdit);
        QWidget::setTabOrder(confirmPasswordLineEdit, buttonBox);

        retranslateUi(ProfileDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ProfileDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ProfileDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ProfileDialog);
    } // setupUi

    void retranslateUi(QDialog *ProfileDialog)
    {
        ProfileDialog->setWindowTitle(QCoreApplication::translate("ProfileDialog", "\344\270\252\344\272\272\344\277\241\346\201\257\347\256\241\347\220\206", nullptr));
        avatarGroupBox->setTitle(QCoreApplication::translate("ProfileDialog", "\345\244\264\345\203\217", nullptr));
        avatarDisplayLabel->setText(QCoreApplication::translate("ProfileDialog", "\345\244\264\345\203\217\351\242\204\350\247\210", nullptr));
        changeAvatarButton->setText(QCoreApplication::translate("ProfileDialog", "\346\233\264\346\215\242\345\244\264\345\203\217(&A)...", nullptr));
        infoGroupBox->setTitle(QCoreApplication::translate("ProfileDialog", "\345\237\272\346\234\254\344\277\241\346\201\257", nullptr));
        emailLabel->setText(QCoreApplication::translate("ProfileDialog", "\351\202\256\347\256\261:", nullptr));
#if QT_CONFIG(tooltip)
        emailLineEdit->setToolTip(QCoreApplication::translate("ProfileDialog", "\351\202\256\347\256\261\344\270\215\345\217\257\344\277\256\346\224\271", nullptr));
#endif // QT_CONFIG(tooltip)
        nicknameLabel->setText(QCoreApplication::translate("ProfileDialog", "\346\230\265\347\247\260(&N):", nullptr));
        passwordGroupBox->setTitle(QCoreApplication::translate("ProfileDialog", "\344\277\256\346\224\271\345\257\206\347\240\201 (\347\225\231\347\251\272\345\210\231\344\270\215\344\277\256\346\224\271)", nullptr));
        oldPasswordLabel->setText(QCoreApplication::translate("ProfileDialog", "\346\227\247\345\257\206\347\240\201(&O):", nullptr));
        newPasswordLabel->setText(QCoreApplication::translate("ProfileDialog", "\346\226\260\345\257\206\347\240\201(&W):", nullptr));
        confirmPasswordLabel->setText(QCoreApplication::translate("ProfileDialog", "\347\241\256\350\256\244\346\226\260\345\257\206\347\240\201(&C):", nullptr));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ProfileDialog: public Ui_ProfileDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILEDIALOG_H
