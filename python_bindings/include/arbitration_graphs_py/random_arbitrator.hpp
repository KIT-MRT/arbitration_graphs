#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <arbitration_graphs/arbitrator.hpp>
#include <arbitration_graphs/behavior.hpp>
#include <arbitration_graphs/random_arbitrator.hpp>

#include "command_wrapper.hpp"
#include "verification_wrapper.hpp"

namespace arbitration_graphs_py {

namespace py = pybind11;
using namespace arbitration_graphs;

class PyRandomArbitrator
        : public RandomArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper> {
public:
    using BaseT = RandomArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;

    explicit PyRandomArbitrator(const std::string& name, const VerifierWrapper& verifier = VerifierWrapper())
            : BaseT(name, verifier) {
    }

    // NOLINTBEGIN(readability-function-size)
    void addOption(const typename BaseT::Behavior::Ptr& behavior,
                   const typename BaseT::Option::FlagsT& flags,
                   const double& weight = 1) {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "add_option", addOption, behavior, flags, weight);
    }
    // NOLINTEND(readability-function-size)

    std::string toYamlAsString(const Time& time) const {
        YAML::Emitter out;
        out << BaseT::toYaml(time);
        return out.c_str();
    }
};

inline void bindRandomArbitrator(py::module& module) {
    using BehaviorT = Behavior<CommandWrapper>;

    using ArbitratorT = Arbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using RandomArbitratorT =
        RandomArbitrator<CommandWrapper, CommandWrapper, VerifierWrapper, VerificationResultWrapper>;

    using OptionT = typename RandomArbitratorT::Option;
    using FlagsT = typename OptionT::FlagsT;

    py::class_<RandomArbitratorT, ArbitratorT, PyRandomArbitrator, std::shared_ptr<RandomArbitratorT>> randomArbitrator(
        module, "RandomArbitrator");
    randomArbitrator
        .def(py::init<const std::string&, const VerifierWrapper&>(),
             py::arg("name") = "RandomArbitrator",
             py::arg("verifier") = VerifierWrapper())
        .def("add_option", &RandomArbitratorT::addOption, py::arg("behavior"), py::arg("flags"), py::arg("weight") = 1)
        .def(
            "to_yaml_as_str",
            [](const PyRandomArbitrator& self, const Time& time) { return self.toYamlAsString(time); },
            py::arg("time"))
        .def("__repr__", [](const RandomArbitratorT& self) { return "<RandomArbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, ArbitratorOptionT, std::shared_ptr<OptionT>> option(randomArbitrator, "Option");
    option.def(py::init<const typename BehaviorT::Ptr&, const FlagsT&, const double&>(),
               py::arg("behavior"),
               py::arg("flags"),
               py::arg("weight"));

    py::enum_<typename OptionT::Flags>(option, "Flags")
        .value("NO_FLAGS", OptionT::NO_FLAGS)
        .value("INTERRUPTABLE", OptionT::INTERRUPTABLE)
        .value("FALLBACK", OptionT::FALLBACK)
        .export_values();
}

} // namespace arbitration_graphs_py
