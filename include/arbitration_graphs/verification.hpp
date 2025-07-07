#pragma once

#include <memory>
#include <ostream>

#include "types.hpp"


namespace arbitration_graphs::verification {

class Result {
public:
    using Ptr = std::shared_ptr<Result>;
    using ConstPtr = std::shared_ptr<const Result>;

    Result() = default;
    Result(const Result&) = default;
    Result(Result&&) = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) = default;
    virtual ~Result() = default;

    virtual bool isOk() const = 0;
};

template <typename EnvironmentModelT, typename CommandT>
class Verifier {
public:
    using Ptr = std::shared_ptr<Verifier>;
    using ConstPtr = std::shared_ptr<const Verifier>;

    Verifier() = default;
    Verifier(const Verifier&) = default;
    Verifier(Verifier&&) = default;
    Verifier& operator=(const Verifier&) = default;
    Verifier& operator=(Verifier&&) = default;
    virtual ~Verifier() = default;

    virtual Result::Ptr analyze(const Time& time,
                                const EnvironmentModelT& environmentModel,
                                const CommandT& command) const = 0;
};

class SimpleResult : public Result {
public:
    explicit SimpleResult(bool isOk) : isOk_(isOk) {
    }
    bool isOk() const override {
        return isOk_;
    };

private:
    bool isOk_{true};
};

/*!
 * @brief The PlaceboVerifier is a dummy verifier that always returns a Result that is "okay".
 *
 * Use this, if you don't care to verify your commands before running them onto your
 * robots. Otherwise this is a good starting point to implement your own meaningful verifier.
 */
template <typename EnvironmentModelT, typename CommandT>
struct PlaceboVerifier : public Verifier<EnvironmentModelT, CommandT> {
    Result::Ptr analyze(const Time& /*time*/,
                        const EnvironmentModelT& /*environmentModel*/,
                        const CommandT& /*command*/) const override {
        return std::make_shared<SimpleResult>(true);
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
inline std::ostream& operator<<(std::ostream& out, const arbitration_graphs::verification::Result& result) {
    out << (result.isOk() ? "is okay" : "is not okay");
    return out;
}
