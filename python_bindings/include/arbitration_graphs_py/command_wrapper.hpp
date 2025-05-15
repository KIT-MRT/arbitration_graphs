#pragma once

#include <stdexcept>

#include <pybind11/pybind11.h>

namespace arbitration_graphs_py {

namespace py = pybind11;

class CommandWrapper {
public:
    CommandWrapper() = default;
    explicit CommandWrapper(py::object command) : command_{std::make_shared<py::object>(std::move(command))} {
    }

    CommandWrapper& operator&=(const CommandWrapper& /*command*/) {
        throw std::runtime_error("This operator is not implemented");
    }

    py::object value() const {
        if (!command_) {
            throw std::runtime_error("CommandWrapper is not initialized");
        }
        return *command_;
    }

private:
    std::shared_ptr<py::object> command_;
};

} // namespace arbitration_graphs_py

namespace pybind11 {
namespace detail {

namespace py = pybind11;

template <>
struct type_caster<arbitration_graphs_py::CommandWrapper> {
public:
    PYBIND11_TYPE_CASTER(arbitration_graphs_py::CommandWrapper, _("CommandWrapper"));

    // Python -> C++
    bool load(handle src, bool /*unused*/) {
        value = arbitration_graphs_py::CommandWrapper(py::reinterpret_borrow<py::object>(src));
        return true;
    }

    // C++ -> Python
    static handle cast(const arbitration_graphs_py::CommandWrapper& src,
                       return_value_policy /*unused*/,
                       handle /*unused*/) {
        return py::reinterpret_borrow<py::object>(src.value());
    }
};

} // namespace detail
} // namespace pybind11
