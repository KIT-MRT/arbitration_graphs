#include "demo/cost_estimator.hpp"

namespace demo {

double CostEstimator::estimateCost(const Command& command, bool /*isActive*/) {
    Positions absolutePath = toAbsolutePath(command.path);

    int dotCounter = dotsAlongPath(absolutePath) + dotsInRadius(absolutePath.back());

    if (dotCounter == 0) {
        return std::numeric_limits<double>::max();
    }

    int pathLength = static_cast<int>(absolutePath.size());
    int neighborhoodSize = static_cast<int>(std::pow(2 * parameters_.pathEndNeighborhoodRadius + 1, 2));
    int numOfCells = pathLength + neighborhoodSize;

    return static_cast<double>(numOfCells) / dotCounter;
}
Positions CostEstimator::toAbsolutePath(const Path& path) const {
    Position pacmanPosition = environmentModel_->pacmanPosition();
    Positions absolutePath{pacmanPosition};

    // Let's follow the path and convert it to absolute positions
    for (const auto& direction : path) {
        pacmanPosition = pacmanPosition + Move(direction).deltaPosition;
        pacmanPosition = environmentModel_->positionConsideringTunnel(pacmanPosition);
        absolutePath.push_back(pacmanPosition);
    }

    return absolutePath;
}

int CostEstimator::dotsAlongPath(const Positions& absolutePath) const {
    int nDots{0};

    for (const auto& position : absolutePath) {
        if (environmentModel_->isDot(position)) {
            nDots++;
        }
    }
    return nDots;
}

int CostEstimator::dotsInRadius(const Position& center) const {
    int nDots{0};

    int radius = parameters_.pathEndNeighborhoodRadius;
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            Position neighbor = {center.x + dx, center.y + dy};
            if (environmentModel_->isInBounds(neighbor) && environmentModel_->isDot(neighbor)) {
                nDots++;
            }
        }
    }

    return nDots;
}
} // namespace demo
