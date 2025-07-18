#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>
#include <arbitration_graphs/random_arbitrator.hpp>

#include "command_wrapper.hpp"
#include "environment_model_wrapper.hpp"
#include "verification.hpp"
#include "yaml_helper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
namespace ag = arbitration_graphs;

inline void bindRandomArbitrator(py::module& module) {
    using Time = ag::Time;

    using ArbitratorT = ag::Arbitrator<EnvironmentModelWrapper, CommandWrapper>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using BehaviorT = typename ArbitratorT::Behavior;

    using RandomArbitratorT = ag::RandomArbitrator<EnvironmentModelWrapper, CommandWrapper>;

    using OptionT = typename RandomArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    using VerifierT = ag::verification::Verifier<EnvironmentModelWrapper, CommandWrapper>;
    using PlaceboVerifierT = ag::verification::PlaceboVerifier<EnvironmentModelWrapper, CommandWrapper>;

    py::classh<RandomArbitratorT, ArbitratorT> randomArbitrator(module, "RandomArbitrator");
    randomArbitrator
        .def(py::init<const std::string&, const VerifierT::Ptr&>(),
             py::arg("name") = "RandomArbitrator",
             py::arg("verifier") = PlaceboVerifierT())
        .def("add_option", &RandomArbitratorT::addOption, py::arg("behavior"), py::arg("flags"), py::arg("weight") = 1)
        .def(
            "to_yaml",
            [](const RandomArbitratorT& self, const Time& time, const EnvironmentModelWrapper& environmentModel) {
                return yaml_helper::toYamlAsPythonObject(self, time, environmentModel);
            },
            py::arg("time"),
            py::arg("environment_model"))
        .def("__repr__", [](const RandomArbitratorT& self) { return "<RandomArbitrator '" + self.name() + "'>"; });

    py::classh<OptionT, ArbitratorOptionT> option(randomArbitrator, "Option");
    option
        .def(py::init<const typename BehaviorT::Ptr&, const FlagsT&, const double&>(),
             py::arg("behavior"),
             py::arg("flags"),
             py::arg("weight"))
        .def("weight", &OptionT::weight);

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NoFlags)
        .value("INTERRUPTABLE", OptionT::Interruptable)
        .value("FALLBACK", OptionT::Fallback)
        .export_values();
}

} // namespace arbitration_graphs_py
