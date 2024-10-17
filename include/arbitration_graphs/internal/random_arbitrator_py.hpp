#pragma once

#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include "../arbitrator.hpp"
#include "../behavior.hpp"
#include "../random_arbitrator.hpp"

namespace arbitration_graphs::python_api {

namespace py = pybind11;

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyRandomArbitratorOption
        : public RandomArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option {
public:
    using BaseT = typename RandomArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option;
    using FlagsT = typename BaseT::FlagsT;

    explicit PyRandomArbitratorOption(const typename Behavior<SubCommandT>::Ptr& behavior,
                                      const FlagsT& flags,
                                      const double& weight)
            : BaseT(behavior, flags, weight) {
    }
};

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
class PyRandomArbitrator : public RandomArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT> {
public:
    using BaseT = RandomArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    explicit PyRandomArbitrator(const std::string& name, const VerifierT& verifier = VerifierT())
            : BaseT(name, verifier) {
    }

    // NOLINTBEGIN(readability-function-size)
    void addOption(const typename BaseT::Option::Ptr& behavior,
                   const typename BaseT::Option::FlagsT& flags,
                   const double& weight = 1) {
        PYBIND11_OVERRIDE_NAME(void, BaseT, "add_option", addOption, behavior, flags, weight);
    }

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

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
void bindRandomArbitrator(py::module& module) {
    using BehaviorT = Behavior<SubCommandT>;

    using ArbitratorT = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using ArbitratorOptionT = typename ArbitratorT::Option;

    using RandomArbitratorT = RandomArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;
    using PyRandomArbitratorT = PyRandomArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    using OptionT = typename RandomArbitratorT::Option;
    using PyOptionT = PyRandomArbitratorOption<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    using FlagsT = typename OptionT::FlagsT;

    py::class_<RandomArbitratorT, ArbitratorT, PyRandomArbitratorT, std::shared_ptr<RandomArbitratorT>>
        randomArbitrator(module, "RandomArbitrator");
    randomArbitrator
        .def(py::init<const std::string&, const VerifierT&>(),
             py::arg("name") = "RandomArbitrator",
             py::arg("verifier") = VerifierT())
        .def("add_option", &RandomArbitratorT::addOption, py::arg("behavior"), py::arg("flags"), py::arg("weight") = 1)
        .def(
            "to_yaml_as_str",
            [](const RandomArbitratorT& self, const Time& time) {
                return static_cast<const PyRandomArbitratorT&>(self).toYamlAsString(time);
            },
            py::arg("time"))
        .def("__repr__", [](const RandomArbitratorT& self) { return "<RandomArbitrator '" + self.name_ + "'>"; });

    py::class_<OptionT, ArbitratorOptionT, PyOptionT, std::shared_ptr<OptionT>> option(randomArbitrator, "Option");
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

} // namespace arbitration_graphs::python_api
