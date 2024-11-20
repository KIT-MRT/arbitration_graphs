#pragma once

#include <ostream>

#include "types.hpp"


namespace arbitration_graphs::verification {

/*!
 * @brief The PlaceboResult is always okay, thus is the PlaceboVerifier a placebo (you probably guessed it).
 *
 * Use this together with PlaceboVerifier, if you don't care to verify your commands before dumping them onto your
 * robots. Otherwise these are a good starting point to implement your own meaningful verifier.
 */
struct PlaceboResult {
    bool isOk() const {
        return isOk_;
    };

    bool isOk_{true};
};
template <typename DataT>
struct PlaceboVerifier {
    static PlaceboResult analyze(const Time& /*time*/, const DataT& /*data*/) {
        return PlaceboResult();
    };
};

} // namespace arbitration_graphs::verification


/**
 * @brief Output stream operator, allows to print a Result object to e.g. std::cout
 *
 * @code {.cpp}
 * std::cout << result << std::endl;
 * @endcode
 */
inline std::ostream& operator<<(std::ostream& out, const arbitration_graphs::verification::PlaceboResult& result) {
    out << (result.isOk() ? "is okay" : "is not okay");
    return out;
}
