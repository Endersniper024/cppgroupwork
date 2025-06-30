#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include "core/Subject.h" // For populating subject combo box
#include "core/Task.h"
#include <QDialog>
#include <QList>


QT_BEGIN_NAMESPACE
namespace Ui {
class TaskDialog;
}
QT_END_NAMESPACE

class TaskDialog : public QDialog {
  Q_OBJECT

public:
  // Constructor for creating a new task, optionally pre-selecting a subject
  explicit TaskDialog(int currentUserId, const QList<Subject> &subjects,
                      QWidget *parent = nullptr, int defaultSubjectId = -1);
  // Constructor for editing an existing task
  explicit TaskDialog(int currentUserId, const Task &taskToEdit,
                      const QList<Subject> &subjects,
                      QWidget *parent = nullptr);
  ~TaskDialog();

  Task getTaskData() const;

private slots:
  void on_buttonBox_accepted();

private:
  Ui::TaskDialog *ui;
  Task m_task;
  int m_currentUserId;
  bool m_isEditMode;
  QList<Subject> m_availableSubjects;

  void populateSubjectComboBox(int defaultSubjectId = -1);
  void populatePriorityComboBox();
  void populateStatusComboBox();
  void initDialogWithTask(const Task &task);
  void updateProgramMonitoringInfo(int taskId);
};

#endif // TASKDIALOG_H