#pragma once

#include <cmath>
#include <functional>
#include <optional>
#include "types.hpp"

namespace behavior_planning {

/*!
 * \brief The Cache class is one templated class for caching computed values that can be accessed by
 *        giving the same or close enough key
 *
 * In general, the value and key that to be cached can be any type.
 */
template <typename ValueType, typename KeyType>
class Cache {
public:
    Cache() = default;

    // different policies for comparing the key
    struct Policy {
        // The ApproximateTime struct is one example that can be used as key for the Cache
        struct ApproximateTime {
            explicit ApproximateTime(const double threshold) : threshold_{threshold} {
            }

            // defines the policy that two time points is close enough
            bool operator()(const Time& lhs, const Time& rhs) const {
                const double diff = std::chrono::duration_cast<std::chrono::milliseconds>(lhs - rhs).count();
                return std::fabs(diff) <= threshold_;
            }

        private:
            double threshold_; // threshold in miliseconds
        };
    };

    /*!
     * \brief Check if there is already value cached that is close enough to the key according the ComparisonPolicy
     *
     * \return  std::nullopt if not cached, otherwise return the cached value
     */
    template <class ComparisonPolicy = std::equal_to<KeyType>>
    std::optional<ValueType> cached(const KeyType& key, const ComparisonPolicy& equal = std::equal_to<KeyType>()) {
        if (value_ && key_ && equal(key, *key_)) {
            return value_;
        }
        return std::nullopt;
    }

    /*!
     * \brief Cache the value and key
     *
     * \param value  The value that is to be cached
     * \param key  The key that is correlated to the value
     */
    void cache(const ValueType& value, const KeyType& key) {
        value_ = value;
        key_ = key;
    }

private:
    std::optional<ValueType> value_;
    std::optional<KeyType> key_;
};

} // namespace behavior_planning
