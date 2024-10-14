#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "../arbitrator.hpp"
#include "../behavior.hpp"
#include "../priority_arbitrator.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyPriorityArbitratorOption
        : public PriorityArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option {
public:
    using BaseT = typename PriorityArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option;
    using FlagsT = typename BaseT::FlagsT;

    explicit PyPriorityArbitratorOption(const typename Behavior<SubCommandT>::Ptr& behavior, const FlagsT& flags)
            : BaseT(behavior, flags) {
    }
};

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyPriorityArbitrator
        : public arbitration_graphs::PriorityArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT> {
public:
    using BaseT = arbitration_graphs::PriorityArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    explicit PyPriorityArbitrator(const std::string& name, const VerifierT& verifier = VerifierT())
            : BaseT(name, verifier) {
    }

    // NOLINTBEGIN(readability-function-size)
    void addOption(const typename BaseT::Option::Ptr& behavior, const typename BaseT::Option::FlagsT& flags) {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "add_option", addOption, behavior, flags);
    }

    YAML::Node toYaml(const arbitration_graphs::Time& time) const override {
        PYBIND11_OVERRIDE(YAML::Node, BaseT, toYaml, time);
    }
    // NOLINTEND(readability-function-size)

    std::string toYamlAsString(const arbitration_graphs::Time& time) const {
        YAML::Emitter out;
        out << toYaml(time);
        return out.c_str();
    }
};

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
void bindPriorityArbitrator(py::module& module) {
    using ArbitratorT = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using ArbitratorOptionT = typename ArbitratorT::Option;
    using PriorityArbitratorT = PriorityArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using PyPriorityArbitratorT = PyPriorityArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using OptionT = typename PriorityArbitratorT::Option;
    using PyOptionT = PyPriorityArbitratorOption<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<PriorityArbitratorT, ArbitratorT, PyPriorityArbitratorT, std::shared_ptr<PriorityArbitratorT>>
        priorityArbitrator(module, "PriorityArbitrator");
    priorityArbitrator
        .def(py::init<const std::string&, const VerifierT&>(),
             py::arg("name") = "PriorityArbitrator",
             py::arg("verifier") = VerifierT())
        .def("add_option", &PriorityArbitratorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def(
            "to_yaml_as_str",
            [](const PriorityArbitratorT& self, const arbitration_graphs::Time& time) {
                return static_cast<const PyPriorityArbitratorT&>(self).toYamlAsString(time);
            },
            py::arg("time"));

    py::class_<OptionT, ArbitratorOptionT, PyOptionT, std::shared_ptr<OptionT>> option(priorityArbitrator, "Option");
    option.def(
        py::init<const typename Behavior<SubCommandT>::Ptr&, const FlagsT&>(), py::arg("behavior"), py::arg("flags"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("INTERRUPTABLE", OptionT::INTERRUPTABLE)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}


} // namespace arbitration_graphs::python_api
