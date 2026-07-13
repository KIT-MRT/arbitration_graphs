#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>
#include <arbitration_graphs/sequence_arbitrator.hpp>

#include "command_wrapper.hpp"
#include "environment_model_wrapper.hpp"
#include "verification.hpp"
#include "yaml_helper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
namespace ag = arbitration_graphs;

inline void bindSequenceArbitrator(py::module& module) {
    using Time = ag::Time;

    using ArbitratorT = ag::Arbitrator<EnvironmentModelWrapper, CommandWrapper>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using BehaviorT = typename ArbitratorT::Behavior;

    using SequenceArbitratorT = ag::SequenceArbitrator<EnvironmentModelWrapper, CommandWrapper>;

    using OptionT = typename SequenceArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    using VerifierT = ag::verification::Verifier<EnvironmentModelWrapper, CommandWrapper>;
    using PlaceboVerifierT = ag::verification::PlaceboVerifier<EnvironmentModelWrapper, CommandWrapper>;

    py::classh<SequenceArbitratorT, ArbitratorT> sequenceArbitrator(module, "SequenceArbitrator");
    sequenceArbitrator
        .def(py::init<const std::string&, const VerifierT::Ptr&>(),
             py::arg("name") = "SequenceArbitrator",
             py::arg("verifier") = PlaceboVerifierT())
        .def("add_option", &SequenceArbitratorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def(
            "to_yaml",
            [](const SequenceArbitratorT& self, const Time& time, const EnvironmentModelWrapper& environmentModel) {
                return yaml_helper::toYamlAsPythonObject(self, time, environmentModel);
            },
            py::arg("time"),
            py::arg("environment_model"))
        .def("__repr__",
             [](const SequenceArbitratorT& self) { return "<SequenceArbitrator '" + self.name() + "'>"; });

    py::classh<OptionT, ArbitratorOptionT> option(sequenceArbitrator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&>(), py::arg("behavior"), py::arg("flags"));

    py::enum_<typename OptionT::Flags>(option, "Flags").value("NO_FLAGS", OptionT::NoFlags);
}

} // namespace arbitration_graphs_py
