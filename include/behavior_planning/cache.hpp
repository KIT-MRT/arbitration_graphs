#pragma once

#include <optional>
#include "types.hpp"

namespace behavior_planning {

template <typename ValueType>
class Cache {
public:
    Cache() = default;

    std::optional<ValueType> cached() {
        if (value_ && cachedTime_) {
            return value_;
        }
        return std::nullopt;
    }

    std::optional<ValueType> cachedWithinOneWorkingStep(const Time& time, const int& workingFrequency) {
        if (cached() && std::chrono::duration_cast<std::chrono::milliseconds>(time - *cachedTime_).count() <=
                            static_cast<double>(1000.0 / workingFrequency)) {
            return value_;
        }
        return std::nullopt;
    }

    void cache(const ValueType& value, const Time& time) {
        value_ = value;
        cachedTime_ = time;
    }

    std::optional<Time> lastCachedTime() {
        return cachedTime_;
    }

private:
    std::optional<ValueType> value_;
    std::optional<Time> cachedTime_;
};

} // namespace behavior_planning
