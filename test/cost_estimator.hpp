#pragma once

#include "dummy_types.hpp"

#include "behavior.hpp"
#include "cost_arbitrator.hpp"


namespace arbitration_graphs_tests {

using namespace arbitration_graphs;


struct CostEstimatorFromCostMap : public CostEstimator<DummyCommand> {
    using CostMap = std::map<DummyCommand, double>;

    CostEstimatorFromCostMap(const CostMap& costMap, const double activationCosts = 0)
            : costMap_{costMap}, activationCosts_{activationCosts} {};

    virtual double estimateCost(const DummyCommand& command, const bool isActive) override {
        if (isActive) {
            return costMap_.at(command) / (1 + activationCosts_);
        } else {
            return (costMap_.at(command) + activationCosts_) / (1 + activationCosts_);
        }
    }

private:
    CostMap costMap_;
    double activationCosts_;
};

} // namespace arbitration_graphs_tests
