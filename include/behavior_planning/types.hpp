#pragma once

#include <chrono>


namespace behavior_planning {

using Clock = std::chrono::steady_clock;
//! duration in seconds as double
using Duration = std::chrono::duration<double, std::ratio<1>>;
using Time = std::chrono::time_point<Clock, Duration>;

} // namespace behavior_planning
