#pragma once

#include <iostream>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

class VerificationResult {
public:
    explicit VerificationResult(bool isOk) : isOk_{isOk} {
    }

    bool isOk() const {
        return isOk_;
    }

private:
    bool isOk_{false};
};


class Verifier {
public:
    explicit Verifier(EnvironmentModel::Ptr environmentModel) : environmentModel_{std::move(environmentModel)} {
    }

    VerificationResult analyze(const Time /*time*/, const Command& command) const {
        Move nextMove = Move{command.path.front()};
        Position nextPosition = environmentModel_->pacmanPosition() + nextMove.deltaPosition;

        ///@todo Verify the next position is valid

        return VerificationResult{false};
    }

private:
    EnvironmentModel::Ptr environmentModel_;
};
} // namespace demo

inline std::ostream& operator<<(std::ostream& out, const demo::VerificationResult& result) {
    out << (result.isOk() ? "is okay" : "is not okay");
    return out;
}
