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


/// @brief A wrapper class (a.k.a. trampoline class) for the AbstractResult class to allow Python overrides.
class PyAbstractResult : public agv::AbstractResult, py::trampoline_self_life_support {
public:
    // NOLINTBEGIN(readability-function-size)
    bool isOk() const override {
        PYBIND11_OVERRIDE_PURE_NAME(bool, AbstractResult, "is_ok", isOk);
    }
    // NOLINTEND(readability-function-size)
};

class PyAbstractVerifier : public agv::AbstractVerifier<EnvironmentModelWrapper, CommandWrapper>,
                           py::trampoline_self_life_support {
public:
    using AbstractVerifierT = agv::AbstractVerifier<EnvironmentModelWrapper, CommandWrapper>;

    // NOLINTBEGIN(readability-function-size)
    agv::AbstractResult::Ptr analyze(const ag::Time& time,
                                     const EnvironmentModelWrapper& environmentModel,
                                     const CommandWrapper& data) const override {
        PYBIND11_OVERRIDE_PURE(agv::AbstractResult::Ptr, AbstractVerifierT, analyze, time, environmentModel, data);
    }
    // NOLINTEND(readability-function-size)
};

inline void bindVerifier(py::module& module) {
    using AbstractVerifierT = agv::AbstractVerifier<EnvironmentModelWrapper, CommandWrapper>;
    using PlaceboVerifierT = agv::PlaceboVerifier<EnvironmentModelWrapper, CommandWrapper>;

    py::classh<agv::AbstractResult, PyAbstractResult>(module, "AbstractResult")
        .def(py::init<>())
        .def("is_ok", &agv::AbstractResult::isOk);

    py::classh<agv::PlaceboResult, agv::AbstractResult>(module, "PlaceboResult")
        .def(py::init<bool>(), py::arg("is_ok") = true)
        .def("is_ok", &agv::PlaceboResult::isOk);

    py::classh<AbstractVerifierT, PyAbstractVerifier>(module, "AbstractVerifier")
        .def(py::init<>())
        .def("analyze", &AbstractVerifierT::analyze, py::arg("time"), py::arg("environment_model"), py::arg("data"));

    py::classh<PlaceboVerifierT, AbstractVerifierT>(module, "PlaceboVerifier").def(py::init<>());
}

} // namespace arbitration_graphs_py
