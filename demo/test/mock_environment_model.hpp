#pragma once

#include "demo/environment_model.hpp"

namespace demo {

class MockEnvironmentModel : public EnvironmentModel {
public:
    using Ptr = std::shared_ptr<MockEnvironmentModel>;
    using ConstPtr = std::shared_ptr<const MockEnvironmentModel>;

    MockEnvironmentModel() : EnvironmentModel() {
        entityPositions_.pacman = {1, 1};
        entityPositions_.blinky = {2, 2};
        entityPositions_.pinky = {3, 3};
        entityPositions_.inky = {4, 4};
        entityPositions_.clyde = {5, 5};
    }

    PositionStore positions() const {
        return entityPositions_;
    }
    void setPositions(const PositionStore& positions) {
        entityPositions_ = positions;
    }
};

} // namespace demo
