#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "../behavior.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

template <typename CommandT>
class PyBehavior : public Behavior<CommandT> {
public:
    using BaseT = Behavior<CommandT>;

    explicit PyBehavior(const std::string& name) : BaseT(name) {
    }

    // NOLINTBEGIN(readability-function-size)
    CommandT getCommand(const Time& time) override {
        PYBIND11_OVERRIDE_PURE_NAME(CommandT, BaseT, "get_command", getCommand, time);
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

    std::ostream& to_stream(std::ostream& output,
                            const Time& time,
                            const std::string& prefix = "",
                            const std::string& suffix = "") const override {
        PYBIND11_OVERRIDE(std::ostream&, BaseT, to_stream, output, time, prefix, suffix);
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

template <typename CommandT>
void bindBehavior(py::module& module) {
    using BehaviorT = Behavior<CommandT>;
    using PyBehaviorT = PyBehavior<CommandT>;

    py::class_<BehaviorT, PyBehaviorT, std::shared_ptr<BehaviorT>>(module, "Behavior")
        .def(py::init<const std::string&>(), py::arg("name") = "Behavior")
        .def("get_command", &BehaviorT::getCommand, py::arg("time"))
        .def("check_invocation_condition", &BehaviorT::checkInvocationCondition, py::arg("time"))
        .def("check_commitment_condition", &BehaviorT::checkCommitmentCondition, py::arg("time"))
        .def("gain_control", &BehaviorT::gainControl, py::arg("time"))
        .def("lose_control", &BehaviorT::loseControl, py::arg("time"))
        .def("to_str", &BehaviorT::to_str, py::arg("time"), py::arg("prefix") = "", py::arg("suffix") = "")
        .def("to_stream",
             &BehaviorT::to_stream,
             py::arg("ostream"),
             py::arg("time"),
             py::arg("prefix") = "",
             py::arg("suffix") = "")
        .def(
            "to_yaml_as_str",
            [](const BehaviorT& self, const Time& time) {
                return static_cast<const PyBehaviorT&>(self).toYamlAsString(time);
            },
            py::arg("time"))
        .def("__repr__", [](const BehaviorT& self) { return "<Behavior '" + self.name_ + "'>"; });
}

} // namespace arbitration_graphs::python_api
