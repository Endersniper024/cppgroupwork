#ifndef METRICCALCULATOR_H
#define METRICCALCULATOR_H

#include "ReportDataStructures.h"
#include "core/Subject.h" // To get subject names
#include "core/Task.h"    // To get task names

#include "Core.h" // For Core::Reports namespace

class Core::Reports::MetricCalculator {
public:
    MetricCalculator();
    CalculatedMetrics calculate(const AggregatedData& aggData,
                                const QList<Subject>& allUserSubjects, /* To map subjectId to name */
                                const QList<Task>& allUserTasks);      /* To map taskId to name */
};
#endif // METRICCALCULATOR_H