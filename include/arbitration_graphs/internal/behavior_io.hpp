#pragma once

#include "../behavior.hpp"


namespace arbitration_graphs {

template <typename EnvironmentModelT, typename CommandT>
std::string Behavior<EnvironmentModelT, CommandT>::to_str(const Time& time,
                                                          const EnvironmentModelT& environmentModel,
                                                          const std::string& prefix,
                                                          const std::string& suffix) const {
    std::stringstream stream;
    to_stream(stream, time, environmentModel, prefix, suffix);
    return stream.str();
}

template <typename EnvironmentModelT, typename CommandT>
std::ostream& Behavior<EnvironmentModelT, CommandT>::to_stream(std::ostream& output,
                                                               const Time& time,
                                                               const EnvironmentModelT& environmentModel,
                                                               const std::string& /*prefix*/,
                                                               const std::string& /*suffix*/) const {
    if (checkInvocationCondition(time, environmentModel)) {
        output << "\033[32mINVOCATION\033[39m ";
    } else {
        output << "\033[31mInvocation\033[39m ";
    }
    if (checkCommitmentCondition(time, environmentModel)) {
        output << "\033[32mCOMMITMENT\033[39m ";
    } else {
        output << "\033[31mCommitment\033[39m ";
    }

    output << name();
    return output;
}

template <typename EnvironmentModelT, typename CommandT>
YAML::Node Behavior<EnvironmentModelT, CommandT>::toYaml(const Time& time,
                                                         const EnvironmentModelT& environmentModel) const {
    YAML::Node node;
    node["type"] = "Behavior";
    node["name"] = name();
    node["invocationCondition"] = checkInvocationCondition(time, environmentModel);
    node["commitmentCondition"] = checkCommitmentCondition(time, environmentModel);
    return node;
}

} // namespace arbitration_graphs
