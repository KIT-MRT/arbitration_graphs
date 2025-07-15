#pragma once

#include <iostream>

#include <arbitration_graphs/verification.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

class VerificationResult : public arbitration_graphs::verification::Result {
public:
    explicit VerificationResult(bool isOk) : isOk_{isOk} {
    }

    bool isOk() const override {
        return isOk_;
    }

private:
    bool isOk_{false};
};


class Verifier : public arbitration_graphs::verification::Verifier<EnvironmentModel, Command> {
public:
    using Ptr = std::shared_ptr<Verifier>;
    using ConstPtr = std::shared_ptr<const Verifier>;

    arbitration_graphs::verification::Result::Ptr analyze(const Time& /*time*/,
                                                          const EnvironmentModel& environmentModel,
                                                          const Command& command) const override {
        Move nextMove = Move{command.path.front()};
        Position nextPosition = environmentModel.pacmanPosition() + nextMove.deltaPosition;

        if (environmentModel.isPassableCell(nextPosition)) {
            return std::make_shared<VerificationResult>(true);
        }

        return std::make_shared<VerificationResult>(false);
    }
};
} // namespace demo

inline std::ostream& operator<<(std::ostream& out, const demo::VerificationResult& result) {
    out << (result.isOk() ? "is okay" : "is not okay");
    return out;
}
