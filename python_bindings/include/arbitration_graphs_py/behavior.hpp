#pragma once

#include <arbitration_graphs/behavior.hpp>
#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "command_wrapper.hpp"
#include "environment_model_wrapper.hpp"
#include "yaml_helper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
namespace ag = arbitration_graphs;

/// @brief A wrapper class (a.k.a. trampoline class) for the Behavior class to allow Python overrides
class PyBehavior : public ag::Behavior<EnvironmentModelWrapper, CommandWrapper>, py::trampoline_self_life_support {
public:
    using BaseT = ag::Behavior<EnvironmentModelWrapper, CommandWrapper>;

    using BaseT::BaseT;

    virtual ~PyBehavior() = default;
    PyBehavior(const PyBehavior&) = default;
    PyBehavior(PyBehavior&&) = default;
    PyBehavior& operator=(const PyBehavior&) = delete;
    PyBehavior& operator=(PyBehavior&&) = delete;

    // NOLINTBEGIN(readability-function-size)
    CommandWrapper getCommand(const ag::Time& time, const EnvironmentModelWrapper& environmentModel) override {
        PYBIND11_OVERRIDE_PURE_NAME(CommandWrapper, BaseT, "get_command", getCommand, time, environmentModel);
    }

    bool checkInvocationCondition(const ag::Time& time,
                                  const EnvironmentModelWrapper& environmentModel) const override {
        PYBIND11_OVERRIDE_NAME(
            bool, BaseT, "check_invocation_condition", checkInvocationCondition, time, environmentModel);
    }

    bool checkCommitmentCondition(const ag::Time& time,
                                  const EnvironmentModelWrapper& environmentModel) const override {
        PYBIND11_OVERRIDE_NAME(
            bool, BaseT, "check_commitment_condition", checkCommitmentCondition, time, environmentModel);
    }

    void gainControl(const ag::Time& time, const EnvironmentModelWrapper& environmentModel) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "gain_control", gainControl, time, environmentModel);
    }

    void loseControl(const ag::Time& time, const EnvironmentModelWrapper& environmentModel) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "lose_control", loseControl, time, environmentModel);
    }

    std::string to_str(const ag::Time& time,
                       const EnvironmentModelWrapper& environmentModel,
                       const std::string& prefix = "",
                       const std::string& suffix = "") const override {
        PYBIND11_OVERRIDE(std::string, BaseT, to_str, time, environmentModel, prefix, suffix);
    }
    // NOLINTEND(readability-function-size)
};

inline void bindBehavior(py::module& module) {
    using BehaviorT = ag::Behavior<EnvironmentModelWrapper, CommandWrapper>;

    py::classh<BehaviorT,  // The base class
               PyBehavior> // The trampoline class enabling Python overrides
        (module, "Behavior")
            .def(py::init<const std::string&>(), py::arg("name") = "Behavior")
            .def(
                "get_command",
                [](BehaviorT& self, const ag::Time& time, const EnvironmentModelWrapper& environmentModel) {
                    return self.getCommand(time, environmentModel).value();
                },
                py::arg("time"),
                py::arg("environment_model"))
            .def("check_invocation_condition",
                 &BehaviorT::checkInvocationCondition,
                 py::arg("time"),
                 py::arg("environment_model"))
            .def("check_commitment_condition",
                 &BehaviorT::checkCommitmentCondition,
                 py::arg("time"),
                 py::arg("environment_model"))
            .def("gain_control", &BehaviorT::gainControl, py::arg("time"), py::arg("environment_model"))
            .def("lose_control", &BehaviorT::loseControl, py::arg("time"), py::arg("environment_model"))
            .def("to_str",
                 &BehaviorT::to_str,
                 py::arg("time"),
                 py::arg("environment_model"),
                 py::arg("prefix") = "",
                 py::arg("suffix") = "")
            .def(
                "to_yaml",
                [](const BehaviorT& self, const ag::Time& time, const EnvironmentModelWrapper& environmentModel) {
                    return yaml_helper::toYamlAsPythonObject(self, time, environmentModel);
                },
                py::arg("time"),
                py::arg("environment_model"))
            .def("name", &BehaviorT::name)
            .def("__repr__", [](const BehaviorT& self) { return "<Behavior '" + self.name() + "'>"; });
}

} // namespace arbitration_graphs_py
