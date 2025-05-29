/********************************************************************************
** Form generated from reading UI file 'ReportSettingsDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPORTSETTINGSDIALOG_H
#define UI_REPORTSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ReportSettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *labelPeriod;
    QComboBox *periodComboBox;
    QGroupBox *customDateGroupBox;
    QFormLayout *formLayout_2;
    QLabel *labelStartDate;
    QDateEdit *startDateEdit;
    QLabel *labelEndDate;
    QDateEdit *endDateEdit;
    QHBoxLayout *llmModelLayout;
    QLabel *labelLlmModel;
    QComboBox *llmModelComboBox;
    QSpacerItem *llmModelSpacer;
    QSpacerItem *verticalSpacer_InputFields;
    QLabel *statusLabel;
    QSpacerItem *verticalSpacer_Main;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ReportSettingsDialog)
    {
        if (ReportSettingsDialog->objectName().isEmpty())
            ReportSettingsDialog->setObjectName(QString::fromUtf8("ReportSettingsDialog"));
        ReportSettingsDialog->resize(420, 300);
        ReportSettingsDialog->setModal(true);
        verticalLayout = new QVBoxLayout(ReportSettingsDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        labelPeriod = new QLabel(ReportSettingsDialog);
        labelPeriod->setObjectName(QString::fromUtf8("labelPeriod"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelPeriod);

        periodComboBox = new QComboBox(ReportSettingsDialog);
        periodComboBox->setObjectName(QString::fromUtf8("periodComboBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, periodComboBox);


        verticalLayout->addLayout(formLayout);

        customDateGroupBox = new QGroupBox(ReportSettingsDialog);
        customDateGroupBox->setObjectName(QString::fromUtf8("customDateGroupBox"));
        customDateGroupBox->setEnabled(false);
        formLayout_2 = new QFormLayout(customDateGroupBox);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        labelStartDate = new QLabel(customDateGroupBox);
        labelStartDate->setObjectName(QString::fromUtf8("labelStartDate"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, labelStartDate);

        startDateEdit = new QDateEdit(customDateGroupBox);
        startDateEdit->setObjectName(QString::fromUtf8("startDateEdit"));
        startDateEdit->setCalendarPopup(true);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, startDateEdit);

        labelEndDate = new QLabel(customDateGroupBox);
        labelEndDate->setObjectName(QString::fromUtf8("labelEndDate"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, labelEndDate);

        endDateEdit = new QDateEdit(customDateGroupBox);
        endDateEdit->setObjectName(QString::fromUtf8("endDateEdit"));
        endDateEdit->setCalendarPopup(true);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, endDateEdit);


        verticalLayout->addWidget(customDateGroupBox);

        llmModelLayout = new QHBoxLayout();
        llmModelLayout->setObjectName(QString::fromUtf8("llmModelLayout"));
        labelLlmModel = new QLabel(ReportSettingsDialog);
        labelLlmModel->setObjectName(QString::fromUtf8("labelLlmModel"));

        llmModelLayout->addWidget(labelLlmModel);

        llmModelComboBox = new QComboBox(ReportSettingsDialog);
        llmModelComboBox->addItem(QString());
        llmModelComboBox->addItem(QString());
        llmModelComboBox->addItem(QString());
        llmModelComboBox->setObjectName(QString::fromUtf8("llmModelComboBox"));

        llmModelLayout->addWidget(llmModelComboBox);

        llmModelSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        llmModelLayout->addItem(llmModelSpacer);


        verticalLayout->addLayout(llmModelLayout);

        verticalSpacer_InputFields = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer_InputFields);

        statusLabel = new QLabel(ReportSettingsDialog);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setWordWrap(true);
        statusLabel->setStyleSheet(QString::fromUtf8("color: red;"));

        verticalLayout->addWidget(statusLabel);

        verticalSpacer_Main = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_Main);

        buttonBox = new QDialogButtonBox(ReportSettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#if QT_CONFIG(shortcut)
        labelPeriod->setBuddy(periodComboBox);
        labelStartDate->setBuddy(startDateEdit);
        labelEndDate->setBuddy(endDateEdit);
        labelLlmModel->setBuddy(llmModelComboBox);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(periodComboBox, startDateEdit);
        QWidget::setTabOrder(startDateEdit, endDateEdit);
        QWidget::setTabOrder(endDateEdit, llmModelComboBox);
        QWidget::setTabOrder(llmModelComboBox, buttonBox);

        retranslateUi(ReportSettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ReportSettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ReportSettingsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ReportSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *ReportSettingsDialog)
    {
        ReportSettingsDialog->setWindowTitle(QCoreApplication::translate("ReportSettingsDialog", "\346\212\245\345\221\212\350\256\276\347\275\256", nullptr));
        labelPeriod->setText(QCoreApplication::translate("ReportSettingsDialog", "\346\212\245\345\221\212\345\221\250\346\234\237(&P):", nullptr));
        customDateGroupBox->setTitle(QCoreApplication::translate("ReportSettingsDialog", "\350\207\252\345\256\232\344\271\211\346\227\245\346\234\237\350\214\203\345\233\264", nullptr));
        labelStartDate->setText(QCoreApplication::translate("ReportSettingsDialog", "\345\274\200\345\247\213\346\227\245\346\234\237(&S):", nullptr));
        startDateEdit->setDisplayFormat(QCoreApplication::translate("ReportSettingsDialog", "yyyy-MM-dd", nullptr));
        labelEndDate->setText(QCoreApplication::translate("ReportSettingsDialog", "\347\273\223\346\235\237\346\227\245\346\234\237(&E):", nullptr));
        endDateEdit->setDisplayFormat(QCoreApplication::translate("ReportSettingsDialog", "yyyy-MM-dd", nullptr));
        labelLlmModel->setText(QCoreApplication::translate("ReportSettingsDialog", "\346\231\272\350\203\275\345\210\206\346\236\220\346\250\241\345\236\213(&L):", nullptr));
        llmModelComboBox->setItemText(0, QCoreApplication::translate("ReportSettingsDialog", "Gemini", nullptr));
        llmModelComboBox->setItemText(1, QCoreApplication::translate("ReportSettingsDialog", "Qwen", nullptr));
        llmModelComboBox->setItemText(2, QCoreApplication::translate("ReportSettingsDialog", "\344\270\215\351\207\207\347\224\250\345\244\247\346\250\241\345\236\213\345\210\206\346\236\220", nullptr));

        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ReportSettingsDialog: public Ui_ReportSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPORTSETTINGSDIALOG_H
