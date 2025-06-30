#include "DataAggregator.h"
#include "core/DatabaseManager.h"
#include <QDebug>

#include "Core.h" // For Core::Reports namespace

namespace Core::Reports {

DataAggregator::DataAggregator() {}

AggregatedData DataAggregator::aggregate(const ReportParameters &params,
                                         DatabaseManager &dbManager) {
  AggregatedData aggData;
  aggData.totalTimeLoggedMinutes = 0;

  // 1. Fetch relevant TimeLogs
  //    Adjust date range based on params.period if not CustomRange
  QDateTime startDate = params.startDate;
  QDateTime endDate = params.endDate;
  // TODO: Logic to set startDate/endDate correctly based on params.period
  // (Daily, Weekly, Monthly) For now, assume startDate and endDate are
  // correctly set in params for the desired range.

  QList<TimeLog> timeLogs =
      dbManager.getTimeLogsByUser(params.userId, startDate, endDate);
  aggData.relevantTimeLogs = timeLogs;

  for (const TimeLog &log : timeLogs) {
    aggData.totalTimeLoggedMinutes += log.durationMinutes;
    if (log.subjectId != -1) {
      aggData.timePerSubject[log.subjectId] += log.durationMinutes;
    }
    if (log.taskId != -1) {
      aggData.timePerTask[log.taskId] += log.durationMinutes;
    }
  }

  // 2. Fetch program time logs
  QList<ProcessTimeLog> processTimeLogs =
      dbManager.getProcessTimeLogsByUser(params.userId, startDate, endDate);
  aggData.relevantProcessTimeLogs = processTimeLogs;

  for (const ProcessTimeLog &log : processTimeLogs) {
    qint64 durationMinutes =
        log.durationSeconds / 60; // Convert seconds to minutes
    aggData.totalProgramTimeMinutes += durationMinutes;
    aggData.timePerProgram[log.processName] += durationMinutes;
    if (log.taskId != -1) {
      aggData.programTimePerTask[log.taskId] += durationMinutes;
    }
  }

  // 3. Fetch relevant Tasks
  //    Tasks considered "in period" could be those completed in period, or due
  //    in period, or worked on in period. Let's get all tasks for the user for
  //    now, then filter.
  QList<Task> allUserTasks = dbManager.getTasksByUser(
      params.userId, TaskStatus(-1), true /* include completed */);
  for (const Task &task : allUserTasks) {
    bool inPeriod =
        (task.creationDate <= endDate &&
         (!task.dueDate.isValid() ||
          task.dueDate >= startDate)) || // Created before/in period and due
                                         // after/in period or no due date
        (task.completionDate.isValid() && task.completionDate >= startDate &&
         task.completionDate <= endDate); // Completed in period

    if (!params.filterSubjectIds.isEmpty() &&
        !params.filterSubjectIds.contains(task.subjectId))
      continue;
    if (!params.filterTaskIds.isEmpty() &&
        !params.filterTaskIds.contains(task.id))
      continue;

    if (inPeriod) { // Simplified logic for "relevant"
      aggData.relevantTasks.append(task);
      aggData.totalTasksInPeriod++;
      if (task.status == TaskStatus::Completed &&
          task.completionDate.isValid() && task.completionDate <= endDate) {
        aggData.completedTasksInPeriod++;
      }
      if (task.estimatedTimeMinutes > 0 || task.actualTimeMinutes > 0) {
        aggData.taskTimeEfficiency[task.id] =
            qMakePair(task.estimatedTimeMinutes, task.actualTimeMinutes);
      }
    }
  }
  qDebug() << "Data Aggregation: Total Logs:" << timeLogs.size()
           << "Total Relevant Tasks:" << aggData.relevantTasks.size();
  return aggData;
}

} // namespace Core::Reports