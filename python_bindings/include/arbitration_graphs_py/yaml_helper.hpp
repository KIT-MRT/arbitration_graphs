#pragma once

#include <arbitration_graphs/types.hpp>
#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

namespace arbitration_graphs_py::yaml_helper {

namespace py = pybind11;

/// @brief Extracts a python YAML representation of the given object.
/// @tparam YamlRepresentableT Object type to convert to YAML. Must implement the toYaml(const Time&) method.
template <typename YamlRepresentableT>
inline py::object toYamlAsPythonObject(const YamlRepresentableT& yamlRepresentable,
                                       const arbitration_graphs::Time& time) {
    YAML::Emitter out;
    out << yamlRepresentable.toYaml(time);
    std::string yamlStr = out.c_str();

    py::object yaml = py::module_::import("yaml");
    return yaml.attr("safe_load")(yamlStr);
}

} // namespace arbitration_graphs_py::yaml_helper
