#include "MetricCalculator.h"
#include <QDebug>
#include <QString>


#include "Core.h" // For Core::Reports namespace

namespace Core::Reports {

MetricCalculator::MetricCalculator() {}

CalculatedMetrics
MetricCalculator::calculate(const AggregatedData &aggData,
                            const QList<Subject> &allUserSubjects,
                            const QList<Task> &allUserTasks) {
  CalculatedMetrics metrics;
  metrics.totalTimeSpentMinutes = aggData.totalTimeLoggedMinutes;

  if (aggData.totalTasksInPeriod > 0) {
    metrics.taskCompletionRate =
        (static_cast<double>(aggData.completedTasksInPeriod) /
         aggData.totalTasksInPeriod) *
        100.0;
  }

  QMap<int, QString> subjectIdToName;
  for (const Subject &s : allUserSubjects)
    subjectIdToName[s.id] = s.name;
  QMap<int, QString> taskIdToName;
  for (const Task &t : allUserTasks)
    taskIdToName[t.id] = t.name;

  for (auto it = aggData.timePerSubject.constBegin();
       it != aggData.timePerSubject.constEnd(); ++it) {
    QString subjectName = subjectIdToName.value(it.key(), "未知科目");
    metrics.timeAllocationBySubjectMinutes[subjectName] = it.value();
    if (metrics.totalTimeSpentMinutes > 0) {
      metrics.timeAllocationBySubjectPercentage[subjectName] =
          (static_cast<double>(it.value()) / metrics.totalTimeSpentMinutes) *
          100.0;
    } else {
      metrics.timeAllocationBySubjectPercentage[subjectName] = 0.0;
    }
  }

  for (auto it = aggData.taskTimeEfficiency.constBegin();
       it != aggData.taskTimeEfficiency.constEnd(); ++it) {
    QString taskName = taskIdToName.value(it.key(), "未知任务");
    int planned = it.value().first;
    qint64 actual = it.value().second;
    if (planned > 0) {
      qint64 diff = actual - planned;
      double ratio = static_cast<double>(actual) / planned;
      metrics.taskEfficiencySummary[taskName] =
          QString("计划: %1, 实际: %2 (差异: %+3d 分钟, %4%)")
              .arg(planned)
              .arg(actual)
              .arg(diff)
              .arg(QString::number(ratio * 100, 'f', 0));
    } else if (actual > 0) {
      metrics.taskEfficiencySummary[taskName] =
          QString("实际: %1 分钟 (未计划)").arg(actual);
    }
  }

  // Calculate program monitoring metrics
  metrics.totalProgramTimeMinutes = aggData.totalProgramTimeMinutes;
  qint64 totalCombinedTime =
      metrics.totalTimeSpentMinutes + metrics.totalProgramTimeMinutes;

  if (totalCombinedTime > 0) {
    metrics.programTimePercentage =
        (static_cast<double>(metrics.totalProgramTimeMinutes) /
         totalCombinedTime) *
        100.0;
  }

  // Program time allocation by program
  for (auto it = aggData.timePerProgram.constBegin();
       it != aggData.timePerProgram.constEnd(); ++it) {
    metrics.timeAllocationByProgramMinutes[it.key()] = it.value();
    if (metrics.totalProgramTimeMinutes > 0) {
      metrics.timeAllocationByProgramPercentage[it.key()] =
          (static_cast<double>(it.value()) / metrics.totalProgramTimeMinutes) *
          100.0;
    } else {
      metrics.timeAllocationByProgramPercentage[it.key()] = 0.0;
    }
  }

  // Compare program time vs manual time for each task
  for (auto it = aggData.programTimePerTask.constBegin();
       it != aggData.programTimePerTask.constEnd(); ++it) {
    QString taskName = taskIdToName.value(it.key(), "未知任务");
    qint64 programTime = it.value();
    qint64 manualTime = aggData.timePerTask.value(it.key(), 0);

    if (programTime > 0 || manualTime > 0) {
      if (programTime > 0 && manualTime > 0) {
        metrics.taskProgramTimeComparison[taskName] =
            QString("手动计时: %1分钟, 程序计时: %2分钟")
                .arg(manualTime)
                .arg(programTime);
      } else if (programTime > 0) {
        metrics.taskProgramTimeComparison[taskName] =
            QString("仅程序计时: %1分钟").arg(programTime);
      } else {
        metrics.taskProgramTimeComparison[taskName] =
            QString("仅手动计时: %1分钟").arg(manualTime);
      }
    }
  }

  qDebug() << "Metrics Calculation: Completion Rate:"
           << metrics.taskCompletionRate;
  qDebug() << "Program Time Percentage:" << metrics.programTimePercentage;

  return metrics;
}

} // namespace Core::Reports