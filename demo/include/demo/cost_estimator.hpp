#pragma once

#include <arbitration_graphs/cost_arbitrator.hpp>

#include "environment_model.hpp"
#include "types.hpp"


namespace demo {

/**
 * @brief The CostEstimator estimates a cost quantity for a given command to be used as a decision heuristic.
 */
struct CostEstimator : public arbitration_graphs::CostEstimator<Command> {
    struct Parameters {
        /// @brief The radius of the neighborhood around the end of the path that is checked for dots.
        /// A radius of 2 will result in a 5x5 neighborhood.
        int pathEndNeighborhoodRadius{3};
    };

    explicit CostEstimator(EnvironmentModel::Ptr environmentModel, const Parameters& parameters)
            : environmentModel_{std::move(environmentModel)}, parameters_{parameters} {
    }

    /**
     * @brief Estimate the cost of a command.
     *
     * The cost is estimated using two metrics: The steps per dot along the path and the dot density at around the end
     * of the path. This is done to balance the short-term gain by following a path with many dots and the long-term
     * gain by moving towards a promising area.
     */
    double estimateCost(const Command& command, bool /*isActive*/) override;

private:
    EnvironmentModel::Ptr environmentModel_;
    Parameters parameters_;
};
} // namespace demo
