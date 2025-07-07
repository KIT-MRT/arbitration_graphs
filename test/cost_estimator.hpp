#pragma once

#include "cost_arbitrator.hpp"

#include "dummy_types.hpp"


namespace arbitration_graphs_tests {

using namespace arbitration_graphs;


struct CostEstimatorFromCostMap : public CostEstimator<DummyEnvironmentModel, DummyCommand> {
    using CostMap = std::map<DummyCommand, double>;

    explicit CostEstimatorFromCostMap(const CostMap& costMap, const double activationCosts = 0)
            : costMap_{costMap}, activationCosts_{activationCosts} {};

    virtual double estimateCost(const Time& /*time*/,
                                const DummyEnvironmentModel& environmentModel,
                                const DummyCommand& command,
                                const bool isActive) override {
        // In an actual application, you might want to get data from the environment model here to help you determine
        // the cost of a command. In this mock implementation, we simply simulate this access to show that we could do
        // so.
        environmentModel.getObservation();
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
