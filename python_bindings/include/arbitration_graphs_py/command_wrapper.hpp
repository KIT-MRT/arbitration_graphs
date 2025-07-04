#pragma once

#include <stdexcept>

#include <pybind11/pybind11.h>

namespace arbitration_graphs_py {

namespace py = pybind11;

/// @brief A thin wrapper class for the Command class allowing the instantiation of the arbitration graph classes.
/// @details To work around the issue of the arbitration graph library being a templated C++ library, we use this
///          wrapper class to allow the instantiation of the arbitration graph classes with arbitrary Python types. This
///          way, we can compile the Python bindings without needing to know the exact types at compile time.
class CommandWrapper {
public:
    CommandWrapper() = default;
    explicit CommandWrapper(py::object command) : command_{std::make_shared<py::object>(std::move(command))} {
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

/// @brief Template specialization implementing type casting for the CommandWrapper class.
/// @details This specialization allows arbitrary Python objects to be passed to and from C++ code as CommandWrapper.
template <>
struct type_caster<arbitration_graphs_py::CommandWrapper> {
public:
    PYBIND11_TYPE_CASTER(arbitration_graphs_py::CommandWrapper, const_name("CommandWrapper"));

    // Python -> C++
    bool load(handle src, bool /*unused*/) {
        // Copy the py::object to increment its reference count (Py_INCREF)
        // This ensures that the C++ side holds a strong reference independently of Python
        // Prevents use-after-free if Python drops its reference
        value = arbitration_graphs_py::CommandWrapper(py::cast<py::object>(src));
        return true;
    }

    // C++ -> Python
    static handle cast(const arbitration_graphs_py::CommandWrapper& src,
                       return_value_policy /*unused*/,
                       handle /*unused*/) {
        // Return a new reference to the Python object (increments ref count)
        // This decouples the C++ object's lifetime from Python’s
        // Avoids shared ownership issues and ensures Python can manage the object normally
        return py::object(src.value()).release();
    }
};

} // namespace detail
} // namespace pybind11
