#include "ReportDisplayDialog.h"
#include "ui_ReportDisplayDialog.h" // Generated from .ui file
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextCursor>
#include <QTextTable>


ReportDisplayDialog::ReportDisplayDialog(
    const Core::Reports::LearningReport &report, QWidget *parent)
    : QDialog(parent), ui(new Ui::ReportDisplayDialog), m_report(report) {
  ui->setupUi(this);
  setWindowTitle(
      tr("学习报告 (%1 - %2)")
          .arg(m_report.parametersUsed.startDate.toString("yyyy-MM-dd"))
          .arg(m_report.parametersUsed.endDate.toString("yyyy-MM-dd")));

  // Assuming your .ui file has a QTabWidget named 'tabWidget' and QTextBrowsers
  // within tabs. e.g.,
  // ui->tabWidget->findChild<QTextBrowser*>("summaryTextBrowser"); For
  // simplicity, I'll set object names in the .ui file like: Tab1 (objectName:
  // overviewTab) -> summaryTextBrowser, metricsTextBrowser Tab2 (objectName:
  // llmInsightsTab) -> positivesTextBrowser, improvementsTextBrowser,
  // suggestionsTextBrowser reportStatusLabel at the bottom.

  populateReportData();

  // Optional: Add a "Copy All" button or action
  QPushButton *copyButton =
      ui->buttonBox->addButton(tr("复制报告"), QDialogButtonBox::ActionRole);
  connect(copyButton, &QPushButton::clicked, this, [this]() {
    QString fullReportText;
    // Tab 1: Overview and Metrics
    fullReportText += tr("概览与统计\n===================\n");
    if (ui->summaryTextBrowser)
      fullReportText += ui->summaryTextBrowser->toPlainText() + "\n\n";
    if (ui->metricsTextBrowser)
      fullReportText += ui->metricsTextBrowser->toPlainText() + "\n\n";

    // Tab 2: LLM Insights
    fullReportText += tr("AI 智能分析与建议\n=========================\n");
    if (ui->positivesTextBrowser)
      fullReportText +=
          tr("积极方面:\n") + ui->positivesTextBrowser->toPlainText() + "\n\n";
    if (ui->improvementsTextBrowser)
      fullReportText += tr("待改进方面:\n") +
                        ui->improvementsTextBrowser->toPlainText() + "\n\n";
    if (ui->suggestionsTextBrowser)
      fullReportText += tr("行动建议:\n") +
                        ui->suggestionsTextBrowser->toPlainText() + "\n\n";

    if (m_report.hasError && !m_report.errorMessage.isEmpty()) {
      fullReportText += tr("\n错误信息:\n") + m_report.errorMessage + "\n";
    }

    QApplication::clipboard()->setText(fullReportText.trimmed());
    ui->reportStatusLabel->setText(tr("报告已复制到剪贴板!"));
  });
}

ReportDisplayDialog::~ReportDisplayDialog() { delete ui; }

QString ReportDisplayDialog::formatTime(qint64 totalMinutes) {
  if (totalMinutes < 0)
    totalMinutes = 0;
  qint64 hours = totalMinutes / 60;
  qint64 minutes = totalMinutes % 60;
  if (hours > 0) {
    return tr("%1小时 %2分钟").arg(hours).arg(minutes);
  } else {
    return tr("%1分钟").arg(minutes);
  }
}

void ReportDisplayDialog::populateReportData() {
  // --- Populate Overview & Metrics Tab ---
  if (ui->summaryTextBrowser) { // Assuming this QTextBrowser exists in your .ui
                                // file
    if (m_report.llmAnalysis.analysisSuccessfullyGenerated &&
        !m_report.llmAnalysis.summary.isEmpty()) {
      ui->summaryTextBrowser->setHtml(
          QString("<h3>%1</h3><p>%2</p>")
              .arg(tr("AI 学习摘要"))
              .arg(m_report.llmAnalysis.summary.toHtmlEscaped().replace(
                  "\n", "<br>")));
    } else if (m_report.parametersUsed.includeLLMAnalysis) {
      ui->summaryTextBrowser->setHtml(
          QString("<p><i>%1</i></p>")
              .arg(m_report.llmAnalysis.summary.isEmpty()
                       ? tr("未能生成AI学习摘要。")
                       : m_report.llmAnalysis.summary.toHtmlEscaped()));
    } else {
      ui->summaryTextBrowser->setHtml(
          QString("<p><i>%1</i></p>").arg(tr("AI智能分析未启用。")));
    }
  }

  if (ui->metricsTextBrowser) { // Assuming this QTextBrowser exists
    QString metricsHtml = QString("<h3>%1</h3>").arg(tr("关键指标"));
    metricsHtml +=
        QString("<p><b>%1:</b> %2%</p>")
            .arg(tr("任务完成率"))
            .arg(QString::number(m_report.metrics.taskCompletionRate, 'f', 1));
    metricsHtml += QString("<p><b>%1:</b> %2 (共 %3 个任务, 完成 %4 个)</p>")
                       .arg(tr("本期任务"))
                       .arg(formatTime(m_report.metrics.totalTimeSpentMinutes))
                       .arg(m_report.aggregatedData.totalTasksInPeriod)
                       .arg(m_report.aggregatedData.completedTasksInPeriod);

    // Add program monitoring data
    if (m_report.metrics.totalProgramTimeMinutes > 0) {
      metricsHtml +=
          QString("<p><b>%1:</b> %2 (占总时间 %3%)</p>")
              .arg(tr("程序自动计时"))
              .arg(formatTime(m_report.metrics.totalProgramTimeMinutes))
              .arg(QString::number(m_report.metrics.programTimePercentage, 'f',
                                   1));
    }

    metricsHtml += QString("<h4>%1:</h4>").arg(tr("各科目时间分配"));
    if (m_report.metrics.timeAllocationBySubjectMinutes.isEmpty()) {
      metricsHtml += QString("<p><i>%1</i></p>").arg(tr("无科目时长数据。"));
    } else {
      QTextTableFormat tableFormat;
      tableFormat.setAlignment(Qt::AlignLeft);
      tableFormat.setHeaderRowCount(1);
      tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 95));

      // Use QTextDocument to build table, then set HTML
      QTextDocument tempDoc;
      QTextCursor cursor(&tempDoc);
      cursor.insertHtml(metricsHtml); // Insert existing HTML first

      QTextTable *subjectTable = cursor.insertTable(
          m_report.metrics.timeAllocationBySubjectMinutes.size() + 1, 3,
          tableFormat);
      // Header
      subjectTable->cellAt(0, 0).firstCursorPosition().insertText(tr("科目"));
      subjectTable->cellAt(0, 1).firstCursorPosition().insertText(tr("用时"));
      subjectTable->cellAt(0, 2).firstCursorPosition().insertText(tr("占比"));
      int row = 1;
      for (auto it =
               m_report.metrics.timeAllocationBySubjectMinutes.constBegin();
           it != m_report.metrics.timeAllocationBySubjectMinutes.constEnd();
           ++it) {
        subjectTable->cellAt(row, 0).firstCursorPosition().insertText(it.key());
        subjectTable->cellAt(row, 1).firstCursorPosition().insertText(
            formatTime(it.value()));
        subjectTable->cellAt(row, 2).firstCursorPosition().insertText(
            QString::number(
                m_report.metrics.timeAllocationBySubjectPercentage.value(
                    it.key(), 0.0),
                'f', 1) +
            "%");
        row++;
      }
      metricsHtml = tempDoc.toHtml(); // Get the full HTML including the table
    }

    // Add program time allocation
    if (!m_report.metrics.timeAllocationByProgramMinutes.isEmpty()) {
      metricsHtml += QString("<h4>%1:</h4>").arg(tr("各程序时间分配"));

      QTextDocument tempDocProgram;
      QTextCursor cursorProgram(&tempDocProgram);
      cursorProgram.insertHtml(metricsHtml);

      QTextTableFormat programTableFormat;
      programTableFormat.setAlignment(Qt::AlignLeft);
      programTableFormat.setHeaderRowCount(1);
      programTableFormat.setWidth(
          QTextLength(QTextLength::PercentageLength, 95));

      QTextTable *programTable = cursorProgram.insertTable(
          m_report.metrics.timeAllocationByProgramMinutes.size() + 1, 3,
          programTableFormat);
      // Header
      programTable->cellAt(0, 0).firstCursorPosition().insertText(tr("程序"));
      programTable->cellAt(0, 1).firstCursorPosition().insertText(tr("用时"));
      programTable->cellAt(0, 2).firstCursorPosition().insertText(
          tr("占程序时间比"));
      int programRow = 1;
      for (auto it =
               m_report.metrics.timeAllocationByProgramMinutes.constBegin();
           it != m_report.metrics.timeAllocationByProgramMinutes.constEnd();
           ++it) {
        programTable->cellAt(programRow, 0)
            .firstCursorPosition()
            .insertText(it.key());
        programTable->cellAt(programRow, 1)
            .firstCursorPosition()
            .insertText(formatTime(it.value()));
        programTable->cellAt(programRow, 2)
            .firstCursorPosition()
            .insertText(
                QString::number(
                    m_report.metrics.timeAllocationByProgramPercentage.value(
                        it.key(), 0.0),
                    'f', 1) +
                "%");
        programRow++;
      }
      metricsHtml = tempDocProgram.toHtml();
    }

    // Add task program vs manual time comparison
    if (!m_report.metrics.taskProgramTimeComparison.isEmpty()) {
      metricsHtml += QString("<h4>%1:</h4>").arg(tr("任务计时对比"));
      metricsHtml += "<ul>";
      for (auto it = m_report.metrics.taskProgramTimeComparison.constBegin();
           it != m_report.metrics.taskProgramTimeComparison.constEnd(); ++it) {
        metricsHtml +=
            QString("<li><b>%1:</b> %2</li>").arg(it.key()).arg(it.value());
      }
      metricsHtml += "</ul>";
    }
    ui->metricsTextBrowser->setHtml(metricsHtml);
  }

  // --- Populate LLM Insights Tab ---
  if (m_report.llmAnalysis.analysisSuccessfullyGenerated) {
    if (ui->positivesTextBrowser) {
      QString positivesHtml =
          m_report.llmAnalysis.positiveObservations.isEmpty()
              ? QString("<em>%1</em>").arg(tr("暂无"))
              : "<ul><li>" +
                    m_report.llmAnalysis.positiveObservations.join(
                        "</li><li>") +
                    "</li></ul>";
      ui->positivesTextBrowser->setHtml(positivesHtml);
    }
    if (ui->improvementsTextBrowser) {
      QString improvementsHtml =
          m_report.llmAnalysis.areasForImprovement.isEmpty()
              ? QString("<em>%1</em>").arg(tr("暂无"))
              : "<ul><li>" +
                    m_report.llmAnalysis.areasForImprovement.join("</li><li>") +
                    "</li></ul>";
      ui->improvementsTextBrowser->setHtml(improvementsHtml);
    }
    if (ui->suggestionsTextBrowser) {
      QString suggestionsHtml =
          m_report.llmAnalysis.actionableSuggestions.isEmpty()
              ? QString("<em>%1</em>").arg(tr("暂无"))
              : "<ul><li>" +
                    m_report.llmAnalysis.actionableSuggestions.join(
                        "</li><li>") +
                    "</li></ul>";
      ui->suggestionsTextBrowser->setHtml(suggestionsHtml);
    }
  } else {
    QString noLLMData = QString("<em>%1</em>")
                            .arg(m_report.llmAnalysis.summary.isEmpty()
                                     ? tr("LLM分析数据不可用。")
                                     : m_report.llmAnalysis.summary);
    if (ui->positivesTextBrowser)
      ui->positivesTextBrowser->setHtml(noLLMData);
    if (ui->improvementsTextBrowser)
      ui->improvementsTextBrowser->setHtml(noLLMData);
    if (ui->suggestionsTextBrowser)
      ui->suggestionsTextBrowser->setHtml(noLLMData);
  }

  // --- Report Status Label ---
  if (m_report.hasError && !m_report.errorMessage.isEmpty()) {
    ui->reportStatusLabel->setText(
        tr("报告生成时出现错误: %1").arg(m_report.errorMessage));
    ui->reportStatusLabel->setStyleSheet("color: red;");
  } else if (m_report.aggregatedData.relevantTimeLogs.isEmpty() &&
             m_report.aggregatedData.relevantTasks.isEmpty()) {
    ui->reportStatusLabel->setText(
        tr("注意: 在选定时期内没有找到用于生成报告的数据。"));
    ui->reportStatusLabel->setStyleSheet("color: orange;");
  } else {
    ui->reportStatusLabel->setText(
        tr("报告生成于: %1")
            .arg(m_report.generationTimestamp.toString("yyyy-MM-dd hh:mm:ss")));
  }
}