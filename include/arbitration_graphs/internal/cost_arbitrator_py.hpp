#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "../arbitrator.hpp"
#include "../behavior.hpp"
#include "../cost_arbitrator.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

template <typename SubCommandT>
class PyCostEstimator : public CostEstimator<SubCommandT> {
public:
    // NOLINTBEGIN(readability-function-size)
    double estimateCost(const SubCommandT& command, const bool isActive) {
        PYBIND11_OVERRIDE_PURE_NAME(
            double, CostEstimator<SubCommandT>, "estimate_cost", estimateCost, command, isActive);
    }
    // NOLINTEND(readability-function-size)
};

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyCostArbitratorOption : public CostArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option {
public:
    using BaseT = typename CostArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option;
    using FlagsT = typename BaseT::FlagsT;
    using CostEstimatorT = CostEstimator<SubCommandT>;

    explicit PyCostArbitratorOption(const typename Behavior<SubCommandT>::Ptr& behavior,
                                    const FlagsT& flags,
                                    const typename CostEstimatorT::Ptr& costEstimator)
            : BaseT(behavior, flags, costEstimator) {
    }

    // NOLINTBEGIN(readability-function-size)
    std::ostream& to_stream(std::ostream& output,
                            const Time& time,
                            const int& option_index,
                            const std::string& prefix = "",
                            const std::string& suffix = "") const override {
        PYBIND11_OVERRIDE(std::ostream&, BaseT, to_stream, output, time, option_index, prefix, suffix);
    }
    YAML::Node toYaml(const Time& time) const override {
        PYBIND11_OVERRIDE(YAML::Node, BaseT, toYaml, time);
    }
    // NOLINTEND(readability-function-size)

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << toYaml(time);
        return out.c_str();
    }
};

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyCostArbitrator
        : public arbitration_graphs::CostArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT> {
public:
    using BaseT = arbitration_graphs::CostArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using CostEstimatorT = CostEstimator<SubCommandT>;

    explicit PyCostArbitrator(const std::string& name, const VerifierT& verifier = VerifierT())
            : BaseT(name, verifier) {
    }

    // NOLINTBEGIN(readability-function-size)
    void addOption(const typename BaseT::Option::Ptr& behavior,
                   const typename BaseT::Option::FlagsT& flags,
                   const typename CostEstimator<SubCommandT>::Ptr& costEstimator) {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "add_option", addOption, behavior, flags, costEstimator);
    }

    YAML::Node toYaml(const Time& time) const override {
        PYBIND11_OVERRIDE(YAML::Node, BaseT, toYaml, time);
    }
    // NOLINTEND(readability-function-size)

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << toYaml(time);
        return out.c_str();
    }
};

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
void bindCostArbitrator(py::module& module) {
    using BehaviorT = Behavior<SubCommandT>;

    using ArbitratorT = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using CostArbitratorT = CostArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using PyCostArbitratorT = PyCostArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    using OptionT = typename CostArbitratorT::Option;
    using PyOptionT = PyCostArbitratorOption<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using FlagsT = typename OptionT::FlagsT;

    using CostEstimatorT = CostEstimator<SubCommandT>;
    using PyCostEstimatorT = PyCostEstimator<SubCommandT>;

    py::class_<CostEstimatorT, PyCostEstimatorT, std::shared_ptr<CostEstimatorT>>(module, "CostEstimator")
        .def(py::init<>());

    py::class_<CostArbitratorT, ArbitratorT, PyCostArbitratorT, std::shared_ptr<CostArbitratorT>> costArbitrator(
        module, "CostArbitrator");
    costArbitrator
        .def(py::init<const std::string&, const VerifierT&>(),
             py::arg("name") = "CostArbitrator",
             py::arg("verifier") = VerifierT())
        .def(
            "add_option", &CostArbitratorT::addOption, py::arg("behavior"), py::arg("flags"), py::arg("cost_estimator"))
        .def(
            "to_yaml_as_str",
            [](const CostArbitratorT& self, const Time& time) {
                return static_cast<const PyCostArbitratorT&>(self).toYamlAsString(time);
            },
            py::arg("time"));

    py::class_<OptionT, ArbitratorOptionT, PyOptionT, std::shared_ptr<OptionT>> option(costArbitrator, "Option");
    option
        .def(py::init<const typename BehaviorT::Ptr&, const FlagsT&, const typename CostEstimatorT::Ptr&>(),
             py::arg("behavior"),
             py::arg("flags"),
             py::arg("cost_estimator"))
        .def("to_stream",
             &OptionT::to_stream,
             py::arg("ostream"),
             py::arg("time"),
             py::arg("option_index"),
             py::arg("prefix") = "",
             py::arg("suffix") = "");

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("INTERRUPTABLE", OptionT::INTERRUPTABLE)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}


} // namespace arbitration_graphs::python_api
