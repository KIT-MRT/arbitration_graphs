#include "demo/cost_estimator.hpp"

#include "utils/utils.hpp"

namespace demo {

double CostEstimator::estimateCost(const Time& /*time*/,
                                   const EnvironmentModel& environmentModel,
                                   const Command& command,
                                   bool /*isActive*/) {
    Positions absolutePath = environmentModel.toAbsolutePath(command.path);

    const int nDotsAlongPath = utils::dotsAlongPath(absolutePath, environmentModel);
    const int nDotsInRadius =
        utils::dotsInRadius(absolutePath.back(), environmentModel, parameters_.pathEndNeighborhoodRadius);
    const int nDots = nDotsAlongPath + nDotsInRadius;

    if (nDots == 0) {
        return std::numeric_limits<double>::max();
    }

    const int pathLength = static_cast<int>(absolutePath.size());
    const int neighborhoodSize = static_cast<int>(std::pow(2 * parameters_.pathEndNeighborhoodRadius + 1, 2));
    const int nCells = pathLength + neighborhoodSize;

    // We can define a cost as the inverse of a benefit.
    // Our benefit is a dot density (number of dots / number of examined cells)
    return static_cast<double>(nCells) / nDots;
}

} // namespace demo
