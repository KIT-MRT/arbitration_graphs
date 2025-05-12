#pragma once

#include <stdexcept>

#include <pybind11/pybind11.h>

namespace arbitration_graphs_py {

namespace py = pybind11;

class CommandWrapper {
public:
    CommandWrapper() = default;
    CommandWrapper(const CommandWrapper&) = default;
    explicit CommandWrapper(py::object command) : command_{std::move(command)} {
    }

    CommandWrapper& operator&=(const CommandWrapper& /*command*/) {
        throw std::runtime_error("This operator is not implemented");
    }

    py::object value() const {
        return command_;
    }

private:
    py::object command_;
};

inline void bindCommandWrapper(py::module& module) {
    py::class_<CommandWrapper>(module, "CommandWrapper")
        .def(py::init<const CommandWrapper&>())
        .def(py::init<py::object>())
        .def("__iand__", &CommandWrapper::operator&=, py::is_operator())
        .def("value", &CommandWrapper::value);
}

} // namespace arbitration_graphs_py
