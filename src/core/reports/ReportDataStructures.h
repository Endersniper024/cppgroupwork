#ifndef REPORTDATASTRUCTURES_H
#define REPORTDATASTRUCTURES_H

#include <QString>
#include <QMap>
#include <QList>
#include <QDateTime>
#include <QPair>
#include "core/Task.h"     // For TaskStatus, TaskPriority (or move enums to a common place)
#include "core/TimeLog.h"  // For PomodoroCycleType
#include "core/reports/ReportParameters.h" // For ReportParameters

#include "Core.h" // For Core::Reports namespace


namespace Core::Reports {
    // Data directly aggregated from database
    struct AggregatedData {
        int totalTasksInPeriod = 0;
        int completedTasksInPeriod = 0;
        QMap<int /*subjectId*/, qint64 /*totalMinutes*/> timePerSubject;
        QMap<int /*taskId*/, qint64 /*totalMinutes*/> timePerTask; // Actual time spent
        QMap<int /*taskId*/, QPair<int /*estimatedMinutes*/, qint64 /*actualMinutes*/>> taskTimeEfficiency;
        int totalTimeLoggedMinutes = 0;

        // For more context to LLM, if needed
        QList<Task> relevantTasks; // Tasks active or completed in period
        QList<TimeLog> relevantTimeLogs; // TimeLogs within the period
    };

    // Calculated metrics based on AggregatedData
    struct CalculatedMetrics {
        double taskCompletionRate = 0.0; // (completedTasks / totalTasks) * 100
        qint64 totalTimeSpentMinutes = 0;
        QMap<QString /*subjectName*/, double /*percentageOfTotalTime*/> timeAllocationBySubjectPercentage;
        QMap<QString /*subjectName*/, qint64 /*minutesPerSubject*/> timeAllocationBySubjectMinutes;
        QMap<QString /*taskName*/, QString /*efficiencyDescription (e.g., +10min, -20%)*/> taskEfficiencySummary;
        // Could add more: e.g., productive days/hours
    };

    // Insights from LLM
    struct LLMInsights {
        // QString overallSummary;
        QString summary;
        QList<QString> positiveObservations;
        QList<QString> areasForImprovement;
        QList<QString> actionableSuggestions;
        QString rawLLMResponse; // For debugging or advanced use
        bool analysisSuccessfullyGenerated = false;
    };

    // The final report structure
    struct LearningReport {
        ReportParameters parametersUsed;
        AggregatedData aggregatedData;
        CalculatedMetrics metrics;
        LLMInsights llmAnalysis;
        QDateTime generationTimestamp;
        bool hasError = false;
        QString errorMessage;

        LearningReport() : generationTimestamp(QDateTime::currentDateTime()) {}
    };
}


#endif // REPORTDATASTRUCTURES_H