#include "ReportGenerator.h"
#include <QCoreApplication> // For processEvents in sync mode
#include <QTimer>           // For timeout in sync mode
#include <QDebug>

#include "Core.h" // For Core::Reports namespace

namespace Core::Reports {
    ReportGenerator::ReportGenerator(DatabaseManager& dbManager,
                                    DataAggregator& aggregator,
                                    MetricCalculator& calculator,
                                    ILLMCommunicator& llmCommunicator,
                                    QObject *parent)
        : QObject(parent), m_dbManager(dbManager), m_aggregator(aggregator),
        m_calculator(calculator), m_llmCommunicator(llmCommunicator),
        m_waitingForLLM(false), m_currentRequestId(0), m_llmEventLoop(nullptr)
    {
        connect(&m_llmCommunicator, &ILLMCommunicator::insightsReady, this, &ReportGenerator::handleLLMInsightsReady);
        connect(&m_llmCommunicator, &ILLMCommunicator::insightsError, this, &ReportGenerator::handleLLMInsightsError);
    }

    void ReportGenerator::generateReportAsync(const ReportParameters& params) {
        m_currentParams = params;
        m_currentReport = LearningReport(); // Reset
        m_currentReport.parametersUsed = params;
        m_waitingForLLM = false;

        // Fetch all subjects and tasks for the user once for context (names)
        QList<Subject> allUserSubjects = m_dbManager.getSubjectsByUser(params.userId);
        QList<Task> allUserTasks = m_dbManager.getTasksByUser(params.userId, TaskStatus(-1), true);


        m_currentReport.aggregatedData = m_aggregator.aggregate(params, m_dbManager);
        if (m_currentReport.aggregatedData.relevantTasks.isEmpty() && m_currentReport.aggregatedData.relevantTimeLogs.isEmpty()) {
            m_currentReport.hasError = true;
            m_currentReport.errorMessage = tr("在选定时期内没有找到相关数据。");
            emit reportReady(m_currentReport); // Emit report with no data message
            return;
        }

        m_currentReport.metrics = m_calculator.calculate(m_currentReport.aggregatedData, allUserSubjects, allUserTasks);

        if (params.includeLLMAnalysis) {
            m_waitingForLLM = true;
            m_currentRequestId++; // Simple request ID
            m_llmCommunicator.requestInsights(m_currentReport.metrics, m_currentReport.aggregatedData, m_currentRequestId);
        } else {
            m_currentReport.llmAnalysis.summary = tr("LLM 分析未启用。");
            emit reportReady(m_currentReport);
        }
    }

    // Simplified synchronous wrapper around async LLM call.
    // WARNING: This will block the calling thread. Use with caution, preferably not on main UI thread.
    LearningReport ReportGenerator::generateReportSync(const ReportParameters& params) {
        m_currentParams = params;
        m_currentReport = LearningReport(); // Reset
        m_currentReport.parametersUsed = params;
        m_waitingForLLM = false;
        m_llmEventLoop = new QEventLoop(this); // Event loop for waiting

        QList<Subject> allUserSubjects = m_dbManager.getSubjectsByUser(params.userId);
        QList<Task> allUserTasks = m_dbManager.getTasksByUser(params.userId, TaskStatus(-1), true);

        m_currentReport.aggregatedData = m_aggregator.aggregate(params, m_dbManager);
        if (m_currentReport.aggregatedData.relevantTasks.isEmpty() && m_currentReport.aggregatedData.relevantTimeLogs.isEmpty()) {
            m_currentReport.hasError = true;
            m_currentReport.errorMessage = tr("在选定时期内没有找到相关数据。");
            if(m_llmEventLoop) { m_llmEventLoop->quit(); m_llmEventLoop->deleteLater(); m_llmEventLoop = nullptr;}
            return m_currentReport;
        }
        m_currentReport.metrics = m_calculator.calculate(m_currentReport.aggregatedData, allUserSubjects, allUserTasks);

        if (params.includeLLMAnalysis) {
            m_waitingForLLM = true;
            m_currentRequestId++;
            m_llmCommunicator.requestInsights(m_currentReport.metrics, m_currentReport.aggregatedData, m_currentRequestId);
            
            QTimer timeoutTimer;
            timeoutTimer.setSingleShot(true);
            connect(&timeoutTimer, &QTimer::timeout, m_llmEventLoop, [this](){
                if(m_waitingForLLM) { // If still waiting when timer fires
                    qWarning() << "LLM request timed out in synchronous call.";
                    m_currentReport.llmAnalysis.summary = tr("LLM 请求超时。");
                    m_currentReport.llmAnalysis.analysisSuccessfullyGenerated = false;
                    m_waitingForLLM = false; // Stop waiting
                    if(m_llmEventLoop && m_llmEventLoop->isRunning()) m_llmEventLoop->quit();
                }
            });
            timeoutTimer.start(30000); // 30-second timeout for LLM

            if (m_waitingForLLM) { // Only exec if still waiting (might have completed very fast)
                m_llmEventLoop->exec(); // Block here until insightsReady/Error or timeout
            }
            timeoutTimer.stop();


        } else {
            m_currentReport.llmAnalysis.summary = tr("LLM 分析未启用。");
        }

        if(m_llmEventLoop) { m_llmEventLoop->deleteLater(); m_llmEventLoop = nullptr; }
        return m_currentReport;
    }


    void ReportGenerator::handleLLMInsightsReady(int requestId, const LLMInsights& insights) {
        if (m_waitingForLLM && requestId == m_currentRequestId) {
            m_currentReport.llmAnalysis = insights;
            m_waitingForLLM = false;
            if (m_llmEventLoop && m_llmEventLoop->isRunning()) {
                m_llmEventLoop->quit();
            } else { // Async path
                emit reportReady(m_currentReport);
            }
        }
    }

    void ReportGenerator::handleLLMInsightsError(int requestId, const QString& errorMessage) {
        if (m_waitingForLLM && requestId == m_currentRequestId) {
            m_currentReport.llmAnalysis.summary = tr("LLM 分析失败: %1").arg(errorMessage);
            m_currentReport.llmAnalysis.analysisSuccessfullyGenerated = false;
            m_currentReport.hasError = true; // Can mark the overall report or just LLM part
            m_currentReport.errorMessage = m_currentReport.llmAnalysis.summary;
            m_waitingForLLM = false;
            if (m_llmEventLoop && m_llmEventLoop->isRunning()) {
                m_llmEventLoop->quit();
            } else { // Async path
                emit reportReady(m_currentReport); // Emit report, LLM part will show error
            }
        }
    }
} // namespace Core::Reports