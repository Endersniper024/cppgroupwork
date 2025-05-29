#ifndef DATAAGGREGATOR_H
#define DATAAGGREGATOR_H

#include "IDataAggregator.h" // Or remove interface and directly implement

#include "Core.h" // For Core::Reports namespace

class Core::Reports::DataAggregator /* : public IDataAggregator */ {
public:
    DataAggregator();
    AggregatedData aggregate(const ReportParameters& params, DatabaseManager& dbManager);
};
#endif // DATAAGGREGATOR_H