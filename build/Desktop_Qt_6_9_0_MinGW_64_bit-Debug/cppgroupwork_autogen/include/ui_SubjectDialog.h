/********************************************************************************
** Form generated from reading UI file 'SubjectDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SUBJECTDIALOG_H
#define UI_SUBJECTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SubjectDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *nameLabel;
    QLineEdit *nameLineEdit;
    QLabel *descriptionLabel;
    QPlainTextEdit *descriptionTextEdit;
    QLabel *colorLabel;
    QHBoxLayout *horizontalLayout;
    QPushButton *colorButton;
    QLabel *colorPreviewLabel;
    QSpacerItem *horizontalSpacer;
    QLabel *tagsLabel;
    QLineEdit *tagsLineEdit;
    QLabel *statusLabel;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SubjectDialog)
    {
        if (SubjectDialog->objectName().isEmpty())
            SubjectDialog->setObjectName("SubjectDialog");
        SubjectDialog->resize(400, 300);
        SubjectDialog->setModal(true);
        verticalLayout = new QVBoxLayout(SubjectDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        nameLabel = new QLabel(SubjectDialog);
        nameLabel->setObjectName("nameLabel");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, nameLabel);

        nameLineEdit = new QLineEdit(SubjectDialog);
        nameLineEdit->setObjectName("nameLineEdit");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, nameLineEdit);

        descriptionLabel = new QLabel(SubjectDialog);
        descriptionLabel->setObjectName("descriptionLabel");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, descriptionLabel);

        descriptionTextEdit = new QPlainTextEdit(SubjectDialog);
        descriptionTextEdit->setObjectName("descriptionTextEdit");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(descriptionTextEdit->sizePolicy().hasHeightForWidth());
        descriptionTextEdit->setSizePolicy(sizePolicy);
        descriptionTextEdit->setMinimumSize(QSize(0, 60));

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, descriptionTextEdit);

        colorLabel = new QLabel(SubjectDialog);
        colorLabel->setObjectName("colorLabel");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, colorLabel);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        colorButton = new QPushButton(SubjectDialog);
        colorButton->setObjectName("colorButton");

        horizontalLayout->addWidget(colorButton);

        colorPreviewLabel = new QLabel(SubjectDialog);
        colorPreviewLabel->setObjectName("colorPreviewLabel");
        colorPreviewLabel->setMinimumSize(QSize(50, 20));
        colorPreviewLabel->setStyleSheet(QString::fromUtf8("border: 1px solid black;"));
        colorPreviewLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(colorPreviewLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        formLayout->setLayout(2, QFormLayout::ItemRole::FieldRole, horizontalLayout);

        tagsLabel = new QLabel(SubjectDialog);
        tagsLabel->setObjectName("tagsLabel");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, tagsLabel);

        tagsLineEdit = new QLineEdit(SubjectDialog);
        tagsLineEdit->setObjectName("tagsLineEdit");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, tagsLineEdit);


        verticalLayout->addLayout(formLayout);

        statusLabel = new QLabel(SubjectDialog);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setWordWrap(true);

        verticalLayout->addWidget(statusLabel);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(SubjectDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);

        verticalLayout->addWidget(buttonBox);

#if QT_CONFIG(shortcut)
        nameLabel->setBuddy(nameLineEdit);
        descriptionLabel->setBuddy(descriptionTextEdit);
        colorLabel->setBuddy(colorButton);
        tagsLabel->setBuddy(tagsLineEdit);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(nameLineEdit, descriptionTextEdit);
        QWidget::setTabOrder(descriptionTextEdit, colorButton);
        QWidget::setTabOrder(colorButton, tagsLineEdit);
        QWidget::setTabOrder(tagsLineEdit, buttonBox);

        retranslateUi(SubjectDialog);

        QMetaObject::connectSlotsByName(SubjectDialog);
    } // setupUi

    void retranslateUi(QDialog *SubjectDialog)
    {
        SubjectDialog->setWindowTitle(QCoreApplication::translate("SubjectDialog", "\347\247\221\347\233\256\350\257\246\346\203\205", nullptr));
        nameLabel->setText(QCoreApplication::translate("SubjectDialog", "\347\247\221\347\233\256\345\220\215\347\247\260(&N):", nullptr));
        descriptionLabel->setText(QCoreApplication::translate("SubjectDialog", "\346\217\217\350\277\260(&D):", nullptr));
        colorLabel->setText(QCoreApplication::translate("SubjectDialog", "\351\242\234\350\211\262(&C):", nullptr));
        colorButton->setText(QCoreApplication::translate("SubjectDialog", "\351\200\211\346\213\251\351\242\234\350\211\262", nullptr));
        colorPreviewLabel->setText(QString());
        tagsLabel->setText(QCoreApplication::translate("SubjectDialog", "\346\240\207\347\255\276(&T):", nullptr));
        tagsLineEdit->setPlaceholderText(QCoreApplication::translate("SubjectDialog", "\351\200\227\345\217\267\345\210\206\351\232\224, \344\276\213\345\246\202: \351\207\215\347\202\271, \345\244\215\344\271\240", nullptr));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SubjectDialog: public Ui_SubjectDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUBJECTDIALOG_H
