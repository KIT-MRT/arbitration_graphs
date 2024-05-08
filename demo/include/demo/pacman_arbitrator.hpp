#pragma once

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {


class PacmanArbitrator {
public:
    Command getCommand() {
        return Command{Direction::UP};
    }
    void updateEnvironmentModel(entt::registry& registry) {
        environmentModel.update(registry);
    }

private:
    EnvironmentModel environmentModel;
};

} // namespace demo