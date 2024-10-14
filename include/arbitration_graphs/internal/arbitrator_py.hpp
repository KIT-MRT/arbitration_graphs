#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "../arbitrator.hpp"
#include "../behavior.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyArbitratorOption : public Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option {
public:
    using BaseT = typename Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option;
    using FlagsT = typename BaseT::FlagsT;

    explicit PyArbitratorOption(const typename Behavior<SubCommandT>::Ptr& behavior, const FlagsT& flags)
            : BaseT(behavior, flags) {
    }

    // NOLINTBEGIN(readability-function-size)
    YAML::Node toYaml(const Time& time) const override {
        PYBIND11_OVERRIDE(YAML::Node, BaseT, toYaml, time);
    }
    // NOLINTEND(readability-function-size)

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << toYaml(time);
        return out.c_str();
    }
};

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyArbitrator : public Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT> {
public:
    using BaseT = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    explicit PyArbitrator(const std::string& name = "Arbitrator", const VerifierT& verifier = VerifierT())
            : BaseT(name, verifier) {
    }

    // NOLINTBEGIN(readability-function-size)
    CommandT getCommand(const arbitration_graphs::Time& time) override {
        PYBIND11_OVERRIDE_PURE_NAME(CommandT, BaseT, "get_command", getCommand, time);
    }

    void addOption(const typename BaseT::Option::Ptr& behavior, const typename BaseT::Option::FlagsT& flags) {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "add_option", addOption, behavior, flags);
    }

    bool checkInvocationCondition(const arbitration_graphs::Time& time) const override {
        PYBIND11_OVERRIDE_NAME(bool, BaseT, "check_invocation_condition", checkInvocationCondition, time);
    }

    bool checkCommitmentCondition(const arbitration_graphs::Time& time) const override {
        PYBIND11_OVERRIDE_NAME(bool, BaseT, "check_commitment_condition", checkCommitmentCondition, time);
    }

    void gainControl(const arbitration_graphs::Time& time) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "gain_control", gainControl, time);
    }

    void loseControl(const arbitration_graphs::Time& time) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "lose_control", loseControl, time);
    }
    // NOLINTEND(readability-function-size)
};

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
void bindArbitrator(py::module& module) {
    using ArbitratorT = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using PyArbitratorT = PyArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using OptionT = typename ArbitratorT::Option;
    using PyOptionT = PyArbitratorOption<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<ArbitratorT, PyArbitratorT, Behavior<CommandT>, std::shared_ptr<ArbitratorT>> arbitrator(module,
                                                                                                        "Arbitrator");
    arbitrator.def("add_option", &ArbitratorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def("options", &ArbitratorT::options)
        .def("get_command", &ArbitratorT::getCommand, py::arg("time"))
        .def("check_invocation_condition", &ArbitratorT::checkInvocationCondition, py::arg("time"))
        .def("check_commitment_condition", &ArbitratorT::checkCommitmentCondition, py::arg("time"))
        .def("gain_control", &ArbitratorT::gainControl, py::arg("time"))
        .def("lose_control", &ArbitratorT::loseControl, py::arg("time"))
        .def("__repr__", [](const ArbitratorT& self) { return "<Arbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, PyOptionT, std::shared_ptr<OptionT>> option(arbitrator, "Option");
    option
        .def(py::init<const typename Behavior<SubCommandT>::Ptr&, const FlagsT&>(),
             py::arg("behavior"),
             py::arg("flags"))
        .def("has_flags", &OptionT::hasFlag, py::arg("flags_to_check"))
        .def(
            "to_yaml_as_str",
            [](const OptionT& self, const arbitration_graphs::Time& time) {
                return static_cast<const PyOptionT&>(self).toYamlAsString(time);
            },
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

} // namespace arbitration_graphs::python_api
