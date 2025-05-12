#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "arbitration_graphs_py/arbitrator.hpp"
#include "arbitration_graphs_py/behavior.hpp"
#include "arbitration_graphs_py/exceptions.hpp"
#include "arbitration_graphs_py/priority_arbitrator.hpp"
#include "arbitration_graphs_py/verification_wrapper.hpp"

namespace arbitration_graphs_py {

PYBIND11_MODULE(_core, mainModule) {
    bindVerificationWrapper(mainModule);

    bindExceptions(mainModule);

    bindBehavior(mainModule);
    bindArbitrator(mainModule);
    bindPriorityArbitrator(mainModule);
}

} // namespace arbitration_graphs_py
