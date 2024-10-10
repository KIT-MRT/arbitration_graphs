#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>

#include "behavior.hpp"
#include "python_api/arbitrator.hpp"
#include "python_api/behavior.hpp"
#include "python_api/verification.hpp"
#include "../test/dummy_types.hpp"

namespace arbitration_graphs_tests {

namespace py = pybind11;

namespace {

void bindConstants(py::module& module) {
    module.attr("strike_through_on") = strikeThroughOn;
    module.attr("strike_through_off") = strikeThroughOff;
    module.attr("invocation_true_string") = invocationTrueString;
    module.attr("invocation_false_string") = invocationFalseString;
    module.attr("commitment_true_string") = commitmentTrueString;
    module.attr("commitment_false_string") = commitmentFalseString;
}

void bindDummyCommand(py::module& module) {
    module.def("DummyCommand", [](const std::string& command) { return std::string(command); });
}

void bindDummyBehavior(py::module& module) {
    py::class_<DummyBehavior, std::shared_ptr<DummyBehavior>, arbitration_graphs::Behavior<DummyCommand>>(
        module, "DummyBehavior")
        .def(py::init<const bool, const bool, const std::string&>(),
             py::arg("invocation_condition"),
             py::arg("commitment_condition"),
             py::arg("name") = "dummy_behavior")
        .def("get_command", &DummyBehavior::getCommand, py::arg("time"))
        .def("check_invocation_condition", &DummyBehavior::checkInvocationCondition, py::arg("time"))
        .def("check_commitment_condition", &DummyBehavior::checkCommitmentCondition, py::arg("time"))
        .def("gain_control", &DummyBehavior::gainControl, py::arg("time"))
        .def("lose_control", &DummyBehavior::loseControl, py::arg("time"));
}

} // namespace

PYBIND11_MODULE(arbitration_graphs_py, mainModule) {
    python_api::bindPlaceboVerifier<DummyCommand>(mainModule);
    python_api::bindBehavior<DummyCommand>(mainModule);
    python_api::bindArbitrator<DummyCommand>(mainModule);

    py::module testingModule = mainModule.def_submodule("testing_types");
    bindConstants(testingModule);
    bindDummyCommand(testingModule);
    bindDummyBehavior(testingModule);
}
} // namespace arbitration_graphs_tests
