#include "TaskDialog.h"
#include "ui_TaskDialog.h" // This header defines Ui::TaskDialog
#include "core/DatabaseManager.h" // Not strictly needed if MainWindow handles DB ops

#include <QMessageBox>
#include <QDebug>

// Constructor for new task
TaskDialog::TaskDialog(int currentUserId, const QList<Subject>& subjects, QWidget *parent, int defaultSubjectId) :
    QDialog(parent),
    ui(new Ui::TaskDialog),
    m_currentUserId(currentUserId),
    m_isEditMode(false),
    m_availableSubjects(subjects)
{
    ui->setupUi(this);
    setWindowTitle(tr("创建新任务"));
    m_task.userId = m_currentUserId; // Assign current user to the task being created
    m_task.creationDate = QDateTime::currentDateTime(); // Set creation date

    populateSubjectComboBox(defaultSubjectId);
    populatePriorityComboBox();
    populateStatusComboBox();

    ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(1)); // Default due date tomorrow

    // Ensure validation controls dialog closure
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TaskDialog::on_buttonBox_accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

// Constructor for editing task
TaskDialog::TaskDialog(int currentUserId, const Task& taskToEdit, const QList<Subject>& subjects, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskDialog),
    m_task(taskToEdit), // Copy task data for editing
    m_currentUserId(currentUserId), // Should match taskToEdit.userId
    m_isEditMode(true),
    m_availableSubjects(subjects)
{
    ui->setupUi(this);
    setWindowTitle(tr("编辑任务 - %1").arg(taskToEdit.name));

    populateSubjectComboBox(taskToEdit.subjectId);
    populatePriorityComboBox();
    populateStatusComboBox();
    initDialogWithTask(taskToEdit);

    // Ensure validation controls dialog closure
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TaskDialog::on_buttonBox_accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

TaskDialog::~TaskDialog() {
    delete ui;
}

void TaskDialog::populateSubjectComboBox(int defaultSubjectId) {
    ui->subjectComboBox->clear();
    int selectionIndex = -1;
    for (int i = 0; i < m_availableSubjects.size(); ++i) {
        const Subject& subject = m_availableSubjects.at(i);
        ui->subjectComboBox->addItem(subject.name, subject.id); // Store subject ID as item data
        if (subject.id == defaultSubjectId) {
            selectionIndex = i;
        }
    }
    if (selectionIndex != -1) {
        ui->subjectComboBox->setCurrentIndex(selectionIndex);
    } else if (ui->subjectComboBox->count() > 0) {
        ui->subjectComboBox->setCurrentIndex(0); // Default to first if no match or no default
    }
}

void TaskDialog::populatePriorityComboBox() {
    ui->priorityComboBox->clear();
    ui->priorityComboBox->addItem(priorityToString(TaskPriority::Low), static_cast<int>(TaskPriority::Low));
    ui->priorityComboBox->addItem(priorityToString(TaskPriority::Medium), static_cast<int>(TaskPriority::Medium));
    ui->priorityComboBox->addItem(priorityToString(TaskPriority::High), static_cast<int>(TaskPriority::High));
    ui->priorityComboBox->setCurrentIndex(1); // Default to Medium
}

void TaskDialog::populateStatusComboBox() {
    ui->statusComboBox->clear();
    ui->statusComboBox->addItem(statusToString(TaskStatus::Pending), static_cast<int>(TaskStatus::Pending));
    ui->statusComboBox->addItem(statusToString(TaskStatus::InProgress), static_cast<int>(TaskStatus::InProgress));
    ui->statusComboBox->addItem(statusToString(TaskStatus::Completed), static_cast<int>(TaskStatus::Completed));
    ui->statusComboBox->setCurrentIndex(0); // Default to Pending
}


void TaskDialog::initDialogWithTask(const Task& task) {
    ui->nameLineEdit->setText(task.name);
    // Subject ComboBox is set by populateSubjectComboBox with task.subjectId
    ui->descriptionTextEdit->setPlainText(task.description);

    int priorityIndex = ui->priorityComboBox->findData(static_cast<int>(task.priority));
    if (priorityIndex != -1) ui->priorityComboBox->setCurrentIndex(priorityIndex);

    ui->dueDateTimeEdit->setDateTime(task.dueDate.isValid() ? task.dueDate : QDateTime::currentDateTime().addDays(1) );
    ui->estimatedTimeSpinBox->setValue(task.estimatedTimeMinutes);

    int statusIndex = ui->statusComboBox->findData(static_cast<int>(task.status));
     if (statusIndex != -1) ui->statusComboBox->setCurrentIndex(statusIndex);

    // Actual time and creation/completion dates are not typically edited directly here
}

Task TaskDialog::getTaskData() const {
    return m_task;
}

void TaskDialog::on_buttonBox_accepted() {
    ui->dialogStatusLabel->clear();
    QString name = ui->nameLineEdit->text().trimmed();

    if (name.isEmpty()) {
        ui->dialogStatusLabel->setText("任务名称不能为空！");
        return; // Keep dialog open
    }
    if (ui->subjectComboBox->currentIndex() == -1) {
        ui->dialogStatusLabel->setText("请选择一个所属科目！");
        return; // Keep dialog open
    }

    m_task.name = name;
    m_task.subjectId = ui->subjectComboBox->currentData().toInt();
    m_task.description = ui->descriptionTextEdit->toPlainText().trimmed();
    m_task.priority = static_cast<TaskPriority>(ui->priorityComboBox->currentData().toInt());
    m_task.dueDate = ui->dueDateTimeEdit->dateTime();
    m_task.estimatedTimeMinutes = ui->estimatedTimeSpinBox->value();
    
    TaskStatus newStatus = static_cast<TaskStatus>(ui->statusComboBox->currentData().toInt());
    if (m_isEditMode) { // Only allow status change in edit mode, new tasks default to Pending
        if (m_task.status != newStatus && newStatus == TaskStatus::Completed) {
            m_task.completionDate = QDateTime::currentDateTime();
        } else if (m_task.status == TaskStatus::Completed && newStatus != TaskStatus::Completed) {
            m_task.completionDate = QDateTime(); // Clear completion date if moved from completed
        }
        m_task.status = newStatus;
    } else {
         m_task.status = TaskStatus::Pending; // New tasks are always pending initially via dialog
    }


    // userId and creationDate are already set in m_task
    // If edit mode, m_task.id is already set.

    qDebug() << "TaskDialog accepted. Task Name:" << m_task.name << "SubjID:" << m_task.subjectId;
    QDialog::accept(); // If all validations pass
}