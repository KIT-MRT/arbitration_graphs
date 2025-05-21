#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>
#include <arbitration_graphs/random_arbitrator.hpp>

#include "command_wrapper.hpp"
#include "verification_wrapper.hpp"
#include "yaml_helper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
namespace ag = arbitration_graphs;

inline void bindRandomArbitrator(py::module& module) {
    using Time = ag::Time;

    using ArbitratorT = ag::Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using BehaviorT = typename ArbitratorT::Behavior;

    using RandomArbitratorT =
        ag::RandomArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;

    using OptionT = typename RandomArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<RandomArbitratorT, ArbitratorT, std::shared_ptr<RandomArbitratorT>> randomArbitrator(module,
                                                                                                    "RandomArbitrator");
    randomArbitrator
        .def(py::init<const std::string&, const VerifierWrapper&>(),
             py::arg("name") = "RandomArbitrator",
             py::arg("verifier") = VerifierWrapper())
        .def("add_option", &RandomArbitratorT::addOption, py::arg("behavior"), py::arg("flags"), py::arg("weight") = 1)
        .def(
            "to_yaml",
            [](const RandomArbitratorT& self, const Time& time) {
                return yaml_helper::toYamlAsPythonObject(self, time);
            },
            py::arg("time"))
        .def("__repr__", [](const RandomArbitratorT& self) { return "<RandomArbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, ArbitratorOptionT, std::shared_ptr<OptionT>> option(randomArbitrator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&, const double&>(),
               py::arg("behavior"),
               py::arg("flags"),
               py::arg("weight"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("INTERRUPTABLE", OptionT::INTERRUPTABLE)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}

} // namespace arbitration_graphs_py
