#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>
#include <arbitration_graphs/cost_arbitrator.hpp>

#include "command_wrapper.hpp"
#include "environment_model_wrapper.hpp"
#include "yaml_helper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
namespace ag = arbitration_graphs;

class PyBatchCostEstimator : public ag::BatchCostEstimator<EnvironmentModelWrapper, CommandWrapper>,
                             public py::trampoline_self_life_support {
public:
    using BaseT = ag::BatchCostEstimator<EnvironmentModelWrapper, CommandWrapper>;
    using CandidateT = typename BaseT::Candidate;

    using BaseT::BaseT;

    virtual ~PyBatchCostEstimator() = default;
    PyBatchCostEstimator(const PyBatchCostEstimator&) = default;
    PyBatchCostEstimator(PyBatchCostEstimator&&) = default;
    PyBatchCostEstimator& operator=(const PyBatchCostEstimator&) = delete;
    PyBatchCostEstimator& operator=(PyBatchCostEstimator&&) = delete;

    // NOLINTBEGIN(readability-function-size)
    std::vector<double> estimateCosts(const ag::Time& time,
                                      const EnvironmentModelWrapper& env,
                                      const std::vector<CandidateT>& candidates) override {
        PYBIND11_OVERRIDE_PURE_NAME(std::vector<double>, BaseT, "estimate_costs", estimateCosts, time, env, candidates);
    }
    // NOLINTEND(readability-function-size)
};

/// @brief A wrapper class (a.k.a. trampoline class) for the CostEstimator class to allow Python overrides.
class PyCostEstimator : public ag::CostEstimator<EnvironmentModelWrapper, CommandWrapper>,
                        py::trampoline_self_life_support {
public:
    using BaseT = ag::CostEstimator<EnvironmentModelWrapper, CommandWrapper>;

    using BaseT::BaseT;

    virtual ~PyCostEstimator() = default;
    PyCostEstimator(const PyCostEstimator&) = default;
    PyCostEstimator(PyCostEstimator&&) = default;
    PyCostEstimator& operator=(const PyCostEstimator&) = delete;
    PyCostEstimator& operator=(PyCostEstimator&&) = delete;

    // NOLINTBEGIN(readability-function-size)
    double estimateCost(const ag::Time& time,
                        const EnvironmentModelWrapper& environmentModel,
                        const CommandWrapper& command,
                        const bool isActive) override {
        PYBIND11_OVERRIDE_PURE_NAME(
            double, BaseT, "estimate_cost", estimateCost, time, environmentModel, command, isActive);
    }
    // NOLINTEND(readability-function-size)
};

inline void bindBatchCostEstimator(py::module& module) {
    using BatchCostEstimatorT = ag::BatchCostEstimator<EnvironmentModelWrapper, CommandWrapper>;

    using CandidateT = typename BatchCostEstimatorT::Candidate;

    py::classh<BatchCostEstimatorT, PyBatchCostEstimator> batchCostEstimator(module, "BatchCostEstimator");
    batchCostEstimator.def(py::init<>())
        .def("estimate_costs",
             &BatchCostEstimatorT::estimateCosts,
             py::arg("time"),
             py::arg("environment_model"),
             py::arg("candidates"));

    py::classh<CandidateT>(batchCostEstimator, "Candidate")
        .def_readonly("command", &CandidateT::command)
        .def_readonly("is_active", &CandidateT::isActive);
}

inline void bindCostEstimator(py::module& module) {
    using CostEstimatorT = ag::CostEstimator<EnvironmentModelWrapper, CommandWrapper>;

    py::classh<CostEstimatorT, PyCostEstimator>(module, "CostEstimator")
        .def(py::init<>())
        .def("estimate_cost",
             &CostEstimatorT::estimateCost,
             py::arg("time"),
             py::arg("environment_model"),
             py::arg("command"),
             py::arg("is_active"));
}

inline void bindPlaceboCostEstimator(py::module& module) {
    using BatchCostEstimatorT = ag::BatchCostEstimator<EnvironmentModelWrapper, CommandWrapper>;
    using PlaceboCostEstimatorT = ag::PlaceboCostEstimator<EnvironmentModelWrapper, CommandWrapper>;

    py::classh<PlaceboCostEstimatorT, BatchCostEstimatorT>(module, "PlaceboCostEstimator")
        .def(py::init<>())
        .def("estimate_costs",
             &BatchCostEstimatorT::estimateCosts,
             py::arg("time"),
             py::arg("environment_model"),
             py::arg("candidates"));
}

inline void bindCostArbitrator(py::module& module) {
    using Time = ag::Time;

    using ArbitratorT = ag::Arbitrator<EnvironmentModelWrapper, CommandWrapper>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using BehaviorT = typename ArbitratorT::Behavior;

    using BatchCostEstimatorT = ag::BatchCostEstimator<EnvironmentModelWrapper, CommandWrapper>;
    using PlaceboCostEstimatorT = ag::PlaceboCostEstimator<EnvironmentModelWrapper, CommandWrapper>;
    using CostArbitratorT = ag::CostArbitrator<EnvironmentModelWrapper, CommandWrapper>;
    using CostEstimatorT = ag::CostEstimator<EnvironmentModelWrapper, CommandWrapper>;

    using OptionT = typename CostArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    using VerifierT = ag::verification::Verifier<EnvironmentModelWrapper, CommandWrapper>;
    using PlaceboVerifierT = ag::verification::PlaceboVerifier<EnvironmentModelWrapper, CommandWrapper>;

    bindBatchCostEstimator(module);
    bindCostEstimator(module);
    bindPlaceboCostEstimator(module);

    py::classh<CostArbitratorT, ArbitratorT> costArbitrator(module, "CostArbitrator");
    costArbitrator
        .def(py::init<const std::string&, const std::shared_ptr<BatchCostEstimatorT>&, const VerifierT::Ptr&>(),
             py::arg("name") = "CostArbitrator",
             py::arg("batch_cost_estimator") = std::make_shared<PlaceboCostEstimatorT>(),
             py::arg("verifier") = std::make_shared<PlaceboVerifierT>())
        .def(py::init<const std::string&, const std::shared_ptr<CostEstimatorT>&, const VerifierT::Ptr&>(),
             py::arg("name") = "CostArbitrator",
             py::arg("cost_estimator") = nullptr,
             py::arg("verifier") = std::make_shared<PlaceboVerifierT>())
        .def("add_option", &CostArbitratorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def(
            "to_yaml",
            [](const CostArbitratorT& self, const Time& time, const EnvironmentModelWrapper& environmentModel) {
                return yaml_helper::toYamlAsPythonObject(self, time, environmentModel);
            },
            py::arg("time"),
            py::arg("environment_model"))
        .def("__repr__", [](const CostArbitratorT& self) { return "<CostArbitrator '" + self.name() + "'>"; });

    py::classh<OptionT, ArbitratorOptionT> option(costArbitrator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&>(), py::arg("behavior"), py::arg("flags"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NoFlags)
        .value("INTERRUPTABLE", OptionT::Interruptable)
        .value("FALLBACK", OptionT::Fallback);
}


} // namespace arbitration_graphs_py
