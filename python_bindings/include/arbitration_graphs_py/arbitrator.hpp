#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>

#include "command_wrapper.hpp"
#include "verification_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
using namespace arbitration_graphs;

class PyArbitratorOption
        : public Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>::Option {
public:
    using BaseT =
        typename Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>::Option;
    using FlagsT = typename BaseT::FlagsT;

    using BaseT::BaseT;

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << BaseT::toYaml(time);
        return out.c_str();
    }
};

class PyArbitrator : public Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper> {
public:
    using BaseT = Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;

    using BaseT::BaseT;

    // NOLINTBEGIN(readability-function-size)
    CommandWrapper getCommand(const Time& time) override {
        PYBIND11_OVERRIDE_PURE_NAME(CommandWrapper, BaseT, "get_command", getCommand, time);
    }

    void addOption(const typename BaseT::Behavior::Ptr& behavior,
                   const typename BaseT::Option::FlagsT& flags) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "add_option", addOption, behavior, flags);
    }

    bool checkInvocationCondition(const Time& time) const override {
        PYBIND11_OVERRIDE_NAME(bool, BaseT, "check_invocation_condition", checkInvocationCondition, time);
    }

    bool checkCommitmentCondition(const Time& time) const override {
        PYBIND11_OVERRIDE_NAME(bool, BaseT, "check_commitment_condition", checkCommitmentCondition, time);
    }

    void gainControl(const Time& time) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "gain_control", gainControl, time);
    }

    void loseControl(const Time& time) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "lose_control", loseControl, time);
    }
    // NOLINTEND(readability-function-size)
};

inline void bindArbitrator(py::module& module) {
    using ArbitratorT = Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;

    using OptionT = typename ArbitratorT::Option;
    using PyOption = PyArbitratorOption;

    using FlagsT = typename OptionT::FlagsT;

    py::class_<ArbitratorT, PyArbitrator, Behavior<CommandWrapper>, std::shared_ptr<ArbitratorT>> arbitrator(
        module, "Arbitrator");
    arbitrator.def("add_option", &ArbitratorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def("options", &ArbitratorT::options)
        .def("check_invocation_condition", &ArbitratorT::checkInvocationCondition, py::arg("time"))
        .def("check_commitment_condition", &ArbitratorT::checkCommitmentCondition, py::arg("time"))
        .def("gain_control", &ArbitratorT::gainControl, py::arg("time"))
        .def("lose_control", &ArbitratorT::loseControl, py::arg("time"))
        .def("__repr__", [](const ArbitratorT& self) { return "<Arbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, PyOption, std::shared_ptr<OptionT>> option(arbitrator, "Option");
    option
        .def(py::init<const typename Behavior<CommandWrapper>::Ptr&, const FlagsT&>(),
             py::arg("behavior"),
             py::arg("flags"))
        .def("has_flags", &OptionT::hasFlag, py::arg("flags_to_check"))
        .def(
            "to_yaml_as_str",
            [](const PyOption& self, const Time& time) { return self.toYamlAsString(time); },
            py::arg("time"))
        .def_readwrite("behavior", &OptionT::behavior_)
        .def_readwrite("flags", &OptionT::flags_)
        .def_readwrite("verification_result", &OptionT::verificationResult_);

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("INTERRUPTABLE", OptionT::INTERRUPTABLE)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}

} // namespace arbitration_graphs_py
