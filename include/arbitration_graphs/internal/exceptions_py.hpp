#pragma once

#include <pybind11/pybind11.h>

#include "../exceptions.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

inline void bindExceptions(py::module& module) {
    py::register_exception<GetCommandCalledWithoutGainControlError>(module, "GetCommandCalledWithoutGainControlError");

    py::register_exception<InvocationConditionIsFalseError>(module, "InvocationConditionIsFalseError");

    py::register_exception<MultipleReferencesToSameInstanceError>(module, "MultipleReferencesToSameInstanceError");

    py::register_exception<InvalidArgumentsError>(module, "InvalidArgumentsError");

    py::register_exception<VerificationError>(module, "VerificationError");

    py::register_exception<NoApplicableOptionPassedVerificationError>(module,
                                                                      "NoApplicableOptionPassedVerificationError");

    py::register_exception<ApplicableOptionFailedVerificationError>(module, "ApplicableOptionFailedVerificationError");
}

} // namespace arbitration_graphs::python_api
