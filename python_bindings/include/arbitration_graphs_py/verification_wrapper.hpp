#pragma once

#include <ostream>
#include <stdexcept>

#include <arbitration_graphs/types.hpp>
#include <pybind11/pybind11.h>

#include "command_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
using namespace pybind11::literals;


/// @brief A thin wrapper class for the VerificationResult to allow the instantiation of the arbitration graph classes.
/// @details Analogous to the CommandWrapper, this wrapper holds a generic Python object.
///          A default constructed VerificationResultWrapper will always return true for isOk(), analogous to the
///          PlaceboResult.
/// @throws: This wrapper will throw an exception if the Python object does not implement an is_ok() method.
class VerificationResultWrapper {
public:
    VerificationResultWrapper() {
        // This is analogous to the PlaceboResult
        constructTrivialResult(true);
    }
    explicit VerificationResultWrapper(bool isOk) {
        // This is analogous the implicit default constructor of the PlaceboResult
        constructTrivialResult(isOk);
    }
    VerificationResultWrapper(const VerificationResultWrapper&) = default;
    explicit VerificationResultWrapper(py::object verificationResult)
            : verificationResult_{std::make_shared<py::object>(std::move(verificationResult))} {
    }

    bool isOk() const {
        if (!py::hasattr(value(), "is_ok")) {
            throw py::attribute_error("Python object must have an 'is_ok' attribute");
        }

        py::object is_ok = value().attr("is_ok"); // NOLINT(readability-identifier-naming)
        if (!py::isinstance<py::function>(is_ok)) {
            throw py::type_error("Python object 'is_ok' attribute must be a function");
        }

        return is_ok().cast<bool>();
    }

    py::object value() const {
        if (!verificationResult_) {
            throw std::runtime_error("VerificationResultWrapper is not initialized");
        }
        return *verificationResult_;
    }

private:
    std::shared_ptr<py::object> verificationResult_;

    /// @brief Constructs a python object with an is_ok() method that returns the given isOk value
    void constructTrivialResult(bool isOk) {
        // NOLINTNEXTLINE readability-identifier-naming
        py::object SimpleNamespace = py::module_::import("types").attr("SimpleNamespace");
        py::object result = SimpleNamespace();

        result.attr("is_ok") = py::cpp_function([isOk]() { return isOk; });

        verificationResult_ = std::make_shared<py::object>(result);
    }
};

/// @brief Output stream operator utilizing the Python __repr__ or __str__ method of the object.
inline std::ostream& operator<<(std::ostream& out, const VerificationResultWrapper& result) {
    if (py::hasattr(result.value(), "__repr__")) {
        out << result.value().attr("__repr__")().cast<std::string>();
        return out;
    }

    if (py::hasattr(result.value(), "__str__")) {
        out << result.value().attr("__str__")().cast<std::string>();
        return out;
    }

    out << (result.isOk() ? "is okay" : "is not okay");
    return out;
}

/// @brief A thin wrapper class for the Verifier analogous to the CommandWrapper and the VerificationResultWrapper.
/// @throws This wrapper will throw an exception if the Python object does not implement an analyze method that takes a
/// Time and a Command as arguments and returns a VerificationResult.
class VerifierWrapper {
public:
    VerifierWrapper() : verifier_(std::make_shared<py::object>(py::none())) {
    }
    VerifierWrapper(const VerifierWrapper&) = default;
    explicit VerifierWrapper(py::object verifier) : verifier_{std::make_shared<py::object>(std::move(verifier))} {
    }

    VerificationResultWrapper analyze(const arbitration_graphs::Time& time, const CommandWrapper& command) const {
        if (value().is(py::none())) {
            // Analogous to the PlaceboVerifier, return a default constructed result,
            // if the verifier is default constructed
            return {};
        }
        if (!py::hasattr(value(), "analyze")) {
            throw py::attribute_error("Python object must have an 'analyze' attribute");
        }

        py::object analyze = value().attr("analyze");
        if (!py::isinstance<py::function>(analyze)) {
            throw py::type_error("Python object 'analyze' attribute must be a function");
        }

        return VerificationResultWrapper(analyze(time, command.value()));
    }

    py::object value() const {
        if (!verifier_) {
            throw std::runtime_error("VerifierWrapper is not initialized");
        }
        return *verifier_;
    }


private:
    std::shared_ptr<py::object> verifier_;
};

} // namespace arbitration_graphs_py

namespace pybind11 {
namespace detail {

/// @brief Type caster specialization analogous to the CommandWrapper.
template <>
struct type_caster<arbitration_graphs_py::VerificationResultWrapper> {
public:
    PYBIND11_TYPE_CASTER(arbitration_graphs_py::VerificationResultWrapper, const_name("VerificationResultWrapper"));

    bool load(handle src, bool /*unused*/) {
        value = arbitration_graphs_py::VerificationResultWrapper(py::cast<py::object>(src));
        return true;
    }

    static handle cast(const arbitration_graphs_py::VerificationResultWrapper& src,
                       return_value_policy /*unused*/,
                       handle /*unused*/) {
        return py::object(src.value()).release();
    }
};

/// @brief Type caster specialization analogous to the CommandWrapper.
template <>
struct type_caster<arbitration_graphs_py::VerifierWrapper> {
public:
    PYBIND11_TYPE_CASTER(arbitration_graphs_py::VerifierWrapper, const_name("VerifierWrapper"));

    bool load(handle src, bool /*unused*/) {
        value = arbitration_graphs_py::VerifierWrapper(py::cast<py::object>(src));
        return true;
    }

    static handle cast(const arbitration_graphs_py::VerifierWrapper& src,
                       return_value_policy /*unused*/,
                       handle /*unused*/) {
        return py::object(src.value()).release();
    }
};

} // namespace detail
} // namespace pybind11
