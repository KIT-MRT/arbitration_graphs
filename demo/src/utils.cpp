#include "utils/utils.hpp"

namespace utils {

int dotsAlongPath(const Positions& absolutePath, const demo::EnvironmentModel::ConstPtr& environmentModel) {
    int nDots{0};

    for (const auto& position : absolutePath) {
        if (environmentModel->isDot(position)) {
            nDots++;
        }
    }
    return nDots;
}

int dotsInRadius(const Position& center,
                 const demo::EnvironmentModel::ConstPtr& environmentModel,
                 int pathEndNeighborhoodRadius) {
    int nDots{0};

    int radius = pathEndNeighborhoodRadius;
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            Position neighbor = {center.x + dx, center.y + dy};
            if (environmentModel->isInBounds(neighbor) && environmentModel->isDot(neighbor)) {
                nDots++;
            }
        }
    }

    return nDots;
}

} // namespace utils
