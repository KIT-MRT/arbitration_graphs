#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>
#include <arbitration_graphs/priority_arbitrator.hpp>

#include "command_wrapper.hpp"
#include "verification_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
using namespace arbitration_graphs;

class PyPriorityArbitrator
        : public PriorityArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper> {
public:
    using BaseT = PriorityArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;

    explicit PyPriorityArbitrator(const std::string& name, const VerifierWrapper& verifier = VerifierWrapper())
            : BaseT(name, verifier) {
    }

    // NOLINTBEGIN(readability-function-size)
    void addOption(const typename BaseT::Behavior::Ptr& behavior,
                   const typename BaseT::Option::FlagsT& flags) override {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "add_option", addOption, behavior, flags);
    }
    // NOLINTEND(readability-function-size)

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << BaseT::toYaml(time);
        return out.c_str();
    }
};

inline void bindPriorityArbitrator(py::module& module) {
    using BehaviorT = Behavior<CommandWrapper>;

    using ArbitratorT = Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using PriorityArbitratorT =
        PriorityArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;

    using OptionT = typename PriorityArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<PriorityArbitratorT, ArbitratorT, PyPriorityArbitrator, std::shared_ptr<PriorityArbitratorT>>
        priorityArbitrator(module, "PriorityArbitrator");
    priorityArbitrator
        .def(py::init<const std::string&, const VerifierWrapper&>(),
             py::arg("name") = "PriorityArbitrator",
             py::arg("verifier") = VerifierWrapper())
        .def("add_option", &PriorityArbitratorT::addOption, py::arg("behavior"), py::arg("flags"))
        .def(
            "to_yaml_as_str",
            [](const PyPriorityArbitrator& self, const Time& time) { return self.toYamlAsString(time); },
            py::arg("time"))
        .def("__repr__", [](const PriorityArbitratorT& self) { return "<PriorityArbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, ArbitratorOptionT, std::shared_ptr<OptionT>> option(priorityArbitrator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&>(), py::arg("behavior"), py::arg("flags"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("INTERRUPTABLE", OptionT::INTERRUPTABLE)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}


} // namespace arbitration_graphs_py
