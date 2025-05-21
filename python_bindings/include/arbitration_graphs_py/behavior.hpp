#pragma once

#include <arbitration_graphs/behavior.hpp>
#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "command_wrapper.hpp"
#include "yaml_helper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
namespace ag = arbitration_graphs;

/// @brief A wrapper class (a.k.a. trampoline class) for the Behavior class to allow Python overrides
class PyBehavior : public ag::Behavior<CommandWrapper> {
public:
    using BaseT = ag::Behavior<CommandWrapper>;

    using Behavior::Behavior;

    // NOLINTBEGIN(readability-function-size)
    CommandWrapper getCommand(const ag::Time& time) override {
        PYBIND11_OVERRIDE_PURE_NAME(CommandWrapper, BaseT, "get_command", getCommand, time);
    }

    bool checkInvocationCondition(const ag::Time& time) const override {
        PYBIND11_OVERRIDE_NAME(bool, BaseT, "check_invocation_condition", checkInvocationCondition, time);
    }

    bool checkCommitmentCondition(const ag::Time& time) const override {
        PYBIND11_OVERRIDE_NAME(bool, BaseT, "check_commitment_condition", checkCommitmentCondition, time);
    }

    void gainControl(const ag::Time& time) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "gain_control", gainControl, time);
    }

    void loseControl(const ag::Time& time) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "lose_control", loseControl, time);
    }

    std::string to_str(const ag::Time& time,
                       const std::string& prefix = "",
                       const std::string& suffix = "") const override {
        PYBIND11_OVERRIDE(std::string, BaseT, to_str, time, prefix, suffix);
    }
    // NOLINTEND(readability-function-size)
};

inline void bindBehavior(py::module& module) {
    using BehaviorT = ag::Behavior<CommandWrapper>;

    py::class_<BehaviorT,                  // The base class
               PyBehavior,                 // The trampoline class enabling Python overrides
               std::shared_ptr<BehaviorT>> // The holder type enabling shared ownership
        (module, "Behavior")
            .def(py::init<const std::string&>(), py::arg("name") = "Behavior")
            .def(
                "get_command",
                [](BehaviorT& self, const ag::Time& time) { return self.getCommand(time).value(); },
                py::arg("time"))
            .def("check_invocation_condition", &BehaviorT::checkInvocationCondition, py::arg("time"))
            .def("check_commitment_condition", &BehaviorT::checkCommitmentCondition, py::arg("time"))
            .def("gain_control", &BehaviorT::gainControl, py::arg("time"))
            .def("lose_control", &BehaviorT::loseControl, py::arg("time"))
            .def("to_str", &BehaviorT::to_str, py::arg("time"), py::arg("prefix") = "", py::arg("suffix") = "")
            .def(
                "to_yaml",
                [](const BehaviorT& self, const ag::Time& time) {
                    return yaml_helper::toYamlAsPythonObject(self, time);
                },
                py::arg("time"))
            .def_readonly("name", &BehaviorT::name_)
            .def("__repr__", [](const BehaviorT& self) { return "<Behavior '" + self.name_ + "'>"; });
}

} // namespace arbitration_graphs_py
