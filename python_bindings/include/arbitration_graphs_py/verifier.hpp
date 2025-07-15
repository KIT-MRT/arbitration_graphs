#pragma once

#include <arbitration_graphs/types.hpp>
#include <arbitration_graphs/verification.hpp>
#include <pybind11/pybind11.h>

#include "command_wrapper.hpp"
#include "environment_model_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
using namespace pybind11::literals;

namespace ag = arbitration_graphs;
namespace agv = ag::verification;


/// @brief A wrapper class (a.k.a. trampoline class) for the Result class to allow Python overrides.
class PyResult : public agv::Result, py::trampoline_self_life_support {
public:
    // NOLINTBEGIN(readability-function-size)
    bool isOk() const override {
        PYBIND11_OVERRIDE_PURE_NAME(bool, Result, "is_ok", isOk);
    }
    // NOLINTEND(readability-function-size)
};

class PyVerifier : public agv::Verifier<EnvironmentModelWrapper, CommandWrapper>, py::trampoline_self_life_support {
public:
    using VerifierT = agv::Verifier<EnvironmentModelWrapper, CommandWrapper>;

    // NOLINTBEGIN(readability-function-size)
    agv::Result::Ptr analyze(const ag::Time& time,
                             const EnvironmentModelWrapper& environmentModel,
                             const CommandWrapper& data) const override {
        PYBIND11_OVERRIDE_PURE(agv::Result::Ptr, VerifierT, analyze, time, environmentModel, data);
    }
    // NOLINTEND(readability-function-size)
};

inline void bindVerifier(py::module& module) {
    using VerifierT = agv::Verifier<EnvironmentModelWrapper, CommandWrapper>;
    using PlaceboVerifierT = agv::PlaceboVerifier<EnvironmentModelWrapper, CommandWrapper>;

    py::classh<agv::Result, PyResult>(module, "Result").def(py::init<>()).def("is_ok", &agv::Result::isOk);

    py::classh<agv::PlaceboResult, agv::Result>(module, "PlaceboResult")
        .def(py::init<bool>(), py::arg("is_ok") = true)
        .def("is_ok", &agv::PlaceboResult::isOk);

    py::classh<VerifierT, PyVerifier>(module, "Verifier")
        .def(py::init<>())
        .def("analyze", &VerifierT::analyze, py::arg("time"), py::arg("environment_model"), py::arg("data"));

    py::classh<PlaceboVerifierT, VerifierT>(module, "PlaceboVerifier").def(py::init<>());
}

} // namespace arbitration_graphs_py
