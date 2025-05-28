/********************************************************************************
** Form generated from reading UI file 'TaskDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TASKDIALOG_H
#define UI_TASKDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TaskDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *nameLabel;
    QLineEdit *nameLineEdit;
    QLabel *subjectLabel;
    QComboBox *subjectComboBox;
    QLabel *descriptionLabel;
    QPlainTextEdit *descriptionTextEdit;
    QLabel *priorityLabel;
    QComboBox *priorityComboBox;
    QLabel *dueDateLabel;
    QDateTimeEdit *dueDateTimeEdit;
    QLabel *estimatedTimeLabel;
    QSpinBox *estimatedTimeSpinBox;
    QLabel *statusLabelInfo;
    QComboBox *statusComboBox;
    QLabel *dialogStatusLabel;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TaskDialog)
    {
        if (TaskDialog->objectName().isEmpty())
            TaskDialog->setObjectName("TaskDialog");
        TaskDialog->resize(480, 450);
        TaskDialog->setModal(true);
        verticalLayout = new QVBoxLayout(TaskDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        nameLabel = new QLabel(TaskDialog);
        nameLabel->setObjectName("nameLabel");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, nameLabel);

        nameLineEdit = new QLineEdit(TaskDialog);
        nameLineEdit->setObjectName("nameLineEdit");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, nameLineEdit);

        subjectLabel = new QLabel(TaskDialog);
        subjectLabel->setObjectName("subjectLabel");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, subjectLabel);

        subjectComboBox = new QComboBox(TaskDialog);
        subjectComboBox->setObjectName("subjectComboBox");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, subjectComboBox);

        descriptionLabel = new QLabel(TaskDialog);
        descriptionLabel->setObjectName("descriptionLabel");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, descriptionLabel);

        descriptionTextEdit = new QPlainTextEdit(TaskDialog);
        descriptionTextEdit->setObjectName("descriptionTextEdit");
        descriptionTextEdit->setMinimumSize(QSize(0, 80));

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, descriptionTextEdit);

        priorityLabel = new QLabel(TaskDialog);
        priorityLabel->setObjectName("priorityLabel");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, priorityLabel);

        priorityComboBox = new QComboBox(TaskDialog);
        priorityComboBox->setObjectName("priorityComboBox");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, priorityComboBox);

        dueDateLabel = new QLabel(TaskDialog);
        dueDateLabel->setObjectName("dueDateLabel");

        formLayout->setWidget(4, QFormLayout::ItemRole::LabelRole, dueDateLabel);

        dueDateTimeEdit = new QDateTimeEdit(TaskDialog);
        dueDateTimeEdit->setObjectName("dueDateTimeEdit");
        dueDateTimeEdit->setCalendarPopup(true);

        formLayout->setWidget(4, QFormLayout::ItemRole::FieldRole, dueDateTimeEdit);

        estimatedTimeLabel = new QLabel(TaskDialog);
        estimatedTimeLabel->setObjectName("estimatedTimeLabel");

        formLayout->setWidget(5, QFormLayout::ItemRole::LabelRole, estimatedTimeLabel);

        estimatedTimeSpinBox = new QSpinBox(TaskDialog);
        estimatedTimeSpinBox->setObjectName("estimatedTimeSpinBox");
        estimatedTimeSpinBox->setMaximum(9999);
        estimatedTimeSpinBox->setSingleStep(15);

        formLayout->setWidget(5, QFormLayout::ItemRole::FieldRole, estimatedTimeSpinBox);

        statusLabelInfo = new QLabel(TaskDialog);
        statusLabelInfo->setObjectName("statusLabelInfo");

        formLayout->setWidget(6, QFormLayout::ItemRole::LabelRole, statusLabelInfo);

        statusComboBox = new QComboBox(TaskDialog);
        statusComboBox->setObjectName("statusComboBox");

        formLayout->setWidget(6, QFormLayout::ItemRole::FieldRole, statusComboBox);


        verticalLayout->addLayout(formLayout);

        dialogStatusLabel = new QLabel(TaskDialog);
        dialogStatusLabel->setObjectName("dialogStatusLabel");
        dialogStatusLabel->setAlignment(Qt::AlignCenter);
        dialogStatusLabel->setWordWrap(true);

        verticalLayout->addWidget(dialogStatusLabel);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(TaskDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);

        verticalLayout->addWidget(buttonBox);

#if QT_CONFIG(shortcut)
        nameLabel->setBuddy(nameLineEdit);
        subjectLabel->setBuddy(subjectComboBox);
        descriptionLabel->setBuddy(descriptionTextEdit);
        priorityLabel->setBuddy(priorityComboBox);
        dueDateLabel->setBuddy(dueDateTimeEdit);
        estimatedTimeLabel->setBuddy(estimatedTimeSpinBox);
        statusLabelInfo->setBuddy(statusComboBox);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(nameLineEdit, subjectComboBox);
        QWidget::setTabOrder(subjectComboBox, descriptionTextEdit);
        QWidget::setTabOrder(descriptionTextEdit, priorityComboBox);
        QWidget::setTabOrder(priorityComboBox, dueDateTimeEdit);
        QWidget::setTabOrder(dueDateTimeEdit, estimatedTimeSpinBox);
        QWidget::setTabOrder(estimatedTimeSpinBox, statusComboBox);
        QWidget::setTabOrder(statusComboBox, buttonBox);

        retranslateUi(TaskDialog);

        QMetaObject::connectSlotsByName(TaskDialog);
    } // setupUi

    void retranslateUi(QDialog *TaskDialog)
    {
        TaskDialog->setWindowTitle(QCoreApplication::translate("TaskDialog", "\344\273\273\345\212\241\350\257\246\346\203\205", nullptr));
        nameLabel->setText(QCoreApplication::translate("TaskDialog", "\344\273\273\345\212\241\345\220\215\347\247\260(&N):", nullptr));
        subjectLabel->setText(QCoreApplication::translate("TaskDialog", "\346\211\200\345\261\236\347\247\221\347\233\256(&S):", nullptr));
        descriptionLabel->setText(QCoreApplication::translate("TaskDialog", "\346\217\217\350\277\260(&D):", nullptr));
        priorityLabel->setText(QCoreApplication::translate("TaskDialog", "\344\274\230\345\205\210\347\272\247(&P):", nullptr));
        dueDateLabel->setText(QCoreApplication::translate("TaskDialog", "\346\210\252\346\255\242\346\227\245\346\234\237(&U):", nullptr));
        dueDateTimeEdit->setDisplayFormat(QCoreApplication::translate("TaskDialog", "yyyy-MM-dd HH:mm", nullptr));
        estimatedTimeLabel->setText(QCoreApplication::translate("TaskDialog", "\351\242\204\350\256\241\347\224\250\346\227\266(\345\210\206\351\222\237)(&E):", nullptr));
        statusLabelInfo->setText(QCoreApplication::translate("TaskDialog", "\347\212\266\346\200\201(&T):", nullptr));
        dialogStatusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TaskDialog: public Ui_TaskDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASKDIALOG_H
