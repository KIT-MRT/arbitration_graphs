#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>

#include "../test/dummy_types.hpp"

namespace arbitration_graphs_tests {

namespace py = pybind11;

void bindDummyBehavior(py::module &m) {
  py::class_<DummyBehavior, std::shared_ptr<DummyBehavior>>(m, "DummyBehavior")
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

PYBIND11_MODULE(arbitration_graphs_py, m) { bindDummyBehavior(m); }
} // namespace arbitration_graphs_tests
