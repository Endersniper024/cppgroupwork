#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include "ReportParameters.h"
#include "ReportDataStructures.h"
#include "DataAggregator.h"
#include "MetricCalculator.h"
#include "ILLMCommunicator.h" // Use the interface
#include "core/DatabaseManager.h"
#include <QObject>
#include <QEventLoop> // For synchronous waiting on async LLM if needed simply

#include "Core.h" // For Core::Reports namespace


namespace Core::Reports {
    class ReportGenerator : public QObject {
        Q_OBJECT
    public:
        explicit ReportGenerator(DatabaseManager& dbManager,
                                DataAggregator& aggregator,
                                MetricCalculator& calculator,
                                ILLMCommunicator& llmCommunicator, // Pass by reference
                                QObject *parent = nullptr);

        // Generates the report; might be async if LLM call is async
        void generateReportAsync(const ReportParameters& params);
        LearningReport generateReportSync(const ReportParameters& params); // Blocking version for simplicity

    signals:
        void reportReady(const LearningReport& report);
        void reportFailed(const QString& errorMessage);

    private slots:
        void handleLLMInsightsReady(int requestId, const LLMInsights& insights);
        void handleLLMInsightsError(int requestId, const QString& errorMessage);

    private:
        DatabaseManager& m_dbManager;
        DataAggregator& m_aggregator;
        MetricCalculator& m_calculator;
        ILLMCommunicator& m_llmCommunicator;

        // For managing async report generation state
        LearningReport m_currentReport;
        ReportParameters m_currentParams;
        bool m_waitingForLLM;
        int m_currentRequestId;
        QEventLoop* m_llmEventLoop; // For synchronous wrapper
    };
}

#endif // REPORTGENERATOR_H