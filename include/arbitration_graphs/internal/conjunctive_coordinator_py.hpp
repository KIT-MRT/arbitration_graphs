#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "../arbitrator.hpp"
#include "../behavior.hpp"
#include "../conjunctive_coordinator.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyConjunctiveCoordinator : public ConjunctiveCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT> {
public:
    using BaseT = ConjunctiveCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    explicit PyConjunctiveCoordinator(const std::string& name, const VerifierT& verifier = VerifierT())
            : BaseT(name, verifier) {
    }

    // NOLINTBEGIN(readability-function-size)
    void addOption(const typename BaseT::Option::Ptr& behavior, const typename BaseT::Option::FlagsT& flags) {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "add_option", addOption, behavior, flags);
    }

    CommandT getCommand(const Time& time) override {
        PYBIND11_OVERRIDE_NAME(CommandT, BaseT, "get_command", getCommand, time);
    }

    bool checkInvocationCondition(const Time& time) const override {
        PYBIND11_OVERRIDE_NAME(bool, BaseT, "check_invocation_condition", checkInvocationCondition, time);
    }

    bool checkCommitmentCondition(const Time& time) const override {
        PYBIND11_OVERRIDE_NAME(bool, BaseT, "check_commitment_condition", checkCommitmentCondition, time);
    }

    void gainControl(const Time& time) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "gain_control", gainControl, time);
    }

    void loseControl(const Time& time) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "lose_control", loseControl, time);
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
void bindConjunctiveCoordinator(py::module& module) {
    using BehaviorT = Behavior<SubCommandT>;

    using ArbitratorT = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using ConjunctiveCoordinatorT = ConjunctiveCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using PyConjunctiveCoordinatorT = PyConjunctiveCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    using OptionT = typename ConjunctiveCoordinatorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<ConjunctiveCoordinatorT,
               ArbitratorT,
               PyConjunctiveCoordinatorT,
               std::shared_ptr<ConjunctiveCoordinatorT>>
        conjunctiveCoordinator(module, "ConjunctiveCoordinator");
    conjunctiveCoordinator
        .def(py::init<const std::string&, const VerifierT&>(),
             py::arg("name") = "ConjunctiveCoordinator",
             py::arg("verifier") = VerifierT())
        .def("add_option", &ConjunctiveCoordinatorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def("get_command", &ConjunctiveCoordinatorT::getCommand, py::arg("time"))
        .def("check_invocation_condition", &ConjunctiveCoordinatorT::checkInvocationCondition, py::arg("time"))
        .def("check_commitment_condition", &ConjunctiveCoordinatorT::checkCommitmentCondition, py::arg("time"))
        .def("gain_control", &ConjunctiveCoordinatorT::gainControl, py::arg("time"))
        .def("lose_control", &ConjunctiveCoordinatorT::loseControl, py::arg("time"))
        .def(
            "to_yaml_as_str",
            [](const ConjunctiveCoordinatorT& self, const Time& time) {
                return static_cast<const PyConjunctiveCoordinatorT&>(self).toYamlAsString(time);
            },
            py::arg("time"))
        .def("__repr__",
             [](const ConjunctiveCoordinatorT& self) { return "<ConjunctiveCoordinator '" + self.name_ + "'>"; });

    py::class_<OptionT, ArbitratorOptionT, std::shared_ptr<OptionT>> option(conjunctiveCoordinator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&>(), py::arg("behavior"), py::arg("flags"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}

} // namespace arbitration_graphs::python_api
