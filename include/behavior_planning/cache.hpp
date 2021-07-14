#pragma once

#include <optional>
#include "types.hpp"

namespace behavior_planning {

/*!
 * \brief The Cache class is one templated class for caching computed values that can be accessed at a later time point
 *
 * In general, the value that need to be cached can be any type.
 */
template <typename ValueType>
class Cache {
public:
    Cache() = default;

    /*!
     * \brief Check if there is already value cached
     *
     * \return  std::nullopt if not cached, otherwise return the cached value
     */
    std::optional<ValueType> cached() {
        if (value_ && cachedTime_) {
            return value_;
        }
        return std::nullopt;
    }

    /*!
     * \brief Check if there is value cached that is within some time from the last cached value and time point, the
     *        time limit is defined by a certain working frequency
     *
     * \param time  The time point where the cached value is to be checked out
     * \param workingFrequency  Defines the time limit from last cached time point
     * \return  std::nullopt if not cached close enough to the param time, otherwise return the cached value
     */
    std::optional<ValueType> cachedWithinOneWorkingStep(const Time& time, const int& workingFrequency) {
        if (cached() && std::chrono::duration_cast<std::chrono::milliseconds>(time - *cachedTime_).count() <=
                            static_cast<double>(1000.0 / workingFrequency)) {
            return value_;
        }
        return std::nullopt;
    }

    /*!
     * \brief Cache the value and time point
     *
     * \param value  The value that is to be cached
     * \param time  The time point that is correlated to the value that is cached
     */
    void cache(const ValueType& value, const Time& time) {
        value_ = value;
        cachedTime_ = time;
    }

    /*!
     * \brief Checkout the last cached time point
     *
     * \return std::nullopt if no time point cached, otherwise the optinal cached time point
     */
    std::optional<Time> lastCachedTime() {
        return cachedTime_;
    }

private:
    std::optional<ValueType> value_;
    std::optional<Time> cachedTime_;
};

} // namespace behavior_planning
