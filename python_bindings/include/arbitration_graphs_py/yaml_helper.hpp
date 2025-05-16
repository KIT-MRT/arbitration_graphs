#pragma once

#include <arbitration_graphs/types.hpp>
#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

namespace arbitration_graphs_py::yaml_helper {

namespace py = pybind11;
using namespace arbitration_graphs;

template <typename BaseT>
inline py::object toYamlAsPythonObject(const BaseT& base, const Time& time) {
    YAML::Emitter out;
    out << base.toYaml(time);
    std::string yamlStr = out.c_str();

    py::object yaml = py::module_::import("yaml");
    return yaml.attr("safe_load")(yamlStr);
}

} // namespace arbitration_graphs_py::yaml_helper
