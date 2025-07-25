#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>

#include "command_wrapper.hpp"
#include "environment_model_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;

inline void bindArbitrator(py::module& module) {
    using ArbitratorT = arbitration_graphs::Arbitrator<EnvironmentModelWrapper, CommandWrapper>;

    using BehaviorT = typename ArbitratorT::Behavior;

    using OptionT = typename ArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    py::classh<ArbitratorT, // The derived class we want to bind
               BehaviorT    // The base class
               >
        arbitrator(module, "Arbitrator");
    arbitrator.def("add_option", &ArbitratorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def("options", &ArbitratorT::options)
        .def("check_invocation_condition",
             &ArbitratorT::checkInvocationCondition,
             py::arg("time"),
             py::arg("environment_model"))
        .def("check_commitment_condition",
             &ArbitratorT::checkCommitmentCondition,
             py::arg("time"),
             py::arg("environment_model"))
        .def("gain_control", &ArbitratorT::gainControl, py::arg("time"), py::arg("environment_model"))
        .def("lose_control", &ArbitratorT::loseControl, py::arg("time"), py::arg("environment_model"))
        .def("__repr__", [](const ArbitratorT& self) { return "<Arbitrator '" + self.name() + "'>"; });

    py::classh<OptionT> option(arbitrator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&>(), py::arg("behavior"), py::arg("flags"))
        .def("has_flags", &OptionT::hasFlag, py::arg("flags_to_check"))
        .def("behavior", &OptionT::behavior)
        .def("verification_result", &OptionT::verificationResult, py::arg("time"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NoFlags)
        .value("INTERRUPTABLE", OptionT::Interruptable)
        .value("FALLBACK", OptionT::Fallback);
}

} // namespace arbitration_graphs_py
