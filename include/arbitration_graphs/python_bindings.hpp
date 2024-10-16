#pragma once

#include "internal/arbitrator_py.hpp"
#include "internal/behavior_py.hpp"
#include "internal/conjunctive_coordinator_py.hpp"
#include "internal/cost_arbitrator_py.hpp"
#include "internal/exceptions_py.hpp"
#include "internal/joint_coordinator_py.hpp"
#include "internal/priority_arbitrator_py.hpp"
#include "internal/verification_py.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
void bindArbitrationGraphs(py::module& module,
                           const std::string& behaviorCommandSuffix = "",
                           const std::string& behaviorSubCommandSuffix = "") {
    bindExceptions(module);
    bindPlaceboVerifier<SubCommandT>(module);

    bindBehavior<CommandT>(module, behaviorCommandSuffix);
    // Only bind behavior for SubCommandT if it is different from CommandT
    if constexpr (!std::is_same_v<CommandT, SubCommandT>) {
        bindBehavior<SubCommandT>(module, behaviorSubCommandSuffix);
    }

    bindArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);

    bindConjunctiveCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);
    bindCostArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);
    bindJointCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);
    bindPriorityArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);
}
} // namespace arbitration_graphs::python_api
