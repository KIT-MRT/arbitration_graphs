#pragma once

#include <stdexcept>

namespace behavior_planning {

class MultipleReferencesToSameInstanceError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace behavior_planning
