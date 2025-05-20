#ifndef REPORTSETTINGSDIALOG_H
#define REPORTSETTINGSDIALOG_H

#include <QDialog>
#include "core/reports/ReportParameters.h" // Defines ReportParameters and ReportPeriod enum
#include "core/User.h"                    // To know the current user for context if needed
#include "core/reports/Core.h" // For Core::Reports namespace

// Forward declare UI class
QT_BEGIN_NAMESPACE
namespace Ui { class ReportSettingsDialog; }
QT_END_NAMESPACE

class ReportSettingsDialog : public QDialog {
    Q_OBJECT

public:
    // Constructor might take current user or list of subjects if filtering is added
    explicit ReportSettingsDialog(const User& currentUser, QWidget *parent = nullptr);
    ~ReportSettingsDialog();

    Core::Reports::ReportParameters getReportParameters() const;

private slots:
    void onPeriodComboBoxChanged(int index);
    void onAccepted(); // Connected to buttonBox's accepted signal

private:
    Ui::ReportSettingsDialog *ui;
    User m_currentUser;

    void setupPeriodComboBox();
    void updateDateEditsBasedOnPeriod(Core::Reports::ReportPeriod period);
};

#endif // REPORTSETTINGSDIALOG_H