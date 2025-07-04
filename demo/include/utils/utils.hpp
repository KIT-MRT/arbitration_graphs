#pragma once

#include "demo/environment_model.hpp"
#include "demo/types.hpp"

namespace utils {

int dotsAlongPath(const Positions& absolutePath, const demo::EnvironmentModel& environmentModel);

int dotsInRadius(const Position& center, const demo::EnvironmentModel& environmentModel, int pathEndNeighborhoodRadius);

} // namespace utils
