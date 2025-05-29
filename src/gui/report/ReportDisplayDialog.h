#ifndef REPORTDISPLAYDIALOG_H
#define REPORTDISPLAYDIALOG_H

#include <QDialog>
#include "core/reports/ReportDataStructures.h" // For LearningReport
#include "core/reports/Core.h" // For Core::Reports namespace

QT_BEGIN_NAMESPACE
namespace Ui { class ReportDisplayDialog; }
QT_END_NAMESPACE

class ReportDisplayDialog : public QDialog {
    Q_OBJECT

public:
    explicit ReportDisplayDialog(const Core::Reports::LearningReport& report, QWidget *parent = nullptr);
    ~ReportDisplayDialog();

private:
    Ui::ReportDisplayDialog *ui;
    const Core::Reports::LearningReport& m_report; // Store a reference to the report data

    void populateReportData();
    QString formatMetricsForDisplay(const Core::Reports::CalculatedMetrics& metrics,
                                    const Core::Reports::AggregatedData& aggData);
    QString formatLLMInsightsForDisplay(const Core::Reports::LLMInsights& insights);
    QString formatTime(qint64 totalMinutes);
};

#endif // REPORTDISPLAYDIALOG_H