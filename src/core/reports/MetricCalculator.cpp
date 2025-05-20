#include "MetricCalculator.h"
#include <QString>
#include <QDebug>

#include "Core.h" // For Core::Reports namespace

namespace Core::Reports {

    MetricCalculator::MetricCalculator() {}

    CalculatedMetrics MetricCalculator::calculate(const AggregatedData& aggData,
                                                const QList<Subject>& allUserSubjects,
                                                const QList<Task>& allUserTasks) {
        CalculatedMetrics metrics;
        metrics.totalTimeSpentMinutes = aggData.totalTimeLoggedMinutes;

        if (aggData.totalTasksInPeriod > 0) {
            metrics.taskCompletionRate = (static_cast<double>(aggData.completedTasksInPeriod) / aggData.totalTasksInPeriod) * 100.0;
        }

        QMap<int, QString> subjectIdToName;
        for(const Subject& s : allUserSubjects) subjectIdToName[s.id] = s.name;
        QMap<int, QString> taskIdToName;
        for(const Task& t : allUserTasks) taskIdToName[t.id] = t.name;


        for (auto it = aggData.timePerSubject.constBegin(); it != aggData.timePerSubject.constEnd(); ++it) {
            QString subjectName = subjectIdToName.value(it.key(), "未知科目");
            metrics.timeAllocationBySubjectMinutes[subjectName] = it.value();
            if (metrics.totalTimeSpentMinutes > 0) {
                metrics.timeAllocationBySubjectPercentage[subjectName] = (static_cast<double>(it.value()) / metrics.totalTimeSpentMinutes) * 100.0;
            } else {
                metrics.timeAllocationBySubjectPercentage[subjectName] = 0.0;
            }
        }

        for (auto it = aggData.taskTimeEfficiency.constBegin(); it != aggData.taskTimeEfficiency.constEnd(); ++it) {
            QString taskName = taskIdToName.value(it.key(), "未知任务");
            int planned = it.value().first;
            qint64 actual = it.value().second;
            if (planned > 0) {
                qint64 diff = actual - planned;
                double ratio = static_cast<double>(actual) / planned;
                metrics.taskEfficiencySummary[taskName] = QString("计划: %1, 实际: %2 (差异: %+3d 分钟, %4%)")
                                                        .arg(planned).arg(actual).arg(diff).arg(QString::number(ratio * 100, 'f', 0));
            } else if (actual > 0) {
                metrics.taskEfficiencySummary[taskName] = QString("实际: %1 分钟 (未计划)").arg(actual);
            }
        }
        qDebug() << "Metrics Calculation: Completion Rate:" << metrics.taskCompletionRate;
        return metrics;
    }

} // namespace Core::Reports