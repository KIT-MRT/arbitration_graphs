#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <util_caching/python_bindings.hpp>

#include "arbitration_graphs_py/arbitrator.hpp"
#include "arbitration_graphs_py/behavior.hpp"
#include "arbitration_graphs_py/cost_arbitrator.hpp"
#include "arbitration_graphs_py/exceptions.hpp"
#include "arbitration_graphs_py/priority_arbitrator.hpp"
#include "arbitration_graphs_py/random_arbitrator.hpp"
#include "arbitration_graphs_py/verifier.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace arbitration_graphs_py {

namespace {
void bindUtilCaching(py::module& module) {
    using AbstractResult = arbitration_graphs::verification::AbstractResult;
    using Time = arbitration_graphs::Time;

    py::module utilCaching = module.def_submodule("util_caching");
    using ApproximateTimeT = util_caching::policies::ApproximateTime<Time, std::chrono::milliseconds>;
    util_caching::python_api::time_based::bindApproximatePolicy<Time, std::chrono::milliseconds>(utilCaching,
                                                                                                 "ApproximateTime");
    util_caching::python_api::time_based::bindCache<Time, AbstractResult::ConstPtr, ApproximateTimeT>(utilCaching);
}
} // namespace

///@brief Bindings for the arbitration_graphs library
///@details This is where it all comes together. The bindings for the individual classes are
///         created here in the main arbitration_graphs module.
PYBIND11_MODULE(arbitration_graphs_py, mainModule) {
    bindExceptions(mainModule);
    py::module verificationModule = mainModule.def_submodule("verification");
    bindVerifier(verificationModule);

    bindBehavior(mainModule);
    bindArbitrator(mainModule);
    bindCostArbitrator(mainModule);
    bindPriorityArbitrator(mainModule);
    bindRandomArbitrator(mainModule);

    // Bind util_caching to be able to access cached verification results
    bindUtilCaching(mainModule);

    // Add the __version__ attribute to the module
#ifdef PROJECT_VERSION
    mainModule.attr("__version__") = MACRO_STRINGIFY(PROJECT_VERSION);
#else
    mainModule.attr("__version__") = "dev";
#endif
}

} // namespace arbitration_graphs_py
