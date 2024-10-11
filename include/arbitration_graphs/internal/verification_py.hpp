#pragma once

#include <pybind11/pybind11.h>

#include "../verification.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;


template <typename DataT>
void bindPlaceboVerifier(py::module& module) {
    using PlaceboResult = verification::PlaceboResult;
    using PlaceboVerifierT = verification::PlaceboVerifier<DataT>;

    py::class_<PlaceboResult>(module, "PlaceboResult").def_static("is_ok", &PlaceboResult::isOk);

    py::class_<PlaceboVerifierT>(module, "PlaceboVerifier")
        .def_static("analyze", &PlaceboVerifierT::analyze, py::arg("time"), py::arg("data"));
}

} // namespace arbitration_graphs::python_api
