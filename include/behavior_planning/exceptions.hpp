#pragma once

#include <stdexcept>

namespace behavior_planning {

class GetCommandCalledWithoutGainControlError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class InvocationConditionIsFalseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class MultipleReferencesToSameInstanceError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class InvalidArgumentsError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class VerificationError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class NoApplicableOptionPassedVerificationError : public VerificationError {
    using VerificationError::VerificationError;
};

class ApplicableOptionFailedVerificationError : public VerificationError {
    using VerificationError::VerificationError;
};

} // namespace behavior_planning
