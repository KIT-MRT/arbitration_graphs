#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>

#include "../test/dummy_types.hpp"
#include "behavior.hpp"
#include "python_api/behavior.hpp"

namespace arbitration_graphs_tests {

namespace py = pybind11;

void bindConstants(py::module &m) {
  m.attr("strike_through_on") = strikeThroughOn;
  m.attr("strike_through_off") = strikeThroughOff;
  m.attr("invocation_true_string") = invocationTrueString;
  m.attr("invocation_false_string") = invocationFalseString;
  m.attr("commitment_true_string") = commitmentTrueString;
  m.attr("commitment_false_string") = commitmentFalseString;
}

void bindDummyCommand(py::module &m) {
  m.def("DummyCommand", [](const std::string &s) { return std::string(s); });
}

void bindDummyBehavior(py::module &m) {
  py::class_<DummyBehavior, std::shared_ptr<DummyBehavior>,
             arbitration_graphs::Behavior<DummyCommand>>(m, "DummyBehavior")
      .def(py::init<const bool, const bool, const std::string &>(),
           py::arg("invocation_condition"), py::arg("commitment_condition"),
           py::arg("name") = "dummy_behavior")
      .def("get_command", &DummyBehavior::getCommand, py::arg("time"))
      .def("check_invocation_condition",
           &DummyBehavior::checkInvocationCondition, py::arg("time"))
      .def("check_commitment_condition",
           &DummyBehavior::checkCommitmentCondition, py::arg("time"))
      .def("gain_control", &DummyBehavior::gainControl, py::arg("time"))
      .def("lose_control", &DummyBehavior::loseControl, py::arg("time"));
}

PYBIND11_MODULE(arbitration_graphs_py, m) {
  python_api::bindBehavior<DummyCommand>(m);
  bindConstants(m);
  bindDummyCommand(m);
  bindDummyBehavior(m);
}
} // namespace arbitration_graphs_tests
