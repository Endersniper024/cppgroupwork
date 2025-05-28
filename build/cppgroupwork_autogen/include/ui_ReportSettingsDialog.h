/********************************************************************************
** Form generated from reading UI file 'ReportSettingsDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPORTSETTINGSDIALOG_H
#define UI_REPORTSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
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
    QCheckBox *includeLLMCheckBox;
    QSpacerItem *verticalSpacer_InputFields;
    QLabel *statusLabel;
    QSpacerItem *verticalSpacer_Main;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ReportSettingsDialog)
    {
        if (ReportSettingsDialog->objectName().isEmpty())
            ReportSettingsDialog->setObjectName("ReportSettingsDialog");
        ReportSettingsDialog->resize(420, 300);
        ReportSettingsDialog->setModal(true);
        verticalLayout = new QVBoxLayout(ReportSettingsDialog);
        verticalLayout->setObjectName("verticalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        labelPeriod = new QLabel(ReportSettingsDialog);
        labelPeriod->setObjectName("labelPeriod");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, labelPeriod);

        periodComboBox = new QComboBox(ReportSettingsDialog);
        periodComboBox->setObjectName("periodComboBox");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, periodComboBox);


        verticalLayout->addLayout(formLayout);

        customDateGroupBox = new QGroupBox(ReportSettingsDialog);
        customDateGroupBox->setObjectName("customDateGroupBox");
        customDateGroupBox->setEnabled(false);
        formLayout_2 = new QFormLayout(customDateGroupBox);
        formLayout_2->setObjectName("formLayout_2");
        labelStartDate = new QLabel(customDateGroupBox);
        labelStartDate->setObjectName("labelStartDate");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::LabelRole, labelStartDate);

        startDateEdit = new QDateEdit(customDateGroupBox);
        startDateEdit->setObjectName("startDateEdit");
        startDateEdit->setCalendarPopup(true);

        formLayout_2->setWidget(0, QFormLayout::ItemRole::FieldRole, startDateEdit);

        labelEndDate = new QLabel(customDateGroupBox);
        labelEndDate->setObjectName("labelEndDate");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::LabelRole, labelEndDate);

        endDateEdit = new QDateEdit(customDateGroupBox);
        endDateEdit->setObjectName("endDateEdit");
        endDateEdit->setCalendarPopup(true);

        formLayout_2->setWidget(1, QFormLayout::ItemRole::FieldRole, endDateEdit);


        verticalLayout->addWidget(customDateGroupBox);

        includeLLMCheckBox = new QCheckBox(ReportSettingsDialog);
        includeLLMCheckBox->setObjectName("includeLLMCheckBox");
        includeLLMCheckBox->setChecked(true);

        verticalLayout->addWidget(includeLLMCheckBox);

        verticalSpacer_InputFields = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

        verticalLayout->addItem(verticalSpacer_InputFields);

        statusLabel = new QLabel(ReportSettingsDialog);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setWordWrap(true);
        statusLabel->setStyleSheet(QString::fromUtf8("color: red;"));

        verticalLayout->addWidget(statusLabel);

        verticalSpacer_Main = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_Main);

        buttonBox = new QDialogButtonBox(ReportSettingsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#if QT_CONFIG(shortcut)
        labelPeriod->setBuddy(periodComboBox);
        labelStartDate->setBuddy(startDateEdit);
        labelEndDate->setBuddy(endDateEdit);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(periodComboBox, startDateEdit);
        QWidget::setTabOrder(startDateEdit, endDateEdit);
        QWidget::setTabOrder(endDateEdit, includeLLMCheckBox);
        QWidget::setTabOrder(includeLLMCheckBox, buttonBox);

        retranslateUi(ReportSettingsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ReportSettingsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ReportSettingsDialog, qOverload<>(&QDialog::reject));

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
        includeLLMCheckBox->setText(QCoreApplication::translate("ReportSettingsDialog", "\345\220\257\347\224\250\345\244\247\346\250\241\345\236\213\346\231\272\350\203\275\345\210\206\346\236\220(&L)", nullptr));
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ReportSettingsDialog: public Ui_ReportSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPORTSETTINGSDIALOG_H
