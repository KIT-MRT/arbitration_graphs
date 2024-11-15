#pragma once

#include "demo/environment_model.hpp"
#include "demo/types.hpp"

namespace utils {

int dotsAlongPath(const Positions& absolutePath, const demo::EnvironmentModel::ConstPtr& environmentModel);

int dotsInRadius(const Position& center,
                 const demo::EnvironmentModel::ConstPtr& environmentModel,
                 int pathEndNeighborhoodRadius);

void printKeybindings();

} // namespace utils
