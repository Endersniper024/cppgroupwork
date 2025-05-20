#ifndef REPORTPARAMETERS_H
#define REPORTPARAMETERS_H

#include <QDateTime>
#include <QList>
#include "ReportEnums.h" // Make sure this header defines ReportPeriod before this include

#include "Core.h" // For Core::Reports namespace

struct Core::Reports::ReportParameters {
    int userId = -1;
    Core::Reports::ReportPeriod period = Core::Reports::ReportPeriod::Weekly;
    QDateTime startDate; // Used if period is CustomRange or to define specific week/month
    QDateTime endDate;
    QList<int> filterSubjectIds; // Optional: empty means all subjects
    QList<int> filterTaskIds;    // Optional: empty means all tasks for selected subjects
    bool includeLLMAnalysis = true;
};

#endif // REPORTPARAMETERS_H