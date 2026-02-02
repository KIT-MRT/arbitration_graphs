#pragma once

#include <limits>
#include <map>
#include <utility>
#include <vector>

#include "cost_arbitrator.hpp"

#include "dummy_types.hpp"


namespace arbitration_graphs_tests {

using namespace arbitration_graphs;


struct CostEstimatorFromCostMap : public CostEstimator<DummyEnvironmentModel, DummyCommand> {
    using CostMap = std::map<DummyCommand, double>;

    explicit CostEstimatorFromCostMap(CostMap costMap, const double activationCosts = 0)
            : costMap_{std::move(costMap)}, activationCosts_{activationCosts} {};
    CostEstimatorFromCostMap(const CostEstimatorFromCostMap&) = default;
    CostEstimatorFromCostMap(CostEstimatorFromCostMap&&) = default;
    CostEstimatorFromCostMap& operator=(const CostEstimatorFromCostMap&) = default;
    CostEstimatorFromCostMap& operator=(CostEstimatorFromCostMap&&) = default;
    virtual ~CostEstimatorFromCostMap() = default;

    double estimateCost(const Time& /*time*/,
                        const DummyEnvironmentModel& environmentModel,
                        const DummyCommand& command,
                        const bool isActive) override {
        // In an actual application, you might want to get data from the environment model here to help you determine
        // the cost of a command. In this mock implementation, we simply simulate this access to show that we could do
        // so.
        environmentModel.getObservation();
        if (isActive) {
            return costMap_.at(command) / (1 + activationCosts_);
        }
        return (costMap_.at(command) + activationCosts_) / (1 + activationCosts_);
    }

private:
    CostMap costMap_;
    double activationCosts_;
};

struct ScaledCostEstimatorFromCostMap : public BatchCostEstimator<DummyEnvironmentModel, DummyCommand> {
    using CandidateT = typename BatchCostEstimator<DummyEnvironmentModel, DummyCommand>::Candidate;
    using CostMap = std::map<DummyCommand, double>;

    explicit ScaledCostEstimatorFromCostMap(CostMap costMap) : costMap_(std::move(costMap)) {
    }
    ScaledCostEstimatorFromCostMap(const ScaledCostEstimatorFromCostMap&) = default;
    ScaledCostEstimatorFromCostMap(ScaledCostEstimatorFromCostMap&&) = default;
    ScaledCostEstimatorFromCostMap& operator=(const ScaledCostEstimatorFromCostMap&) = default;
    ScaledCostEstimatorFromCostMap& operator=(ScaledCostEstimatorFromCostMap&&) = default;
    virtual ~ScaledCostEstimatorFromCostMap() = default;

    std::vector<double> estimateCosts(const Time& /*time*/,
                                      const DummyEnvironmentModel& environmentModel,
                                      const std::vector<CandidateT>& candidates) override {
        // If the access to the environment model is expensive, batch-wise processing can help reduce the overhead.
        environmentModel.getObservation();

        std::vector<double> rawCosts;
        rawCosts.reserve(candidates.size());
        for (const auto& candidate : candidates) {
            rawCosts.push_back(costMap_.at(candidate.command));
        }

        // Compute max cost in the batch
        double maxCost = *std::max_element(rawCosts.begin(), rawCosts.end());

        std::vector<double> scaledCosts;
        scaledCosts.reserve(candidates.size());
        for (double cost : rawCosts) {
            scaledCosts.push_back(cost / maxCost);
        }

        return scaledCosts;
    }

private:
    CostMap costMap_;
};

} // namespace arbitration_graphs_tests
