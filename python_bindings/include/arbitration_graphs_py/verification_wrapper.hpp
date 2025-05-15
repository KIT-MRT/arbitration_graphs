#pragma once

#include <ostream>
#include <stdexcept>

#include <arbitration_graphs/types.hpp>
#include <pybind11/pybind11.h>

#include "command_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
using namespace pybind11::literals;


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
            : verificationResult_{std::move(verificationResult)} {
    }

    bool isOk() const {
        if (!py::hasattr(verificationResult_, "is_ok")) {
            throw py::attribute_error("Python object must have an 'is_ok' attribute");
        }

        py::object is_ok = verificationResult_.attr("is_ok"); // NOLINT(readability-identifier-naming)
        if (!py::isinstance<py::function>(is_ok)) {
            throw py::type_error("Python object 'is_ok' attribute must be a function");
        }

        return is_ok().cast<bool>();
    }

    py::object value() const {
        return verificationResult_;
    }

private:
    py::object verificationResult_;

    /// @brief Constructs a python object with an is_ok() method that returns the given isOk value
    void constructTrivialResult(bool isOk) {
        // NOLINTNEXTLINE readability-identifier-naming
        py::object SimpleNamespace = py::module_::import("types").attr("SimpleNamespace");
        py::object result = SimpleNamespace();

        result.attr("is_ok") = py::cpp_function([isOk]() { return isOk; });

        verificationResult_ = result;
    }
};

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

class VerifierWrapper {
public:
    VerifierWrapper() : verifier_(py::none()) {
    }
    VerifierWrapper(const VerifierWrapper&) = default;
    explicit VerifierWrapper(py::object verifier) : verifier_{std::move(verifier)} {
    }

    VerificationResultWrapper analyze(const arbitration_graphs::Time& time, const CommandWrapper& command) const {
        if (verifier_.is(py::none())) {
            // Analogous to the PlaceboVerifier, return a default constructed result,
            // if the verifier is default constructed
            return {};
        }
        if (!py::hasattr(verifier_, "analyze")) {
            throw py::attribute_error("Python object must have an 'analyze' attribute");
        }

        py::object analyze = verifier_.attr("analyze");
        if (!py::isinstance<py::function>(analyze)) {
            throw py::type_error("Python object 'analyze' attribute must be a function");
        }

        return VerificationResultWrapper(analyze(time, command.value()));
    }


private:
    py::object verifier_;
};


inline void bindVerificationWrapper(py::module& module) {
    py::class_<VerificationResultWrapper>(module, "VerificationResultWrapper")
        .def(py::init<const VerificationResultWrapper&>())
        .def(py::init<py::object>())
        .def("value", &VerificationResultWrapper::value);

    py::class_<VerifierWrapper>(module, "VerifierWrapper")
        .def(py::init<py::object>())
        .def("analyze", &VerifierWrapper::analyze);
}


} // namespace arbitration_graphs_py
