#pragma once

#include <ostream>
#include <stdexcept>

#include <arbitration_graphs/types.hpp>
#include <arbitration_graphs/verification.hpp>
#include <pybind11/pybind11.h>

#include "command_wrapper.hpp"

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

class PyAbstractVerifier : public agv::AbstractVerifier<CommandWrapper>, py::trampoline_self_life_support {
public:
    using AbstractVerifierT = agv::AbstractVerifier<CommandWrapper>;

    // NOLINTBEGIN(readability-function-size)
    agv::AbstractResult::Ptr analyze(const ag::Time& time, const CommandWrapper& data) const override {
        PYBIND11_OVERRIDE_PURE(agv::AbstractResult::Ptr, AbstractVerifierT, analyze, time, data);
    }
    // NOLINTEND(readability-function-size)
};

inline void bindVerifier(py::module& module) {
    using AbstractVerifierT = agv::AbstractVerifier<CommandWrapper>;
    using PlaceboVerifierT = agv::PlaceboVerifier<CommandWrapper>;

    py::class_<agv::AbstractResult, PyAbstractResult, py::smart_holder>(module, "AbstractResult")
        .def(py::init<>())
        .def("is_ok", &agv::AbstractResult::isOk);

    py::class_<agv::PlaceboResult, agv::AbstractResult, py::smart_holder>(module, "PlaceboResult")
        .def(py::init<bool>(), py::arg("is_ok") = true)
        .def("is_ok", &agv::PlaceboResult::isOk);

    py::class_<AbstractVerifierT, PyAbstractVerifier, py::smart_holder>(module, "AbstractVerifier")
        .def(py::init<>())
        .def("analyze", &AbstractVerifierT::analyze);

    py::class_<PlaceboVerifierT, AbstractVerifierT, py::smart_holder>(module, "PlaceboVerifier").def(py::init<>());
}

} // namespace arbitration_graphs_py
