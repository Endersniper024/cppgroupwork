#ifndef CORE_H
#define CORE_H

// #include "ReportParameters.h"
// #include "ReportDataStructures.h"
// #include "DataAggregator.h"
// #include "MetricCalculator.h"
// #include "ILLMCommunicator.h" // Use the interface
// #include "LLMCommunicator.h"  // Concrete implementation
// #include "ReportEnums.h"
// #include "IDataAggregator.h"
// #include "ReportGenerator.h"

// namespace Core::Reports{
//     // class ReportGenerator;
//     enum class ReportPeriod;
// }

namespace Core::Reports { // Assuming your report classes are in this namespace
    class ReportGenerator;
    class DataAggregator;
    class MetricCalculator;
    class ILLMCommunicator; // Interface
    class LLMCommunicator;  // Concrete implementation (if MainWindow instantiates it directly)
    class CalculatedMetrics;
    class AggregatedData;
    class IDataAggregator;
    class LLMInsights;
    class LLMCommunicator;
    enum class ReportPeriod;
    class ReportEnums;

    struct ReportParameters;
    struct LearningReport;
} // namespace Core::Reports

#endif // CORE_H