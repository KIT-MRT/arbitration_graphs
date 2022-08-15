#pragma once

#include "types.hpp"


namespace behavior_planning::verification {

/*!
 * @brief The PlaceboResult is always okay, thus is the PlaceboVerifier a placebo (you probably guessed it).
 *
 * Use this together with PlaceboVerifier, if you don't care to verify your commands before dumping them onto your
 * robots. Otherwise these are a good starting point to implement your own meaningful verifier.
 */
struct PlaceboResult {
    static bool isOk() {
        return true;
    };
};
template <typename DataT>
struct PlaceboVerifier {
    static PlaceboResult analyze(const Time& /*time*/, const DataT& /*data*/) {
        return PlaceboResult();
    };
};

} // namespace behavior_planning::verification