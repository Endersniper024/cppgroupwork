#include "ReportSettingsDialog.h"
#include "ui_ReportSettingsDialog.h" // Generated from .ui file
#include <QDate>
#include <QDebug>
#include <QMessageBox>
// #include "ReportPeriod.h" // Add this include for Core::Reports::ReportPeriod

ReportSettingsDialog::ReportSettingsDialog(const User& currentUser, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReportSettingsDialog),
    m_currentUser(currentUser)
{
    ui->setupUi(this);
    setWindowTitle(tr("报告设置"));

    setupPeriodComboBox();
    // Initialize date edits to a sensible default (e.g., last week)
    onPeriodComboBoxChanged(ui->periodComboBox->findData(static_cast<int>(Core::Reports::ReportPeriod::Weekly))); // Default to weekly
    if(ui->periodComboBox->currentIndex() == -1 && ui->periodComboBox->count() > 0) { // Fallback if weekly not found
        ui->periodComboBox->setCurrentIndex(0);
    }


    connect(ui->periodComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ReportSettingsDialog::onPeriodComboBoxChanged);

    // The .ui file should connect buttonBox's accepted() signal to this dialog's accept() slot
    // And rejected() to reject(). We can add custom logic by connecting to onAccepted.
    // For validation before closing:
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ReportSettingsDialog::onAccepted);
    // reject is usually fine as default
}

ReportSettingsDialog::~ReportSettingsDialog() {
    delete ui;
}

void ReportSettingsDialog::setupPeriodComboBox() {
    ui->periodComboBox->addItem(tr("本周"), static_cast<int>(Core::Reports::ReportPeriod::Weekly));
    ui->periodComboBox->addItem(tr("上周"), static_cast<int>(Core::Reports::ReportPeriod::CustomRange)); // Will need custom logic
    ui->periodComboBox->addItem(tr("本月"), static_cast<int>(Core::Reports::ReportPeriod::Monthly));
    ui->periodComboBox->addItem(tr("上个月"), static_cast<int>(Core::Reports::ReportPeriod::CustomRange)); // Will need custom logic
    ui->periodComboBox->addItem(tr("自定义范围"), static_cast<int>(Core::Reports::ReportPeriod::CustomRange));
    // Add more predefined periods as needed
}

void ReportSettingsDialog::onPeriodComboBoxChanged(int index) {
    if (index < 0) return;
    Core::Reports::ReportPeriod selectedPeriod = static_cast<Core::Reports::ReportPeriod>(ui->periodComboBox->itemData(index).toInt());
    
    QDate today = QDate::currentDate();
    QDate startDate, endDate;

    // Special handling for "上周" and "上个月" if they are distinct from CustomRange in logic
    QString currentText = ui->periodComboBox->itemText(index);

    if (selectedPeriod == Core::Reports::ReportPeriod::Weekly || currentText == tr("本周")) {
        startDate = today.addDays(-(today.dayOfWeek() - 1)); // Monday of current week
        endDate = startDate.addDays(6);                      // Sunday of current week
        ui->customDateGroupBox->setEnabled(false);
    } else if (currentText == tr("上周")) {
        endDate = today.addDays(-(today.dayOfWeek()));      // Sunday of last week
        startDate = endDate.addDays(-6);                     // Monday of last week
        selectedPeriod = Core::Reports::ReportPeriod::CustomRange; // Treat as custom for parameter passing
        ui->customDateGroupBox->setEnabled(false); // Still set by logic, not user
    } else if (selectedPeriod == Core::Reports::ReportPeriod::Monthly || currentText == tr("本月")) {
        startDate = QDate(today.year(), today.month(), 1);
        endDate = QDate(today.year(), today.month(), today.daysInMonth());
        ui->customDateGroupBox->setEnabled(false);
    } else if (currentText == tr("上个月")) {
        QDate firstDayLastMonth = QDate(today.year(), today.month(), 1).addMonths(-1);
        startDate = firstDayLastMonth;
        endDate = QDate(firstDayLastMonth.year(), firstDayLastMonth.month(), firstDayLastMonth.daysInMonth());
        selectedPeriod = Core::Reports::ReportPeriod::CustomRange; // Treat as custom
        ui->customDateGroupBox->setEnabled(false);
    } else if (selectedPeriod == Core::Reports::ReportPeriod::CustomRange) {
        ui->customDateGroupBox->setEnabled(true);
        // Don't change dates if user explicitly chose "Custom Range"
        // unless it's the first time or dates are invalid
        if (!ui->startDateEdit->date().isValid() || !ui->endDateEdit->date().isValid()){
            ui->startDateEdit->setDate(today.addDays(-7));
            ui->endDateEdit->setDate(today);
        }
        return; // Return early as dates are user-controlled
    } else { // Default or unknown, set to custom last 7 days
        ui->customDateGroupBox->setEnabled(true);
        startDate = today.addDays(-7);
        endDate = today;
    }
    
    ui->startDateEdit->setDate(startDate);
    ui->endDateEdit->setDate(endDate);
}


Core::Reports::ReportParameters ReportSettingsDialog::getReportParameters() const {
    Core::Reports::ReportParameters params;
    params.userId = m_currentUser.id; // Set internally

    int currentIndex = ui->periodComboBox->currentIndex();
    params.period = static_cast<Core::Reports::ReportPeriod>(ui->periodComboBox->itemData(currentIndex).toInt());
    
    // For "上周" and "上个月", even though period enum might be CustomRange,
    // the dates are set correctly by onPeriodComboBoxChanged.
    // If the user selected "Custom Range" explicitly, then groupbox is enabled.
    if (ui->customDateGroupBox->isEnabled()) { // True for explicit "Custom Range"
        params.period = Core::Reports::ReportPeriod::CustomRange; // Ensure it's set
        params.startDate = QDateTime(ui->startDateEdit->date(), QTime(0, 0, 0));
        params.endDate = QDateTime(ui->endDateEdit->date(), QTime(23, 59, 59));
    } else { // Predefined period, dates were set by onPeriodComboBoxChanged
        params.startDate = QDateTime(ui->startDateEdit->date(), QTime(0,0,0));
        params.endDate = QDateTime(ui->endDateEdit->date(), QTime(23,59,59));
        // If period was "上周" or "上个月", params.period is already CustomRange
        // If it was "本周" or "本月", params.period reflects that.
    }


    params.includeLLMAnalysis = ui->includeLLMCheckBox->isChecked();
    // params.filterSubjectIds; // Implement if subject filter list is added
    return params;
}

void ReportSettingsDialog::onAccepted() {
    // Perform validation
    QDateTime startDate = QDateTime(ui->startDateEdit->date(), QTime(0, 0, 0));
    QDateTime endDate = QDateTime(ui->endDateEdit->date(), QTime(23, 59, 59));

    if (!startDate.isValid() || !endDate.isValid()) {
        QMessageBox::warning(this, tr("输入错误"), tr("请输入有效的日期。"));
        return;
    }
    if (startDate > endDate) {
        QMessageBox::warning(this, tr("输入错误"), tr("开始日期不能晚于结束日期。"));
        return;
    }
    // Add more validations if needed (e.g., date range not too large)

    QDialog::accept(); // If all validations pass
}