#ifndef ILLMCOMMUNICATOR_H
#define ILLMCOMMUNICATOR_H

#include "ReportDataStructures.h" // For CalculatedMetrics, AggregatedData, LLMInsights
#include <QString>
#include <QObject> // To support signals/slots for async operations

#include "Core.h"

class Core::Reports::ILLMCommunicator : public QObject {
    Q_OBJECT
public:
    explicit ILLMCommunicator(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ILLMCommunicator() = default;

    virtual void configure(const QString& apiKey, const QString& apiUrl, const QString& modelName = "") = 0;
    // Asynchronous request
    virtual void requestInsights(const CalculatedMetrics& metrics, const AggregatedData& contextForLLM, int requestId = 0) = 0;

signals:
    void insightsReady(int requestId, const LLMInsights& insights);
    void insightsError(int requestId, const QString& errorMessage);
};

#endif // ILLMCOMMUNICATOR_H