#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>
#include <arbitration_graphs/cost_arbitrator.hpp>

#include "command_wrapper.hpp"
#include "verification_wrapper.hpp"
#include "yaml_helper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
namespace ag = arbitration_graphs;

/// @brief A wrapper class (a.k.a. trampoline class) for the CostEstimator class to allow Python overrides.
class PyCostEstimator : public ag::CostEstimator<CommandWrapper> {
public:
    // NOLINTBEGIN(readability-function-size)
    double estimateCost(const CommandWrapper& command, const bool isActive) override {
        PYBIND11_OVERRIDE_PURE_NAME(
            double, CostEstimator<CommandWrapper>, "estimate_cost", estimateCost, command, isActive);
    }
    // NOLINTEND(readability-function-size)
};

inline void bindCostArbitrator(py::module& module) {
    using Time = ag::Time;

    using ArbitratorT = ag::Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using BehaviorT = typename ArbitratorT::Behavior;

    using CostArbitratorT =
        ag::CostArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;
    using CostEstimatorT = ag::CostEstimator<CommandWrapper>;

    using OptionT = typename CostArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<CostEstimatorT, PyCostEstimator, std::shared_ptr<CostEstimatorT>>(module, "CostEstimator")
        .def(py::init<>());

    py::class_<CostArbitratorT, ArbitratorT, std::shared_ptr<CostArbitratorT>> costArbitrator(module, "CostArbitrator");
    costArbitrator
        .def(py::init<const std::string&, const VerifierWrapper&>(),
             py::arg("name") = "CostArbitrator",
             py::arg("verifier") = VerifierWrapper())
        .def(
            "add_option", &CostArbitratorT::addOption, py::arg("behavior"), py::arg("flags"), py::arg("cost_estimator"))
        .def(
            "to_yaml",
            [](const CostArbitratorT& self, const Time& time) { return yaml_helper::toYamlAsPythonObject(self, time); },
            py::arg("time"))
        .def("__repr__", [](const CostArbitratorT& self) { return "<CostArbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, ArbitratorOptionT, std::shared_ptr<OptionT>> option(costArbitrator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&, const typename CostEstimatorT::Ptr&>(),
               py::arg("behavior"),
               py::arg("flags"),
               py::arg("cost_estimator"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("INTERRUPTABLE", OptionT::INTERRUPTABLE)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}


} // namespace arbitration_graphs_py
