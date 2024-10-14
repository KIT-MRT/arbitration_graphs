#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "behavior.hpp"
#include "python_bindings.hpp"

#include "cost_estimator.hpp"
#include "dummy_types.hpp"

namespace py = pybind11;
using namespace arbitration_graphs;
using namespace arbitration_graphs_tests;

// We need to define this operator for the coordinators to work
DummyCommand& operator&=(DummyCommand& a, const DummyCommand& b) {
    a += b;
    return a;
}

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

    py::class_<DummyCommandInt>(module, "DummyCommandInt")
        .def(py::init<const int>())
        .def(py::init<const DummyCommand&>())
        .def("__eq__", [](const DummyCommandInt& self, int otherCommand) { return self == otherCommand; })
        .def_readwrite("command_", &DummyCommandInt::command_);

    module.def(
        "__eq__",
        [](int commandInt, const DummyCommandInt& commandObject) { return commandInt == commandObject; },
        py::is_operator());
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
        .def("lose_control", &DummyBehavior::loseControl, py::arg("time"))
        .def_readwrite("invocation_condition", &DummyBehavior::invocationCondition_)
        .def_readwrite("commitment_condition", &DummyBehavior::commitmentCondition_)
        .def_readwrite("lose_control_counter", &DummyBehavior::loseControlCounter_);
}

void bindDummyVerifier(py::module& module) {
    py::class_<DummyResult>(module, "DummyResult")
        .def(py::init<>())
        .def("is_ok", &DummyResult::isOk)
        .def("__str__", [](const DummyResult& result) { return result.isOk() ? "is okay" : "is not okay"; });

    py::class_<DummyVerifier>(module, "DummyVerifier")
        .def(py::init<std::string>(), py::arg("wrong") = "")
        .def("analyze", &DummyVerifier::analyze)
        .def_readwrite("wrong", &DummyVerifier::wrong_);
}

template <typename CommandT>
void bindCostEstimatorFromCostMap(py::module& module) {
    py::class_<CostEstimatorFromCostMap, CostEstimator<CommandT>, std::shared_ptr<CostEstimatorFromCostMap>>(
        module, "CostEstimatorFromCostMap")
        .def(py::init<const CostEstimatorFromCostMap::CostMap&, double>(),
             py::arg("costMap"),
             py::arg("activationCosts") = 0)
        .def("estimate_cost", &CostEstimatorFromCostMap::estimateCost, py::arg("command"), py::arg("isActive"));
}

void bindTestingTypes(py::module& module) {
    bindConstants(module);
    bindDummyCommand(module);
    bindDummyBehavior(module);
    bindCostEstimatorFromCostMap<DummyCommand>(module);
}

} // namespace

PYBIND11_MODULE(arbitration_graphs_py, mainModule) {
    python_api::bindArbitrationGraphs<DummyCommand>(mainModule);

    py::module testingModule = mainModule.def_submodule("testing_types");
    bindTestingTypes(testingModule);
}

PYBIND11_MODULE(arbitration_graphs_py_with_subcommand, mainModule) {
    python_api::bindArbitrationGraphs<DummyCommandInt, DummyCommand>(mainModule, "Int");

    py::module testingModule = mainModule.def_submodule("testing_types");
    bindTestingTypes(testingModule);
}

PYBIND11_MODULE(arbitration_graphs_py_with_verifier, mainModule) {
    py::module testingModule = mainModule.def_submodule("testing_types");
    bindDummyVerifier(testingModule); // Verifier must be known before binding the main library

    python_api::bindArbitrationGraphs<DummyCommand, DummyCommand, DummyVerifier, DummyResult>(mainModule);
    bindTestingTypes(testingModule);
}
