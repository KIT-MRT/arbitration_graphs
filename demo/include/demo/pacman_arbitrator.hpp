#pragma once

#include "types.hpp"

namespace demo {


class PacmanArbitrator {
public:
    Command getCommand() {
        return Command{Command::Direction::UP};
    }
};

} // namespace demo