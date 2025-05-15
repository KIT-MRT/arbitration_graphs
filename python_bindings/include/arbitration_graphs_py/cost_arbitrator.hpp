#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>
#include <arbitration_graphs/cost_arbitrator.hpp>

#include "command_wrapper.hpp"
#include "verification_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
using namespace arbitration_graphs;

class PyCostEstimator : public CostEstimator<CommandWrapper> {
public:
    // NOLINTBEGIN(readability-function-size)
    double estimateCost(const CommandWrapper& command, const bool isActive) override {
        PYBIND11_OVERRIDE_PURE_NAME(
            double, CostEstimator<CommandWrapper>, "estimate_cost", estimateCost, command, isActive);
    }
    // NOLINTEND(readability-function-size)
};

class PyCostArbitratorOption
        : public CostArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>::Option {
public:
    using BaseT =
        typename CostArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>::Option;
    using FlagsT = typename BaseT::FlagsT;
    using CostEstimatorT = CostEstimator<CommandWrapper>;

    explicit PyCostArbitratorOption(const typename Behavior<CommandWrapper>::Ptr& behavior,
                                    const FlagsT& flags,
                                    const typename CostEstimatorT::Ptr& costEstimator)
            : BaseT(behavior, flags, costEstimator) {
    }

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << BaseT::toYaml(time);
        return out.c_str();
    }
};

class PyCostArbitrator
        : public CostArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper> {
public:
    using BaseT = CostArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;
    using CostEstimatorT = CostEstimator<CommandWrapper>;

    explicit PyCostArbitrator(const std::string& name, const VerifierWrapper& verifier = VerifierWrapper())
            : BaseT(name, verifier) {
    }

    // NOLINTBEGIN(readability-function-size)
    void addOption(const typename BaseT::Behavior::Ptr& behavior,
                   const typename BaseT::Option::FlagsT& flags,
                   const typename CostEstimator<CommandWrapper>::Ptr& costEstimator) {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "add_option", addOption, behavior, flags, costEstimator);
    }
    // NOLINTEND(readability-function-size)

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << BaseT::toYaml(time);
        return out.c_str();
    }
};

inline void bindCostArbitrator(py::module& module) {
    using BehaviorT = Behavior<CommandWrapper>;

    using ArbitratorT = Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using CostArbitratorT = CostArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;

    using OptionT = typename CostArbitratorT::Option;
    using PyOptionT = PyCostArbitratorOption;
    using FlagsT = typename OptionT::FlagsT;

    using CostEstimatorT = CostEstimator<CommandWrapper>;

    py::class_<CostEstimatorT, PyCostEstimator, std::shared_ptr<CostEstimatorT>>(module, "CostEstimator")
        .def(py::init<>());

    py::class_<CostArbitratorT, ArbitratorT, PyCostArbitrator, std::shared_ptr<CostArbitratorT>> costArbitrator(
        module, "CostArbitrator");
    costArbitrator
        .def(py::init<const std::string&, const VerifierWrapper&>(),
             py::arg("name") = "CostArbitrator",
             py::arg("verifier") = VerifierWrapper())
        .def(
            "add_option", &CostArbitratorT::addOption, py::arg("behavior"), py::arg("flags"), py::arg("cost_estimator"))
        .def(
            "to_yaml_as_str",
            [](const PyCostArbitrator& self, const Time& time) { return self.toYamlAsString(time); },
            py::arg("time"))
        .def("__repr__", [](const CostArbitratorT& self) { return "<CostArbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, ArbitratorOptionT, PyOptionT, std::shared_ptr<OptionT>> option(costArbitrator, "Option");
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
