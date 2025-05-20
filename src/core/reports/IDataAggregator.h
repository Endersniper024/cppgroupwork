#ifndef IDATAAGGREGATOR_H
#define IDATAAGGREGATOR_H

#include "ReportParameters.h"
#include "ReportDataStructures.h"
// Forward declare DatabaseManager if not including its header directly

#include "Core.h" // For Core::Reports namespace

class DatabaseManager;

class Core::Reports::IDataAggregator {
public:
    virtual ~IDataAggregator() = default;
    virtual AggregatedData aggregate(const ReportParameters& params, DatabaseManager& dbManager) = 0;
};
#endif // IDATAAGGREGATOR_H