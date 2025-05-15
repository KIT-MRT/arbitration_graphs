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
#include "arbitration_graphs_py/verification_wrapper.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace arbitration_graphs_py {

PYBIND11_MODULE(arbitration_graphs_py, mainModule) {
    bindVerificationWrapper(mainModule);
namespace {
void bindUtilCaching(py::module& module) {
    py::module utilCaching = module.def_submodule("util_caching");
    using ApproximateTimeT = util_caching::policies::ApproximateTime<Time, std::chrono::milliseconds>;
    util_caching::python_api::time_based::bindApproximatePolicy<Time, std::chrono::milliseconds>(utilCaching,
                                                                                                 "ApproximateTime");
    util_caching::python_api::time_based::bindCache<Time, VerificationResultWrapper, ApproximateTimeT>(utilCaching);
}
} // namespace

    bindExceptions(mainModule);

    bindBehavior(mainModule);
    bindArbitrator(mainModule);
    bindCostArbitrator(mainModule);
    bindPriorityArbitrator(mainModule);
    bindRandomArbitrator(mainModule);

    bindUtilCaching(mainModule);


#ifdef PROJECT_VERSION
    mainModule.attr("__version__") = MACRO_STRINGIFY(PROJECT_VERSION);
#else
    mainModule.attr("__version__") = "dev";
#endif
}

} // namespace arbitration_graphs_py
