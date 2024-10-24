#include "demo/cost_estimator.hpp"

#include "utils/utils.hpp"

namespace demo {

double CostEstimator::estimateCost(const Command& command, bool /*isActive*/) {
    Positions absolutePath = environmentModel_->toAbsolutePath(command.path);

    const int nDotsAlongPath = 0; ///@todo (utils.hpp might be helpful)
    const int nDotsInRadius = 0; ///@todo  (utils.hpp might be helpful)
    const int nDots = nDotsAlongPath + nDotsInRadius;

    if (nDots == 0) {
        return std::numeric_limits<double>::max();
    }

    const int pathLength = 0; ///@todo
    const int neighborhoodSize = 0; ///@todo
    const int nCells = pathLength + neighborhoodSize;

    // We can define a cost as the inverse of a benefit.
    // Our benefit is a dot density (number of dots / number of examined cells)
    return static_cast<double>(nCells) / nDots;
}

} // namespace demo
