#pragma once

#include <memory>
#include <ostream>

#include "types.hpp"


namespace arbitration_graphs::verification {

class AbstractResult {
public:
    using Ptr = std::shared_ptr<AbstractResult>;
    using ConstPtr = std::shared_ptr<const AbstractResult>;

    virtual ~AbstractResult() = default;

    virtual bool isOk() const = 0;
};

template <typename DataT>
class AbstractVerifier {
public:
    using Ptr = std::shared_ptr<AbstractVerifier<DataT>>;
    using ConstPtr = std::shared_ptr<const AbstractVerifier<DataT>>;

    virtual ~AbstractVerifier() = default;

    virtual AbstractResult::Ptr analyze(const Time& time, const DataT& data) const = 0;
};

/*!
 * @brief The PlaceboResult is always okay, thus is the PlaceboVerifier a placebo (you probably guessed it).
 *
 * Use this together with PlaceboVerifier, if you don't care to verify your commands before dumping them onto your
 * robots. Otherwise these are a good starting point to implement your own meaningful verifier.
 */
class PlaceboResult : public AbstractResult {
public:
    explicit PlaceboResult(bool isOk = true) : isOk_(isOk) {
    }
    bool isOk() const override {
        return isOk_;
    };

private:
    bool isOk_{true};
};
template <typename DataT>
struct PlaceboVerifier : public AbstractVerifier<DataT> {
    AbstractResult::Ptr analyze(const Time& /*time*/, const DataT& /*data*/) const override {
        return std::make_shared<PlaceboResult>();
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
inline std::ostream& operator<<(std::ostream& out, const arbitration_graphs::verification::AbstractResult& result) {
    out << (result.isOk() ? "is okay" : "is not okay");
    return out;
}
