#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>

#include "command_wrapper.hpp"
#include "verification_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;

inline void bindArbitrator(py::module& module) {
    using ArbitratorT =
        arbitration_graphs::Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;
    using BehaviorT = typename ArbitratorT::Behavior;
    using OptionT = typename ArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<ArbitratorT,                  // The derived class we want to bind
               BehaviorT,                    // The base class
               std::shared_ptr<ArbitratorT>> // The holder type enabling shared ownership
        arbitrator(module, "Arbitrator");
    arbitrator.def("add_option", &ArbitratorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def("options", &ArbitratorT::options)
        .def("check_invocation_condition", &ArbitratorT::checkInvocationCondition, py::arg("time"))
        .def("check_commitment_condition", &ArbitratorT::checkCommitmentCondition, py::arg("time"))
        .def("gain_control", &ArbitratorT::gainControl, py::arg("time"))
        .def("lose_control", &ArbitratorT::loseControl, py::arg("time"))
        .def("__repr__", [](const ArbitratorT& self) { return "<Arbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, std::shared_ptr<OptionT>> option(arbitrator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&>(), py::arg("behavior"), py::arg("flags"))
        .def("has_flags", &OptionT::hasFlag, py::arg("flags_to_check"))
        .def_readwrite("behavior", &OptionT::behavior_)
        .def_readwrite("flags", &OptionT::flags_)
        .def_readwrite("verification_result", &OptionT::verificationResult_);

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("INTERRUPTABLE", OptionT::INTERRUPTABLE)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}

} // namespace arbitration_graphs_py
