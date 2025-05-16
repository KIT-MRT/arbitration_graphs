#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>
#include <arbitration_graphs/priority_arbitrator.hpp>

#include "command_wrapper.hpp"
#include "verification_wrapper.hpp"
#include "yaml_helper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
namespace ag = arbitration_graphs;

inline void bindPriorityArbitrator(py::module& module) {
    using Time = ag::Time;

    using ArbitratorT = ag::Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using BehaviorT = typename ArbitratorT::Behavior;

    using PriorityArbitratorT =
        ag::PriorityArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;

    using OptionT = typename PriorityArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<PriorityArbitratorT, ArbitratorT, std::shared_ptr<PriorityArbitratorT>> priorityArbitrator(
        module, "PriorityArbitrator");
    priorityArbitrator
        .def(py::init<const std::string&, const VerifierWrapper&>(),
             py::arg("name") = "PriorityArbitrator",
             py::arg("verifier") = VerifierWrapper())
        .def("add_option", &PriorityArbitratorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def(
            "to_yaml",
            [](const PriorityArbitratorT& self, const Time& time) -> py::object {
                return yaml_helper::toYamlAsPythonObject(self, time);
            },
            py::arg("time"))
        .def("__repr__", [](const PriorityArbitratorT& self) { return "<PriorityArbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, ArbitratorOptionT, std::shared_ptr<OptionT>> option(priorityArbitrator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&>(), py::arg("behavior"), py::arg("flags"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("INTERRUPTABLE", OptionT::INTERRUPTABLE)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}

} // namespace arbitration_graphs_py
