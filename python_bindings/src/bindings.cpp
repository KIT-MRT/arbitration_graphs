#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "arbitration_graphs_py/arbitrator.hpp"
#include "arbitration_graphs_py/behavior.hpp"
#include "arbitration_graphs_py/exceptions.hpp"
#include "arbitration_graphs_py/priority_arbitrator.hpp"
#include "arbitration_graphs_py/verification_wrapper.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace arbitration_graphs_py {

PYBIND11_MODULE(arbitration_graphs_py, mainModule) {
    bindVerificationWrapper(mainModule);

    bindExceptions(mainModule);

    bindBehavior(mainModule);
    bindArbitrator(mainModule);
    bindPriorityArbitrator(mainModule);


#ifdef PROJECT_VERSION
    mainModule.attr("__version__") = MACRO_STRINGIFY(PROJECT_VERSION);
#else
    mainModule.attr("__version__") = "dev";
#endif
}

} // namespace arbitration_graphs_py
