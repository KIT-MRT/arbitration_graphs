#pragma once

#include <arbitration_graphs/behavior.hpp>
#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "command_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
using namespace arbitration_graphs;

class PyBehavior : public arbitration_graphs::Behavior<CommandWrapper> {
public:
    using BaseT = arbitration_graphs::Behavior<CommandWrapper>;

    using Behavior::Behavior;

    // NOLINTBEGIN(readability-function-size)
    CommandWrapper getCommand(const Time& time) override {
        PYBIND11_OVERRIDE_PURE_NAME(CommandWrapper, BaseT, "get_command", getCommand, time);
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

    std::string to_str(const Time& time,
                       const std::string& prefix = "",
                       const std::string& suffix = "") const override {
        PYBIND11_OVERRIDE(std::string, BaseT, to_str, time, prefix, suffix);
    }
    // NOLINTEND(readability-function-size)

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << BaseT::toYaml(time);
        return out.c_str();
    }
};

inline void bindBehavior(py::module& module, const std::string& bindingName = "Behavior") {
    using BehaviorT = arbitration_graphs::Behavior<CommandWrapper>;

    py::class_<BehaviorT, PyBehavior, std::shared_ptr<BehaviorT>>(module, bindingName.c_str())
        .def(py::init<const std::string&>(), py::arg("name") = "Behavior")
        .def(
            "get_command",
            [](BehaviorT& self, const Time& time) { return self.getCommand(time).value(); },
            py::arg("time"))
        .def("check_invocation_condition", &BehaviorT::checkInvocationCondition, py::arg("time"))
        .def("check_commitment_condition", &BehaviorT::checkCommitmentCondition, py::arg("time"))
        .def("gain_control", &BehaviorT::gainControl, py::arg("time"))
        .def("lose_control", &BehaviorT::loseControl, py::arg("time"))
        .def("to_str", &BehaviorT::to_str, py::arg("time"), py::arg("prefix") = "", py::arg("suffix") = "")
        .def(
            "to_yaml_as_str",
            [](const PyBehavior& self, const Time& time) { return self.toYamlAsString(time); },
            py::arg("time"))
        .def_readonly("name", &BehaviorT::name_)
        .def("__repr__", [](const BehaviorT& self) { return "<Behavior '" + self.name_ + "'>"; });
}

} // namespace arbitration_graphs_py
