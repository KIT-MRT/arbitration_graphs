#pragma once

#include <iostream>

#include <arbitration_graphs/verification.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

class VerificationResult : public arbitration_graphs::verification::AbstractResult {
public:
    explicit VerificationResult(bool isOk) : isOk_{isOk} {
    }

    bool isOk() const override {
        return isOk_;
    }

private:
    bool isOk_{false};
};


class Verifier : public arbitration_graphs::verification::AbstractVerifier<Command> {
public:
    using Ptr = std::shared_ptr<Verifier>;
    using ConstPtr = std::shared_ptr<const Verifier>;

    explicit Verifier(EnvironmentModel::Ptr environmentModel) : environmentModel_{std::move(environmentModel)} {
    }

    arbitration_graphs::verification::AbstractResult::Ptr analyze(const Time& /*time*/,
                                                                  const Command& command) const override {
        Move nextMove = Move{command.path.front()};
        Position nextPosition = environmentModel_->pacmanPosition() + nextMove.deltaPosition;

        if (environmentModel_->isPassableCell(nextPosition)) {
            return std::make_shared<VerificationResult>(true);
        }

        return std::make_shared<VerificationResult>(false);
    }

private:
    EnvironmentModel::Ptr environmentModel_;
};
} // namespace demo

inline std::ostream& operator<<(std::ostream& out, const demo::VerificationResult& result) {
    out << (result.isOk() ? "is okay" : "is not okay");
    return out;
}
