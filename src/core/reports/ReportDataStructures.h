#ifndef REPORTDATASTRUCTURES_H
#define REPORTDATASTRUCTURES_H

#include "core/ProcessLink.h" // For ProcessTimeLog
#include "core/Task.h" // For TaskStatus, TaskPriority (or move enums to a common place)
#include "core/TimeLog.h"                  // For PomodoroCycleType
#include "core/reports/ReportParameters.h" // For ReportParameters
#include <QDateTime>
#include <QList>
#include <QMap>
#include <QPair>
#include <QString>


#include "Core.h" // For Core::Reports namespace

namespace Core::Reports {
// Data directly aggregated from database
struct AggregatedData {
  int totalTasksInPeriod = 0;
  int completedTasksInPeriod = 0;
  QMap<int /*subjectId*/, qint64 /*totalMinutes*/> timePerSubject;
  QMap<int /*taskId*/, qint64 /*totalMinutes*/>
      timePerTask; // Actual time spent
  QMap<int /*taskId*/,
       QPair<int /*estimatedMinutes*/, qint64 /*actualMinutes*/>>
      taskTimeEfficiency;
  int totalTimeLoggedMinutes = 0;

  // Program monitoring data
  QMap<QString /*processName*/, qint64 /*totalMinutes*/> timePerProgram;
  QMap<int /*taskId*/, qint64 /*programMinutes*/> programTimePerTask;
  int totalProgramTimeMinutes = 0;

  // For more context to LLM, if needed
  QList<Task> relevantTasks;       // Tasks active or completed in period
  QList<TimeLog> relevantTimeLogs; // TimeLogs within the period
  QList<ProcessTimeLog>
      relevantProcessTimeLogs; // Program time logs within the period
};

// Calculated metrics based on AggregatedData
struct CalculatedMetrics {
  double taskCompletionRate = 0.0; // (completedTasks / totalTasks) * 100
  qint64 totalTimeSpentMinutes = 0;
  QMap<QString /*subjectName*/, double /*percentageOfTotalTime*/>
      timeAllocationBySubjectPercentage;
  QMap<QString /*subjectName*/, qint64 /*minutesPerSubject*/>
      timeAllocationBySubjectMinutes;
  QMap<QString /*taskName*/,
       QString /*efficiencyDescription (e.g., +10min, -20%)*/>
      taskEfficiencySummary;

  // Program monitoring metrics
  qint64 totalProgramTimeMinutes = 0;
  double programTimePercentage = 0.0; // (programTime / totalTime) * 100
  QMap<QString /*programName*/, double /*percentageOfProgramTime*/>
      timeAllocationByProgramPercentage;
  QMap<QString /*programName*/, qint64 /*minutesPerProgram*/>
      timeAllocationByProgramMinutes;
  QMap<QString /*taskName*/, QString /*programTimeVsManualTime*/>
      taskProgramTimeComparison;

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
} // namespace Core::Reports

#endif // REPORTDATASTRUCTURES_H