#pragma once

#include <limits>
#include <stdexcept>

#include <arbitration_graphs/cost_arbitrator.hpp>

#include "environment_model.hpp"
#include "types.hpp"


namespace demo {

struct CostEstimator : public arbitration_graphs::CostEstimator<Command> {
    struct Parameters {
        /// @brief The radius of the neighborhood around the end of the path that is checked for dots.
        /// A radius of 2 will result in a 5x5 neighborhood.
        int pathEndNeighborhoodRadius{3};
    };

    explicit CostEstimator(EnvironmentModel::Ptr environmentModel, const Parameters& paramters)
            : environmentModel_{std::move(environmentModel)}, parameters_{paramters} {
    }

    double estimateCost(const Command& command, bool /*isActive*/) override;

private:
    Positions toAbsolutePath(const Path& path) const;
    int dotsAlongPath(const Positions& absolutePath) const;
    int dotsInRadius(const Position& center) const;

    EnvironmentModel::Ptr environmentModel_;
    Parameters parameters_;
};
} // namespace demo
