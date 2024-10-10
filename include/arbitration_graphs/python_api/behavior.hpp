#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "../behavior.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

std::string yamlNodeToString(const YAML::Node &node) {
  YAML::Emitter out;
  out << node;
  return out.c_str();
}

template <typename CommandT>
class PyBehavior : public arbitration_graphs::Behavior<CommandT> {
public:
  PyBehavior(const std::string &name)
      : arbitration_graphs::Behavior<CommandT>(name) {}

  CommandT getCommand(const arbitration_graphs::Time &time) override {
    PYBIND11_OVERRIDE_PURE_NAME(
        CommandT,                               // Return type
        arbitration_graphs::Behavior<CommandT>, // Parent class
        "get_command",                          // Method name in Python
        getCommand,                             // Method name in C++
        time                                    // Arguments
    );
  }

  bool checkInvocationCondition(
      const arbitration_graphs::Time &time) const override {
    PYBIND11_OVERRIDE_NAME(
        bool,                                   // Return type
        arbitration_graphs::Behavior<CommandT>, // Parent class
        "check_invocation_condition",           // Method name in Python
        checkInvocationCondition,               // Method name in C++
        time                                    // Arguments
    );
  }

  bool checkCommitmentCondition(
      const arbitration_graphs::Time &time) const override {
    PYBIND11_OVERRIDE_NAME(bool, arbitration_graphs::Behavior<CommandT>,
                           "check_commitment_condition",
                           checkCommitmentCondition, time);
  }

  void gainControl(const arbitration_graphs::Time &time) override {
    PYBIND11_OVERRIDE_NAME(void, arbitration_graphs::Behavior<CommandT>,
                           "gain_control", gainControl, time);
  }

  void loseControl(const arbitration_graphs::Time &time) override {
    PYBIND11_OVERRIDE_NAME(void, arbitration_graphs::Behavior<CommandT>,
                           "lose_control", loseControl, time);
  }

  std::string to_str(const arbitration_graphs::Time &time,
                     const std::string &prefix = "",
                     const std::string &suffix = "") const override {
    PYBIND11_OVERRIDE(std::string,                            // Return type
                      arbitration_graphs::Behavior<CommandT>, // Parent class
                      to_str,              // Method name in C++
                      time, prefix, suffix // Arguments
    );
  }

  YAML::Node toYaml(const arbitration_graphs::Time &time) const override {
    PYBIND11_OVERRIDE(YAML::Node, arbitration_graphs::Behavior<CommandT>,
                      toYaml, time);
  }

  std::string toYamlAsString(const arbitration_graphs::Time &time) const {
    return yamlNodeToString(toYaml(time));
  }
};

template <typename CommandT> void bindBehavior(py::module &m) {
  using BehaviorType = arbitration_graphs::Behavior<CommandT>;
  py::class_<BehaviorType, PyBehavior<CommandT>, std::shared_ptr<BehaviorType>>(
      m, "Behavior")
      .def(py::init<const std::string &>(), py::arg("name") = "Behavior")
      .def("get_command", &BehaviorType::getCommand, py::arg("time"))
      .def("check_invocation_condition",
           &BehaviorType::checkInvocationCondition, py::arg("time"))
      .def("check_commitment_condition",
           &BehaviorType::checkCommitmentCondition, py::arg("time"))
      .def("gain_control", &BehaviorType::gainControl, py::arg("time"))
      .def("lose_control", &BehaviorType::loseControl, py::arg("time"))
      .def("to_str", &BehaviorType::to_str, py::arg("time"),
           py::arg("prefix") = "", py::arg("suffix") = "")
      .def(
          "to_yaml_as_str",
          [](const BehaviorType &self, const arbitration_graphs::Time &time) {
            return static_cast<const PyBehavior<CommandT> &>(self)
                .toYamlAsString(time);
          },
          py::arg("time"))
      .def("__repr__", [](const BehaviorType &self) {
        return "<Behavior '" + self.name_ + "'>";
      });
}

} // namespace arbitration_graphs::python_api
