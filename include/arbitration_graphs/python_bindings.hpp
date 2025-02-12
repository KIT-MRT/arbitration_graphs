#pragma once

#include <util_caching/python_bindings.hpp>

#include "internal/arbitrator_py.hpp"
#include "internal/behavior_py.hpp"
#include "internal/conjunctive_coordinator_py.hpp"
#include "internal/cost_arbitrator_py.hpp"
#include "internal/exceptions_py.hpp"
#include "internal/joint_coordinator_py.hpp"
#include "internal/priority_arbitrator_py.hpp"
#include "internal/random_arbitrator_py.hpp"
#include "internal/verification_py.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
void bindArbitrationGraphs(py::module& module,
                           const std::string& behaviorBindingName = "Behavior",
                           const std::string& behaviorSubCommandBindingName = "BehaviorSubCommand") {
    bindExceptions(module);
    bindPlaceboVerifier<SubCommandT>(module);

    bindBehavior<CommandT>(module, behaviorBindingName);
    // Bind the behavior for the SubCommandT only if it is different from the CommandT
    if constexpr (!std::is_same_v<CommandT, SubCommandT>) {
        bindBehavior<SubCommandT>(module, behaviorSubCommandBindingName);
    }

    bindArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);

    bindConjunctiveCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);
    bindCostArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);
    bindJointCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);
    bindPriorityArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);
    bindRandomArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>(module);

    py::module utilCaching = module.def_submodule("util_caching");
    using ApproximateTimeT = util_caching::policies::ApproximateTime<Time, std::chrono::milliseconds>;
    util_caching::python_api::time_based::bindApproximatePolicy<Time, std::chrono::milliseconds>(utilCaching,
                                                                                                 "ApproximateTime");
    util_caching::python_api::time_based::bindCache<Time, VerificationResultT, ApproximateTimeT>(utilCaching);
}
} // namespace arbitration_graphs::python_api
