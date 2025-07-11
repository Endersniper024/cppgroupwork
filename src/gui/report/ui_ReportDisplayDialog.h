/********************************************************************************
** Form generated from reading UI file 'ReportDisplayDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPORTDISPLAYDIALOG_H
#define UI_REPORTDISPLAYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReportDisplayDialog
{
public:
    QVBoxLayout *verticalLayout_Main;
    QTabWidget *tabWidget;
    QWidget *overviewTab;
    QVBoxLayout *verticalLayout_Overview;
    QLabel *labelSummaryTitle;
    QTextBrowser *summaryTextBrowser;
    QFrame *line_overview;
    QLabel *labelMetricsTitle;
    QTextBrowser *metricsTextBrowser;
    QWidget *llmInsightsTab;
    QVBoxLayout *verticalLayout_LLM;
    QLabel *labelPositives;
    QTextBrowser *positivesTextBrowser;
    QLabel *labelImprovements;
    QTextBrowser *improvementsTextBrowser;
    QLabel *labelSuggestions;
    QTextBrowser *suggestionsTextBrowser;
    QSpacerItem *verticalSpacer_LLM;
    QLabel *reportStatusLabel;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ReportDisplayDialog)
    {
        if (ReportDisplayDialog->objectName().isEmpty())
            ReportDisplayDialog->setObjectName(QString::fromUtf8("ReportDisplayDialog"));
        ReportDisplayDialog->resize(700, 550);
        ReportDisplayDialog->setModal(true);
        verticalLayout_Main = new QVBoxLayout(ReportDisplayDialog);
        verticalLayout_Main->setObjectName(QString::fromUtf8("verticalLayout_Main"));
        tabWidget = new QTabWidget(ReportDisplayDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        overviewTab = new QWidget();
        overviewTab->setObjectName(QString::fromUtf8("overviewTab"));
        verticalLayout_Overview = new QVBoxLayout(overviewTab);
        verticalLayout_Overview->setObjectName(QString::fromUtf8("verticalLayout_Overview"));
        labelSummaryTitle = new QLabel(overviewTab);
        labelSummaryTitle->setObjectName(QString::fromUtf8("labelSummaryTitle"));
        labelSummaryTitle->setAlignment(Qt::AlignCenter);

        verticalLayout_Overview->addWidget(labelSummaryTitle);

        summaryTextBrowser = new QTextBrowser(overviewTab);
        summaryTextBrowser->setObjectName(QString::fromUtf8("summaryTextBrowser"));
        summaryTextBrowser->setReadOnly(true);

        verticalLayout_Overview->addWidget(summaryTextBrowser);

        line_overview = new QFrame(overviewTab);
        line_overview->setObjectName(QString::fromUtf8("line_overview"));
        line_overview->setFrameShape(QFrame::HLine);
        line_overview->setFrameShadow(QFrame::Sunken);

        verticalLayout_Overview->addWidget(line_overview);

        labelMetricsTitle = new QLabel(overviewTab);
        labelMetricsTitle->setObjectName(QString::fromUtf8("labelMetricsTitle"));
        labelMetricsTitle->setAlignment(Qt::AlignCenter);

        verticalLayout_Overview->addWidget(labelMetricsTitle);

        metricsTextBrowser = new QTextBrowser(overviewTab);
        metricsTextBrowser->setObjectName(QString::fromUtf8("metricsTextBrowser"));
        metricsTextBrowser->setReadOnly(true);

        verticalLayout_Overview->addWidget(metricsTextBrowser);

        tabWidget->addTab(overviewTab, QString());
        llmInsightsTab = new QWidget();
        llmInsightsTab->setObjectName(QString::fromUtf8("llmInsightsTab"));
        verticalLayout_LLM = new QVBoxLayout(llmInsightsTab);
        verticalLayout_LLM->setObjectName(QString::fromUtf8("verticalLayout_LLM"));
        labelPositives = new QLabel(llmInsightsTab);
        labelPositives->setObjectName(QString::fromUtf8("labelPositives"));

        verticalLayout_LLM->addWidget(labelPositives);

        positivesTextBrowser = new QTextBrowser(llmInsightsTab);
        positivesTextBrowser->setObjectName(QString::fromUtf8("positivesTextBrowser"));
        positivesTextBrowser->setReadOnly(true);
        positivesTextBrowser->setMinimumSize(QSize(0, 80));

        verticalLayout_LLM->addWidget(positivesTextBrowser);

        labelImprovements = new QLabel(llmInsightsTab);
        labelImprovements->setObjectName(QString::fromUtf8("labelImprovements"));

        verticalLayout_LLM->addWidget(labelImprovements);

        improvementsTextBrowser = new QTextBrowser(llmInsightsTab);
        improvementsTextBrowser->setObjectName(QString::fromUtf8("improvementsTextBrowser"));
        improvementsTextBrowser->setReadOnly(true);
        improvementsTextBrowser->setMinimumSize(QSize(0, 80));

        verticalLayout_LLM->addWidget(improvementsTextBrowser);

        labelSuggestions = new QLabel(llmInsightsTab);
        labelSuggestions->setObjectName(QString::fromUtf8("labelSuggestions"));

        verticalLayout_LLM->addWidget(labelSuggestions);

        suggestionsTextBrowser = new QTextBrowser(llmInsightsTab);
        suggestionsTextBrowser->setObjectName(QString::fromUtf8("suggestionsTextBrowser"));
        suggestionsTextBrowser->setReadOnly(true);
        suggestionsTextBrowser->setMinimumSize(QSize(0, 80));

        verticalLayout_LLM->addWidget(suggestionsTextBrowser);

        verticalSpacer_LLM = new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_LLM->addItem(verticalSpacer_LLM);

        tabWidget->addTab(llmInsightsTab, QString());

        verticalLayout_Main->addWidget(tabWidget);

        reportStatusLabel = new QLabel(ReportDisplayDialog);
        reportStatusLabel->setObjectName(QString::fromUtf8("reportStatusLabel"));
        reportStatusLabel->setWordWrap(true);
        reportStatusLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_Main->addWidget(reportStatusLabel);

        buttonBox = new QDialogButtonBox(ReportDisplayDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);

        verticalLayout_Main->addWidget(buttonBox);


        retranslateUi(ReportDisplayDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ReportDisplayDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ReportDisplayDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ReportDisplayDialog);
    } // setupUi

    void retranslateUi(QDialog *ReportDisplayDialog)
    {
        ReportDisplayDialog->setWindowTitle(QCoreApplication::translate("ReportDisplayDialog", "\345\255\246\344\271\240\346\212\245\345\221\212", nullptr));
        labelSummaryTitle->setText(QCoreApplication::translate("ReportDisplayDialog", "<h3>AI \345\255\246\344\271\240\346\221\230\350\246\201</h3>", nullptr));
        labelMetricsTitle->setText(QCoreApplication::translate("ReportDisplayDialog", "<h3>\345\205\263\351\224\256\346\214\207\346\240\207</h3>", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(overviewTab), QCoreApplication::translate("ReportDisplayDialog", "\346\246\202\350\247\210\344\270\216\347\273\237\350\256\241", nullptr));
        labelPositives->setText(QCoreApplication::translate("ReportDisplayDialog", "<h4>\347\247\257\346\236\201\346\226\271\351\235\242\357\274\232</h4>", nullptr));
        labelImprovements->setText(QCoreApplication::translate("ReportDisplayDialog", "<h4>\345\276\205\346\224\271\350\277\233\346\226\271\351\235\242\357\274\232</h4>", nullptr));
        labelSuggestions->setText(QCoreApplication::translate("ReportDisplayDialog", "<h4>\350\241\214\345\212\250\345\273\272\350\256\256\357\274\232</h4>", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(llmInsightsTab), QCoreApplication::translate("ReportDisplayDialog", "AI \346\231\272\350\203\275\345\210\206\346\236\220\344\270\216\345\273\272\350\256\256", nullptr));
        reportStatusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ReportDisplayDialog: public Ui_ReportDisplayDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPORTDISPLAYDIALOG_H
