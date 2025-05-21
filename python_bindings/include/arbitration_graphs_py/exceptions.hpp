#pragma once

#include <pybind11/pybind11.h>

#include <arbitration_graphs/exceptions.hpp>

namespace arbitration_graphs_py {

namespace py = pybind11;
namespace ag = arbitration_graphs;

inline void bindExceptions(py::module& module) {
    py::register_exception<ag::GetCommandCalledWithoutGainControlError>(module,
                                                                        "GetCommandCalledWithoutGainControlError");
    py::register_exception<ag::InvocationConditionIsFalseError>(module, "InvocationConditionIsFalseError");
    py::register_exception<ag::MultipleReferencesToSameInstanceError>(module, "MultipleReferencesToSameInstanceError");
    py::register_exception<ag::InvalidArgumentsError>(module, "InvalidArgumentsError");
    py::register_exception<ag::VerificationError>(module, "VerificationError");
    py::register_exception<ag::NoApplicableOptionPassedVerificationError>(module,
                                                                          "NoApplicableOptionPassedVerificationError");
    py::register_exception<ag::ApplicableOptionFailedVerificationError>(module,
                                                                        "ApplicableOptionFailedVerificationError");
}

} // namespace arbitration_graphs_py
