#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "../arbitrator.hpp"
#include "../behavior.hpp"
#include "../joint_coordinator.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyJointCoordinator : public JointCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT> {
public:
    using BaseT = JointCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    explicit PyJointCoordinator(const std::string& name, const VerifierT& verifier = VerifierT())
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
    // NOLINTEND(readability-function-size)

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << BaseT::toYaml(time);
        return out.c_str();
    }
};

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
void bindJointCoordinator(py::module& module) {
    using BehaviorT = Behavior<SubCommandT>;

    using ArbitratorT = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using JointCoordinatorT = JointCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using PyJointCoordinatorT = PyJointCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    using OptionT = typename JointCoordinatorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<JointCoordinatorT, ArbitratorT, PyJointCoordinatorT, std::shared_ptr<JointCoordinatorT>>
        jointCoordinator(module, "JointCoordinator");
    jointCoordinator
        .def(py::init<const std::string&, const VerifierT&>(),
             py::arg("name") = "JointCoordinator",
             py::arg("verifier") = VerifierT())
        .def("add_option", &JointCoordinatorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def("get_command", &JointCoordinatorT::getCommand, py::arg("time"))
        .def("check_invocation_condition", &JointCoordinatorT::checkInvocationCondition, py::arg("time"))
        .def("check_commitment_condition", &JointCoordinatorT::checkCommitmentCondition, py::arg("time"))
        .def("gain_control", &JointCoordinatorT::gainControl, py::arg("time"))
        .def("lose_control", &JointCoordinatorT::loseControl, py::arg("time"))
        .def("is_active", &JointCoordinatorT::isActive)
        .def(
            "to_yaml_as_str",
            [](const JointCoordinatorT& self, const Time& time) {
                return static_cast<const PyJointCoordinatorT&>(self).toYamlAsString(time);
            },
            py::arg("time"))
        .def("__repr__", [](const JointCoordinatorT& self) { return "<JointCoordinator '" + self.name_ + "'>"; });

    py::class_<OptionT, ArbitratorOptionT, std::shared_ptr<OptionT>> option(jointCoordinator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&>(), py::arg("behavior"), py::arg("flags"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}

} // namespace arbitration_graphs::python_api
